#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    Map<int, ItemAbility> ItemAbilitiesData;
    Array<ItemAbilityDescriptor> ActiveItemAbilities;

    //---------------------------------------------------------------------------
    //							ItemAbilityDescriptor
    //---------------------------------------------------------------------------

    ItemAbilityDescriptor::ItemAbilityDescriptor(const ItemAbility* itemAbility, const ItemExtension* itemExtension)
    {
        Ability = itemAbility;
        Extension = itemExtension;
        ScriptInstance.AbilityId = Ability ? Ability->UId : Invalid;
        ScriptInstance.ExtensionId = Extension ? Extension->UId : Invalid;
    }

    constexpr bool ItemAbilityDescriptor::operator==(const ItemAbilityDescriptor& other) const noexcept {
        return ScriptInstance.AbilityId == other.ScriptInstance.AbilityId && ScriptInstance.ExtensionId == other.ScriptInstance.ExtensionId;
    }
    constexpr bool ItemAbilityDescriptor::operator<(const ItemAbilityDescriptor& other) const noexcept {
        if (ScriptInstance.AbilityId != other.ScriptInstance.AbilityId) return ScriptInstance.AbilityId < other.ScriptInstance.AbilityId;
        return ScriptInstance.ExtensionId < other.ScriptInstance.ExtensionId;
    }
    constexpr bool ItemAbilityDescriptor::operator!=(const ItemAbilityDescriptor& other) const noexcept { return !(*this == other); }
    constexpr bool ItemAbilityDescriptor::operator>(const ItemAbilityDescriptor& other) const noexcept { return other < *this; }
    constexpr bool ItemAbilityDescriptor::operator<=(const ItemAbilityDescriptor& other) const noexcept { return !(other < *this); }
    constexpr bool ItemAbilityDescriptor::operator>=(const ItemAbilityDescriptor& other) const noexcept { return !(*this < other); }


    inline void ClearActiveItemAbilities() { ActiveItemAbilities.Clear(); }
    int GetActiveItemAbilitiesCount() { return static_cast<int>(ActiveItemAbilities.GetNum()); }

    inline void BuildItemAbilityDescriptor(ItemAbilityDescriptor& desc, const ItemAbility* itemAbility, const ItemExtension* itemExtension, const uint index)
    {
        desc.Ability = itemAbility;
        desc.Extension = itemExtension;
        desc.ScriptInstance.Value = itemExtension->AbilityValue[index];
        desc.ScriptInstance.Chance = itemExtension->AbilityChance[index];
        desc.ScriptInstance.Duration = itemExtension->AbilityDuration[index];
        desc.ScriptInstance.Range = itemExtension->AbilityRange[index];
        
        for (uint i = 0; i < ItemAbilityParamsMax; ++i)
            desc.ScriptInstance.Params[i] = itemAbility->ScriptInstance.Params[i];
    }

    inline void BuildItemAbilitySearchDescriptor(ItemAbilityDescriptor& key, const ItemAbility* itemAbility, const ItemExtension* itemExtension, const uint index) noexcept
    {
        if (!itemAbility || !itemExtension || index == Invalid) return;

        key.Ability = itemAbility;
        key.Extension = itemExtension;
        key.ScriptInstance.AbilityId = itemAbility->UId;
        key.ScriptInstance.ExtensionId = itemExtension->UId;
        key.ScriptInstance.Value = itemExtension->AbilityValue[index];
    }

    void AddItemAbilityDescriptor(const ItemAbilityDescriptor& desc)
    {
        if (desc.Ability && desc.Ability->OnAddFunc != Invalid)
        {
            DEBUG_MSG("AddItemAbilityDescriptor: call OnAddFunc() [index: " + Z(desc.Ability->OnAddFunc) + "] ...");
            // ToDo: prepare some instances to pass out in func
            parser->CallFunc(desc.Ability->OnAddFunc);
        }
        ActiveItemAbilities.InsertSorted(desc);
    }

    void RemoveItemAbilityDescriptor(const ItemAbilityDescriptor& desc)
    {
        uint index = ActiveItemAbilities.SearchEqualSorted(desc);
        if (index == Invalid) return;

        if (desc.Ability && desc.Ability->OnRemoveFunc != Invalid)
        {
            DEBUG_MSG("RemoveItemAbilityDescriptor: call OnRemoveFunc() [index: " + Z(desc.Ability->OnRemoveFunc) + "] ...");
            // ToDo: prepare some instances
            parser->CallFunc(desc.Ability->OnRemoveFunc);
        }
        ActiveItemAbilities.RemoveSorted(desc);
    }

    //---------------------------------------------------------------------------
    //							ItemAbility
    //---------------------------------------------------------------------------

    bool InitializeItemAbility(const zSTRING& instanceName, ItemAbility& itemAbility)
    {
        const int index = parser->GetIndex(instanceName);
        if (index == Invalid)
        {
            DEBUG_MSG("InitializeItemAbility: item ability instance '" + instanceName + "' not found!");
            return false;
        }

        parser->CreateInstance(index, &itemAbility.ScriptInstance);
        const int abilityId = itemAbility.ScriptInstance.Id;

        if (ItemAbilitiesData.HasKey(abilityId))
        {
            DEBUG_MSG("InitializeItemAbility: item ability '" + instanceName + "' with id: " + Z(abilityId) + " seems already registered!");
            return false;
        }

        itemAbility.UId = abilityId;
        itemAbility.InstanceName = instanceName;
        itemAbility.TriggerType = (ItemAbilityTriggerIndex)itemAbility.ScriptInstance.Type;
        itemAbility.AbilityFlags = (ItemAbilityFlags)itemAbility.ScriptInstance.Flags;

        itemAbility.ConditionFunc = parser->GetIndex(itemAbility.ScriptInstance.Condition);
        itemAbility.OnAddFunc = parser->GetIndex(itemAbility.ScriptInstance.OnAdd);
        itemAbility.OnRemoveFunc = parser->GetIndex(itemAbility.ScriptInstance.OnRemove);
        itemAbility.OnTriggerFunc = parser->GetIndex(itemAbility.ScriptInstance.OnTrigger);

        return true;
    }

    void InitItemAbilitiesData()
    {
        zCParser* par = zCParser::GetParser();
        zCPar_Symbol* indxArray = par->GetSymbol("StExt_ItemAbilitiesIndexArray");
        if (!indxArray)
        {
            DEBUG_MSG("'StExt_ItemAbilitiesIndexArray' not found!");
            return;
        }

        int count = 0;
        ItemAbilitiesData = Map<int, ItemAbility>();
        DEBUG_MSG("Initialize " + Z((int)indxArray->ele) + " item abilities...");
        for (uint i = 0; i < indxArray->ele; ++i)
        {
            ItemAbility itemAbility = ItemAbility();
            if (!InitializeItemAbility(indxArray->stringdata[i], itemAbility)) continue;            

            ItemAbilitiesData.Insert(itemAbility.UId, itemAbility);
            ++count;
        }
        ActiveItemAbilities = Array<ItemAbilityDescriptor>();
        DEBUG_MSG("Initialized " + Z(count) + " item abilities!");
    }

    inline const ItemAbility* GetItemAbility(const int abilityId)
    {
        auto it = ItemAbilitiesData.GetSafePair(abilityId);
        if (!it)
        {
            DEBUG_MSG("GetItemAbility: ability with id: " + Z abilityId + " not found!");
            return Null;
        }
        return &it->GetValue();
    }


    void ApplyItemAbilities(const oCItem* item, const ItemExtension* extension)
    {
        if (!item || !extension)
        {
            DEBUG_MSG_IF(!item, "ApplyItemAbilities: item is null!");
            DEBUG_MSG_IF(!extension, "ApplyItemAbilities: item extension is null!");
            return;
        }

        for (uint i = 0; i < ItemExtension_Abilities_Max; ++i)
        {
            if (extension->AbilityId[i] == Invalid) continue;
                
            const int abilityId = extension->AbilityId[i];
            const ItemAbility* itemAbility = GetItemAbility(abilityId);
            if (!itemAbility)
            {
                DEBUG_MSG("ApplyItemAbilities: item itemAbility with id: " + Z(abilityId) + " not found!");
                continue;
            }
            ItemAbilityDescriptor abilityDescriptor = ItemAbilityDescriptor(itemAbility, extension);
            BuildItemAbilityDescriptor(abilityDescriptor, itemAbility, extension, i);
            AddItemAbilityDescriptor(abilityDescriptor);
        }
    }
    
    void RemoveItemAbilities(const oCItem* item, const ItemExtension* extension)
    {
        if (!item || !extension)
        {
            DEBUG_MSG_IF(!item, "RemoveItemAbilities: item is null!");
            DEBUG_MSG_IF(!extension, "RemoveItemAbilities: item extension is null!");
            return;
        }

        ItemAbilityDescriptor abilityDescriptor = ItemAbilityDescriptor(Null, extension);
        for (uint i = 0; i < ItemExtension_Abilities_Max; ++i)
        {
            if (extension->AbilityId[i] == Invalid) continue;

            const int abilityId = extension->AbilityId[i];
            const ItemAbility* itemAbility = GetItemAbility(abilityId);
            if (!itemAbility)
            {
                DEBUG_MSG("RemoveItemAbilities: item itemAbility with id: " + Z(abilityId) + " not found!");
                continue;
            }

            BuildItemAbilitySearchDescriptor(abilityDescriptor, itemAbility, extension, i);
            RemoveItemAbilityDescriptor(abilityDescriptor);
        }
    }

    bool IsItemAbilityActive(const int abilityId)
    {
        if (abilityId < 0 || ActiveItemAbilities.IsEmpty()) return false;

        const uint abilitiesCount = ActiveItemAbilities.GetNum();
        uint low = 0;
        uint high = abilitiesCount;
        while (low < high)
        {
            uint mid = (low + high) / 2;
            if (ActiveItemAbilities[mid].ScriptInstance.AbilityId < abilityId)
                low = mid + 1;
            else high = mid;
        }
        return (low < abilitiesCount) && (ActiveItemAbilities[low].ScriptInstance.AbilityId == abilityId);
    }

    void UseItemAbilityFromItem(oCItem* item) {}



}
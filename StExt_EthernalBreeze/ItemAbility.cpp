#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	std::map<int, ItemAbility*> ItemAbilitiesData = {};
	Array<ItemAbilityRecord*> EquipedItemAbilitiesData = {};


    void InitializeItemAbility(zSTRING itemAbility)
    {
        int index = parser->GetIndex(itemAbility);
        if (index == Invalid)
        {
            DEBUG_MSG("InitializeItemAbility - item ability instance '" + itemAbility + "' not found!");
            return;
        }

        ItemAbility* ability = new ItemAbility();
        parser->CreateInstance(index, &ability->C_ItemAbility);
        int abilityId = ability->C_ItemAbility.Id;

        if (ItemAbilitiesData.find(abilityId) != ItemAbilitiesData.end())
        {
            DEBUG_MSG("InitializeItemAbility - item ability '" + itemAbility + "' with id: " + Z abilityId + " seems already registered!");
            delete ability;
            return;
        }

        ability->ConditionFunc = parser->GetIndex(ability->C_ItemAbility.Condition);
        ability->OnApplyFunc = parser->GetIndex(ability->C_ItemAbility.OnApply);
        ability->InstanceId = index;
        ability->InstanceName = itemAbility;
        ItemAbilitiesData.insert({ abilityId, ability });
        DEBUG_MSG("InitializeItemAbility - item ability '" + itemAbility + "' with id: " + Z abilityId + " registered! Total: " + Z (int)ItemAbilitiesData.size());
    }

    ItemAbility* GetItemAbility(const int abilityId)
    {
        ItemAbility* result = Null;
        try { result = ItemAbilitiesData.at(abilityId); }
        catch (const std::out_of_range& e) { DEBUG_MSG("GetItemAbility - ability with id: " + Z abilityId + " not found!"); }
        return result;
    }

    ItemAbilityRecord* BuildItemAbilityRecord(oCItem* item, C_EnchantmentData* enchantment, ItemAbility* itemAbility, bool isFromWeapon, int value, int chance, int duration)
    {
        ItemAbilityRecord* result = new ItemAbilityRecord();
        result->Ability = itemAbility;
        result->Enchantment = enchantment;
        result->Item = item;
        result->C_ItemAbilityRecord.Id = itemAbility->C_ItemAbility.Id;
        result->C_ItemAbilityRecord.Value = value;
        result->C_ItemAbilityRecord.Duration = duration;
        result->C_ItemAbilityRecord.Chance = chance;
        result->C_ItemAbilityRecord.IsFromWeapon = isFromWeapon;
        return result;
    }

    void ApplyItemAbilities(oCItem* item, C_EnchantmentData* enchantment)
    {
        if (!item || !enchantment) return;

        for (int i = 0; i < EnchantAbilityMax; i++)
        {
            if (enchantment->AbilityId[i] > Invalid)
            {
                int abilityId = enchantment->AbilityId[i];
                int abilityValue = enchantment->AbilityValue[i];
                int abilityChance = enchantment->AbilityChance[i];
                int abilityDuration = enchantment->AbilityDuration[i];

                ItemAbility* ability = GetItemAbility(abilityId);
                if (!ability) continue;

                if ((ability->C_ItemAbility.Type == StExt_ItemAbilityType_Offensive) &&
                    HasFlag(item->mainflag, item_kat_nf | item_kat_ff)) continue;

                ItemAbilityRecord* record = BuildItemAbilityRecord(item, enchantment, ability, false, abilityValue, abilityChance, abilityDuration);
                EquipedItemAbilitiesData.Insert(record);
            }
        }
    }
    void RemoveItemAbilities(oCItem* item, C_EnchantmentData* enchantment) 
    {
        if (!item || !enchantment) return;

        size_t i = 0;
        while (i < EquipedItemAbilitiesData.GetNum())
        {
            auto entry = EquipedItemAbilitiesData[i];
            if (entry->Item == item || entry->Enchantment == enchantment)
            {
                // Warning: there is issue, when reference to inner struct can be keeped by scripts
                // such access can cause exception in moment of unapplying abilities and some update loop!
                delete entry;
                EquipedItemAbilitiesData.RemoveAt(i);
            }
            else ++i;
        }
    }

    bool GetItemAbilityChance(int abilityId, int chance)
    {
        ItemAbility* ability = GetItemAbility(abilityId);
        return GetItemAbilityChance(ability, chance);
    }   

    bool GetItemAbilityChance(ItemAbility* ability, int chance)
    {
        if (!ability)
            return (HasFlag(ability->C_ItemAbility.Flags, StExt_ItemAbilityFlag_AlwaysSuccessChance) || (chance >= 1000) || ((rand() % 1000) < chance));        
        return false;
    }

    bool FilterItemAbility(ItemAbility* ability, int abilityType, int includeFlags, int excludeFlags)
    {
        if (!ability) return false;
        if ((abilityType != Invalid) && (abilityType != ability->C_ItemAbility.Type)) return false;
        if ((includeFlags != 0) && !HasFlag(ability->C_ItemAbility.Flags, includeFlags)) return false;
        if ((excludeFlags != 0) && HasFlag(ability->C_ItemAbility.Flags, excludeFlags)) return false;
        if (HasFlag(ability->C_ItemAbility.Flags, StExt_ItemAbilityFlag_UseTimeBinding) && 
            !HasFlag(ability->C_ItemAbility.TimeBindingFlags, StExt_CurrentDayPart)) return false;
        if (ability->ConditionFunc != Invalid)
        {
            int isCondition = *(int*)parser->CallFunc(ability->ConditionFunc);
            if (!isCondition) return false;
        }        
        return true;
    }

    int GetEquipedItemAbilitiesCount() { return (int)EquipedItemAbilitiesData.GetNum(); }
    bool IsItemAbilityEquiped(int abilityId)
    {
        if (abilityId < 0) return false;

        size_t max = EquipedItemAbilitiesData.GetNum();
        for (size_t i = 0; i < max; ++i)
        {
            if (!EquipedItemAbilitiesData[i])
            {
                DEBUG_MSG("IsItemAbilityEquiped - ability record is null!");
                continue;
            }
            if (EquipedItemAbilitiesData[i]->C_ItemAbilityRecord.Id == abilityId) return true;
        }
        return false;
    }

    ItemAbilityRecord* GetOffenciveAbilityFromItem(oCItem* item)
    {
        ItemAbilityRecord* result = Null;
        if (!item) return result;

        C_EnchantmentData* enchantment = GetEnchantmentData(item);
        if (!enchantment) return result;

        // ToDo: select one of weapons offencive ability
        // build new record and return it
        // delete such entry at the end of handling i gues
        for (int i = 0; i < EnchantAbilityMax; i++)
        {
            if (enchantment->AbilityId[i] > Invalid)
            {
                int abilityId = enchantment->AbilityId[i];
                int abilityValue = enchantment->AbilityValue[i];
                int abilityChance = enchantment->AbilityChance[i];
                int abilityDuration = enchantment->AbilityDuration[i];

                ItemAbility* ability = GetItemAbility(abilityId);
                if (!FilterItemAbility(ability, StExt_ItemAbilityType_Offensive)) continue;
                if (!GetItemAbilityChance(abilityId, abilityChance)) continue;

                result = BuildItemAbilityRecord(item, enchantment, ability, true, abilityValue, abilityChance, abilityDuration);
                break;
            }
        }

        return result;
    }

    ItemAbilityRecord* SelectItemAbility(int abilityType, int includeFlags, int excludeFlags)
    {
        ItemAbilityRecord* result = Null;
        size_t max = EquipedItemAbilitiesData.GetNum();
        for (size_t i = 0; i < max; ++i)
        {
            if (!EquipedItemAbilitiesData[i])
            {
                DEBUG_MSG("SelectItemAbility - ability record is null!");
                continue;
            }
            ItemAbility* ability = EquipedItemAbilitiesData[i]->Ability;
            int abilityId = ability->C_ItemAbility.Id;

            if (!FilterItemAbility(ability, abilityType, includeFlags, excludeFlags)) continue;
            if (!GetItemAbilityChance(abilityId, EquipedItemAbilitiesData[i]->C_ItemAbilityRecord.Chance)) continue;
            result = EquipedItemAbilitiesData[i];
            break;
        }       
        return result;
    }


}
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    const string SlotPrefix = "StExt_Gear";

    enum class ItemDisplayMode : int
    {
        UnderArmor = 0,     // Overlay extra items by armor
        Never = 1,          // Never display such items
        Overlay = 2,        // Hide original armor and display only such items
        Always = 3,         // Display this items anyway
    };

    inline zSTRING GetAdditionalItemSlotName(const int wear) { return string::Combine("%s_%i", SlotPrefix, wear); }
    inline bool IsAdditionalItemSlot(const zSTRING& name) { return name.StartWith(SlotPrefix); }
    inline bool IsAdditionalArmorItem(oCItem* item) { return item && item->wear > wear_separator && item->HasFlag(item_kat_armor); }

    bool oCNpc::CanEquipAdditionalArmor(oCItem* item)
    {
        if (HasFlag(item->wear, wear_torso)) return true;
        oCItem* armor = GetEquippedArmor();
        if (!armor) return true;
        return (armor->wear & item->wear) != item->wear;
    }

    void oCNpc::GetEquippedArmors(Array<oCItem*>& array)
    {
        auto contList = inventory2.inventory.next;
        while (contList) 
        {
            oCItem* item = contList->data;
            if (item->HasFlag(ITM_CAT_ARMOR) && item->HasFlag(ITM_FLAG_ACTIVE)) array.Insert(item);
            contList = contList->next;
        }
    }

    TNpcSlot* oCNpc::CreateAdditionalItemSlot(const zSTRING& name)
    {
        TNpcSlot* slot = GetInvSlot(name);
        if (slot) return slot;
        CreateInvSlot(name);
        return GetInvSlot(name);
    }

    void RemoveSoftSkinItem(const oCItem* item)
    {
        if (item->visual_change.IsEmpty()) return;

        zCModel* model = player->GetModel();
        const zSTRING visualName = (Z(item->visual_change)).Upper().Replace(".ASC", "");
        for (int i = 0; i < model->meshSoftSkinList.GetNum(); ++i)
        {
            zCMeshSoftSkin* softSkin = model->meshSoftSkinList[i];
            if (softSkin->GetVisualName() == visualName)
                model->meshSoftSkinList.Remove(softSkin);
        }
    }

    void ApplySoftSkinItem(const oCItem* item)
    {
        if (item->visual_change.IsEmpty()) return;

        ItemDisplayMode displayMode = 
            (ItemDisplayMode)parser->GetSymbol("StExt_Config_ExtraItemsDisplayMode")->single_intdata;
        bool hasArmor = player->GetEquippedArmor() != Null;
        bool doNotApplySkin = (displayMode == ItemDisplayMode::Never) || ((displayMode == ItemDisplayMode::UnderArmor) && hasArmor);
        //if ((displayMode == StExt_ExtraItemsDisplayMode_OverlayArmor) && hasArmor) RemoveSoftSkinItem(player->GetEquippedArmor());

        if (doNotApplySkin) return;
        zCModel* model = player->GetModel();
        const zSTRING visualName = (Z(item->visual_change)).Upper().Replace(".ASC", ".MDM");

        zCModelMeshLib* meshLib = new zCModelMeshLib();
        bool loadSccess = zCModelMeshLib::LoadMDM(visualName, Null, model, &meshLib);
        if (!loadSccess || !model)
        {
            DEBUG_MSG("ApplySoftSkinItem: fail LoadMDM! visualName: " + visualName);
            return;
        }

        auto& softSkinList = meshLib->meshSoftSkinList;
        for (int i = 0; i < softSkinList.GetNum(); ++i)
        {
            zCMeshSoftSkin* softSkin = softSkinList[i];
            if (!softSkin) continue;
            softSkin->AddRef();
            model->meshSoftSkinList.Insert(softSkin);
        }
        meshLib->Release();
    }

    void oCNpc::EquipAdditionalArmorItem(oCItem* item)
    {
        EquipItem(item);
        PutAdditionalArmorItemToSlot(item);
        ApplySoftSkinItem(item);
    }

    void oCNpc::UnequipAdditionalArmorItem(int wear)
    {
        if (wear <= wear_separator) return;
        for (int i = 0; i < invSlot.GetNumInList(); ++i) 
        {
            TNpcSlot* slot = invSlot[i];
            if (!slot) continue;
            const zSTRING slotName = slot->name;

            if (slotName.StartWith(SlotPrefix)) 
            {
                oCItem* item = slot->vob->CastTo<oCItem>();
                if (!item) continue;
                if (item->wear & (wear > wear_separator))
                {
                    UnequipItem_StExt(item);
                    RemoveSoftSkinItem(item);
                }
            }
        }
    }

    void oCNpc::PutAdditionalArmorItemToSlot(oCItem* item) 
    {
        if (!item) return;
        const zSTRING slotName = GetAdditionalItemSlotName(item->wear);
        TNpcSlot* slot = CreateAdditionalItemSlot(slotName);
        if (slot)
            PutInSlot(slot, item, True);
        DEBUG_MSG_IF(!slot, "PutAdditionalArmorItemToSlot: fail to put item to slot: '" + slotName + "'!");
    }

    void oCNpc::RemoveAdditionalArmorItemFromSlot(oCItem* item) 
    {
        const zSTRING slotName = GetAdditionalItemSlotName(item->wear);
        TNpcSlot* slot = GetInvSlot(slotName);
        if (!slot)
        {
            DEBUG_MSG("RemoveAdditionalArmorItemFromSlot: slot: '" + slotName + "' not found!");
            return;
        }
        oCVob* vob = RemoveFromSlot(slot, 0, 1);
    }

    HOOK Hook_oCNpc_UnequipItem PATCH(&oCNpc::UnequipItem, &oCNpc::UnequipItem_StExt);
    void oCNpc::UnequipItem_StExt(oCItem* item)
    {
        if (!item) return;
        if (IsAdditionalArmorItem(item) && item->HasFlag(ITM_FLAG_ACTIVE))
        {
            item->AddRef();
            THISCALL(Hook_oCNpc_UnequipItem)(item);
            UnequipAdditionalArmorItem(item->wear);
            RemoveAdditionalArmorItemFromSlot(item);
            item->Release();
            return;
        }        
        THISCALL(Hook_oCNpc_UnequipItem)(item);
    }

    HOOK Hook_oCNpc_EquipArmor PATCH(&oCNpc::EquipArmor, &oCNpc::EquipArmor_StExt);
    void oCNpc::EquipArmor_StExt(oCItem* item) 
    {
        if (!item) return;
        if (item->HasFlag(ITM_FLAG_ACTIVE)) { UnequipItem(item); return; }
        if (!CanEquipAdditionalArmor(item)) return;

        if (this->IsSelfPlayer())
        {
            if (!CanUse(item)) { DisplayCannotUse(); return; }
            UnequipAdditionalArmorItem(item->wear);
            parser->SetInstance("STEXT_ITEM", item);
            parser->CallFunc(OnArmorEquipFunc);
        }

        THISCALL(Hook_oCNpc_EquipArmor)(item);
        if (this->IsSelfPlayer())
        {
            if ((item->wear == wear_head) && parser->GetSymbol("StExt_Config_HideHelm")->single_intdata)
            {
                zCModel* pModel = this->GetModel();
                zCModelNodeInst* pNode = pModel ? pModel->SearchNode("ZS_HELMET") : Null;
                if (pNode && pModel)
                    pModel->SetNodeVisual(pNode, Null, false);
            }
            if (IsAdditionalArmorItem(item) && !item->HasFlag(ITM_FLAG_ACTIVE)) { EquipAdditionalArmorItem(item); }
        }
    }

    // Hook placed in another file
    void UnArchiveAdditionalArmors(oCNpc* npc)
    {
        if (!npc || !npc->IsSelfPlayer()) return;
        for (int i = 0; i < npc->invSlot.GetNumInList(); ++i)
        {
            TNpcSlot* slot = npc->invSlot[i];
            if (IsAdditionalItemSlot(slot->name))
            {
                oCItem* item = slot->vob->CastTo<oCItem>();
                if (item) 
                {
                    item->SetFlag(ITM_FLAG_ACTIVE);
                    item->AddRef();
                    slot->inInventory = True;
                    ApplySoftSkinItem(item);
                }
            }
        }
    }

}
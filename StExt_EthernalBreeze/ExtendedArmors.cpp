#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    const string SlotPrefix = "StExt_Gear";
    const int ITM_WEAR_SEPARATOR = 1 << 6;
    const int ITM_WEAR_GLOVES = 1 << 7;
    const int ITM_WEAR_BOOTS = 1 << 8;
    const int ITM_WEAR_PANTS = 1 << 9;
    const int ITM_WEAR_COLAR = 1 << 10;
    const int ITM_WEAR_PAULDRONS = 1 << 11;

    const int StExt_ExtraItemsDisplayMode_UnderArmor = 0;		// Overlay extra items by armor
    const int StExt_ExtraItemsDisplayMode_None = 1;				// Never display such items
    const int StExt_ExtraItemsDisplayMode_OverlayArmor = 2;		// Hide original armor and display only such items
    const int StExt_ExtraItemsDisplayMode_Always = 3;			// Display this items anyway

    zSTRING GetAdditionalItemSlotName(int wear) { return string::Combine("%s%i", SlotPrefix, wear); }
    bool IsAdditionalItemSlot(const zSTRING& name) { return name.StartWith(SlotPrefix); }
    bool IsAdditionalArmorItem(oCItem* item) { return item && (uint)item->wear > ITM_WEAR_SEPARATOR && item->HasFlag(ITM_CAT_ARMOR); }

    bool oCNpc::CanEquipAdditionalArmor(oCItem* item)
    {
        if (item->wear & ITM_WEAR_TORSO) return true;
        oCItem* armor = GetEquippedArmor();
        if (!armor) return true;
        return (armor->wear & item->wear) != item->wear;
    }

    Array<oCItem*> oCNpc::GetEquippedArmors() 
    {
        Array<oCItem*> array;
        auto contList = inventory2.inventory.next;
        while (contList) 
        {
            oCItem* item = contList->data;
            if (item->HasFlag(ITM_CAT_ARMOR) && item->HasFlag(ITM_FLAG_ACTIVE))
                array.Insert(item);
            contList = contList->next;
        }
        return array;
    }

    TNpcSlot* oCNpc::CreateAdditionalItemSlot(const zSTRING& name)
    {
        TNpcSlot* slot = GetInvSlot(name);
        if (slot) 
            return slot;
        CreateInvSlot(name);
        return GetInvSlot(name);
    }

    void RemoveSoftSkinItem(oCItem* item)
    {
        if (item->visual_change.IsEmpty()) return;

        zCModel* model = player->GetModel();
        zSTRING visualName = (Z item->visual_change).Upper().Replace(".ASC", "");

        for (int i = 0; i < model->meshSoftSkinList.GetNum(); i++)
        {
            zCMeshSoftSkin* softSkin = model->meshSoftSkinList[i];
            if (softSkin->GetVisualName() == visualName)
                model->meshSoftSkinList.Remove(softSkin);
        }
    }

    void ApplySoftSkinItem(oCItem* item)
    {
        if (item->visual_change.IsEmpty()) return;

        int displayMode = parser->GetSymbol("StExt_Config_ExtraItemsDisplayMode")->single_intdata;
        bool doNotApplySkin = false;
        bool hasArmor = player->GetEquippedArmor() != Null;

        if ((displayMode == StExt_ExtraItemsDisplayMode_None) ||
            ((displayMode == StExt_ExtraItemsDisplayMode_UnderArmor) && hasArmor)) doNotApplySkin = true;
        //if ((displayMode == StExt_ExtraItemsDisplayMode_OverlayArmor) && hasArmor) RemoveSoftSkinItem(player->GetEquippedArmor());

        if (doNotApplySkin) return;        

        zCModel* model = player->GetModel();
        zSTRING visualName = (Z item->visual_change).Upper().Replace(".ASC", ".MDM");

        zCModelMeshLib* meshLib = new zCModelMeshLib();
        bool loadSccess = zCModelMeshLib::LoadMDM(visualName, Null, model, &meshLib);
        if (!loadSccess) return;

        auto& softSkinList = meshLib->meshSoftSkinList;
        for (int i = 0; i < softSkinList.GetNum(); i++)
        {
            zCMeshSoftSkin* softSkin = softSkinList[i]; // why original is 0?
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
        if ((uint)wear <= ITM_WEAR_SEPARATOR)
            return;

        for (int i = 0; i < invSlot.GetNumInList(); i++) 
        {
            TNpcSlot* slot = invSlot[i];
            zSTRING slotName = slot->name;

            if (slotName.StartWith(SlotPrefix)) 
            {
                oCItem* item = slot->vob->CastTo<oCItem>();
                if (!item)
                    continue;

                uint collision = item->wear & wear;
                if (collision > ITM_WEAR_SEPARATOR)
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
        zCModel* model = GetModel();
        zSTRING slotName = GetAdditionalItemSlotName(item->wear);
        TNpcSlot* slot = CreateAdditionalItemSlot(slotName);
        PutInSlot(slot, item, True);
    }

    void oCNpc::RemoveAdditionalArmorItemFromSlot(oCItem* item) 
    {
        zSTRING slotName = GetAdditionalItemSlotName(item->wear);
        TNpcSlot* slot = GetInvSlot(slotName);
        if (!slot) return;
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
        }
        else
            THISCALL(Hook_oCNpc_UnequipItem)(item);
    }

    HOOK Hook_oCNpc_EquipArmor PATCH(&oCNpc::EquipArmor, &oCNpc::EquipArmor_StExt);
    void oCNpc::EquipArmor_StExt(oCItem* item) 
    {
        if (!item)
            return;

        if (item->HasFlag(ITM_FLAG_ACTIVE)) {
            UnequipItem(item);
            return;
        }
        if (!CanUse(item) && this == player) {
            DisplayCannotUse();
            return;
        }
        if (!CanEquipAdditionalArmor(item)) return;        
        
        if (this->IsSelfPlayer())
        {
            UnequipAdditionalArmorItem(item->wear);
            parser->SetInstance("StExt_Item", item);
            parser->CallFunc(OnArmorEquipFunc);
        }

        THISCALL(Hook_oCNpc_EquipArmor)(item);

        if (this->IsSelfPlayer())
        {
            if ((item->wear == wear_head) && (bool)parser->GetSymbol("StExt_Config_HideHelm")->single_intdata)
            {
                zCModel* pModel = this->GetModel();
                zCModelNodeInst* pNode = pModel->SearchNode("ZS_HELMET");
                pModel->SetNodeVisual(pNode, Null, false);
            }
            if (IsAdditionalArmorItem(item) && !item->HasFlag(ITM_FLAG_ACTIVE))
                EquipAdditionalArmorItem(item);
        }
    }

    // Hook placed in another file
    void UnArchiveAdditionalArmors(oCNpc* npc)
    {
        if (!npc) return;
        if (!npc->IsSelfPlayer()) return;

        for (int i = 0; i < npc->invSlot.GetNumInList(); i++)
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
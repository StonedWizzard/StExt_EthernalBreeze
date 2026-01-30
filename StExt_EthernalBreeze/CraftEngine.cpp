#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	Map<int, ReceiptInfo*> ReceiptsData;
    StringMap<const ReceiptInfo*> ReceiptsIndexer;
    CraftContext* CurrentCraftContext;


    //-------------------------------------------------------------------
    //						  CraftContext impl.
    //-------------------------------------------------------------------

    CraftContext::CraftContext(const ReceiptInfo* receipt): Receipt(receipt), ReceiptDescriptor{}
    {
        IsValid = false;
        IsReady = false;

        ReceiptDescriptor.ReceiptId = Receipt->ScriptInstance.Id;
        ReceiptDescriptor.Type = Receipt->ScriptInstance.Type;
        ReceiptDescriptor.IsReady = IsReady;
        ReceiptDescriptor.Flags = Receipt->ScriptInstance.Flags;

        // ToDo: initialize zero's...
        ReceiptDescriptor.Exp = 0;
        ReceiptDescriptor.Chance = 0;
        ReceiptDescriptor.Power = 0;

        memcpy(ReceiptDescriptor.Values, Receipt->ScriptInstance.Vars, sizeof(ReceiptDescriptor.Values));
        for (int i = 0; i < ReceiptInfo_Slots_Max; ++i)
        {
            Items[i] = Null;
            ReceiptDescriptor.SlotItemsCount[i] = Invalid;
            ReceiptDescriptor.SlotItemsPtrs[i] = None;
            ReceiptDescriptor.SlotFlags[i] = (!Receipt->Slots[i].IsEmpty) ? static_cast<int>(receipt->Slots[i].Flags) : Invalid;
        }
    }

    //-------------------------------------------------------------------
    //						    Craft core
    //-------------------------------------------------------------------

    inline const ReceiptInfo* GetReceiptById(const int id)
    {
        auto it = ReceiptsData.GetSafePair(id);
        if (it) return it->GetValue();
        return Null;
    }
    inline const ReceiptInfo* GetReceiptByName(const zSTRING& name)
    {
        auto it = ReceiptsIndexer.Find(name);
        if (it) return *it;
        return Null;
    }


    bool IsItemMatchSlot(const ReceiptSlotInfo& slot, const oCItem* item)
    {
        if (!item)
        {
            DEBUG_MSG("IsItemMatchSlot - item is null!");
            return false;
        }
        const ItemClassKey itmClass = GetItemClassKey(item);
        const ItemExtension* itmExtension = GetItemExtension(item);
        const zSTRING instanceName = GetItemInstanceName(item);

        if (HasFlag(slot.EnabledFilters, FilterFlags::ItemsCount) && item->amount < slot.ItemsCount) return false;
        if (HasFlag(slot.EnabledFilters, FilterFlags::ItemMainFlags) && !HasFlag(item->mainflag, slot.ItemMainFlags)) return false;
        if (HasFlag(slot.EnabledFilters, FilterFlags::ItemBaseFlags) && !HasFlag(item->flags, slot.ItemBaseFlags)) return false;
        if (HasFlag(slot.EnabledFilters, FilterFlags::ItemAdditionalFlags) && !HasFlag(item->hitp, slot.ItemAdditionalFlags)) return false;
        if (HasFlag(slot.EnabledFilters, FilterFlags::ItemDamageType) && !HasFlag(item->damageTypes, slot.ItemDamageType)) return false;
        if (HasFlag(slot.EnabledFilters, FilterFlags::ItemInstances) && !slot.ItemInstances.HasEqual(instanceName)) return false;
        if (HasFlag(slot.EnabledFilters, FilterFlags::ItemClasses) && !slot.ItemClasses.HasEqual(itmClass)) return false;

        if (itmExtension)
        {
            if (HasFlag(slot.EnabledFilters, FilterFlags::ItemLevel) && !ValueInRange(itmExtension->Level, slot.ItemLevel.Min, slot.ItemLevel.Max)) return false;
            if (HasFlag(slot.EnabledFilters, FilterFlags::ItemRank) && !ValueInRange(itmExtension->Rank, slot.ItemRank.Min, slot.ItemRank.Max)) return false;
            if (HasFlag(slot.EnabledFilters, FilterFlags::ItemQuality) && !ValueInRange(itmExtension->Quality, slot.ItemQuality.Min, slot.ItemQuality.Max)) return false;

            if (HasFlag(slot.EnabledFilters, FilterFlags::ItemIncludeTags))
            {
                for (uint i = 0; i < slot.ItemIncludeTags.GetNum(); ++i) 
                {
                    if (!IsIndexInBounds(i, (uint)ItemExtension_Tags_Max))
                    {
                        DEBUG_MSG("IsItemMatchSlot - filter index (Tags) is out of range! Slot: " + slot.Pattern);
                        continue;
                    }
                    if (itmExtension->Tags[slot.ItemIncludeTags[i]] == 0) return false;
                }
            }
            if (HasFlag(slot.EnabledFilters, FilterFlags::ItemExcludeTags))
            {
                for (uint i = 0; i < slot.ItemExcludeTags.GetNum(); ++i)
                {
                    if (!IsIndexInBounds(i, (uint)ItemExtension_Tags_Max))
                    {
                        DEBUG_MSG("IsItemMatchSlot - filter index (Tags) is out of range! Slot: " + slot.Pattern);
                        continue;
                    }
                    if (itmExtension->Tags[slot.ItemExcludeTags[i]] != 0) return false;
                }
            }

            if (HasFlag(slot.EnabledFilters, FilterFlags::ItemCraftData))
            {
                for (auto& pair : slot.ItemCraftData.GetArray())
                {
                    const int key = pair.GetKey();
                    const ValueRange<int>& value = pair.GetValue();
                    if (!IsIndexInBounds(key, ItemExtension_CraftData_Max))
                    {
                        DEBUG_MSG("IsItemMatchSlot - filter index (CraftData) is out of range! Slot: " + slot.Pattern);
                        continue;
                    }
                    if (!ValueInRange(itmExtension->CraftData[key], value.Min, value.Max)) return false;
                }
            }

            if (HasFlag(slot.EnabledFilters, FilterFlags::ItemCraftFlags))
            {
                for (auto& pair : slot.ItemCraftFlags.GetArray())
                {
                    const int key = pair.GetKey();
                    const ValueRange<byte>& value = pair.GetValue();
                    if (!IsIndexInBounds(key, ItemExtension_CraftFlags_Max))
                    {
                        DEBUG_MSG("IsItemMatchSlot - filter index (CraftFlags) is out of range! Slot: " + slot.Pattern);
                        continue;
                    }
                    if (!ValueInRange(itmExtension->CraftFlags[key], value.Min, value.Max)) return false;
                }
            }            

            if (HasFlag(slot.EnabledFilters, FilterFlags::BaseItemInstances) && !slot.BaseItemInstances.HasEqual(itmExtension->BaseInstanceName)) return false;
        }
        return true;
    }

    void FindItemsForSlot(const ReceiptSlotInfo& slot, Map<const oCItem*, int>& result)
    {
        if (!player || !player->inventory2.GetContents())
        {
            DEBUG_MSG("FindItemsForSlot - player (or it's inventory) seems not initialized!?");
            return;
        }
        if (slot.IsEmpty) return;

        zCListSort<oCItem>* it = player->inventory2.GetContents()->GetNextInList();
        while (it)
        {
            const oCItem* itm = it->GetData();
            if (IsItemMatchSlot(slot, itm)) {
                result.Insert(itm, itm->amount);
            }            
            it = it->GetNextInList();
        }
    }

    const oCItem* FindItemForSlot(const ReceiptSlotInfo& slot, int& count)
    {
        count = Invalid;
        if (slot.IsEmpty) return Null;

        Map<const oCItem*, int> itemsList = Map<const oCItem*, int>();
        FindItemsForSlot(slot, itemsList);
        if (!itemsList.IsEmpty())
        {
            const auto& pair = itemsList.GetArray().GetFirst();
            count = pair.GetValue();
            return pair.GetKey();
        }
        return Null;
    }


    void ClearCraftContext() 
    {
        parser->SetInstance("STEXT_RECEIPTDESCRIPTOR", Null);
        parser->SetInstance("STEXT_RECEIPTINFO", Null);
        SAFE_DELETE(CurrentCraftContext); 
    }

    void ValidateCraftContext()
    {
        if (!CurrentCraftContext) return;

        int isValid, isReady;

        CurrentCraftContext->IsValid = (CurrentCraftContext->Receipt->ConditionFunc != Invalid) ?
            static_cast<bool>(*(int*)parser->CallFunc(CurrentCraftContext->Receipt->ConditionFunc)) : true;

        // ToDo - check slots

        CurrentCraftContext->ReceiptDescriptor.IsReady = CurrentCraftContext->IsReady;

        // ToDo - make 'automatic' check for profession?

        // ToDo: Check if chosen items is present and valid, same as count enought...
        // check and choise items from inventory
    }

    void UpdateCraftContext()
    {
        if (!CurrentCraftContext)
        {
            ClearCraftContext();
            return;
        }
        parser->SetInstance("STEXT_RECEIPTDESCRIPTOR", &CurrentCraftContext->ReceiptDescriptor);
        parser->SetInstance("STEXT_RECEIPTINFO", const_cast<void*>(static_cast<const void*>(&CurrentCraftContext->Receipt->ScriptInstance)));
        ValidateCraftContext();
    }

    bool SetCraftContextSlotItem(const uint slotIndex, const oCItem* item, const int itemsCount)
    {
        if (!item || !CurrentCraftContext || !IsIndexInBounds(slotIndex, (uint)ReceiptInfo_Slots_Max))
        {
            DEBUG_MSG_IF((!item), "SetCraftContextSlotItem: item is null!");
            DEBUG_MSG_IF((!CurrentCraftContext), "SetCraftContextSlotItem: craft context is not set!");
            DEBUG_MSG_IF((!IsIndexInBounds(slotIndex, (uint)ReceiptInfo_Slots_Max)), "SetCraftContextSlotItem: slot index out of range!");
            return false;
        }

        const ReceiptSlotInfo& slot = CurrentCraftContext->Receipt->Slots[slotIndex];
        if (!IsItemMatchSlot(slot, item)) return false;

        CurrentCraftContext->Items[slotIndex] = const_cast<oCItem*>(item);
        CurrentCraftContext->ReceiptDescriptor.SlotItemsCount[slotIndex] = itemsCount;
        CurrentCraftContext->ReceiptDescriptor.SlotItemsPtrs[slotIndex] = (int)item;
        return true;
    }

    void PickCraftContextItems()
    {
        if (!CurrentCraftContext) return;

        for (int i = 0; i < ReceiptInfo_Slots_Max; ++i)
        {
            if (CurrentCraftContext->Receipt->Slots[i].IsEmpty) continue;
            if (HasFlag(CurrentCraftContext->Receipt->Slots[i].Flags, SlotFlags::AllowSelection)) continue;

            int itemsCount = Invalid;
            const oCItem* itm = FindItemForSlot(CurrentCraftContext->Receipt->Slots[i], itemsCount);
            if (itm)
                SetCraftContextSlotItem(static_cast<uint>(i), itm, itemsCount);
        }
    }

    // Kind of craft entry point.
    CraftContext* SetCraftContext(const int id)
    {
        ClearCraftContext();
        const ReceiptInfo* receipt = GetReceiptById(id);
        if (!receipt)
        {
            DEBUG_MSG("SetCraftContext: receipt instance id: " + Z(id) + " is incorrect!");
            return Null;
        }

        CurrentCraftContext = new CraftContext(receipt);
        parser->SetInstance("STEXT_RECEIPTDESCRIPTOR", &CurrentCraftContext->ReceiptDescriptor);
        parser->SetInstance("STEXT_RECEIPTINFO", const_cast<void*>(static_cast<const void*>(&CurrentCraftContext->Receipt->ScriptInstance)));

        PickCraftContextItems();

        ValidateCraftContext();

        
        //UpdateCraftContext();
        bool initialized = static_cast<bool>(*(int*)parser->CallFunc(StExt_InitializeCraftContextFunc));
        if (!initialized)
        {
            DEBUG_MSG("SetCraftContext: receipt initialization (Id: " + Z(id) + ") is failed (by script)!");
            ClearCraftContext();
            return Null;
        }
        return CurrentCraftContext;
    }

    inline bool IsCraftContextExist() { return CurrentCraftContext != Null; }
    inline bool IsCraftContextValid() { return CurrentCraftContext != Null && CurrentCraftContext->IsValid; }
    inline bool IsCraftContextReady() { return CurrentCraftContext != Null && CurrentCraftContext->IsReady; }

    

    void StartCraft()
    {
        if(!CurrentCraftContext)
        {
            DEBUG_MSG("StartCraft: craft context is not set!");
            return;
        }
        UpdateCraftContext();

        if (!CurrentCraftContext->IsValid || !CurrentCraftContext->IsReady)
        {
            DEBUG_MSG_IF((!CurrentCraftContext->IsValid), "StartCraft: craft context is not valid (for now)!");
            DEBUG_MSG_IF((!CurrentCraftContext->IsReady), "StartCraft: craft context is not ready (for now)!");
            return;
        }


    }

    //-------------------------------------------------------------------
    //						    Initialization
    //-------------------------------------------------------------------

    bool InitializeReceipt(const zSTRING& instanceName)
    {
        const int instanceIndex = parser->GetIndex(instanceName);
        if (instanceIndex == Invalid)
        {
            DEBUG_MSG("InitializeReceipt: receipt instance '" + instanceName + "' not found!");
            return false;
        }

        ReceiptInfo* receiptInfo = new ReceiptInfo();
        parser->CreateInstance(instanceIndex, &receiptInfo->ScriptInstance);
        const int receiptId = receiptInfo->ScriptInstance.Id;

        if (receiptId == Invalid)
        {
            DEBUG_MSG("InitializeReceipt: receipt instance '" + instanceName + "' has incorrect Id!");
            return false;
        }
        if (GetReceiptById(receiptId) != Null)
        {
            DEBUG_MSG("InitializeReceipt: receipt instance with Id: " + Z(receiptId) + " already registered! InstanceName: '" + instanceName + "'");
            return false;
        }
        if (GetReceiptByName(instanceName) != Null)
        {
            DEBUG_MSG("InitializeReceipt: receipt instance with name '" + instanceName + "' seems already indexed!");
            return false;
        }

        receiptInfo->OnCraftFunc = parser->GetIndex(receiptInfo->ScriptInstance.OnCraft);
        receiptInfo->OnFailFunc = parser->GetIndex(receiptInfo->ScriptInstance.OnFail);
        receiptInfo->ConditionFunc = parser->GetIndex(receiptInfo->ScriptInstance.Condition);

        bool compilationFailed = false;
        for (uint i = 0; i < ReceiptInfo_Slots_Max; ++i)
        {
            receiptInfo->SlotsData[i].ItemsCount = 1;

            if (!CompileSlotPattern(receiptInfo->ScriptInstance.SlotPattern[i], receiptInfo->ScriptInstance.SlotFlags[i], receiptInfo->SlotsData[i]))
            {
                DEBUG_MSG("InitializeReceipt: fail to initialize receipt slot! (InstanceName: '" + 
                    instanceName + "'; SlotIndex: " + Z(static_cast<int>(i)) + ") Line = '" + Z(receiptInfo->Slots[i].Pattern) + "'");
                compilationFailed = true;
            }
        }
        if (compilationFailed) return false;

        ReceiptsData.Insert(receiptId, receiptInfo);
        ReceiptsIndexer.Insert(instanceName, GetReceiptById(receiptId));
        return true;
    }

    void InitializeReceipts()
    {
        DEBUG_MSG("InitializeReceipts: start initialization...");

        ReceiptsData = Map<int, ReceiptInfo*>();

        const zCPar_Symbol* receiptsIndxArray = parser->GetSymbol("StExt_ReceiptsIndexArray");
        if (!receiptsIndxArray)
        {
            DEBUG_MSG("InitializeReceipts: 'StExt_ReceiptsIndexArray' not found!");
            return;
        }

        const int receiptsTotal = receiptsIndxArray->ele;
        int receiptFail = 0;

        DEBUG_MSG("InitializeReceipts: read " + Z((int)receiptsIndxArray->ele) + " receipts...");
        for (uint i = 0; i < receiptsIndxArray->ele; ++i)
        {
            if(!InitializeReceipt(NormalizeInstanceName(receiptsIndxArray->stringdata[i])))
                ++receiptFail;
        }

        DEBUG_MSG_IFELSE(receiptFail, 
            "InitializeReceipts: - receipts initialization has some errors (" + Z receiptFail + ")!", 
            "InitializeReceipts: receipts was successfully initialized!");
    }


	void InitCraftEngine()
	{
        DEBUG_MSG("InitCraftEngine: initialize CraftEngine... ");
        
        InitReceiptSlotParser();
        InitializeReceipts();

        CurrentCraftContext = Null;
        parser->SetInstance("STEXT_RECEIPTDESCRIPTOR", Null);
        parser->SetInstance("STEXT_RECEIPTINFO", Null);
        DEBUG_MSG("InitCraftEngine: initialize CraftEngine done!");
	}

    //-------------------------------------------------------------------
    //						        API
    //-------------------------------------------------------------------

#define GET_ITEM_EXTENSION(id, extVar)                                                                              \
    ItemExtension* extVar = GetItemExtension(id);                                                                   \
    if (!(extVar)) { DEBUG_MSG(Z(__func__) + ": item extension with UId: " + Z((int)(id)) + " not found!"); return; }


    inline void IdentifyItem(const uint extensionId)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->Tags[(int)ItemTags::Unidentified] = False;
    }

    inline void ChangeItemLevel(const uint extensionId, int delta)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->ChangeLevel(delta);
    }

    inline void ChangeItemRank(const uint extensionId, int delta)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->ChangeRank(delta);
    }

    inline void ChangeItemQuality(const uint extensionId, int delta)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->ChangeQuality(delta);
    }

    inline void ChangeItemProperty(const uint extensionId, int propId, int propVal)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->ChangeProperty(propId, propVal);
    }

    inline void SetItemProperty(const uint extensionId, int propId, int propVal)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->SetProperty(propId, propVal);
    }

    inline void SetItemTag(const uint extensionId, int tagId, byte tagVal)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->SetTag(tagId, tagVal);
    }

    inline void SetItemCraftFlag(const uint extensionId, int flagId, byte flag)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->SetCraftFlag(flagId, flag);
    }

    inline void SetItemCraftData(const uint extensionId, int dataId, int data)
    {
        GET_ITEM_EXTENSION(extensionId, ext);
        ext->SetCraftData(dataId, data);
    }


}
// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

// HOOKS
void oCNpc::OnDamage_StExt(oSDamageDescriptor& desc);

void oCNpc::OnDamage_Sound_StExt(oSDamageDescriptor& desc);

void oCNpc::DisplayCannotUse_StExt();

void oCNpc::ChangeAttribute_StExt(int attribute, int value);

void oCNpc::Archive_StExt(zCArchiver& ar);

void oCNpc::Unarchive_StExt(zCArchiver& ar);

void oCNpc::RemoveItemEffects_StExt(oCItem* item);

void oCNpc::AddItemEffects_StExt(oCItem* item);

void oCNpc::ProcessNpc_StExt();

void oCNpc::UnequipItem_StExt(oCItem* item);

void oCNpc::EquipArmor_StExt(oCItem* item);

int oCNpc::CanUse_StExt(oCItem* item);

bool oCNpc::CanEquipAdditionalArmor(oCItem* item);

void oCNpc::GetEquippedArmors(Array<oCItem*>& array);

TNpcSlot* oCNpc::CreateAdditionalItemSlot(const zSTRING& name);

void oCNpc::EquipAdditionalArmorItem(oCItem* item);

void oCNpc::UnequipAdditionalArmorItem(int wear);

void oCNpc::PutAdditionalArmorItemToSlot(oCItem* item);

void oCNpc::RemoveAdditionalArmorItemFromSlot(oCItem* item);

void oCNpc::StopAllVoices_StExt();

int oCNpc::UpdateNextVoice_StExt();

int oCNpc::EV_PlaySound_StExt(oCMsgConversation*);

int oCNpc::EV_OutputSVM_StExt(oCMsgConversation*);

void oCNpc::OpenInventory_StExt(int mode);
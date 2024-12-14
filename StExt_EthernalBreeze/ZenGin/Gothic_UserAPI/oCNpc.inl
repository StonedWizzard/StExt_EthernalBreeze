// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

// HOOKS
void oCNpc::OnDamage_StExt(oSDamageDescriptor& desc);

//void oCNpc::OnDamage_Hit_StExt(oSDamageDescriptor& desc);

//void oCNpc::OnDamage_Events_StExt(oSDamageDescriptor& desc);

//void oCNpc::OnDamage_Anim_StExt(oSDamageDescriptor& desc);

void oCNpc::ChangeAttribute_StExt(int attribute, int value);

void oCNpc::Archive_StExt(zCArchiver& ar);

void oCNpc::Unarchive_StExt(zCArchiver& ar);

void oCNpc::RemoveItemEffects_StExt(oCItem* item);

void oCNpc::AddItemEffects_StExt(oCItem* item);

void oCNpc::ProcessNpc_StExt();

void oCNpc::UnequipItem_StExt(oCItem* item);

void oCNpc::EquipArmor_StExt(oCItem* item);

int oCNpc::CanUse_StExt(oCItem* item);


// EXTENDED ARMORS

bool oCNpc::CanEquipAdditionalArmor(oCItem* item);

Array<oCItem*> oCNpc::GetEquippedArmors();

TNpcSlot* oCNpc::CreateAdditionalItemSlot(const zSTRING& name);

void oCNpc::EquipAdditionalArmorItem(oCItem* item);

void oCNpc::UnequipAdditionalArmorItem(int wear);

void oCNpc::PutAdditionalArmorItemToSlot(oCItem* item);

void oCNpc::RemoveAdditionalArmorItemFromSlot(oCItem* item);

//void oCNpc::InitModel_StExt();

#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	struct DamageMeta
	{
		oCNpc* Target;
		oCNpc* Attacker;
		oCItem* Weapon;

		oCNpc* TargetBefore;
		oCNpc* AttackerBefore;
		oCItem* WeaponBefore;
		oCNpc::oSDamageDescriptor* Desc;

		bool IsExtraDamage;
		bool IsDotDamage;
		bool IsAoeDamage;
		bool IsReflectDamage;
		bool IsAbility;
		bool IsOverlayDamage;
		bool IsInitialDamage;
		bool TargetIsImmortal;

		DamageInfo DamageInfo;
	};

	struct ExtraDamageParams
	{
		bool HasPendingExtraParams;
		int DamageType;
		int DamageFlags;
	};

	ExtraDamageInfo ExtraDamage;
	ExtraDamageInfo DotDamage;
	ExtraDamageInfo ReflectDamage;

	ExtraDamageParams DamageExtraParams;
	Array<DamageMeta> DamageMetaData;

	int MaxSpellId;
	int StExt_AbilityPrefix;
	StringMap<int> SpellFxNames;

	
#if DebugEnabled 
	#define DEBUG_MSG_DAMDESC(desc, msg, target) PrintDamageDescriptorDebug(desc, msg, target)
#else
	#define DEBUG_MSG_DAMDESC(desc, msg, target) ((void)0)
#endif

	//-----------------------------------------------------------------
	//						UTIL FUNCTIONS
	//-----------------------------------------------------------------

	inline void PrintDamageDescriptorDebug(oCNpc::oSDamageDescriptor& desc, zSTRING msg, oCNpc* target)
	{
		DEBUG_MSG("");
		DEBUG_MSG("-------------------------------------------------------------");
		DEBUG_MSG_DAM("PrintDamageDescriptorDebug", msg, desc.pNpcAttacker, target);
		DEBUG_MSG("DescriptorFlags: " + Z(desc.dwFieldsValid) + " | DamageFlags: " + Z(desc.enuModeDamage) + " | WeaponFlags: " + Z(desc.enuModeWeapon));
		DEBUG_MSG("DamageReal: " + Z((int)desc.fDamageTotal) + " | DamageTotal: " + Z((int)desc.fDamageReal) + " | DamageEffective: " + Z((int)desc.fDamageEffective));

		bool bFinished = desc.bFinished ? true : false;
		bool bIsDead = desc.bIsDead ? true : false;
		bool bIsUnconscious = desc.bIsUnconscious ? true : false;
		DEBUG_MSG("IsFinished: " + Z((int)bFinished) + " | IsDead: " + Z((int)bIsDead) + " | IsUnconscious: " + Z((int)bIsUnconscious));

		zSTRING damage = "Damage[] = ";
		zSTRING effectiveDamage = Z"EffectiveDamage[] = ";
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			damage += Z desc.aryDamage[i] + Z" | ";
			effectiveDamage += Z desc.aryDamageEffective[i] + Z" | ";
		}
		DEBUG_MSG(damage);
		DEBUG_MSG(effectiveDamage);
		if (desc.pFXHit)
			DEBUG_MSG(Z"FxName: " + desc.pFXHit->fxName + " | SpellId: " + Z((int)desc.nSpellID) + " | SpellLevel: " + Z((int)desc.nSpellLevel) + " | SpellCat: " + Z((int)desc.nSpellCat));
		DEBUG_MSG("-------------------------------------------------------------");
		DEBUG_MSG("");
	}

	template<typename D>
	inline void ApplyDamagesGeneric(const ulong type, D* damage, int& total)
	{
		if (total <= 0)
		{
			DEBUG_MSG("ApplyDamagesGeneric - total damage is less then 0!");
			return;
		}

		float damTypesCount = 0.0f;
		for (ulong i = 0; i < oEDamageIndex_MAX; ++i) {
			if (type & (1 << i))
				++damTypesCount; 
		}
		if ((type == 0UL) || (damTypesCount <= 0.5f)) { damage[dam_index_barrier] = static_cast<D>(total); return; }
		
		float dam = (float)total / (damTypesCount + 0.5f);
		if (dam < 1.0f) dam = 1.0f;
		for (ulong i = 0; i < oEDamageIndex_MAX; ++i) 
			damage[i] = type & (1 << i) ? static_cast<D>(dam) : D{};
	}
	void ApplyDamages(ulong type, ulong* damage, int& total) { ApplyDamagesGeneric(type, damage, total); }
	void ApplyDamages(int type, int* damage, int& total) { ApplyDamagesGeneric(static_cast<ulong>(type), damage, total); }

	inline int GetFxSpellId(zSTRING& fxName)
	{
		int result = Invalid;
		if (SpellFxNames.IsEmpty() || fxName.IsEmpty())
		{
			DEBUG_MSG_IF(SpellFxNames.IsEmpty(), "GetFxSpellId - SpellFxNames is null!");
			DEBUG_MSG_IF(fxName.IsEmpty(), "GetFxSpellId - fxName is empty!");
			return result;
		}

		auto pair = SpellFxNames.Find(fxName);
		if (pair)
			result = *pair;
		return result;
	}

	inline int GetFxTrueId(oCNpc::oSDamageDescriptor* desc, bool& isAbility)
	{
		if (!desc || !desc->pFXHit) return Invalid;

		int spellId = static_cast<int>(desc->nSpellID);
		if (spellId <= 0)
			spellId = GetFxSpellId(desc->pFXHit->fxName.Upper());

		if ((desc->nSpellCat == 0UL) && (desc->nSpellID == 0UL) && (spellId > 0))
		{
			isAbility = true;
			return spellId + StExt_AbilityPrefix;
		}
		return spellId;
	}

	inline void SetScriptDamageActors(oCNpc* atk, oCNpc* target, oCItem* weap)
	{
		parser->SetInstance(StExt_TargetNpc_SymId, target);
		parser->SetInstance(StExt_AttackNpc_SymId, atk);
		parser->SetInstance(StExt_AttackWeapon_SymId, weap);
	}


	inline void BuildDescriptor(oCNpc::oSDamageDescriptor& desc)
	{
		desc.bOnce = 1;
		desc.bFinished = 0;
		desc.bIsDead = 0;
		desc.bIsUnconscious = 0;
		desc.bDamageDontKill = 0;

		for (int i = 0; i < oEDamageIndex_MAX; ++i) {
			desc.aryDamage[i] = 0UL;
			desc.aryDamageEffective[i] = 0UL;
		}

		desc.nSpellID = 0UL;
		desc.nSpellCat = 0UL;
		desc.nSpellLevel = 0UL;

		desc.fTimeDuration = 0.0f;
		desc.fTimeInterval = 0.0f;
		desc.fDamagePerInterval = 0.0f;

		desc.fAzimuth = 0.0f;
		desc.fElevation = 0.0f;
		desc.fTimeCurrent = 0.0f;
		desc.vecLocationHit = zVEC3();
		desc.vecDirectionFly = zVEC3();

		desc.pVobParticleFX = Null;
		desc.pParticleFX = Null;
		desc.pVisualFX = Null;
		desc.pFXHit = Null;
		desc.pItemWeapon = Null;
		desc.strVisualFX = zSTRING();
	}

	inline void BuildDescriptor(oCNpc::oSDamageDescriptor& desc, oCNpc* atk, oCNpc* target, const ExtraDamageInfo& damStruct)
	{
		BuildDescriptor(desc);

		desc.dwFieldsValid = oCNpc::oEDamageDescFlag_Attacker | oCNpc::oEDamageDescFlag_Npc | 
			oCNpc::oEDamageDescFlag_Inflictor | oCNpc::oEDamageDescFlag_DamageType | oCNpc::oEDamageDescFlag_Damage;

		desc.enuModeDamage = 0UL;
		desc.enuModeWeapon = oETypeWeapon_Special;

		desc.pNpcAttacker = atk;
		desc.pVobAttacker = atk;
		desc.pVobHit = target;

		int totalDamage = 0;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			desc.aryDamage[i] = 0UL;
			desc.aryDamageEffective[i] = 0UL;
			if (damStruct.Damage[i] <= 0) continue;

			totalDamage += damStruct.Damage[i];
			desc.aryDamage[i] = static_cast<ulong>(damStruct.Damage[i]);
			desc.enuModeDamage |= static_cast<ulong>(1 << i);
		}

		desc.fDamageTotal = static_cast<float>(totalDamage);
		desc.fDamageReal = static_cast<float>(totalDamage);
		desc.fDamageEffective = static_cast<float>(totalDamage);
		desc.fDamageMultiplier = 1.0f;

		if (target)
		{
			desc.vecDirectionFly = target->GetPositionWorld();
			desc.vecLocationHit = target->GetPositionWorld();
		}
	}

	inline void BuildDescriptor(oCNpc::oSDamageDescriptor& desc, oCNpc* atk, oCNpc* target, int damType, int damTotal)
	{
		BuildDescriptor(desc);

		desc.dwFieldsValid = oCNpc::oEDamageDescFlag_Attacker | oCNpc::oEDamageDescFlag_Npc |
			oCNpc::oEDamageDescFlag_Inflictor | oCNpc::oEDamageDescFlag_DamageType | oCNpc::oEDamageDescFlag_Damage;

		desc.enuModeWeapon = oETypeWeapon_Special;
		desc.pNpcAttacker = atk;
		desc.pVobAttacker = atk;
		desc.pVobHit = target;

		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			desc.aryDamage[i] = 0UL;
			desc.aryDamageEffective[i] = 0UL;
		}
		desc.enuModeDamage = static_cast<ulong>(damType);
		ApplyDamages(desc.enuModeDamage, desc.aryDamage, damTotal);

		desc.fDamageTotal = static_cast<float>(damTotal);
		desc.fDamageReal = static_cast<float>(damTotal);
		desc.fDamageEffective = static_cast<float>(damTotal);
		desc.fDamageMultiplier = 1.0f;

		if (target)
		{
			desc.vecDirectionFly = target->GetPositionWorld();
			desc.vecLocationHit = target->GetPositionWorld();
		}
	}


	inline void UpdateDamageInfo(DamageInfo& damageInfo, oCNpc::oSDamageDescriptor& desc)
	{
		int total = 0;
		int real = 0;
		ulong damageEnum = 0;

		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			int baseDam = static_cast<int>(desc.aryDamage[i]);
			int effDam = static_cast<int>(desc.aryDamageEffective[i]);
			int finalDam = (effDam > 0) ? effDam : 
				(baseDam > 0) ? baseDam : 0;

			damageInfo.Damage[i] = finalDam;
			if (finalDam > 0) damageEnum |= (1 << i);
			total += baseDam;
			real += finalDam;
		}

		damageInfo.TotalDamage = static_cast<int>(desc.fDamageTotal > 0.0f ? desc.fDamageTotal : total);
		damageInfo.RealDamage = static_cast<int>(desc.fDamageReal > 0.0f ? desc.fDamageReal : real);
		if (damageInfo.TotalDamage <= 0) damageInfo.TotalDamage = 0;
		if (damageInfo.RealDamage <= 0) damageInfo.RealDamage = 0;

		damageInfo.DamageEnum = static_cast<int>(damageEnum);
		damageInfo.WeaponEnum = static_cast<int>(desc.enuModeWeapon);

		bool isAbility = false;
		damageInfo.SpellId = desc.pFXHit ? GetFxTrueId(&desc, isAbility) : static_cast<int>(desc.nSpellID);
		if (isAbility) damageInfo.DamageFlags |= StExt_DamageType_Ability;
	}
	inline void BuildDamageInfo(DamageInfo& damageInfo, oCNpc::oSDamageDescriptor& desc)
	{
		memset(&damageInfo, 0, sizeof(DamageInfo));
		damageInfo.DamageType = 0;
		damageInfo.DamageFlags = 0;
		damageInfo.BlockDamage = 0;
		damageInfo.StopProcess = false;
		damageInfo.IsInitial = false;
		UpdateDamageInfo(damageInfo, desc);
	}

	inline void CreateIncomingDamage(IncomingDamageInfo& damageInfo, DamageMeta* damageMeta)
	{
		damageInfo = IncomingDamageInfo{};

		damageInfo.Flags = 0;
		damageInfo.DamageType = damageMeta->DamageInfo.DamageType;
		damageInfo.DamageFlags = damageMeta->DamageInfo.DamageFlags;
		damageInfo.SpellId = damageMeta->DamageInfo.SpellId;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
			damageInfo.Damage[i] = damageMeta->DamageInfo.Damage[i];
		damageInfo.DamageTotal = damageMeta->DamageInfo.RealDamage;
		
		if (damageMeta->IsDotDamage) damageInfo.Flags |= StExt_IncomingDamageFlag_Index_DotDamage;
		if (damageMeta->IsReflectDamage) damageInfo.Flags |= StExt_IncomingDamageFlag_Index_ReflectDamage;
		if (damageMeta->IsExtraDamage) damageInfo.Flags |= StExt_IncomingDamageFlag_Index_ExtraDamage;
		if (damageMeta->IsAoeDamage) damageInfo.Flags |= StExt_IncomingDamageFlag_Index_AoeDamage;
		if (damageMeta->Attacker) damageInfo.Flags |= StExt_IncomingDamageFlag_Index_HasAttacker;
		if (damageMeta->Weapon) damageInfo.Flags |= StExt_IncomingDamageFlag_Index_HasWeapon;
	}
	inline void CreateIncomingDamage(IncomingDamageInfo& damageInfo, const int damage)
	{
		damageInfo = IncomingDamageInfo{};
		damageInfo.Flags = StExt_IncomingDamageFlag_Index_Contextual;
		damageInfo.DamageType = 0;
		damageInfo.DamageFlags = 0;
		damageInfo.SpellId = 0;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
			damageInfo.Damage[i] = 0;

		damageInfo.Damage[0] = damage;
		damageInfo.DamageTotal = damage;
	}


	inline DamageMeta& PushDamageMeta(oCNpc* target, oCNpc::oSDamageDescriptor& desc)
	{
		DamageMeta damageMeta = DamageMeta{};
		damageMeta.TargetBefore = dynamic_cast<oCNpc*>((zCVob*)parser->GetSymbol(StExt_TargetNpc_SymId)->GetInstanceAdr());
		damageMeta.AttackerBefore = dynamic_cast<oCNpc*>((zCVob*)parser->GetSymbol(StExt_AttackNpc_SymId)->GetInstanceAdr());
		damageMeta.WeaponBefore = dynamic_cast<oCItem*>((zCVob*)parser->GetSymbol(StExt_AttackWeapon_SymId)->GetInstanceAdr());

		SetScriptDamageActors(desc.pNpcAttacker, target, desc.pItemWeapon);

		damageMeta.Target = target;
		damageMeta.Attacker = desc.pNpcAttacker;
		damageMeta.Weapon = desc.pItemWeapon;
		damageMeta.Desc = &desc;
		const ulong descriptorFlags = desc.dwFieldsValid;

		damageMeta.IsExtraDamage = HasFlag(descriptorFlags, (ulong)DamageDescFlag_ExtraDamage);
		damageMeta.IsDotDamage = HasFlag(descriptorFlags, (ulong)DamageDescFlag_DotDamage);
		damageMeta.IsAoeDamage = HasFlag(descriptorFlags, (ulong)DamageDescFlag_AoeDamage);
		damageMeta.IsReflectDamage = HasFlag(descriptorFlags, (ulong)DamageDescFlag_ReflectDamage);
		damageMeta.IsInitialDamage = !damageMeta.IsExtraDamage && !damageMeta.IsDotDamage && !damageMeta.IsAoeDamage && !damageMeta.IsReflectDamage;
		damageMeta.IsOverlayDamage = HasFlag(descriptorFlags, (ulong)oCNpc::oEDamageDescFlag_OverlayActivate) || HasFlag(descriptorFlags, (ulong)oCNpc::oEDamageDescFlag_OverlayInterval) ||
			HasFlag(descriptorFlags, (ulong)oCNpc::oEDamageDescFlag_OverlayDuration) || HasFlag(descriptorFlags, (ulong)oCNpc::oEDamageDescFlag_OverlayDamage);
		damageMeta.TargetIsImmortal = *(int*)parser->CallFunc(IsNpcImmortalFunc);
		damageMeta.IsAbility = false;

		BuildDamageInfo(damageMeta.DamageInfo, desc);
		damageMeta.DamageInfo.SpellId = desc.pFXHit ? GetFxTrueId(&desc, damageMeta.IsAbility) : static_cast<int>(desc.nSpellID);
		damageMeta.DamageInfo.IsInitial = damageMeta.IsInitialDamage;

		if (DamageExtraParams.HasPendingExtraParams)
		{
			damageMeta.DamageInfo.DamageType |= DamageExtraParams.DamageType;
			damageMeta.DamageInfo.DamageFlags |= DamageExtraParams.DamageFlags;
			DamageExtraParams.HasPendingExtraParams = false;
		}

		DEBUG_MSG("PushDamageMeta: damage meta was PUSHED to stack! Stack level: " + Z((int)DamageMetaData.GetNum() + 1));
		DamageMeta& ref = DamageMetaData.InsertEnd(damageMeta);
		parser->SetInstance(StExt_DamageInfo_SymId, &ref.DamageInfo);
		return ref;
	}

	inline void PopDamageMeta()
	{
		if (DamageMetaData.IsEmpty())
		{
			DEBUG_MSG("PopDamageMeta: Stack is empty!");
			SetScriptDamageActors(Null, Null, Null);
			parser->SetInstance(StExt_DamageInfo_SymId, Null);
			return;
		}

		DamageMeta& damageMeta = DamageMetaData.GetLast();
		SetScriptDamageActors(damageMeta.AttackerBefore, damageMeta.TargetBefore, damageMeta.WeaponBefore);
		DamageMetaData.RemoveAt(DamageMetaData.GetNum() - 1U);

		if (!DamageMetaData.IsEmpty())
			parser->SetInstance(StExt_DamageInfo_SymId, &DamageMetaData.GetLast().DamageInfo);
		else
			parser->SetInstance(StExt_DamageInfo_SymId, Null);
		DEBUG_MSG("PopDamageMeta: damage meta was POPED from stack! Stack level: " + Z((int)DamageMetaData.GetNum()));
	}

	inline DamageMeta* GetDamageMeta()
	{
		if (DamageMetaData.IsEmpty())
		{
			DEBUG_MSG("GetDamageMeta: Stack is empty!");
			return Null;
		}
		return &DamageMetaData.GetLast();
	}

	inline void ClearDamageMeta()
	{
		DEBUG_MSG_IF(DamageMetaData.GetNum() > 0, "ClearDamageMeta: there is a damageMeta left in stack!");
		DamageExtraParams.HasPendingExtraParams = false;
		DamageMetaData.Clear();
	}


	//-----------------------------------------------------------------
	//					DAMAGE PROCESSING FUNCTIONS
	//-----------------------------------------------------------------

	inline void ApplyExtraDamageGeneric(oCNpc* atk, oCNpc* target, ExtraDamageInfo& extraDam, const ulong flags)
	{
		DEBUG_MSG("ApplyExtraDamageGeneric: ENTER!");
		oCNpc* pTarget = zDYNAMIC_CAST<oCNpc>(target);
		oCNpc* pAttaker = zDYNAMIC_CAST<oCNpc>(atk);
		if (!pAttaker || !pTarget)
		{
			DEBUG_MSG_IF(!pAttaker, "ApplyExtraDamageGeneric: Attaker is null!");
			DEBUG_MSG_IF(!pTarget, "ApplyExtraDamageGeneric: Target is null!");
			return;
		}

		oCNpc::oSDamageDescriptor desc;
		BuildDescriptor(desc, pAttaker, pTarget, extraDam);
		desc.dwFieldsValid |= flags;
		if(HasFlag(extraDam.DamageFlags, StExt_DamageFlag_Aoe) || HasFlag(extraDam.DamageFlags, StExt_DamageFlag_Chain)) desc.dwFieldsValid |= DamageDescFlag_AoeDamage;

		DEBUG_MSG_DAMDESC(desc, "ApplyExtraDamageGeneric", pTarget);
		if (desc.fDamageTotal < 1.0f) {
			DEBUG_MSG("ApplyExtraDamageGeneric: total damage is less than 0! Skipped!");
			return;
		}

		DEBUG_MSG_IF(DamageExtraParams.HasPendingExtraParams, "ApplyExtraDamageGeneric: DamageExtraParams wasn't readed before.");
		DamageExtraParams.HasPendingExtraParams = true;
		DamageExtraParams.DamageType = extraDam.DamageType;
		DamageExtraParams.DamageFlags = extraDam.DamageFlags;

		pTarget->OnDamage(desc);
	}	

	void ApplyExtraDamage(oCNpc* atk, oCNpc* target) { ApplyExtraDamageGeneric(atk, target, ExtraDamage, DamageDescFlag_ExtraDamage); }
	void ApplyDotDamage(oCNpc* atk, oCNpc* target) { ApplyExtraDamageGeneric(atk, target, DotDamage, DamageDescFlag_DotDamage); }
	void ApplyReflectDamage(oCNpc* atk, oCNpc* target) { ApplyExtraDamageGeneric(atk, target, ReflectDamage, DamageDescFlag_ReflectDamage); }

	void ApplySingleDamage(oCNpc* atk, oCNpc* target, const int damType, const int damTotal, const int damageType, const int damageFlags)
	{
		DEBUG_MSG("ApplySingleDamage: ENTER!");
		oCNpc* pTarget = zDYNAMIC_CAST<oCNpc>(target);
		oCNpc* pAttaker = zDYNAMIC_CAST<oCNpc>(atk);
		if (!pAttaker || !pTarget)
		{
			DEBUG_MSG_IF(!pAttaker, "ApplySingleDamage: Attaker is null!");
			DEBUG_MSG_IF(!pTarget, "ApplySingleDamage: Target is null!");
			return;
		}

		oCNpc::oSDamageDescriptor desc;
		BuildDescriptor(desc, atk, target, damType, damTotal);
		desc.dwFieldsValid |= DamageDescFlag_ExtraDamage;
		if (HasFlag(damageFlags, StExt_DamageFlag_Aoe) || HasFlag(damageFlags, StExt_DamageFlag_Chain)) desc.dwFieldsValid |= DamageDescFlag_AoeDamage;

		DEBUG_MSG_DAMDESC(desc, "ApplySingleDamage", pTarget);
		if (desc.fDamageTotal < 1.0f) {
			DEBUG_MSG("ApplySingleDamage: total damage is less than 0! Skipped!");
			return;
		}

		DEBUG_MSG_IF(DamageExtraParams.HasPendingExtraParams, "ApplyExtraDamageGeneric: DamageExtraParams wasn't readed before.");
		DamageExtraParams.HasPendingExtraParams = true;
		DamageExtraParams.DamageType = damageType;
		DamageExtraParams.DamageFlags = damageFlags;

		pTarget->OnDamage(desc);
	}


	void ProcessExtraDamage(oCNpc::oSDamageDescriptor& desc, oCNpc* target)
	{
		DEBUG_MSG_DAM("ProcessExtraDamage", "ENTER", desc.pNpcAttacker, target);

		if (!target) {
			DEBUG_MSG_FUNC("ProcessExtraDamage", "Target is null!");
			desc.bFinished = 1;
			return;
		}

		DamageMeta* currentDamageMeta = GetDamageMeta();
		DEBUG_MSG_IF(!currentDamageMeta, "ProcessExtraDamage: fail to get currentDamageMeta!");
		DEBUG_MSG_IF(currentDamageMeta && currentDamageMeta->Target != target, "ProcessExtraDamage: use DamageStackMeta for incorrect character!");

		int damageTotal = 0;
		int damageReal = 0;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			int dam = static_cast<int>(desc.aryDamage[i]);
			if (dam <= 0) continue;

			damageTotal += dam;
			if (target->protection[i] < 0) continue;

			dam = dam - target->protection[i];
			dam = dam <= 0 ? 1 : dam;
			damageReal += dam;
			desc.aryDamageEffective[i] = static_cast<ulong>(dam);
		}

		if (damageReal <= 0)
		{
			DEBUG_MSG_FUNC("ProcessExtraDamage", "EXIT. RealDamage: " + Z(damageReal) + "!");
			desc.bFinished = 1;
			return;
		}
		desc.fDamageTotal = static_cast<float>(damageTotal);
		desc.fDamageEffective = desc.fDamageReal = static_cast<float>(damageReal);

		if(currentDamageMeta)
			UpdateDamageInfo(currentDamageMeta->DamageInfo, desc);
		target->ChangeAttribute(NPC_ATR_HITPOINTS, -damageReal);

		int damMsgType = StExt_DamageMessageType_Default;
		if (HasFlag(desc.dwFieldsValid, (ulong)DamageDescFlag_DotDamage)) damMsgType |= StExt_DamageMessageType_Dot;
		if (HasFlag(desc.dwFieldsValid, (ulong)DamageDescFlag_ReflectDamage)) damMsgType |= StExt_DamageMessageType_Reflect;
		parser->CallFunc(PrintDamageFunc, damageReal, damMsgType);

		if (target->IsHuman() && (target->attribute[0] <= 1) && !target->IsUnconscious())
		{
			desc.bIsUnconscious = 1;
			target->DropAllInHand();
			target->SetWeaponMode(NPC_WEAPON_NONE);
			if (target->attribute[0] <= 0)
				target->SetAttribute(NPC_ATR_HITPOINTS, 1);
			target->SetBodyState(BS_UNCONSCIOUS);
			target->state.StartAIState(-4, FALSE, 0, 0, FALSE);
			target->AssessDefeat_S(desc.pNpcAttacker);
		}
		else if (target->attribute[0] <= 0)
		{
			desc.bIsDead = 1;
			target->DoDie(desc.pNpcAttacker);
		}

		if (!desc.bIsDead && !desc.bIsUnconscious)
			parser->CallFunc(OnPostDamageFunc);

		DEBUG_MSG_DAM("ProcessExtraDamage", "EXIT. RealDamage: " + Z(damageReal), desc.pNpcAttacker, target);
	}
	

	HOOK Hook_oCNpc_OnDamage PATCH (&oCNpc::OnDamage, &oCNpc::OnDamage_StExt);
	void oCNpc::OnDamage_StExt(oSDamageDescriptor& desc)
	{
		if (IsLevelChanging || IsLoading || !this) { return; }
		DEBUG_MSG_DAM("OnDamage_StExt", "ENTER", desc.pNpcAttacker, this);
		DEBUG_MSG_DAMDESC(desc, "OnDamage_StExt - ENTER", this);

		DamageMeta& damageMeta = PushDamageMeta(this, desc);

		// fix looped dot
		if (damageMeta.IsOverlayDamage && (desc.fDamageTotal < 1.0f))
		{
			DEBUG_MSG_DAM("OnDamage_StExt", "EXIT. Reason: looped dot detected!", desc.pNpcAttacker, this);
			PopDamageMeta();
			desc.bFinished = 1;
			return;
		}

		if (desc.pFXHit)
		{
			int applyFxDamage = *(int*)parser->CallFunc(FxDamageCanBeAppliedFunc);
			if (!applyFxDamage)
			{
				DEBUG_MSG_DAM("OnDamage_StExt", "EXIT. Reason: pFxHit is blocked (friendly fier)", desc.pNpcAttacker, this);
				PopDamageMeta();
				desc.bFinished = 1;
				return;
			}
		}

		// Incoming damage is from my mod. Handle it separately and leave.
		if (!damageMeta.IsInitialDamage)
		{
			ProcessExtraDamage(desc, this);
			DEBUG_MSG_DAM("OnDamage_StExt", "EXIT. Reason: Extra damage handled.", desc.pNpcAttacker, this);
			PopDamageMeta();
			desc.bFinished = 1;
			return;
		}

		// Call script damage pre-process
		if (damageMeta.Attacker)
		{
			parser->CallFunc(OnDamageBeginFunc);
			if ((damageMeta.DamageInfo.BlockDamage > 0) || damageMeta.DamageInfo.StopProcess)
			{
				DEBUG_MSG_DAM("OnDamage_StExt", "EXIT. Reason: damage was blocked.", desc.pNpcAttacker, this);
				PopDamageMeta();
				desc.bFinished = 1;
				return;
			}
		}

		// Original damage handler
		DEBUG_MSG_DAM("OnDamage", "ENTER.", desc.pNpcAttacker, this);
		THISCALL(Hook_oCNpc_OnDamage)(desc);

		UpdateDamageInfo(damageMeta.DamageInfo, desc);
		SetScriptDamageActors(damageMeta.Attacker, damageMeta.Target, damageMeta.Weapon);
		parser->SetInstance(StExt_DamageInfo_SymId, &damageMeta.DamageInfo);

		DEBUG_MSG_DAM("OnDamage", "EXIT.", desc.pNpcAttacker, this);
		DEBUG_MSG_DAMDESC(desc, "OnDamage AFTER original OnDamage", this);		

		// Call script damage post-process
		if (damageMeta.Attacker)
		{
			parser->CallFunc(OnPostDamageFunc);
			parser->CallFunc(OnDamageAfterFunc);
		}

		PopDamageMeta();
		DEBUG_MSG_DAMDESC(desc, "OnDamage_StExt - EXIT", this);
		DEBUG_MSG_DAM("OnDamage_StExt", "EXIT.", desc.pNpcAttacker, this);
	}


	HOOK ivk_oCNpc_ChangeAttribute PATCH(&oCNpc::ChangeAttribute, &oCNpc::ChangeAttribute_StExt);
	void oCNpc::ChangeAttribute_StExt(int attrIndex, int value)
	{
		if ((attrIndex == NPC_ATR_HITPOINTS) && (value < 0))
		{
			int damage = value * (-1);
			oCNpc* attaker = Null;
			oCItem* weapon = Null;

			oCNpc* oldTarget = dynamic_cast<oCNpc*>((zCVob*)parser->GetSymbol(StExt_TargetNpc_SymId)->GetInstanceAdr());
			oCNpc* oldAttacker = dynamic_cast<oCNpc*>((zCVob*)parser->GetSymbol(StExt_AttackNpc_SymId)->GetInstanceAdr());
			oCItem* oldWeap = dynamic_cast<oCItem*>((zCVob*)parser->GetSymbol(StExt_AttackWeapon_SymId)->GetInstanceAdr());

			DamageMeta* currentDamageMeta = GetDamageMeta();
			IncomingDamageInfo currentDamageInfo = IncomingDamageInfo{};
			
			if (currentDamageMeta && currentDamageMeta->Target == this)
			{
				attaker = currentDamageMeta->Attacker;
				weapon = currentDamageMeta->Weapon;
				UpdateDamageInfo(currentDamageMeta->DamageInfo, *currentDamageMeta->Desc);
				CreateIncomingDamage(currentDamageInfo, currentDamageMeta);
			}
			else
			{
				DEBUG_MSG_IF(currentDamageMeta && currentDamageMeta->Target != this, "ChangeAttribute_StExt: use DamageStackMeta for incorrect character!");
				DEBUG_MSG("ChangeAttribute_StExt: create IncomingDamageInfo...");
				CreateIncomingDamage(currentDamageInfo, damage);
			}

			if (currentDamageInfo.DamageTotal != damage)
			{
				DEBUG_MSG("ChangeAttribute_StExt: actual damage and IncomingDamageInfo damages was different! Was: " + Z(currentDamageInfo.DamageTotal));
				currentDamageInfo.DamageTotal = damage;
			}
			if (attaker) currentDamageInfo.Flags |= StExt_IncomingDamageFlag_Index_HasAttacker;
			if (weapon) currentDamageInfo.Flags |= StExt_IncomingDamageFlag_Index_HasWeapon;

			DEBUG_MSG_DAM("ChangeAttribute_StExt", "ENTER. InitialHp: " + Z(this->attribute[0]) + " | Damage: " + Z(damage), attaker, this);

			SetScriptDamageActors(attaker, this, weapon);
			parser->SetInstance(StExt_IcomingDamageInfo_SymId, &currentDamageInfo);
			value += *(int*)parser->CallFunc(ProcessHpDamageFunc);

			if (value >= 0) 
			{
				DEBUG_MSG_FUNC("ChangeAttribute_StExt", "Damage to '" + this->name[0] + "' was absorbed! (value now: " + Z(value) + ")");
				SetScriptDamageActors(oldAttacker, oldTarget, oldWeap);
				parser->SetInstance(StExt_IcomingDamageInfo_SymId, Null);
				return;
			}

			damage = value >= 0 ? 0 : value * (-1);
			DEBUG_MSG_FUNC("ChangeAttribute_StExt", "Apply " + Z(damage) + " damage to '" + this->name[0] + "' ...");

			const bool isKill = ((this->attribute[0] + value) <= 0);
			const bool isExtraDamage = HasFlag(currentDamageInfo.Flags, StExt_IncomingDamageFlag_Index_ExtraDamage);
			const bool isExtraDamageDontKill = HasFlag(currentDamageInfo.Flags, StExt_IncomingDamageFlag_Index_DontKill);
			const int preserveHp = isKill && (isExtraDamage && isExtraDamageDontKill) ? -1 : 0;
			value = (damage > (this->attribute[0] + preserveHp)) ? -(this->attribute[0] + preserveHp) : value;

			DEBUG_MSG_FUNC("ChangeAttribute", "ENTER");
			THISCALL(ivk_oCNpc_ChangeAttribute)(attrIndex, value);
			DEBUG_MSG_FUNC("ChangeAttribute", "EXIT");

			SetScriptDamageActors(oldAttacker, oldTarget, oldWeap);
			parser->SetInstance(StExt_IcomingDamageInfo_SymId, Null);
			return;
		}
		THISCALL(ivk_oCNpc_ChangeAttribute)(attrIndex, value);
	}
}
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	ExtraDamageInfo ExtraDamage;
	DotDamageInfo DotDamage;
	ExtraDamageInfo ReflectDamage;
	IncomingDamageInfo IncomingDamage;

	StringMap<int> SpellFxNames;
	
	int MaxSpellId;
	int StExt_AbilityPrefix;
	oCNpc::oSDamageDescriptor* CurrentDescriptor = Null;
	DamageMetaData* DamageMeta = Null;

	inline void SetDamageMeta(oCNpc::oSDamageDescriptor* desc, DamageMetaData* meta, oCNpc* target)
	{
		CurrentDescriptor = desc;
		DamageMeta = meta;
		if (CurrentDescriptor)
		{
			parser->SetInstance("STEXT_TARGETNPC", target);
			parser->SetInstance("STEXT_ATTACKNPC", CurrentDescriptor->pNpcAttacker);
			parser->SetInstance("STEXT_ATTACKWEAPON", CurrentDescriptor->pItemWeapon);
		}
		else
		{
			IncomingDamage = IncomingDamageInfo();
			memset(&IncomingDamage, 0, sizeof(IncomingDamageInfo));
			IncomingDamage.ScriptInstance.Processed = true;

			parser->SetInstance("STEXT_INCOMINGDAMAGEINFO", Null);
			parser->SetInstance("STEXT_TARGETNPC", Null);
			parser->SetInstance("STEXT_ATTACKNPC", Null);
			parser->SetInstance("STEXT_ATTACKWEAPON", Null);
		}
	}
	
	void ClearDamageMeta() { SetDamageMeta(Null, Null, Null); }

	void ApplyDamages(ULONG type, ULONG* damage, int& total)
	{
		float dam = 0.0f;
		unsigned long mask = type;
		for (int i = 0; i < oEDamageIndex_MAX; ++i) 
		{
			if (mask & 1) {
				dam += 1.0f;
			}
			mask >>= 1;
		}

		if (total <= 0) total = 5;
		if (dam < 1.0f) 
		{ 
			damage[dam_index_barrier] = total;
			return; 
		}

		if (dam < 5.0f) dam = 5.0f;
		dam = total / dam + 0.5f;
		for (int i = 0; i < oEDamageIndex_MAX; i++)
			damage[i] = type & (1 << i) ? static_cast<unsigned long>(dam) : 0UL;
	}
	void ApplyDamages(int type, int* damage, int& total)
	{
		float dam = 0.0f;
		int mask = type;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			if (mask & 1) {
				dam += 1.0f;
			}
			mask >>= 1;
		}

		if (total <= 0) total = 5;
		if (dam < 1.0f)
		{
			damage[dam_index_barrier] = total;
			return;
		}

		dam = total / dam + 0.5f;
		if (dam < 5.0f) dam = 5.0f;
		for (int i = 0; i < oEDamageIndex_MAX; i++)
			damage[i] = type & (1 << i) ? static_cast<int>(dam) : 0;
	}

	void AddDamages(int type, int* damage, int& total)
	{
		float dam = 0.0f;
		int mask = type;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			if (mask & 1) {
				dam += 1.0f;
			}
			mask >>= 1;
		}

		if (total <= 0) total = 5;
		if (dam < 1.0)
		{
			damage[dam_index_barrier] = total;
			return;
		}

		dam = total / dam + 0.5f;
		for (int i = 0; i < dam_index_max; i++) {
			if (type & (1 << i)) {
				damage[i] = ValidateValueMin(damage[i] + static_cast<int>(dam), 0);
			}
		}			
	}

	inline void* GetDamageStruct(zSTRING structName)
	{
		zCPar_Symbol* psDam = parser->GetSymbol(structName);
		if (!psDam) return Null;
		return psDam->GetInstanceAdr();		
	}

	inline oCNpc::oSDamageDescriptor BuildDescriptor(oCNpc* atk, oCNpc* target, int* damage, int& totalDamage)
	{
		oCNpc::oSDamageDescriptor desc;
		memset(&desc, 0, sizeof oCNpc::oSDamageDescriptor);

		for (int i = 0; i < oEDamageIndex_MAX; i++)
		{
			if (damage[i] > 0)
			{
				totalDamage += damage[i];
				desc.aryDamage[i] = static_cast<unsigned long>(damage[i]);
				desc.enuModeDamage |= (1 << i);
			}
		}
		desc.fDamageTotal = static_cast<float>(totalDamage);
		desc.fDamageMultiplier = 1.0f;
		desc.vecLocationHit = target->GetPositionWorld();
		desc.pNpcAttacker = atk;
		desc.pVobAttacker = atk;
		desc.pVobHit = target;
		desc.bOnce = true;
		return desc;
#pragma region DescriptorFields
		/*
		desc.dwFieldsValid = 0;
		desc.pVobAttacker = Null;
		desc.pNpcAttacker = Null;
		desc.pVobHit = Null;
		desc.pFXHit = Null;
		desc.pItemWeapon = 0;
		desc.nSpellID = 0;
		desc.nSpellCat = 0;
		desc.nSpellLevel = 0;
		desc.enuModeDamage = 0;
		desc.enuModeWeapon = 0;
		for (int i = 0; i < 8; i++)
			desc.aryDamage[i] = 0;
		desc.fDamageTotal = 0;
		desc.fDamageMultiplier = 0;
		desc.vecLocationHit = zVEC3();
		desc.vecDirectionFly = zVEC3();
		desc.strVisualFX = "";
		desc.fTimeDuration = 0;
		desc.fTimeInterval = 0;
		desc.fDamagePerInterval = 0;
		desc.bDamageDontKill = 0;
		desc.bOnce = 0;
		desc.bFinished = 0;
		desc.bIsDead = 0;
		desc.bIsUnconscious = 0;
		desc.lReserved = 0;
		desc.fAzimuth = 0;
		desc.fElevation = 0;
		desc.fTimeCurrent = 0;
		desc.fDamageReal = 0;
		desc.fDamageEffective = 0;
		for (int i = 0; i < 8; i++)
			desc.aryDamageEffective[i] = 0;
		desc.pVobParticleFX = Null;
		desc.pParticleFX = Null;
		desc.pVisualFX = Null;
		*/
#pragma endregion	
	}

	void ApplyExtraDamage(oCNpc* atk, oCNpc* target)
	{
		oCNpc* pTarget = zDYNAMIC_CAST<oCNpc>(target);
		oCNpc* pAttaker = zDYNAMIC_CAST<oCNpc>(atk);
		if ((!pAttaker) || (!pTarget))
		{
			DEBUG_MSG("ApplyExtraDamage - some actor is null!");
			return;
		}
		DEBUG_MSG(pTarget->name + zSTRING(" ApplyExtraDamage - Create damage descriptor..."));

		void* damStructpointer = GetDamageStruct("StExt_ExtraDamageInfo");
		if (!damStructpointer)
		{
			DEBUG_MSG(pTarget->name + zSTRING(" ApplyExtraDamage - Create damage descriptor - damage struct symbol is not found!"));
			return;
		}
		ExtraDamage = *dynamic_cast<ExtraDamageInfo*>((ExtraDamageInfo*)damStructpointer);

		int totalDamage = 0;
		oCNpc::oSDamageDescriptor desc = BuildDescriptor(pAttaker, pTarget, ExtraDamage.Damage, totalDamage);
		desc.dwFieldsValid |= DamageDescFlag_ExtraDamage;

		if (totalDamage < 1)
		{
			DEBUG_MSG(pTarget->name + zSTRING(" ApplyExtraDamage - Damage is 0. Skipped!"));
			return;
		}

		DEBUG_MSG(pTarget->name + zSTRING(" ApplyExtraDamage - damage descriptor applied!"));
		pTarget->OnDamage(desc);
		return;
	}

	void ApplyDotDamage(oCNpc* atk, oCNpc* target)
	{		
		oCNpc* pTarget = zDYNAMIC_CAST<oCNpc>(target);
		oCNpc* pAttaker = zDYNAMIC_CAST<oCNpc>(atk);
		if ((!pAttaker) || (!pTarget))
		{
			DEBUG_MSG("ApplyDotDamage - some actor is null!");
			return;
		}
		DEBUG_MSG(pTarget->name + zSTRING(" ApplyDotDamage - Create damage descriptor..."));
		
		void* damStructpointer = GetDamageStruct("StExt_DotDamageInfo");
		if (!damStructpointer)
		{
			DEBUG_MSG(pTarget->name + zSTRING(" ApplyDotDamage - Create damage descriptor - damage struct symbol is not found!"));
			return;
		}
		DotDamage = *dynamic_cast<DotDamageInfo*>((DotDamageInfo*)damStructpointer);
		int totalDamage = 0;
		oCNpc::oSDamageDescriptor desc = BuildDescriptor(pAttaker, pTarget, DotDamage.Damage, totalDamage);
		desc.dwFieldsValid |= DamageDescFlag_ExtraDamage | DamageDescFlag_DotDamage;
		
		if (totalDamage < 1)
		{
			DEBUG_MSG(pTarget->name + zSTRING(" ApplyDotDamage - Damage is 0. Skipped!"));
			return;
		}

		DEBUG_MSG(pTarget->name + zSTRING(" ApplyDotDamage - damage descriptor applied!"));
		pTarget->OnDamage(desc);
		return;
	}

	void ApplyReflectDamage(oCNpc* atk, oCNpc* target)
	{
		oCNpc* pTarget = zDYNAMIC_CAST<oCNpc>(target);
		oCNpc* pAttaker = zDYNAMIC_CAST<oCNpc>(atk);
		if ((!pAttaker) || (!pTarget))
		{
			DEBUG_MSG("ApplyReflectDamage - some actor is null!");
			return;
		}
		DEBUG_MSG(pTarget->name + zSTRING(" ApplyReflectDamage - Create damage descriptor..."));

		void* damStructpointer = GetDamageStruct("StExt_ReflectDamageInfo");
		if (!damStructpointer)
		{
			DEBUG_MSG(pTarget->name + zSTRING(" ApplyReflectDamage - Create damage descriptor - damage struct symbol is not found!"));
			return;
		}
		ReflectDamage = *dynamic_cast<ExtraDamageInfo*>((ExtraDamageInfo*)damStructpointer);

		int totalDamage = 0;
		oCNpc::oSDamageDescriptor desc = BuildDescriptor(pAttaker, pTarget, ReflectDamage.Damage, totalDamage);
		desc.dwFieldsValid |= DamageDescFlag_ExtraDamage | DamageDescFlag_ReflectDamage;

		if (totalDamage < 1)
		{
			DEBUG_MSG(pTarget->name + zSTRING(" ApplyReflectDamage - Damage is 0. Skipped!"));
			return;
		}

		DEBUG_MSG(pTarget->name + zSTRING(" ApplyReflectDamage - damage descriptor applied!"));
		pTarget->OnDamage(desc);
		return;
	}

	inline DamageInfo BuildDamageInfo(oCNpc::oSDamageDescriptor& desc)
	{
		DamageInfo result = DamageInfo();
		result.SpellId = static_cast<int>(desc.nSpellID);
		int totalDamage = 0;
		for (int i = 0; i < oEDamageIndex_MAX; i++)
		{
			unsigned long dam = desc.aryDamage[i] >= desc.aryDamageEffective[i] ? desc.aryDamage[i] : desc.aryDamageEffective[i];
			result.Damage[i] = static_cast<int>(dam);
			result.DamageEffective[i] = static_cast<int>(desc.aryDamageEffective[i]);
			totalDamage += result.Damage[i];
			if (result.Damage[i] > 0)
				desc.enuModeDamage |= (1 << i);
		}
		result.TotalDamage = desc.fDamageTotal > 0.0f ? static_cast<int>(desc.fDamageTotal) : totalDamage;
		result.RealDamage = static_cast<int>(desc.fDamageReal);
		result.DamageEnum = static_cast<int>(desc.enuModeDamage);
		result.WeaponEnum = static_cast<int>(desc.enuModeWeapon);
		result.DamageType = 0;
		result.DamageFlags = 0;
		result.BlockDamage = 0;
		result.StopProcess = false;
		result.IsInitial = false;
		return result;
	}

	inline bool IsDamageInitial(unsigned long flags) 
	{ 
		return (!HasFlag(flags, (ulong)oCNpc::oEDamageDescFlag_OverlayActivate) && !HasFlag(flags, (ulong)oCNpc::oEDamageDescFlag_OverlayInterval) &&
		!HasFlag(flags, (ulong)oCNpc::oEDamageDescFlag_OverlayDuration) && !HasFlag(flags, (ulong)oCNpc::oEDamageDescFlag_OverlayDamage) &&
		!HasFlag(flags, (ulong)DamageDescFlag_ExtraDamage) && !HasFlag(flags, (ulong)DamageDescFlag_DotDamage) && !HasFlag(flags, (ulong)DamageDescFlag_ReflectDamage));
	}

	inline int GetFxSpellId(zSTRING& fxName)
	{
		int result = Invalid;
		if (SpellFxNames.IsEmpty())
		{
			DEBUG_MSG("GetFxSpellId - SpellFxNames is null!");
			return result;
		}
		if (fxName.IsEmpty())
		{
			DEBUG_MSG("GetFxSpellId - fxName is empty!");
			return result;
		}

		auto pair = SpellFxNames.Find(fxName);
		if (pair)
			result = *pair;
		return result;
	}

	void UpdateIncomingDamage(int damage, oCNpc* target)
	{
		IncomingDamage = IncomingDamageInfo();
		memset(&IncomingDamage, 0, sizeof(IncomingDamageInfo));

		IncomingDamage.Target = target;
		IncomingDamage.Attacker = Null;
		IncomingDamage.Desc = Null;
		IncomingDamage.Weapon = Null;
		IncomingDamage.ScriptInstance.DamageTotal = damage;
		IncomingDamage.ScriptInstance.Processed = false;
		IncomingDamage.ScriptInstance.Flags = 0;

		if (CurrentDescriptor && (!IsLevelChanging && !IsLoading))
		{
			IncomingDamage.Desc = CurrentDescriptor;
			IncomingDamage.Attacker = CurrentDescriptor->pNpcAttacker;
			IncomingDamage.Weapon = CurrentDescriptor->pItemWeapon;

			if (DamageMeta)
			{
				IncomingDamage.ScriptInstance.SpellId = DamageMeta->IsAbility ? DamageMeta->AbilityId : DamageMeta->SpellId;
				if(DamageMeta->IsExtraDamage)
					IncomingDamage.ScriptInstance.Flags |= StExt_IncomingDamageFlag_Index_ExtraDamage;
			}
			else
			{
				unsigned long descriptorFlags = CurrentDescriptor->dwFieldsValid;
				bool isExtraDamage = HasFlag(descriptorFlags, (ulong)DamageDescFlag_ExtraDamage) || HasFlag(descriptorFlags, (ulong)DamageDescFlag_DotDamage) ||
					HasFlag(descriptorFlags, (ulong)DamageDescFlag_ReflectDamage) || HasFlag(descriptorFlags, (ulong)DamageDescFlag_IsAbilityDamage);

				if (isExtraDamage) 
					IncomingDamage.ScriptInstance.Flags |= StExt_IncomingDamageFlag_Index_ExtraDamage;
				IncomingDamage.ScriptInstance.SpellId = static_cast<int>(CurrentDescriptor->nSpellID);
			}

			if (HasFlag(CurrentDescriptor->dwFieldsValid, (ulong)DamageDescFlag_DotDamage)) IncomingDamage.ScriptInstance.Flags |= StExt_IncomingDamageFlag_Index_DotDamage;
			if (HasFlag(CurrentDescriptor->dwFieldsValid, (ulong)DamageDescFlag_ReflectDamage)) IncomingDamage.ScriptInstance.Flags |= StExt_IncomingDamageFlag_Index_ReflectDamage;

			unsigned long damTotal = 0UL;
			for (int i = 0; i < oEDamageIndex_MAX; i++)
			{
				unsigned long dam = 0UL;
				dam = CurrentDescriptor->aryDamageEffective[i];
				if(dam <= 0) dam = CurrentDescriptor->aryDamage[i];
				IncomingDamage.ScriptInstance.Damage[i] = static_cast<int>(dam);
				damTotal += dam;
			}

			if ((damTotal == 0UL) && (damage > 0))
			{
				unsigned long damageArray[oEDamageIndex_MAX];
				int damageTemp = damage;
				ApplyDamages(CurrentDescriptor->enuModeDamage, damageArray, damageTemp);
				for (int i = 0; i < oEDamageIndex_MAX; i++)
					IncomingDamage.ScriptInstance.Damage[i] = static_cast<int>(damageArray[i]);
			}
		}

		parser->SetInstance("STEXT_INCOMINGDAMAGEINFO", &IncomingDamage.ScriptInstance);
		parser->SetInstance("STEXT_TARGETNPC", IncomingDamage.Target);
		parser->SetInstance("STEXT_ATTACKNPC", IncomingDamage.Attacker);
		parser->SetInstance("STEXT_ATTACKWEAPON", IncomingDamage.Weapon);
	}

	void ProcessExtraDamage(oCNpc::oSDamageDescriptor& desc, DamageMetaData* damageMeta, oCNpc* target)
	{
		DEBUG_MSG(target->name + Z(" ProcessExtraDamage - ENTER"));

		int damageTotal = 0;
		int damageReal = 0;
		for (int i = 0; i < oEDamageIndex_MAX; i++)
		{
			int dam = static_cast<int>(desc.aryDamage[i]);
			if (dam > 0)
			{
				DEBUG_MSG(target->name + zSTRING(" ProcessExtraDamage - Damage[" + Z i + "] = " + Z dam));

				damageTotal += dam;
				if (target->protection[i] < 0) continue;

				dam = dam - target->protection[i];
				dam = dam < 5 ? 5 : dam;
				damageReal += dam;
				desc.aryDamageEffective[i] = static_cast<unsigned long>(dam);
			}
		}

		if (damageReal <= 0) return;
		desc.fDamageTotal = static_cast<float>(damageTotal);
		desc.fDamageEffective = desc.fDamageReal = static_cast<float>(damageReal);
		
		int damType = StExt_DamageMessageType_Default;
		if (HasFlag(desc.dwFieldsValid, (ulong)DamageDescFlag_DotDamage)) damType |= StExt_DamageMessageType_Dot;
		if (HasFlag(desc.dwFieldsValid, (ulong)DamageDescFlag_ReflectDamage)) damType |= StExt_DamageMessageType_Reflect;
		parser->CallFunc(PrintDamageFunc, damageReal, damType);

		SetDamageMeta(&desc, damageMeta, target);
		target->ChangeAttribute(NPC_ATR_HITPOINTS, -damageReal);

		if (desc.pNpcAttacker)
		{
			DEBUG_MSG(Z"attackNpc: " + Z desc.pNpcAttacker->GetName(0));
			void* oldSelf = parser->GetSymbol("SELF")->GetInstanceAdr();
			void* oldOther = parser->GetSymbol("OTHER")->GetInstanceAdr();
			parser->SetInstance("SELF", target);
			parser->SetInstance("OTHER", desc.pNpcAttacker);

			if (target->IsHuman() && (target->attribute[0] == 1) && !target->IsUnconscious())
			{
				DEBUG_MSG(target->name + zSTRING(" ProcessExtraDamage - IsUnconscious!"));
				desc.bIsUnconscious = true;
			
				target->DropAllInHand();
				target->SetWeaponMode(NPC_WEAPON_NONE);
				target->SetAttribute(NPC_ATR_HITPOINTS, 1);
				target->SetBodyState(BS_UNCONSCIOUS);				
				target->state.StartAIState(-4, FALSE, 0, 0, FALSE);
				target->AssessDefeat_S(desc.pNpcAttacker);
			}
			else if (target->attribute[0] <= 0)
			{
				DEBUG_MSG(target->name + zSTRING(" ProcessExtraDamage - IsDead!"));
				desc.bIsDead = true;			
				target->DoDie(desc.pNpcAttacker);	
			}

			parser->SetInstance("SELF", oldSelf);
			parser->SetInstance("OTHER", oldOther);
		}
		DEBUG_MSG(target->name + zSTRING(" ProcessExtraDamage - true damage: ") + Z damageReal);
	}

	inline void PrintDamageDescriptorDebug(oCNpc::oSDamageDescriptor& desc, zSTRING msg, oCNpc* target)
	{
		DEBUG_MSG("");
		DEBUG_MSG(target->name + Z": <" + msg + Z">");

		if (desc.pVobAttacker) DEBUG_MSG(Z"pVobAttacker: " + Z desc.pVobAttacker->GetObjectName());
		else DEBUG_MSG(Z"pVobAttacker: ???");
		if (desc.pNpcAttacker) DEBUG_MSG(Z"attackNpc: " + Z desc.pNpcAttacker->GetName(0));
		else DEBUG_MSG(Z"attackNpc: ???");
		if (desc.pVobHit) DEBUG_MSG(Z"pVobHit: " + Z desc.pVobHit->GetObjectName());
		else DEBUG_MSG(Z"pVobHit: ???");
		if (desc.pItemWeapon) DEBUG_MSG(Z"pItemWeapon: " + Z desc.pItemWeapon->GetObjectName());
		else DEBUG_MSG(Z"pItemWeapon: ???");

		DEBUG_MSG(Z"flags: " + Z desc.dwFieldsValid);
		DEBUG_MSG(Z"damage flags: " + Z desc.enuModeDamage);

		DEBUG_MSG(Z"damage total: " + Z desc.fDamageTotal);
		DEBUG_MSG(Z"damage real: " + Z desc.fDamageReal);
		DEBUG_MSG(Z"damage effective: " + Z desc.fDamageEffective);
		zSTRING damage = Z"damage[8] = ";
		zSTRING effectiveDamage = Z"effective damage[8] = ";
		for (int i = 0; i < oEDamageIndex_MAX; i++)
		{
			damage += Z desc.aryDamage[i] + Z" | ";
			effectiveDamage += Z desc.aryDamageEffective[i] + Z" | ";
		}
		DEBUG_MSG(damage);
		DEBUG_MSG(effectiveDamage);
		DEBUG_MSG(Z"spellId: " + Z (int)desc.nSpellID);
		DEBUG_MSG(Z"nSpellCat: " + Z desc.nSpellCat);
		DEBUG_MSG(Z"nSpellLevel: " + Z desc.nSpellLevel);
		if (desc.pFXHit) DEBUG_MSG(Z"pFxHit fxName: " + desc.pFXHit->fxName);
		DEBUG_MSG("");
	}

	// Hooks
	HOOK Hook_oCNpc_OnDamage PATCH (&oCNpc::OnDamage, &oCNpc::OnDamage_StExt);
	void oCNpc::OnDamage_StExt(oSDamageDescriptor& desc)
	{
		if (IsLevelChanging || IsLoading || (this->IsSelfPlayer() && (parser->GetSymbol("StExt_ImmortalFlagTime")->intdata > 0)))
		{
			DEBUG_MSG_IF(IsLevelChanging, this->name + Z(" OnDamage - damage done on level changing! Stop it."));
			desc.bFinished = true;
			return;
		}
		PrintDamageDescriptorDebug(desc, "OnDamage ENTER", this);

		// Collect data before damage processing
		unsigned long descriptorFlags = desc.dwFieldsValid;
		bool isInitial = IsDamageInitial(descriptorFlags);		
		bool isExtraDamage = HasFlag(descriptorFlags, DamageDescFlag_ExtraDamage) || HasFlag(descriptorFlags, DamageDescFlag_DotDamage) ||
			HasFlag(descriptorFlags, DamageDescFlag_ReflectDamage) || HasFlag(descriptorFlags, DamageDescFlag_IsAbilityDamage);
		bool isDot = HasFlag(descriptorFlags, oCNpc::oEDamageDescFlag_OverlayActivate) || HasFlag(descriptorFlags, oCNpc::oEDamageDescFlag_OverlayInterval) ||
			HasFlag(descriptorFlags, oCNpc::oEDamageDescFlag_OverlayDuration) || HasFlag(descriptorFlags, oCNpc::oEDamageDescFlag_OverlayDamage);

		int fightMode = Invalid;
		int abilityId = Invalid;
		int spellId = Invalid;
		bool isAbility = false;
		bool isDamageInfo = false;
		bool isConditionValid = (bool)this->IsConditionValid();

		// fix looped dot
		if (isDot && (desc.fDamageTotal < 1.0f))
		{
			DEBUG_MSG(this->name + Z(" OnDamage - damage seems looped dot with 0 damage... Break it."));
			desc.bFinished = true;
			return;
		}

		parser->SetInstance("STEXT_TARGETNPC", this);
		parser->SetInstance("STEXT_ATTACKNPC", desc.pNpcAttacker);
		parser->SetInstance("STEXT_ATTACKWEAPON", desc.pItemWeapon);
		int isImmortal = *(int*)parser->CallFunc(IsNpcImmortalFunc);

		if (desc.pFXHit)
		{
			spellId = static_cast<int>(desc.nSpellID);
			if (spellId <= 0) spellId = GetFxSpellId(desc.pFXHit->fxName.Upper());

			if ((desc.nSpellCat == 0UL) && (desc.nSpellID == 0UL) && (spellId > 0))
			{
				abilityId = spellId + StExt_AbilityPrefix;
				isAbility = true;
			}
			if (isAbility)
			{
				int applyFxDamage = *(int*)parser->CallFunc(FxDamageCanBeAppliedFunc);
				if (!applyFxDamage)
				{
					DEBUG_MSG(this->name + Z(" OnDamage - pFxHit is blocked!"));
					desc.bFinished = true;
					return;
				}
			}
		}

		DamageMetaData damageMeta = DamageMetaData();
		memset(&damageMeta, 0, sizeof(DamageMetaData));
		damageMeta.DescriptorFlags = descriptorFlags;
		damageMeta.IsInitial = isInitial;
		damageMeta.IsExtraDamage = isExtraDamage;
		damageMeta.IsDot = isDot;
		damageMeta.IsAbility = isAbility;
		damageMeta.IsDamageInfo = isDamageInfo;
		damageMeta.IsConditionValid = isConditionValid;
		damageMeta.FightMode = fightMode;
		damageMeta.AbilityId = abilityId;
		damageMeta.SpellId = spellId;
		damageMeta.DamageType = 0;
		damageMeta.DamageFlags = 0;

		// Incoming damage is from my mod. Handle it separately and leave
		if (isExtraDamage)
		{
			int isExtraDamageProhibited = *(int*)parser->CallFunc(IsExtraDamageProhibitedFunc, spellId);
			if (!isConditionValid || isImmortal || isExtraDamageProhibited)
			{
				desc.bFinished = true;
				SetDamageMeta(Null, Null, Null);
				DEBUG_MSG(this->name + Z(" OnDamage - ProcessExtraDamage CANCELED"));
				return;
			}
			DEBUG_MSG(this->name + Z(" OnDamage - ProcessExtraDamage..."));
			ProcessExtraDamage(desc, &damageMeta, this);
			DEBUG_MSG(this->name + Z(" OnDamage - ProcessExtraDamage DONE"));
			desc.bFinished = true;
			SetDamageMeta(Null, Null, Null);
			return;
		}

		DamageInfo damageInfo = DamageInfo();
		memset(&damageInfo, 0, sizeof(DamageInfo));
		if (desc.pNpcAttacker)
		{
			fightMode = desc.pNpcAttacker->GetWeaponMode();
			if (isInitial && !isImmortal && isConditionValid)
			{
				damageInfo = BuildDamageInfo(desc);
				damageInfo.IsInitial = (int)isInitial;
				damageInfo.SpellId = isAbility ? abilityId : spellId;
				parser->SetInstance("STEXT_DAMAGEINFO", &damageInfo);
				isDamageInfo = true;
				DEBUG_MSG(this->name + Z(" OnDamage - damage info builded."));
			}
		}

		if (isDamageInfo)
		{
			DEBUG_MSG(this->name + Z(" OnDamage - call StExt_OnDamageBegin()"));
			parser->CallFunc(OnDamageBeginFunc);
			DEBUG_MSG(this->name + Z(" OnDamage - exit StExt_OnDamageBegin()"));

			if ((damageInfo.BlockDamage > 0) || damageInfo.StopProcess)
			{
				DEBUG_MSG(this->name + Z(" OnDamage - damage blocked! (by script)"));
				desc.bFinished = true;
				SetDamageMeta(Null, Null, Null);
				return;
			}
			damageMeta.DamageType = damageInfo.DamageType;
			damageMeta.DamageFlags = damageInfo.DamageFlags;
		}
		SetDamageMeta(&desc, &damageMeta, this);

		int barrierDamage = static_cast<int>(desc.aryDamage[0] + desc.aryDamageEffective[0]);
		if (barrierDamage > 0)
		{
			ChangeAttribute(NPC_ATR_HITPOINTS, -barrierDamage);
			desc.aryDamage[0] = 0UL;
			desc.aryDamageEffective[0] = 0UL;
		}
		
		// Original damage handler
		DEBUG_MSG(this->name + zSTRING(" OnDamage - call original OnDamage..."));
		SetDamageMeta(&desc, &damageMeta, this);
		THISCALL(Hook_oCNpc_OnDamage)(desc);		
		DEBUG_MSG(this->name + zSTRING(" OnDamage - call original OnDamage DONE"));

		PrintDamageDescriptorDebug(desc, "OnDamage AFTER original OnDamage", this);
		damageMeta.IsConditionValid = isConditionValid = (bool)this->IsConditionValid();
		bool isDamageInfoUpdated = false;

		if (isConditionValid)
		{
			DEBUG_MSG(this->name + zSTRING(" OnDamage - PostProcessDamage..."));
			SetDamageMeta(&desc, &damageMeta, this);

			if (!isDot && !HasFlag(descriptorFlags, DamageDescFlag_DotDamage) && !HasFlag(descriptorFlags, DamageDescFlag_ReflectDamage))
			{
				if (!isDamageInfo)
				{
					damageInfo = BuildDamageInfo(desc);
					damageInfo.IsInitial = (int)isInitial;
					damageInfo.SpellId = isAbility ? abilityId : spellId;
					isDamageInfoUpdated = true;
					parser->SetInstance("STEXT_DAMAGEINFO", &damageInfo);
				}

				SetDamageMeta(&desc, &damageMeta, this);
				parser->CallFunc(OnPostDamageFunc);
				damageMeta.IsConditionValid = isConditionValid = (bool)this->IsConditionValid();
				DEBUG_MSG(this->name + zSTRING(" OnDamage - OnPostDamageFunc DONE"));
			}

			if (isConditionValid && isDamageInfo && !isImmortal && (!damageInfo.BlockDamage || !damageInfo.StopProcess))
			{
				// Update (mod)damage descriptor
				if (!isDamageInfoUpdated)
				{
					damageInfo.RealDamage = static_cast<int>(desc.fDamageReal);
					damageInfo.TotalDamage = static_cast<int>(desc.fDamageTotal);
					for (int i = 0; i < oEDamageIndex_MAX; i++)
					{
						damageInfo.Damage[i] = static_cast<int>(desc.aryDamage[i]);
						damageInfo.DamageEffective[i] = static_cast<int>(desc.aryDamageEffective[i]);
						if ((desc.aryDamage[i] > 0) || (desc.aryDamageEffective[i] > 0))
							damageInfo.DamageEnum |= (unsigned long)(1 << i);
					}
					parser->SetInstance("STEXT_DAMAGEINFO", &damageInfo);
				}

				SetDamageMeta(&desc, &damageMeta, this);
				parser->CallFunc(OnDamageAfterFunc);
				DEBUG_MSG(this->name + zSTRING(" OnDamage - OnDamageAfterFunc DONE"));
			}
		}
		SetDamageMeta(Null, Null, Null);
	}

	HOOK ivk_oCNpc_ChangeAttribute PATCH(&oCNpc::ChangeAttribute, &oCNpc::ChangeAttribute_StExt);
	void oCNpc::ChangeAttribute_StExt(int attrIndex, int value)
	{
		if ((attrIndex == NPC_ATR_HITPOINTS) && (value < 0))
		{
			int damage = value * (-1);
			DEBUG_MSG("Apply damage to '" + Z this->name[0] + "'. Hp: " + Z(this->attribute[0]) + " Damage: " + Z damage);
			UpdateIncomingDamage(damage, this);
			value += *(int*)parser->CallFunc(ProcessHpDamageFunc);
			damage = value >= 0 ? 0 : value * (-1);
			DEBUG_MSG("Apply damage to '" + Z this->name[0] + "'. Damage after: " + Z damage);
			if (value >= 0)
			{
				DEBUG_MSG("Apply damage to '" + Z this->name[0] + "'. Damage fully absorbed!");
				return;
			}

			bool isExtraDamage = HasFlag(IncomingDamage.ScriptInstance.Flags, StExt_IncomingDamageFlag_Index_ExtraDamage);
			bool isExtraDamageDontKill = HasFlag(IncomingDamage.ScriptInstance.Flags, StExt_IncomingDamageFlag_Index_DontKill);
			bool isKill = ((this->attribute[0] + value) <= 0);
			int preserveHp = isKill && isExtraDamage && isExtraDamageDontKill ? -1 : 0;
			value = (value * (-1) > this->attribute[0] + preserveHp) ? -(this->attribute[0] + preserveHp) : value;
		}
		THISCALL(ivk_oCNpc_ChangeAttribute)(attrIndex, value);
		//DEBUG_MSG_IF((attrIndex == NPC_ATR_HITPOINTS),"Apply damage to '" + Z this->name[0] + "' Done. Hp after: " + Z this->attribute[0]);
	}
}
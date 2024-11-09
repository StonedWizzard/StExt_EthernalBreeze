#include <UnionAfx.h>
#include <string> 
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	CDamageInfo DamageInfo;
	CExtraDamage ExtraDamage;
	CDotDamage DotDamage;
	CExtraDamage ReflectDamage;
	CIncomingDamage IncomingDamage;

	void ApplyDamages(ULONG type, ULONG* damage, int& total)
	{
		float dam = 0;
		if (type & dam_barrier)
			dam += 1.0;
		if (type & dam_blunt)
			dam += 1.0;
		if (type & dam_edge)
			dam += 1.0;
		if (type & dam_fire)
			dam += 1.0;
		if (type & dam_fly)
			dam += 1.0;
		if (type & dam_magic)
			dam += 1.0;
		if (type & dam_point)
			dam += 1.0;
		if (type & dam_fall)
			dam += 1.0;

		if (dam <= 0) 
		{ 
			DEBUG_MSG("ApplyDamages - dam is 0!");
			return; 
		}
		if (total <= 0) 
		{ 
			DEBUG_MSG("ApplyDamages - total is 0!");
			return; 
		}

		dam = total / dam + 0.5f;
		if (damage[dam_index_barrier] && (type & dam_barrier))
			damage[dam_index_barrier] = dam;
		if (damage[dam_index_blunt] && (type & dam_blunt))
			damage[dam_index_blunt] = dam;
		if (damage[dam_index_edge] && (type & dam_edge))
			damage[dam_index_edge] = dam;
		if (damage[dam_index_fire] && (type & dam_fire))
			damage[dam_index_fire] = dam;
		if (damage[dam_index_fly] && (type & dam_fly))
			damage[dam_index_fly] = dam;
		if (damage[dam_index_magic] && (type & dam_magic))
			damage[dam_index_magic] = dam;
		if (damage[dam_index_point] && (type & dam_point))
			damage[dam_index_point] = dam;
		if (damage[dam_index_fall] && (type & dam_fall))
			damage[dam_index_fall] = dam;
	}

	inline void* GetDamageStruct(zSTRING structName)
	{
		zCPar_Symbol* psDam = parser->GetSymbol(structName);
		if (!psDam) return Null;
		return psDam->GetInstanceAdr();		
	}

	inline oCNpc::oSDamageDescriptor BuildDescriptor(oCNpc* atk, oCNpc* target, int damages[oEDamageIndex_MAX], int& totalDamage)
	{
		oCNpc::oSDamageDescriptor desc;
		memset(&desc, 0, sizeof oCNpc::oSDamageDescriptor);

		for (int i = 0; i < oEDamageIndex_MAX; i++)
		{
			if (damages[i] > 0)
			{
				totalDamage += damages[i];
				desc.aryDamage[i] = static_cast<unsigned long>(damages[i]);
				desc.enuModeDamage |= (1 << i);
			}
		}
		desc.fDamageTotal = totalDamage;
		desc.fDamageMultiplier = 1.0;
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
		ExtraDamage = *dynamic_cast<CExtraDamage*>((CExtraDamage*)damStructpointer);

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
		DotDamage = *dynamic_cast<CDotDamage*>((CDotDamage*)damStructpointer);
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
		ReflectDamage = *dynamic_cast<CExtraDamage*>((CExtraDamage*)damStructpointer);

		int totalDamage = 0;
		oCNpc::oSDamageDescriptor desc = BuildDescriptor(pAttaker, pTarget, ReflectDamage.Damage, totalDamage);
		desc.dwFieldsValid |= DamageDescFlag_ExtraDamage | DamageDescFlag_DotDamage;

		if (totalDamage < 1)
		{
			DEBUG_MSG(pTarget->name + zSTRING(" ApplyReflectDamage - Damage is 0. Skipped!"));
			return;
		}

		DEBUG_MSG(pTarget->name + zSTRING(" ApplyReflectDamage - damage descriptor applied!"));
		pTarget->OnDamage(desc);
		return;
	}

	inline CDamageInfo BuildDamageInfo(oCNpc::oSDamageDescriptor& desc)
	{
		CDamageInfo result = CDamageInfo();
		result.SpellId = desc.nSpellID;
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
		result.RealDamage = desc.fDamageReal;
		result.DamageEnum = desc.enuModeDamage;
		result.WeaponEnum = desc.enuModeWeapon;
		result.DamageType = 0;
		result.DamageFlags = 0;
		result.BlockDamage = 0;
		result.StopProcess = false;
		result.IsInitial = false;
		return result;
	}

	inline bool HasFlag(unsigned long flags, unsigned long bits) { return (flags & bits) != 0; }

	inline bool IsDamageInitial(unsigned long flags) 
	{ 
		return (!HasFlag(flags, oCNpc::oEDamageDescFlag_OverlayActivate) && !HasFlag(flags, oCNpc::oEDamageDescFlag_OverlayInterval) && 
		!HasFlag(flags, oCNpc::oEDamageDescFlag_OverlayDuration) && !HasFlag(flags, oCNpc::oEDamageDescFlag_OverlayDamage) &&
		!HasFlag(flags, DamageDescFlag_ExtraDamage) && !HasFlag(flags, DamageDescFlag_DotDamage) && !HasFlag(flags, DamageDescFlag_ReflectDamage));
	}

	int GetAbilityId(zSTRING fxName)
	{
		int result = 0;
		int maxSpell = parser->GetSymbol("max_spell")->single_intdata;
		int abilityPreffix = parser->GetSymbol("StExt_AbilityPrefix")->single_intdata;
		zCPar_Symbol* arr = parser->GetSymbol("spellfxinstancenames");
		zSTRING fxName2 = fxName;
		std::string input = fxName.ToChar();
		size_t pos = input.rfind('_');
		if (pos != std::string::npos) 
			fxName2 = Z (input.substr(0, pos).c_str());
		
		for (int i = 0; i < maxSpell; i++)
		{
			zSTRING tmp = "SPELLFX_" + arr->stringdata[i].Upper();
			if (tmp == fxName.Upper() || tmp == fxName2.Upper())
			{
				result = abilityPreffix + i;
				break;
			}
		}
		return result;
	}

	void UpdateIncomingDamage(CIncomingDamage& dam)
	{
		if (!dam.Desc)
		{
			IncomingDamage.ScriptInstance.Processed = true;
			return;
		}

		for (int i = 0; i < oEDamageIndex_MAX; i++)
		{
			IncomingDamage.ScriptInstance.Damage[i] = static_cast<unsigned long>(IncomingDamage.Desc->aryDamageEffective[i]);
			IncomingDamage.ScriptInstance.DamageTotal += IncomingDamage.ScriptInstance.Damage[i];
		}
		IncomingDamage.ScriptInstance.SpellId = static_cast<int>(dam.Desc->nSpellID);
		parser->SetInstance("StExt_IncomingDamageInfo", &IncomingDamage.ScriptInstance);
	}

	void SetIncomingDamage(oCNpc::oSDamageDescriptor desc)
	{
		IncomingDamage = CIncomingDamage();
		memset(&IncomingDamage, 0, sizeof(CIncomingDamage));
		if (!desc.pNpcAttacker)
		{
			IncomingDamage.ScriptInstance.Processed = true;
			return;
		}

		IncomingDamage.ScriptInstance.Processed = false;
		IncomingDamage.Attacker = desc.pNpcAttacker;
		IncomingDamage.Desc = &desc;
		UpdateIncomingDamage(IncomingDamage);		
	}

	// Hooks
	HOOK Hook_oCNpc_OnDamage PATCH (&oCNpc::OnDamage, &oCNpc::OnDamage_StExt);
	void oCNpc::OnDamage_StExt(oSDamageDescriptor& desc)
	{
		bool isInitial = IsDamageInitial(desc.dwFieldsValid);
		bool isAbility = false;
		bool isAbilitySecondary = false;

		DEBUG_MSG(this->name + zSTRING(" OnDamage - enter"));
		if (desc.pVobAttacker) DEBUG_MSG(this->name + zSTRING(" OnDamage - pVobAttacker: ") + Z desc.pVobAttacker->GetObjectName());
		if (desc.pNpcAttacker) DEBUG_MSG(this->name + zSTRING(" OnDamage - attackNpc: ") + Z desc.pNpcAttacker->GetName(0));
		DEBUG_MSG(this->name + zSTRING(" OnDamage - isInitial: ") + Z isInitial);
		DEBUG_MSG(this->name + zSTRING(" OnDamage - flags: ") + Z desc.dwFieldsValid);
		DEBUG_MSG(this->name + zSTRING(" OnDamage - damage flags: ") + Z desc.enuModeDamage);

		// for cheat/debug and some skills
		if (this->IsSelfPlayer() && (parser->GetSymbol("StExt_ImmortalFlagTime")->intdata > 0))
		{
			DEBUG_MSG(this->name + zSTRING(" OnDamage - damage ignored"));
			return;
		}

		if (desc.pFXHit)
		{
			DEBUG_MSG(this->name + zSTRING(" OnDamage - pFxHit detected!"));
			if (desc.pFXHit->spellType <= 0)
			{
				DEBUG_MSG(this->name + zSTRING(" OnDamage - pFxHit is ability"));
				desc.nSpellID = GetAbilityId(desc.pFXHit->fxName);
				isAbility = true;
				isAbilitySecondary = desc.nSpellID <= 0;
				desc.dwFieldsValid |= DamageDescFlag_IsAbilityDamage;
			}

			DEBUG_MSG(this->name + zSTRING(" OnDamage - pFxHit fxName: ") + desc.pFXHit->fxName);
			DEBUG_MSG(this->name + zSTRING(" OnDamage - pFxHit spellId: ") + Z desc.nSpellID);
		}

		// call damage mod damage handler only if damage come from other npc
		// and this damage initial, not dot or something
		if (desc.pNpcAttacker && isInitial)
		{
			int onDamageFuncIndex = Invalid;
			DEBUG_MSG(this->name + zSTRING(" OnDamage - initialize"));
			
			DamageInfo = BuildDamageInfo(desc);
			DamageInfo.IsInitial = isInitial;
				
			DEBUG_MSG(this->name + zSTRING(" OnDamage - set damage info"));
			parser->SetInstance("StExt_DamageInfo", &DamageInfo);
			parser->SetInstance("StExt_AttackNpc", desc.pNpcAttacker);
			parser->SetInstance("StExt_TargetNpc", this);

			if (isAbility && isAbilitySecondary)
			{
				int applyFxDamage = *(int*)parser->CallFunc(FxDamageCanBeAppliedFunc);
				if (!applyFxDamage)
				{
					DamageInfo.BlockDamage += 1;
					DEBUG_MSG(this->name + zSTRING(" OnDamage - skip spellfx aftereffect"));
				}
			}

			// Extra damage handler
			DEBUG_MSG(this->name + zSTRING(" OnDamage - call StExt_OnDamageBegin()"));
			parser->CallFunc(OnDamageBeginFunc);
			DEBUG_MSG(this->name + zSTRING(" OnDamage - exit StExt_OnDamageBegin()"));

			if (DamageInfo.BlockDamage > 0)
			{
				DEBUG_MSG(this->name + zSTRING(" OnDamage - damage blocked!"));
				DamageInfo.StopProcess = true;
				return;
			}
			else if (DamageInfo.StopProcess)
			{
				DEBUG_MSG(this->name + zSTRING(" OnDamage - (extra) damage stopped!"));
			}
			else if (!DamageInfo.StopProcess)
			{
				// Update damage
				for (int i = 0; i < oEDamageIndex_MAX; i++)
				{
					desc.aryDamage[i] = DamageInfo.Damage[i];
					desc.aryDamageEffective[i] = DamageInfo.DamageEffective[i];
					if ((desc.aryDamage[i] > 0) || (desc.aryDamageEffective[i] > 0))
						desc.enuModeDamage |= (1 << i);
				}
			}
		}
		else if (!isInitial)
			DamageInfo.StopProcess = true;
		
		bool bowLoopDamage = false;
		if (desc.pNpcAttacker) 
			bowLoopDamage = (desc.pNpcAttacker->GetWeaponMode() == NPC_WEAPON_BOW) || (desc.pNpcAttacker->GetWeaponMode() == NPC_WEAPON_CBOW);

		if (!isInitial && bowLoopDamage && desc.fDamageTotal < 1.0f)
		{
			DEBUG_MSG(this->name + zSTRING(" OnDamage - damage seems looped. Break it."));
			desc.bFinished = true;
			return;
		}

		// Original damage handler
		DEBUG_MSG(this->name + zSTRING(" OnDamage - call original OnDamage()"));
		THISCALL(Hook_oCNpc_OnDamage)(desc);
		DEBUG_MSG(this->name + zSTRING(" OnDamage - call original OnDamage() - done"));
	}

	void DoAfterDamage(oCNpc::oSDamageDescriptor desc, oCNpc* target)
	{
		DamageInfo.RealDamage = desc.fDamageReal;
		DamageInfo.TotalDamage = desc.fDamageTotal;
		DamageInfo.DamageEnum = desc.enuModeDamage;
		DamageInfo.WeaponEnum = desc.enuModeWeapon;
		for (int i = 0; i < oEDamageIndex_MAX; i++)
		{
			DamageInfo.Damage[i] = desc.aryDamage[i];
			DamageInfo.DamageEffective[i] = desc.aryDamageEffective[i];
		}

		parser->SetInstance("StExt_DamageInfo", &DamageInfo);
		parser->SetInstance("StExt_AttackNpc", desc.pNpcAttacker);
		parser->SetInstance("StExt_TargetNpc", target);
		parser->CallFunc(OnDamageAfterFunc);
	}

	HOOK Hook_oCNpc_OnDamage_Hit PATCH (&oCNpc::OnDamage_Hit, &oCNpc::OnDamage_Hit_StExt);
	void oCNpc::OnDamage_Hit_StExt(oSDamageDescriptor& desc)
	{
		parser->SetInstance("StExt_Self", this);

		// do extra damage from mod	
		if(HasFlag(desc.dwFieldsValid, DamageDescFlag_ExtraDamage) || HasFlag(desc.dwFieldsValid, DamageDescFlag_DotDamage) || HasFlag(desc.dwFieldsValid, DamageDescFlag_ReflectDamage) || HasFlag(desc.dwFieldsValid, DamageDescFlag_IsAbilityDamage))
		{
			DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - process extra damage"));
			int isImmortal = *(int*)parser->CallFunc(IsNpcImmortalFunc);

			if (!desc.pNpcAttacker || isImmortal || attribute[0] <= 2)
			{
				DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - process extra damage - skip"));
				return;
			}

			desc.fDamageTotal = 0;

			for (int i = 0; i < oEDamageIndex_MAX; i++)
			{
				int dam = static_cast<int>(desc.aryDamage[i]);
				if (dam > 0)
				{
					DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - process extra damage [" + Z i + "] = " + Z desc.aryDamage[i]));
					if (protection[i] < 0) continue;

					dam = dam - protection[i];
					dam = dam < 5 ? 5 : dam;
					desc.aryDamageEffective[i] = static_cast<unsigned long>(dam);
					desc.fDamageTotal += static_cast<float>(desc.aryDamageEffective[i]);

					// handle barrier damage
					if (i == 0 || i == 4)
					{
						desc.aryDamageEffective[i] = 0UL;
						desc.aryDamage[i] = 0UL;
						desc.enuModeDamage &= ~(1 << i);
					}
				}
			}
			desc.fDamageTotal = desc.fDamageTotal < 5.0f ? 5.0f : desc.fDamageTotal;
			desc.fDamageEffective = desc.fDamageTotal;
			desc.fDamageReal = desc.fDamageTotal;

			int damType = StExt_DamageMessageType_Default;
			if (HasFlag(desc.dwFieldsValid, DamageDescFlag_DotDamage)) damType |= StExt_DamageMessageType_Dot;
			if (HasFlag(desc.dwFieldsValid, DamageDescFlag_ReflectDamage)) damType |= StExt_DamageMessageType_Reflect;

			parser->SetInstance("StExt_Other", desc.pNpcAttacker);
			parser->CallFunc(PrintDamageFunc, static_cast<int>(desc.fDamageTotal), damType);

			SetIncomingDamage(desc);
			int hpBefore = attribute[0];
			ChangeAttribute(NPC_ATR_HITPOINTS, static_cast<int>(-desc.fDamageTotal));
			int hpLoss = hpBefore - attribute[0];
			if (hpLoss > 0)
			{
				parser->SetInstance("StExt_Self", this);
				parser->SetInstance("StExt_Other", desc.pNpcAttacker);
				parser->CallFunc(OnSncDamageFunc, hpLoss);
			}
			desc.fDamageReal = static_cast<float>(hpLoss);

			// Extra damage dont kill humans
			if (IsHuman() && attribute[0] <= 0)
			{
				attribute[0] = 1;
				desc.bIsUnconscious = true;
				DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - unconcision???"));
			}

			memset(&DamageInfo, 0, sizeof(CDamageInfo));
			DamageInfo.StopProcess = true;
			IncomingDamage.ScriptInstance.Processed = true;
			DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - process extra damage. True damage: ") + Z static_cast<int>(desc.fDamageReal));
			return;
		}
		
		SetIncomingDamage(desc);
		// Call original Ondamage_Hit
		DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - call original OnDamage_Hit() Flags: ") + Z desc.dwFieldsValid);
		if (desc.pFXHit)
			DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - pFXHit refs: " + Z desc.pFXHit->refCtr));
		if (desc.pVisualFX)
			DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - pVisualFX refs: " + Z desc.pVisualFX->refCtr));

		if (desc.aryDamage[0] > 0)
		{
			ChangeAttribute(NPC_ATR_HITPOINTS, -static_cast<int>(desc.aryDamage[0]));
			desc.aryDamage[0] = 0UL;
		}
		THISCALL(Hook_oCNpc_OnDamage_Hit)(desc);
		DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - exit original OnDamage_Hit()"));

		// Process after damage effect
		if (desc.pNpcAttacker && !DamageInfo.StopProcess)
		{
			DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - call StExt_OnDamageAfter()"));			
			DoAfterDamage(desc, this);
			DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - exit StExt_OnDamageAfter()"));
		}

		DEBUG_MSG(this->name + zSTRING(" OnDamage_Hit - call DONE!"));
		DamageInfo = CDamageInfo();
		memset(&DamageInfo, 0, sizeof(CDamageInfo));
		DamageInfo.StopProcess = true;
		IncomingDamage.ScriptInstance.Processed = true;
	}

	HOOK Hook_oCNpc_OnDamage_Anim PATCH(&oCNpc::OnDamage_Anim, &oCNpc::OnDamage_Anim_StExt);
	void oCNpc::OnDamage_Anim_StExt(oSDamageDescriptor& desc)
	{
		if (HasFlag(desc.dwFieldsValid, DamageDescFlag_ExtraDamage) || HasFlag(desc.dwFieldsValid, DamageDescFlag_DotDamage) ||
			HasFlag(desc.dwFieldsValid, DamageDescFlag_ReflectDamage) || HasFlag(desc.dwFieldsValid, DamageDescFlag_IsAbilityDamage)) return;			
		THISCALL(Hook_oCNpc_OnDamage_Anim)(desc);
	}

	HOOK ivk_oCNpc_ChangeAttribute PATCH(&oCNpc::ChangeAttribute, &oCNpc::ChangeAttribute_StExt);
	void oCNpc::ChangeAttribute_StExt(int attrIndex, int value) 
	{
		int hpWas = this->attribute[0];
		int esDamage = 0, hpDamage = 0;
		bool isHit = false;
		bool esHit = false;

		if ((attrIndex == NPC_ATR_HITPOINTS) && (value < 0))
		{
			DEBUG_MSG("Apply damage to '" + Z name[0] + "'. Hp: " + zSTRING(this->attribute[0]) + " Damage: " + Z (value * -1));

			isHit = true;
			parser->SetInstance("StExt_Self", this);
			UpdateIncomingDamage(IncomingDamage);

			// first apply resists to damage
			if (!IncomingDamage.ScriptInstance.Processed)
			{
				DEBUG_MSG("Apply resists to damage to '" + Z name[0] + "'. Damage before: " + zSTRING(value * -1));
				value += *(int*)parser->CallFunc(ApplyResistsFunc);
			}

			// apply es
			DEBUG_MSG("Apply es to damage to '" + Z name[0] + "'. Damage before: " + zSTRING(value * -1));
			int valueBeforeEs = value;
			value = *(int*)parser->CallFunc(ApplyDamageToEsFunc, value);

			int val = value >= 0 ? 0 : value;
			int trueDam = (valueBeforeEs - val) * -1;
			if (trueDam > 0)
			{
				esDamage = trueDam;
				esHit = true;
			}
			DEBUG_MSG("Apply damage to '" + Z name[0] + "'. Damage after: " + zSTRING(value >= 0 ? 0 : value * -1));
		}
		if (isHit)
		{
			DEBUG_MSG("Apply damage to '" + Z name + "'. isHit: " + Z isHit);
			DEBUG_MSG("Apply damage to '" + Z name + "'. esHit: " + Z esHit);
		}
		// when was hit, but reduced by es
		if (isHit && esHit)
		{
			DEBUG_MSG("Apply damage to '" + Z name + "'. Es hit detected");
			if (!IncomingDamage.ScriptInstance.Processed && IncomingDamage.Attacker)
			{
				DEBUG_MSG("Apply damage to '" + Z name + "'. Es hit detected - call exp func");
				parser->SetInstance("StExt_Other", IncomingDamage.Attacker);
				parser->CallFunc(OnSncDamageFunc, esDamage);
				parser->CallFunc(OnModDamageExpFunc, esDamage, IncomingDamage.ScriptInstance.SpellId);
			}

			if (value >= 0)
			{
				DEBUG_MSG("Apply damage to '" + Z name + "'. Damage blocked by es: " + Z esDamage);
				return;
			}
		}

		THISCALL(ivk_oCNpc_ChangeAttribute)(attrIndex, value);
		if (isHit)
		{
			hpDamage = hpWas - this->attribute[0];
			DEBUG_MSG("Apply damage to '" + Z name + "'. Hp hit detected. Damage: " + Z hpDamage);
			if ((hpDamage > 0) && !IncomingDamage.ScriptInstance.Processed && IncomingDamage.Attacker)
			{
				DEBUG_MSG("Apply damage to '" + Z name + "'. Hp hit detected - call exp func");
				parser->SetInstance("StExt_Other", IncomingDamage.Attacker);
				parser->CallFunc(OnModDamageExpFunc, hpDamage, IncomingDamage.ScriptInstance.SpellId);
			}
			DEBUG_MSG("Apply damage to '" + Z name + "'. Hp after: " + zSTRING(this->attribute[0]));
		}
	}
};
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	const int SPELLEFFECT_STORE_TIME = 3600;

	struct DamageMeta
	{
		oCNpc* Target;
		oCNpc* Attacker;
		oCItem* Weapon;
		oCNpc::oSDamageDescriptor* Desc;

		bool IsExtraDamage;
		bool IsDotDamage;
		bool IsAoeDamage;
		bool IsReflectDamage;
		bool IsAbility;
		bool IsOverlayDamage;
		bool IsInitialDamage;
		bool TargetIsImmortal;
		bool SpellFxInDict;

		DamageInfo DamageInfo;
	};

	struct DamageInfoMeta
	{
		bool IsPending;

		oCNpc* Target;
		oCNpc* Attacker;
		oCItem* Weapon;

		DamageInfo DamageInfo;
	};

	struct ExtraDamageParams
	{
		bool HasPendingExtraParams;
		int DamageType;
		int DamageFlags;
	};

	struct PendingDamage
	{
		oCNpc* Target;
		oCNpc* Attacker;

		bool IsMock;
		uint Flags;
		int FrameTime;
		int DamageSelectorFunc;		
		ExtraDamageInfo DamageInfo;
	};

	ExtraDamageInfo ExtraDamage;
	ExtraDamageInfo DotDamage;
	ExtraDamageInfo ReflectDamage;

	ExtraDamageParams DamageExtraParams;
	DamageInfoMeta DamageInfoMetaData;
	Array<DamageMeta> DamageMetaData;
	Array<PendingDamage> PendingDamageData;
	Map<oCVisualFX*, PendingDamage> PendingEffectDamageData;

	int MaxSpellId;
	int StExt_AbilityPrefix;
	StringMap<int> SpellFxNames;

	
#if DebugEnabled 
	#define DEBUG_MSG_DAMDESC(desc, msg, target) PrintDamageDescriptorDebug(desc, msg, target)
	#define DEBUG_MSG_DAMMETA(meta, msg) PrintDamageMetaDebug(meta, msg)
	#define DEBUG_MSG_DAMINFO(info, msg) PrintDamageInfoDebug(info, msg)
	#define DEBUG_MSG_DAMINCOMINFO(info, msg) PrintIncomingDamageInfoDebug(info, msg)
#else
	#define DEBUG_MSG_DAMDESC(desc, msg, target) ((void)0)
	#define DEBUG_MSG_DAMMETA(desc, msg) ((void)0)
	#define DEBUG_MSG_DAMINFO(desc, msg) ((void)0)
	#define DEBUG_MSG_DAMINCOMINFO(info, msg) ((void)0)
#endif

	//-----------------------------------------------------------------
	//						DEBUG FUNCTIONS
	//-----------------------------------------------------------------

	inline zSTRING SafeNpcName(oCNpc* npc) { return npc ? (npc->IsSelfPlayer() ? "Hero" : npc->name[0]) : "<Null>"; }
	inline zSTRING SafeItemName(oCItem* item) { return (!item) ? "<Null>" : item->name; }
	inline zSTRING GetNpcHpEs(oCNpc* npc)
	{
		if (!npc) return "HP: ???/??? | ES: ???/???";

		oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
		if (!npcEx) return "HP: " + Z(npc->attribute[0]) + "/" + Z(npc->attribute[1]) + " | ES: ???/???";

		int esCur; GetNpcExtensionVar(npcEx->m_pVARS[StExt_AiVar_Uid], StExt_AiVar_EsCur, esCur);
		int esMax; GetNpcExtensionVar(npcEx->m_pVARS[StExt_AiVar_Uid], StExt_AiVar_EsMax, esMax);
		return "HP: " + Z(npc->attribute[0]) + "/" + Z(npc->attribute[1]) + " | ES: " + Z(esCur) + "/" + Z(esMax);
	}

	inline void PrintDamageDescriptorDebug(oCNpc::oSDamageDescriptor& desc, zSTRING msg, oCNpc* target)
	{
		DEBUG_MSG("-------------------------------------------------------------");
		DEBUG_MSG_DAM(" <<< DAMAGEDESCRIPTOR >>> ", msg, desc.pNpcAttacker, target);
		DEBUG_MSG("");

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
	}
	inline void PrintDamageInfoDebug(const DamageInfo& info, zSTRING title)
	{
		DEBUG_MSG("-------------------------------------------------------------");
		DEBUG_MSG(" <<< DAMAGEINFO: " + title + " >>> ");
		DEBUG_MSG("");

		zSTRING dmgArray = Z"Damage[]: ";
		for (int i = 0; i < 8; ++i)
			dmgArray += Z(info.Damage[i]) + " | ";
		DEBUG_MSG(dmgArray);
		DEBUG_MSG("TotalDamage: " + Z(info.TotalDamage) + " | RealDamage: " + Z(info.RealDamage) + " | SpellId: " + Z(info.SpellId));
		DEBUG_MSG("DamageEnum: " + Z(info.DamageEnum) +	" | WeaponEnum: " + Z(info.WeaponEnum) + " | DamageType: " + Z(info.DamageType) + " | DamageFlags: " + Z(info.DamageFlags));
		DEBUG_MSG("BlockDamage: " + Z(info.BlockDamage) + " | StopProcess: " + Z(info.StopProcess) + " | IsInitial: " + Z(info.IsInitial));
		DEBUG_MSG("-------------------------------------------------------------");
	}
	inline void PrintDamageMetaDebug(const DamageMeta& meta, zSTRING title)
	{
		DEBUG_MSG("=============================================================");
		DEBUG_MSG(" <<< DAMAGEMETA: " + title + " >>> ");
		DEBUG_MSG("");

		DEBUG_MSG("Target: " + SafeNpcName(meta.Target) + " [" + Z((int)meta.Target) + "] | " + 
			"Attacker: " + SafeNpcName(meta.Attacker) + " [" + Z((int)meta.Attacker) + "] | " + 
			"Weapon: " + SafeItemName(meta.Weapon) + " [" + Z((int)meta.Weapon) + "]");
		DEBUG_MSG("IsExtraDamage: " + Z(meta.IsExtraDamage) + " | IsDotDamage: " + Z(meta.IsDotDamage) + " | IsAoeDamage: " + Z(meta.IsAoeDamage) + " | IsReflectDamage: " + Z(meta.IsReflectDamage));
		DEBUG_MSG("IsAbility: " + Z(meta.IsAbility) + " | IsOverlayDamage: " + Z(meta.IsOverlayDamage) + " | IsInitialDamage: " + Z(meta.IsInitialDamage) + " | TargetIsImmortal: " + Z(meta.TargetIsImmortal));
		DEBUG_MSG("=============================================================");
	}
	inline void PrintIncomingDamageInfoDebug(const IncomingDamageInfo& info, zSTRING title)
	{
		DEBUG_MSG("-------------------------------------------------------------");
		DEBUG_MSG(" <<< INCOMINGDAMAGEINFO: " + title + " >>> ");
		DEBUG_MSG("");

		zSTRING dmgArray = Z"Damage[]: ";
		for (int i = 0; i < 8; ++i)
			dmgArray += Z(info.Damage[i]) + " | ";
		DEBUG_MSG(dmgArray);
		DEBUG_MSG("DamageTotal: " + Z(info.DamageTotal) + " | SpellId: " + Z(info.SpellId));
		DEBUG_MSG("Flags: " + Z(info.Flags) + " | DamageType: " + Z(info.DamageType) + " | DamageFlags: " + Z(info.DamageFlags));
		DEBUG_MSG("-------------------------------------------------------------");
	}

	//-----------------------------------------------------------------
	//						UTIL FUNCTIONS
	//-----------------------------------------------------------------

	inline int GetCurNpcEs(oCNpc* npc)
	{
		if (!npc) return 0;
		oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
		if (!npcEx) return 0;

		int esCur; GetNpcExtensionVar(npcEx->m_pVARS[StExt_AiVar_Uid], StExt_AiVar_EsCur, esCur);
		return esCur;
	}

	inline bool IsDamageDescriptorSane(oCNpc::oSDamageDescriptor& desc)
	{
		const int MAX_REASONABLE_DAMAGE = 100000;
		const int MAX_REASONABLE_TOTAL = 300000;

		if (desc.fDamageTotal < 0 || desc.fDamageTotal > MAX_REASONABLE_TOTAL) return false;
		if (desc.fDamageReal < 0 || desc.fDamageReal > MAX_REASONABLE_TOTAL) return false;
		if (desc.fDamageEffective < 0 || desc.fDamageEffective > MAX_REASONABLE_TOTAL) return false;

		ulong damageSum = 0;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			ulong dam = desc.aryDamage[i];
			if (dam > MAX_REASONABLE_DAMAGE)
				return false;
			damageSum += dam;
		}
		if (damageSum > MAX_REASONABLE_TOTAL) return false;
		if (desc.pNpcAttacker && (uintptr_t)desc.pNpcAttacker < 0x10000) return false;
		return true;
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

	inline int GetFxTrueId(oCNpc::oSDamageDescriptor* desc, bool& isAbility)
	{
		if (!desc || !desc->pFXHit)
		{
			isAbility = false;
			return Invalid;
		}

		int spellId = static_cast<int>(desc->nSpellID);
		if (spellId > 0)
		{
			isAbility = false;
			return spellId;
		}

		zSTRING fxNameKey = desc->pFXHit->fxName;
		if (SpellFxNames.IsEmpty() || fxNameKey.IsEmpty())
		{
			DEBUG_MSG_IF(SpellFxNames.IsEmpty(), "GetFxSpellId - SpellFxNames is null!");
			DEBUG_MSG_IF(fxNameKey.IsEmpty(), "GetFxSpellId - fxName is empty!");
			isAbility = false;
			return Invalid;
		}

		fxNameKey.Upper().Replace("SPELLFX_", "");
		auto pair = SpellFxNames.Find(fxNameKey);
		if (pair) 
		{
			spellId = *pair;
			isAbility = true;
			return spellId + StExt_AbilityPrefix;
		}

		pair = SpellFxNames.FindApprox(fxNameKey);
		if (pair)
		{
			spellId = *pair;
			isAbility = true;
			return spellId + StExt_AbilityPrefix;
		}
		isAbility = true;
		return Invalid;
	}

	inline void SetScriptDamageActors(oCNpc* atk, oCNpc* target, oCItem* weap)
	{
		parser->SetInstance(StExt_TargetNpc_SymId, target);
		parser->SetInstance(StExt_AttackNpc_SymId, atk);
		parser->SetInstance(StExt_AttackWeapon_SymId, weap);
	}

	inline void BuildDescriptor(oCNpc::oSDamageDescriptor& desc)
	{
		desc.bOnce = True;
		desc.bFinished = False;
		desc.bIsDead = False;
		desc.bIsUnconscious = False;
		desc.bDamageDontKill = False;

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

	inline void BuildDescriptor(oCNpc::oSDamageDescriptor& desc, oCNpc* atk, oCNpc* target, const ExtraDamageInfo& damStruct, const ulong flags)
	{
		BuildDescriptor(desc);

		desc.dwFieldsValid = oCNpc::oEDamageDescFlag_Attacker | oCNpc::oEDamageDescFlag_Npc | 
			oCNpc::oEDamageDescFlag_Inflictor | oCNpc::oEDamageDescFlag_DamageType | oCNpc::oEDamageDescFlag_Damage;

		desc.dwFieldsValid |= flags;
		if (HasFlag(damStruct.DamageFlags, StExt_DamageFlag_Aoe) || HasFlag(damStruct.DamageFlags, StExt_DamageFlag_Chain)) desc.dwFieldsValid |= DamageDescFlag_AoeDamage;

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

	inline void CreateIncomingDamage(IncomingDamageInfo& incDamInfo, DamageInfo& damInfo)
	{
		incDamInfo = IncomingDamageInfo{};

		incDamInfo.Flags = 0;
		incDamInfo.DamageType = damInfo.DamageType;
		incDamInfo.DamageFlags = damInfo.DamageFlags;
		incDamInfo.SpellId = damInfo.SpellId;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
			incDamInfo.Damage[i] = damInfo.Damage[i];
		incDamInfo.DamageTotal = damInfo.RealDamage;

		if (HasFlag(damInfo.DamageFlags, StExt_DamageFlag_Dot)) incDamInfo.Flags |= StExt_IncomingDamageFlag_Index_DotDamage;
		if (HasFlag(damInfo.DamageFlags, StExt_DamageFlag_Aoe)) incDamInfo.Flags |= StExt_IncomingDamageFlag_Index_AoeDamage;
		if (HasFlag(damInfo.DamageFlags, StExt_DamageFlag_Reflect)) incDamInfo.Flags |= StExt_IncomingDamageFlag_Index_ReflectDamage;
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

		damageMeta.Target = target;
		damageMeta.Attacker = desc.pNpcAttacker;
		damageMeta.Weapon = desc.pItemWeapon;
		SetScriptDamageActors(desc.pNpcAttacker, target, desc.pItemWeapon);

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
		damageMeta.SpellFxInDict = false;

		BuildDamageInfo(damageMeta.DamageInfo, desc);
		damageMeta.DamageInfo.SpellId = GetFxTrueId(&desc, damageMeta.IsAbility);
		damageMeta.DamageInfo.IsInitial = damageMeta.IsInitialDamage;

		if (DamageExtraParams.HasPendingExtraParams)
		{
			damageMeta.DamageInfo.DamageType |= DamageExtraParams.DamageType;
			damageMeta.DamageInfo.DamageFlags |= DamageExtraParams.DamageFlags;
			DamageExtraParams.HasPendingExtraParams = false;
		}

		DamageMeta& ref = DamageMetaData.InsertEnd(damageMeta);
		DEBUG_MSG("PushDamageMeta: damage meta was PUSHED to stack! Stack level: " + Z((int)DamageMetaData.GetNum()) + " | descPtr: " + Z((int)&desc));

		parser->SetInstance(StExt_DamageInfo_SymId, &ref.DamageInfo);
		DEBUG_MSG_DAMMETA(ref, "PushDamageMeta");
		DEBUG_MSG_DAMINFO(ref.DamageInfo, "PushDamageMeta");
		return ref;
	}

	inline void PopDamageMeta()
	{
		if (DamageMetaData.IsEmpty())
		{
			DEBUG_MSG("PopDamageMeta: Stack is empty!");
			SetScriptDamageActors(Null, Null, Null);
			parser->SetInstance(StExt_DamageInfo_SymId, Null);
			DamageInfoMetaData.IsPending = false;
			return;
		}

		DamageMeta& damageMeta = DamageMetaData.GetLast();

		DEBUG_MSG_IF(DamageInfoMetaData.IsPending, "PopDamageMeta: DamageInfoMetaData is pending!?");
		DamageInfoMetaData.IsPending = true;
		DamageInfoMetaData.Attacker = damageMeta.Attacker;
		DamageInfoMetaData.Target = damageMeta.Target;
		DamageInfoMetaData.Weapon = damageMeta.Weapon;
		DamageInfoMetaData.DamageInfo = damageMeta.DamageInfo;

		DamageMetaData.RemoveAt(DamageMetaData.GetNum() - 1U);

		DEBUG_MSG("PopDamageMeta: damage meta was POPED from stack! Stack level: " + Z((int)DamageMetaData.GetNum()));
		if (!DamageMetaData.IsEmpty())
		{
			DamageMeta& damageMeta = DamageMetaData.GetLast();
			DEBUG_MSG_DAMMETA(damageMeta, "PopDamageMeta");
			DEBUG_MSG_DAMINFO(damageMeta.DamageInfo, "PopDamageMeta");
			parser->SetInstance(StExt_DamageInfo_SymId, &damageMeta.DamageInfo);
		}
		else
			parser->SetInstance(StExt_DamageInfo_SymId, Null);
	}

	inline DamageMeta* GetDamageMeta(bool isOverlay = false)
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
		DamageInfoMetaData.IsPending = false;
		DamageMetaData.Clear();
		PendingEffectDamageData.Clear();
	}

	int FindTargets(Array<oCNpc*>& targetsList, oCNpc* atk, oCNpc* target, ExtraDamageInfo& damageStruct, int damageSelectorFunc = Invalid)
	{
		bool isAoe = HasFlag(damageStruct.DamageFlags, StExt_DamageFlag_Aoe);
		bool isChain = HasFlag(damageStruct.DamageFlags, StExt_DamageFlag_Chain);
		bool inMassDamage = (isAoe || isChain) && (damageStruct.MaxTargets > 0);

		if (!inMassDamage) return Invalid;

		oCNpc* center = Null;
		if (target && (HasFlag(damageStruct.DamageFlags, StExt_DamageFlag_Target) ||
			HasFlag(damageStruct.DamageFlags, StExt_DamageFlag_Single))) center = target;
		else if (atk) center = atk;
		else if (target) center = target;
		else
		{
			DEBUG_MSG("FindTargets: center npc is null! Skipped!");
			return Invalid;
		}

		int foundNpcs = 0;
		if (damageSelectorFunc == Invalid)
			damageSelectorFunc = StExt_IsEnemyFuncIndex;
		const float radius = static_cast<float>(damageStruct.Radius < 100 ? 100 : damageStruct.Radius);
		center->ClearVobList();
		center->CreateVobList(radius);

		zCVob* pVob = Null;
		oCNpc* npc = Null;
		for (int i = 0; i < center->vobList.GetNum(); ++i)
		{
			if (i > 8192 || foundNpcs > damageStruct.MaxTargets) break;
			pVob = center->vobList.GetSafe(i);
			if (!pVob) continue;

			npc = zDYNAMIC_CAST<oCNpc>(pVob);
			if (!npc) continue;
			if (npc->IsDead() || npc->IsUnconscious()) continue;

			parser->SetInstance(StExt_ModSelf_SymId, atk);
			parser->SetInstance(StExt_ModOther_SymId, npc);
			bool isEnemy = *(int*)parser->CallFunc(damageSelectorFunc);
			if (isEnemy) {
				targetsList.Insert(npc);
				++foundNpcs;
			}
		}
		return foundNpcs;
	}

	inline void CheckExtraDamages(bool& hasDamage, bool& hasDotDamage, ExtraDamageInfo& damageStruct)
	{
		int totalDamage = 0;
		int totalDotDamage = 0;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			totalDamage += damageStruct.Damage[i];
			totalDotDamage += damageStruct.DotDamage[i];
		}
		hasDamage = (totalDamage > 0);
		hasDotDamage = (totalDotDamage > 0);
	}


	//-----------------------------------------------------------------
	//					DAMAGE PROCESSING FUNCTIONS
	//-----------------------------------------------------------------

	inline void AddDotDamage(oCNpc* atk, oCNpc* target, ExtraDamageInfo& extraDam)
	{
		static int dotDamageFuncIndex = parser->GetIndex("StExt_OnDotDamage");
		static int extraDamagePtrIndex = parser->GetIndex("StExt_ExtraDamageInfoPtr");

		if (!target) {
			DEBUG_MSG("AddDotDamage: target npc is null! Skipped!");
			return;
		}
		if (target->IsDead() || target->IsUnconscious()) return;

		parser->SetInstance(StExt_ModSelf_SymId, target);
		parser->SetInstance(StExt_ModOther_SymId, atk);
		parser->SetInstance(extraDamagePtrIndex, &extraDam);
		parser->CallFunc(dotDamageFuncIndex);
		parser->SetInstance(extraDamagePtrIndex, Null);
	}

	inline void AddPendingDamage(oCNpc* atk, oCNpc* target, const ExtraDamageInfo& extraDam, const ulong flags, int damageSelectorFunc = Invalid)
	{
		PendingDamage pendingDamage = PendingDamage{};
		pendingDamage.IsMock = false;
		pendingDamage.Attacker = atk;
		pendingDamage.Target = target;
		pendingDamage.Flags = flags;
		pendingDamage.FrameTime = Invalid;
		pendingDamage.DamageSelectorFunc = damageSelectorFunc;
		pendingDamage.DamageInfo = extraDam;
		PendingDamageData.Insert(pendingDamage);
	}

	inline void AddPendingSpellDamage(oCNpc* atk, oCNpc* target, oCVisualFX* hitFx, const ExtraDamageInfo& extraDam, const ulong flags, int damageSelectorFunc)
	{
		if (!hitFx) return;

		PendingDamage pendingDamage = PendingDamage{};
		pendingDamage.IsMock = false;
		pendingDamage.Attacker = atk;
		pendingDamage.Target = target;
		pendingDamage.Flags = flags;
		pendingDamage.FrameTime = SPELLEFFECT_STORE_TIME;
		pendingDamage.DamageSelectorFunc = damageSelectorFunc;
		pendingDamage.DamageInfo = extraDam;
		PendingEffectDamageData.Insert(hitFx, pendingDamage);
	}

	inline void DoExtraDamage(oCNpc* atk, oCNpc* target, ExtraDamageInfo& extraDam, const ulong flags, const bool hasDamage, const bool hasDotDamage)
	{
		if (!target) {
			DEBUG_MSG("DoExtraDamage: target npc is null! Skipped!");
			return;
		}
		if (target->IsDead() || target->IsUnconscious()) return;

		if (hasDotDamage)
			AddDotDamage(atk, target, extraDam);

		if (hasDamage)
		{
			oCNpc::oSDamageDescriptor desc;
			BuildDescriptor(desc, atk, target, extraDam, flags);

			DamageExtraParams.HasPendingExtraParams = true;
			DamageExtraParams.DamageType = extraDam.DamageType;
			DamageExtraParams.DamageFlags = extraDam.DamageFlags;
			if (HasFlag(flags, (ulong)DamageDescFlag_DotDamage)) DamageExtraParams.DamageFlags |= StExt_DamageFlag_Dot;
			if (HasFlag(flags, (ulong)DamageDescFlag_ReflectDamage)) DamageExtraParams.DamageFlags |= StExt_DamageFlag_Reflect;

			target->OnDamage(desc);
			//desc.Release();
		}
	}

	void ApplyExtraDamage_Generic(oCNpc* atk, oCNpc* target, ExtraDamageInfo& damageStruct, const ulong flags, int damageSelectorFunc = Invalid)
	{ 
		bool hasDamage = false, hasDotDamage = false;
		CheckExtraDamages(hasDamage, hasDotDamage, damageStruct);
		if (!hasDamage && !hasDotDamage) {
			DEBUG_MSG("ApplyExtraDamage_Generic: total damage is less than 0! Skipped!");
			return;
		}

		Array<oCNpc*> targetsList;
		if (target)
			targetsList.Insert(target);

		FindTargets(targetsList, atk, target, damageStruct, damageSelectorFunc);
		if (targetsList.IsEmpty()) {
			DEBUG_MSG("ApplyExtraDamage_Generic: no targets to damage! Skipped!");
			return;
		}

		for (uint i = 0; i < targetsList.GetNum(); ++i)
			DoExtraDamage(atk, targetsList[i], damageStruct, flags, hasDamage, hasDotDamage);
	}

	void ApplyExtraDamage(oCNpc* atk, oCNpc* target) { ApplyExtraDamage_Generic(atk, target, ExtraDamage, DamageDescFlag_ExtraDamage); }
	
	void ApplyScriptDamage(oCNpc* atk, oCNpc* target, ExtraDamageInfo& damStruct, const int damageSelectorFuncIndex) { 
		ApplyExtraDamage_Generic(atk, target, damStruct, DamageDescFlag_ExtraDamage, damageSelectorFuncIndex);
	}

	void ApplyDotDamage(oCNpc* atk, oCNpc* target)
	{
		if (!target) {
			DEBUG_MSG("ApplyDotDamage: Target is null!");
			return;
		}

		bool hasDamage = false, hasDotDamage = false;
		CheckExtraDamages(hasDamage, hasDotDamage, DotDamage);
		if (!hasDamage) {
			DEBUG_MSG("ApplyDotDamage: total damage is less than 0! Skipped!");
			return;
		}
		DoExtraDamage(atk, target, DotDamage, DamageDescFlag_DotDamage, hasDamage, false);
	}

	void ApplyReflectDamage(oCNpc* atk, oCNpc* target) { AddPendingDamage(atk, target, ReflectDamage, DamageDescFlag_ReflectDamage); }

	void ApplyScriptPendingDamage(oCNpc* atk, oCNpc* target, ExtraDamageInfo& damStruct, const int damageSelectorFuncIndex) {
		AddPendingDamage(atk, target, damStruct, DamageDescFlag_ExtraDamage, damageSelectorFuncIndex);
	}

	void ProcessPendingDamages()
	{
		if (IsLevelChanging || IsLoading)
		{
			DEBUG_MSG("ProcessPendingDamages: BREAK: can't process while level is changing!");
			PendingDamageData.Clear();
			PendingEffectDamageData.Clear();
			return;
		}

		if (!PendingDamageData.IsEmpty())
		{
			for (uint i = 0U; i < PendingDamageData.GetNum(); ++i)
			{
				PendingDamage& pendingDamage = PendingDamageData[i];
				ApplyExtraDamage_Generic(pendingDamage.Attacker, pendingDamage.Target, pendingDamage.DamageInfo, pendingDamage.Flags, pendingDamage.DamageSelectorFunc);
			}
			PendingDamageData.Clear();
		}

		if (PendingEffectDamageData.IsEmpty()) return;
		auto* pairs = PendingEffectDamageData.begin();
		for (int i = PendingEffectDamageData.GetNum() - 1; i >= 0; --i)
		{
			auto& pair = pairs[i];
			if (--pair.GetValue().FrameTime <= 0)
				PendingEffectDamageData.Remove(pair.GetKey());
		}
	}


	void ProcessExtraDamage(oCNpc::oSDamageDescriptor& desc, oCNpc* target)
	{
		DEBUG_MSG_DAM("ProcessExtraDamage", "ENTER", desc.pNpcAttacker, target);

		if (!target) {
			DEBUG_MSG_FUNC("ProcessExtraDamage", "target is null!");
			return;
		}

		if (target->IsDead() || target->IsUnconscious()){
			DEBUG_MSG_IF(target->IsDead(), "ProcessExtraDamage: target is dead!");
			DEBUG_MSG_IF(target->IsUnconscious(), "ProcessExtraDamage: target is unconscious!");
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

		if (damageReal <= 0) {
			DEBUG_MSG_FUNC("ProcessExtraDamage", "EXIT. RealDamage: " + Z(damageReal) + "!");
			return;
		}
		desc.fDamageTotal = static_cast<float>(damageTotal);
		desc.fDamageEffective = desc.fDamageReal = static_cast<float>(damageReal);

		int totalHp = target->attribute[0] + GetCurNpcEs(target);
		if(currentDamageMeta)
			UpdateDamageInfo(currentDamageMeta->DamageInfo, desc);
		target->ChangeAttribute(NPC_ATR_HITPOINTS, -damageReal);
		damageReal = totalHp - (target->attribute[0] + GetCurNpcEs(target));

		target->OnDamage_Condition(desc);
		target->OnDamage_Effects_End(desc);
		target->OnDamage_Events(desc);

		int damMsgType = StExt_DamageMessageType_Default;
		if (HasFlag(desc.dwFieldsValid, (ulong)DamageDescFlag_DotDamage)) damMsgType |= StExt_DamageMessageType_Dot;
		if (HasFlag(desc.dwFieldsValid, (ulong)DamageDescFlag_ReflectDamage)) damMsgType |= StExt_DamageMessageType_Reflect;
		parser->CallFunc(PrintDamageFunc, damageReal, damMsgType);

		DEBUG_MSG_DAM("ProcessExtraDamage", "EXIT. RealDamage: " + Z(damageReal), desc.pNpcAttacker, target);
	}

	HOOK Hook_oCNpc_EV_DamagePerFrame PATCH(&oCNpc::EV_DamagePerFrame, &oCNpc::EV_DamagePerFrame_StExt);
	int oCNpc::EV_DamagePerFrame_StExt(oCMsgDamage* msg)
	{
		bool hasMeta = false;
		if (msg && !msg->deleted)
		{
			oSDamageDescriptor& desc = msg->descDamage;
			if (!IsDamageDescriptorSane(desc))
			{
				DEBUG_MSG("EV_DamagePerFrame_StExt: BREAK: damage descriptor is corrupted!?");
				return True;
			}

			if (desc.pNpcAttacker)
			{
				DamageMeta& damageMeta = PushDamageMeta(this, desc);
				damageMeta.DamageInfo.DamageFlags |= StExt_DamageFlag_Dot;
				hasMeta = true;
			}
		}

		int result = THISCALL(Hook_oCNpc_EV_DamagePerFrame)(msg);
		if (hasMeta) PopDamageMeta();
		return result;
	}
	
	HOOK Hook_oCNpc_OnDamage PATCH (&oCNpc::OnDamage, &oCNpc::OnDamage_StExt);
	void oCNpc::OnDamage_StExt(oSDamageDescriptor& desc)
	{
		if (IsLevelChanging || IsLoading || !this) 
		{ 
			DEBUG_MSG_IF(!this, "OnDamage_StExt: BREAK: npc is Null!");
			DEBUG_MSG_IF(IsLevelChanging || IsLoading, "OnDamage_StExt: BREAK: can't process while level is changing!");
			return; 
		}

		if (!IsDamageDescriptorSane(desc))
		{
			DEBUG_MSG("OnDamage_StExt: BREAK: damage descriptor is corrupted!?");
			return;
		}

		DEBUG_MSG_DAM("OnDamage_StExt", "ENTER", desc.pNpcAttacker, this);
		DEBUG_MSG_DAMDESC(desc, "OnDamage_StExt - ENTER", this);
		DamageMeta& damageMeta = PushDamageMeta(this, desc);

		if (desc.pFXHit)
		{
			if (damageMeta.Attacker && !damageMeta.Attacker->IsSelfPlayer())
			{
				int applyFxDamage = *(int*)parser->CallFunc(FxDamageCanBeAppliedFunc);
				if (!applyFxDamage)
				{
					DEBUG_MSG_DAM("OnDamage_StExt", "EXIT. Reason: pFxHit is blocked (friendly fier)", desc.pNpcAttacker, this);
					PopDamageMeta();
					OnDamage_Effects_End(desc);
					return;
				}
			}

			auto fxDamagePair = PendingEffectDamageData.GetSafePair(desc.pFXHit);
			if (fxDamagePair)
			{
				damageMeta.SpellFxInDict = true;
				PendingDamage& pendingDamage = fxDamagePair->GetValue();
				if (!pendingDamage.IsMock)
				{
					int totalDamage = 0;
					int totalDotDamage = 0;
					for (int i = 0; i < oEDamageIndex_MAX; ++i)
					{
						totalDotDamage += pendingDamage.DamageInfo.DotDamage[i];
						if (pendingDamage.DamageInfo.Damage[i] <= 0) continue;

						totalDamage += pendingDamage.DamageInfo.Damage[i];
						desc.aryDamage[i] += static_cast<ulong>(pendingDamage.DamageInfo.Damage[i]);
						desc.enuModeDamage |= static_cast<ulong>(1 << i);
					}
					desc.fDamageReal += static_cast<float>(totalDamage);
					desc.dwFieldsValid |= pendingDamage.Flags;

					UpdateDamageInfo(damageMeta.DamageInfo, desc);
					damageMeta.DamageInfo.DamageType = pendingDamage.DamageInfo.DamageType;
					damageMeta.DamageInfo.DamageFlags = pendingDamage.DamageInfo.DamageFlags;

					if (totalDotDamage > 0)
						AddDotDamage(desc.pNpcAttacker, this, pendingDamage.DamageInfo);
				}
				else damageMeta.DamageInfo.StopAoe = true;
			}
		}

		if (damageMeta.IsOverlayDamage)
		{
			DEBUG_MSG_DAM("OnDamage_StExt", "EXIT. Reason: Handle overlay damage...", desc.pNpcAttacker, this);
			damageMeta.DamageInfo.DamageFlags |= StExt_DamageFlag_Dot;
			THISCALL(Hook_oCNpc_OnDamage)(desc);
			PopDamageMeta();
			return;
		}

		// Incoming damage is from my mod. Handle it separately and leave.
		if (!damageMeta.IsInitialDamage)
		{
			ProcessExtraDamage(desc, this);
			DEBUG_MSG_DAM("OnDamage_StExt", "EXIT. Reason: Extra damage handled.", desc.pNpcAttacker, this);
			PopDamageMeta();
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
		DEBUG_MSG_DAMMETA(damageMeta, "OnDamage");
		DEBUG_MSG_DAMINFO(damageMeta.DamageInfo, "OnDamage");

		// Call script damage post-process
		if (damageMeta.Attacker)
			parser->CallFunc(OnDamageAfterFunc);
		
		// prevent multi-aoe damage from spells
		// mark spellFx as processed
		if (desc.pFXHit && !damageMeta.SpellFxInDict)
		{
			PendingDamage mockDamage = PendingDamage{};
			mockDamage.IsMock = true;
			mockDamage.FrameTime = SPELLEFFECT_STORE_TIME;
			PendingEffectDamageData.Insert(desc.pFXHit, mockDamage);
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
			if (this->IsDead()) {
				DEBUG_MSG("ChangeAttribute_StExt: EXIT. Reason: npc already dead");
				return;
			}

			int damage = value * (-1);
			IncomingDamageInfo currentDamageInfo;
			DamageMeta* currentDamageMeta = GetDamageMeta();
			oCNpc* attaker = Null;
			oCItem* weapon = Null;

			if (currentDamageMeta && currentDamageMeta->Target == this && IsDamageDescriptorSane(*currentDamageMeta->Desc))
			{
				attaker = currentDamageMeta->Attacker;
				weapon = currentDamageMeta->Weapon;
				UpdateDamageInfo(currentDamageMeta->DamageInfo, *currentDamageMeta->Desc);
				CreateIncomingDamage(currentDamageInfo, currentDamageMeta);
				DEBUG_MSG("ChangeAttribute_StExt: used descPtr: " + Z((int)currentDamageMeta->Desc));
			}
			else
			{
				DEBUG_MSG_IF(currentDamageMeta && currentDamageMeta->Target != this, "ChangeAttribute_StExt: use DamageStackMeta for incorrect character!");
				DEBUG_MSG_IF(DamageInfoMetaData.IsPending && DamageInfoMetaData.Target != this, "ChangeAttribute_StExt: use DamageInfoMetaData for incorrect character!");

				// try retrieve last damage info
				if (DamageInfoMetaData.IsPending && DamageInfoMetaData.Target == this)
				{
					attaker = DamageInfoMetaData.Attacker;
					weapon = DamageInfoMetaData.Weapon;
					CreateIncomingDamage(currentDamageInfo, DamageInfoMetaData.DamageInfo);
					DamageInfoMetaData.IsPending = false;
				}
				else
				{
					DEBUG_MSG("ChangeAttribute_StExt: create empty IncomingDamageInfo!");
					CreateIncomingDamage(currentDamageInfo, damage);
				}
			}

			if (currentDamageInfo.DamageTotal != damage)
			{
				DEBUG_MSG("ChangeAttribute_StExt: actual damage and IncomingDamageInfo damages was different! Was: " + Z(currentDamageInfo.DamageTotal));
				currentDamageInfo.DamageTotal = damage;
			}
			if (attaker) currentDamageInfo.Flags |= StExt_IncomingDamageFlag_Index_HasAttacker;
			if (weapon) currentDamageInfo.Flags |= StExt_IncomingDamageFlag_Index_HasWeapon;

			DEBUG_MSG_DAM("ChangeAttribute_StExt", "ENTER. " + GetNpcHpEs(this) + " | Damage: " + Z(damage), attaker, this);
			DEBUG_MSG_DAMINCOMINFO(currentDamageInfo, "ChangeAttribute");

			SetScriptDamageActors(attaker, this, weapon);
			parser->SetInstance(StExt_IcomingDamageInfo_SymId, &currentDamageInfo);
			value += *(int*)parser->CallFunc(ProcessHpDamageFunc);
			DEBUG_MSG_DAM("ChangeAttribute_StExt", "AFTER SCRIPT. " + GetNpcHpEs(this) + " | Value: " + Z(value), attaker, this);

			if (value >= 0) 
			{
				DEBUG_MSG_FUNC("ChangeAttribute_StExt", "Damage to '" + SafeNpcName(this) + "' was absorbed! (value now: " + Z(value) + ")");
				parser->CallFunc(OnPostDamageFunc);
				parser->SetInstance(StExt_IcomingDamageInfo_SymId, Null);
				DamageInfoMetaData.IsPending = false;
				return;
			}

			damage = value * (-1);
			DEBUG_MSG_FUNC("ChangeAttribute_StExt", "Apply " + Z(damage) + " damage to '" + SafeNpcName(this) + "' ...");
			
			/*
			const bool isKill = ((this->attribute[0] + value) <= 0);
			const bool isExtraDamage = HasFlag(currentDamageInfo.Flags, StExt_IncomingDamageFlag_Index_ExtraDamage);
			const bool isExtraDamageDontKill = HasFlag(currentDamageInfo.Flags, StExt_IncomingDamageFlag_Index_DontKill);

			if (isKill && isExtraDamage && isExtraDamageDontKill) value = -(this->attribute[0] - 1);
			*/
			THISCALL(ivk_oCNpc_ChangeAttribute)(attrIndex, value);

			SetScriptDamageActors(attaker, this, weapon);
			parser->SetInstance(StExt_IcomingDamageInfo_SymId, &currentDamageInfo);
			parser->CallFunc(OnPostDamageFunc);

			parser->SetInstance(StExt_IcomingDamageInfo_SymId, Null);
			DamageInfoMetaData.IsPending = false;
			return;
		}
		THISCALL(ivk_oCNpc_ChangeAttribute)(attrIndex, value);
	}
}
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	ItemExtension::ItemExtension() 
	{
		InstanceName = zSTRING();
		BaseInstanceName = zSTRING();
		Preffix = zSTRING();
		Affix = zSTRING();
		Suffix = zSTRING();
		VisualEffect = zSTRING();
		FillArray(StatId, Invalid);
		FillArray(OwnStatId, Invalid);
		FillArray(AbilityId, Invalid);
	};

	void ItemExtension::Initialize()
	{
		Type = ItemClassKey_GetType(ItemClassID);
		Class = ItemClassKey_GetClass(ItemClassID);
		SubClass = ItemClassKey_GetSubClass(ItemClassID);
		ItemClassData = GetItemClassDescriptor(ItemClassID);
	}

	void ItemExtension::UpgradeDamage(const int valueDelta, const float mult)
	{
		if (valueDelta == 0 || !ItemClassData) return;

		int damageTotal = 0;
		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			int initialDamage = Properties[(int)ItemProperty::InitialDamageIndex + i];
			if (initialDamage <= 0) continue;

			float damageDelta = initialDamage * (valueDelta * mult);
			if (ItemClassData->PrimaryDamageTypesList.HasEqualSorted(i))
				damageDelta += damageDelta * ItemClassData->PrimaryDamageBonus;
			
			damageDelta *= ItemClassData->DamageMult;
			Damage[i] = ValidateValueMin(static_cast<int>(Damage[i] + damageDelta), 0);
			damageTotal += static_cast<int>(damageDelta);
		}
		DamageTotal += damageTotal;
		ValidateValueMin(DamageTotal, 0);
	}

	void ItemExtension::UpgradeProtection(const int valueDelta, const float mult)
	{
		if (valueDelta == 0 || !ItemClassData) return;

		for (int i = 0; i < oEDamageIndex_MAX; ++i)
		{
			int initialProtection = Properties[(int)ItemProperty::InitialProtectionIndex + i];
			if (initialProtection <= 0) continue;

			float protectionDelta = initialProtection * (valueDelta * mult);
			if (ItemClassData->PrimaryProtectionTypesList.HasEqualSorted(i))
				protectionDelta += protectionDelta * ItemClassData->PrimaryProtectionBonus;

			protectionDelta *= ItemClassData->ProtectionMult;
			Protection[i] = ValidateValueMin(static_cast<int>(Protection[i] + protectionDelta), 0);
		}
	}

	void ItemExtension::UpgradeCondition(const int valueDelta, const float mult)
	{
		if (valueDelta == 0 || !ItemClassData) return;

		for (int i = 0; i < ItemExtension_Conditions_Max; ++i)
		{
			int initialCondAtrId = Properties[(int)ItemProperty::InitialConditionAttributeIndex + i];
			int initialCondValue = Properties[(int)ItemProperty::InitialConditionAttributeValueIndex + i];
			if ((initialCondAtrId != CondAtr[i]) || (initialCondAtrId == 0)) continue;

			const ExtraStatData* condData = GetExtraConditionDataById(initialCondAtrId);
			if (!condData) continue;

			float conditionDelta = initialCondValue * (valueDelta * mult);
			conditionDelta *= ItemClassData->ConditionMult;
			CondValue[i] = ValidateValue(static_cast<int>(CondValue[i] + conditionDelta), condData->RollMinCap, condData->RollMaxCap);
		}
	}

	byte ItemExtension::GetTag(const int tagId) { return (tagId < 0 || tagId >= ItemExtension_Tags_Max) ? 0 : Tags[tagId]; }
	byte ItemExtension::GetCraftFlag(const int flagId) { return (flagId < 0 || flagId >= ItemExtension_CraftFlags_Max) ? 0 : CraftFlags[flagId]; }
	int ItemExtension::GetCraftData(const int dataId) { return (dataId < 0 || dataId >= ItemExtension_CraftData_Max) ? Invalid : CraftData[dataId]; }

	void ItemExtension::SetTag(const int tagId, const byte tagValue) { if (IsIndexInBounds(tagId, ItemExtension_Tags_Max)) { Tags[tagId] = tagValue; } }
	void ItemExtension::SetCraftFlag(const int flagId, const byte flagValue) { if (IsIndexInBounds(flagId, ItemExtension_CraftFlags_Max)) { CraftFlags[flagId] = flagValue; } }
	void ItemExtension::SetCraftData(const int dataId, const int dataValue) { if (IsIndexInBounds(dataId, ItemExtension_CraftData_Max)) { CraftData[dataId] = dataValue; } }

	void ItemExtension::ChangeLevel(const int value)
	{
		if (value == 0 || !ItemClassData) return;

		const int valueNow = ValidateValue(Level + value, 1, ItemsGeneratorConfigs.ItemLevelMax);
		const int valueWas = Level;
		const int valueDelta = valueNow - valueWas;
		Level = valueNow;

		if(ItemClassData->UpgradeDamageOnLevelChange) 
			UpgradeDamage(valueDelta, ItemClassData->DamageLevelBonus);
		if (ItemClassData->UpgradeProtectionOnLevelChange)
			UpgradeProtection(valueDelta, ItemClassData->ProtectionLevelBonus);
		if (ItemClassData->UpgradeConditionsOnLevelChange)
			UpgradeCondition(valueDelta, ItemClassData->ConditionLevelBonus);
	}

	void ItemExtension::ChangeRank(const int value)
	{
		if (value == 0 || !ItemClassData) return;

		const int valueNow = ValidateValue(Rank + value, 0, ItemsGeneratorConfigs.ItemRankMax);
		const int valueWas = Rank;
		const int valueDelta = valueNow - valueWas;
		Rank = valueNow;

		if (ItemClassData->UpgradeDamageOnRankChange)
			UpgradeDamage(valueDelta, ItemClassData->DamageRankBonus);
		if (ItemClassData->UpgradeProtectionOnRankChange)
			UpgradeProtection(valueDelta, ItemClassData->ProtectionRankBonus);
		if (ItemClassData->UpgradeConditionsOnRankChange)
			UpgradeCondition(valueDelta, ItemClassData->ConditionRankBonus);
	}

	void ItemExtension::ChangeQuality(const int value)
	{
		if (value == 0 || !ItemClassData) return;

		const int maxCap = Rank >= ItemsGeneratorConfigs.ItemRankForExtraQualityThreshold ?
			ItemsGeneratorConfigs.ItemQualityHardCap : ItemsGeneratorConfigs.ItemQualitySoftCap;
		const int valueNow = ValidateValue(Quality + value, 0, maxCap);
		const int valueWas = Quality;
		const int valueDelta = valueNow - valueWas;
		Quality = valueNow;

		if (ItemClassData->UpgradeDamageOnQualityChange)
			UpgradeDamage(valueDelta, ItemClassData->DamageQualityBonus);
		if (ItemClassData->UpgradeProtectionOnQualityChange)
			UpgradeProtection(valueDelta, ItemClassData->ProtectionQualityBonus);
		if (ItemClassData->UpgradeConditionsOnQualityChange)
			UpgradeCondition(valueDelta, ItemClassData->ExtraConditionQualityBonus);
	}

	void ItemExtension::UpdatePrice()
	{
		float priceMult = 1.0f + (Level * ItemClassData->PriceLevelBonus) + (Rank * ItemClassData->PriceRankBonus) + (Quality * ItemClassData->PriceQualityBonus);
		int itemExtraCost = Properties[(int)ItemProperty::AdditionalCost];
		for (int i = 0; i < ItemExtension_Stats_Max; ++i)
		{
			if (StatId[i] == Invalid) continue;
			const ExtraStatData* stat = GetExtraStatDataById(StatId[i]);
			if (!stat) continue;

			const int statCost = static_cast<int>((StatValue[i] * stat->CostPerStat) * ItemsGeneratorConfigs.ItemStatPriceMult);
			itemExtraCost += statCost <= 0 ? 1 : statCost;
		}

		for (int i = 0; i < ItemExtension_OwnStats_Max; ++i)
		{
			if (OwnStatId[i] == Invalid) continue;
			const ExtraStatData* stat = GetExtraStatDataById(StatId[i]);
			if (!stat) continue;

			const int statCost = static_cast<int>((OwnStatValue[i] * stat->CostPerStat) * ItemsGeneratorConfigs.ItemStatPriceMult);
			itemExtraCost += statCost <= 0 ? 1 : statCost;
		}

		// TODO - count abilities

		Cost = ValidateValueMin(static_cast<int>(((Properties[(int)ItemProperty::InitialCost] + itemExtraCost) * priceMult) * ItemClassData->PriceMult), 1);		
	}

	int ItemExtension::GetProperty(const int propertyId) { return (IsIndexInBounds(propertyId, ItemExtension_Props_Max)) ? Properties[propertyId] : 0; }
	void ItemExtension::SetProperty(const int propertyId, const int propertyValue) { if (IsIndexInBounds(propertyId, ItemExtension_Props_Max)) { Properties[propertyId] = propertyValue; } }
	void ItemExtension::ChangeProperty(const int propertyId, const int propertyValue) { if (IsIndexInBounds(propertyId, ItemExtension_Props_Max)) { Properties[propertyId] += propertyValue; } }

	void ItemExtension::AddDamage(const int damage, const int damType)
	{
		if (!IsIndexInBounds(damType, (int)oEDamageIndex_MAX)) return;
		int dam = damage;
		DamageTypes |= 1 << damType;
		DamageTotal += damage;
		Damage[damType] += damage;
	}

	void ItemExtension::AddProtection(const int protection, const int protType)
	{
		if (!IsIndexInBounds(protType, (int)oEDamageIndex_MAX)) return;
		Protection[protType] = ValidateValueMin(Protection[protType] + protection, 0);
	}

	void ItemExtension::AddSpecialDamage(const int damageMin, const int damageMax, const int damType)
	{
		SpecialDamageTypes |= damType;
		SpecialDamageMax += ValidateValueMin(SpecialDamageMax + damageMax, 0);
		SpecialDamageMin += ValidateValue(SpecialDamageMin + damageMin, 0, SpecialDamageMax);
	}

	void ItemExtension::AddSpecialProtection(const int protection, const int protType)
	{
		SpecialProtectionTypes |= protType;
		SpecialProtection = ValidateValueMin(SpecialProtection + protection, 0);
	}

	int ItemExtension::FindStat(const int statId) { return FindIndexInArray(StatId, statId); }
	int ItemExtension::FindAbility(const int abilityId) { return FindIndexInArray(AbilityId, abilityId); }
	int ItemExtension::FindCondition(const int conditionId) { return FindIndexInArray(CondAtr, conditionId); }

	int ItemExtension::GetStats(Array<int>& buffer)
	{
		buffer.Clear();
		for (int i = 0; i < ItemExtension_Stats_Max; ++i)
		{
			if (StatId[i] == Invalid) continue;
			buffer.InsertEnd(StatId[i]);
		}
		buffer.QuickSort();
		return static_cast<int>(buffer.GetNum());
	}

	int ItemExtension::GetAbilities(Array<int>& buffer)
	{
		buffer.Clear();
		for (int i = 0; i < ItemExtension_Abilities_Max; ++i)
		{
			if (AbilityId[i] == Invalid) continue;
			buffer.InsertEnd(AbilityId[i]);
		}
		return static_cast<int>(buffer.GetNum());
	}

	int ItemExtension::GetConditions(Array<int>& buffer)
	{
		buffer.Clear();
		for (int i = 0; i < ItemExtension_Conditions_Max; ++i)
		{
			if (CondAtr[i] == 0 || CondAtr[i] == Invalid) continue;
			buffer.InsertEnd(CondAtr[i]);
		}
		return static_cast<int>(buffer.GetNum());
	}


	void ItemExtension::AddStat(const ExtraStatData* statData, const int value, const int duration)
	{
		if (!statData || value <= 0) return;

		int statCellIndex = FindStat(statData->Id);
		if (statCellIndex == Invalid)
			statCellIndex = FindEmptySlotIndex(StatId, Properties[(int)ItemProperty::StatsMax], Invalid);
		if (statCellIndex == Invalid) return;

		StatId[statCellIndex] = statData->Id;
		StatValue[statCellIndex] = ValidateValue(StatValue[statCellIndex] + value, statData->MinCap, statData->MaxCap);
		StatDuration[statCellIndex] += duration;
	}
	void ItemExtension::AddStat(const int statId, const int value, const int duration)
	{
		const ExtraStatData* statData = GetExtraStatDataById(statId);
		ItemExtension::AddStat(statData, value, duration);
	}

	void ItemExtension::AddAbility(const ItemAbility* abilityData, const int value, const int chance, const int duration, const int range)
	{
		if (!abilityData) return;

		int abilityCellIndex = FindAbility(abilityData->UId);
		if (abilityCellIndex == Invalid)
			abilityCellIndex = FindEmptySlotIndex(StatId, Properties[(int)ItemProperty::AbilitiesMax], Invalid);
		if (abilityCellIndex == Invalid) return;

	}
	void ItemExtension::AddAbility(const int abilityId, const int value, const int chance, const int duration, const int range)
	{
		const ItemAbility* abilityData = GetItemAbility(abilityId);
		ItemExtension::AddAbility(abilityData, value, chance, duration, range);
	}

	void ItemExtension::AddCondition(const ExtraStatData* conditionData, const int value)
	{
		if (!conditionData) return;

		int condCellIndex = FindCondition(conditionData->Id);
		if (condCellIndex == Invalid) 
			condCellIndex = FindEmptySlotIndex(CondAtr, Invalid, 0);
		if (condCellIndex == Invalid) return;

		CondAtr[condCellIndex] = conditionData->Id;
		CondValue[condCellIndex] = ValidateValue(CondValue[condCellIndex] + value, conditionData->MinCap, conditionData->MaxCap);
	}
	void ItemExtension::AddCondition(const int conditionId, const int value)
	{
		if (conditionId == 0) return;
		const ExtraStatData* conditionData = GetExtraConditionDataById(conditionId);
		AddCondition(conditionData, value);
	}

	int ItemExtension::GetFreeSockets()	{ return ValidateValueMin(Properties[(int)ItemProperty::SocketsMax] - Properties[(int)ItemProperty::SocketsUsed], 0); }
	void ItemExtension::AddSockets(const int sockets) 
	{ 
		if (!ItemClassData || sockets <= 0) return;
		Properties[(int)ItemProperty::SocketsMax] = ValidateValue(Properties[(int)ItemProperty::SocketsMax] + sockets, 0, ItemClassData->SocketsCap);
	}
	void ItemExtension::FillSockets(const int sockets) 
	{
		if ((sockets <= 0) || (sockets > GetFreeSockets())) return;
		Properties[(int)ItemProperty::SocketsUsed] = ValidateValue(Properties[(int)ItemProperty::SocketsUsed] + sockets, 0, Properties[(int)ItemProperty::SocketsMax]);
	}


	void ItemExtension::Archive(zCArchiver& arc)
	{
		arc.WriteRaw("UId", &UId, sizeof(UId));
		arc.WriteString("InstanceName", InstanceName);
		arc.WriteString("BaseInstanceName", BaseInstanceName);

		arc.WriteRaw("ItemClassID", &ItemClassID, sizeof(ItemClassID));
		arc.WriteRaw("Flags", &Flags, sizeof(Flags));
		arc.WriteRaw("Tags", Tags, sizeof(Tags));

		// crafting
		arc.WriteRaw("CraftData", CraftData, sizeof(CraftData));
		arc.WriteRaw("CraftFlags", CraftFlags, sizeof(CraftFlags));

		// names / affixes
		arc.WriteString("OwnName", OwnName);
		arc.WriteString("Preffix", Preffix);
		arc.WriteString("Affix", Affix);
		arc.WriteString("Suffix", Suffix);
		arc.WriteString("AttachedSoulName", AttachedSoulName);

		// visual effect
		arc.WriteString("VisualEffect", VisualEffect);
		arc.WriteRaw("VisualEffectData", VisualEffectData, sizeof(VisualEffectData));

		// level/quality/rank/properties
		arc.WriteInt("Level", Level);
		arc.WriteInt("Rank", Rank);
		arc.WriteInt("Quality", Quality);
		arc.WriteRaw("Properties", Properties, sizeof(Properties));

		// special damage / protection
		arc.WriteInt("SpecialDamageMin", SpecialDamageMin);
		arc.WriteInt("SpecialDamageMax", SpecialDamageMax);
		arc.WriteRaw("SpecialDamageTypes", &SpecialDamageTypes, sizeof(SpecialDamageTypes));

		arc.WriteInt("SpecialProtection", SpecialProtection);
		arc.WriteRaw("SpecialProtectionTypes", &SpecialProtectionTypes, sizeof(SpecialProtectionTypes));

		// extra flags
		arc.WriteInt("ExtraFlags_Main", ExtraFlags_Main);
		arc.WriteInt("ExtraFlags_Base", ExtraFlags_Base);
		arc.WriteInt("ExtraFlags_Additional", ExtraFlags_Additional);

		// conditions
		arc.WriteRaw("CondAtr", CondAtr, sizeof(CondAtr));
		arc.WriteRaw("CondValue", CondValue, sizeof(CondValue));

		// damage/protection arrays
		arc.WriteInt("DamageTypes", DamageTypes);
		arc.WriteInt("DamageTotal", DamageTotal);
		arc.WriteRaw("Damage", Damage, sizeof(Damage));
		arc.WriteRaw("Protection", Protection, sizeof(Protection));
		arc.WriteInt("Range", Range);

		// cost/weight
		arc.WriteInt("Cost", Cost);
		arc.WriteInt("Weight", Weight);

		// stats
		arc.WriteRaw("StatId", StatId, sizeof(StatId));
		arc.WriteRaw("StatValue", StatValue, sizeof(StatValue));
		arc.WriteRaw("StatDuration", StatDuration, sizeof(StatDuration));

		// own stats
		arc.WriteRaw("OwnStatId", OwnStatId, sizeof(OwnStatId));
		arc.WriteRaw("OwnStatValue", OwnStatValue, sizeof(OwnStatValue));

		// abilities
		arc.WriteRaw("AbilityId", AbilityId, sizeof(AbilityId));
		arc.WriteRaw("AbilityChance", AbilityChance, sizeof(AbilityChance));
		arc.WriteRaw("AbilityValue", AbilityValue, sizeof(AbilityValue));
		arc.WriteRaw("AbilityDuration", AbilityDuration, sizeof(AbilityDuration));
	}

	void ItemExtension::UnArchive(zCArchiver& arc)
	{
		arc.ReadRaw("UId", &UId, sizeof(UId));
		arc.ReadString("InstanceName", InstanceName);
		arc.ReadString("BaseInstanceName", BaseInstanceName);

		arc.ReadRaw("ItemClassID", &ItemClassID, sizeof(ItemClassID));
		arc.ReadRaw("Flags", &Flags, sizeof(Flags));
		arc.ReadRaw("Tags", Tags, sizeof(Tags));

		// crafting
		arc.ReadRaw("CraftData", CraftData, sizeof(CraftData));
		arc.ReadRaw("CraftFlags", CraftFlags, sizeof(CraftFlags));

		// names / affixes
		arc.ReadString("OwnName", OwnName);
		arc.ReadString("Preffix", Preffix);
		arc.ReadString("Affix", Affix);
		arc.ReadString("Suffix", Suffix);
		arc.ReadString("AttachedSoulName", AttachedSoulName);

		// visual effect
		arc.ReadString("VisualEffect", VisualEffect);
		arc.ReadRaw("VisualEffectData", VisualEffectData, sizeof(VisualEffectData));

		// level/quality/rank/properties
		arc.ReadInt("Level", Level);
		arc.ReadInt("Rank", Rank);
		arc.ReadInt("Quality", Quality);
		arc.ReadRaw("Properties", Properties, sizeof(Properties));

		// special damage / protection
		arc.ReadInt("SpecialDamageMin", SpecialDamageMin);
		arc.ReadInt("SpecialDamageMax", SpecialDamageMax);
		arc.ReadRaw("SpecialDamageTypes", &SpecialDamageTypes, sizeof(SpecialDamageTypes));

		arc.ReadInt("SpecialProtection", SpecialProtection);
		arc.ReadRaw("SpecialProtectionTypes", &SpecialProtectionTypes, sizeof(SpecialProtectionTypes));

		// extra flags
		arc.ReadInt("ExtraFlags_Main", ExtraFlags_Main);
		arc.ReadInt("ExtraFlags_Base", ExtraFlags_Base);
		arc.ReadInt("ExtraFlags_Additional", ExtraFlags_Additional);

		// conditions
		arc.ReadRaw("CondAtr", CondAtr, sizeof(CondAtr));
		arc.ReadRaw("CondValue", CondValue, sizeof(CondValue));

		// damage/protection arrays
		arc.ReadInt("DamageTypes", DamageTypes);
		arc.ReadInt("DamageTotal", DamageTotal);
		arc.ReadRaw("Damage", Damage, sizeof(Damage));
		arc.ReadRaw("Protection", Protection, sizeof(Protection));
		arc.ReadInt("Range", Range);

		// cost/weight
		arc.ReadInt("Cost", Cost);
		arc.ReadInt("Weight", Weight);

		// stats
		arc.ReadRaw("StatId", StatId, sizeof(StatId));
		arc.ReadRaw("StatValue", StatValue, sizeof(StatValue));
		arc.ReadRaw("StatDuration", StatDuration, sizeof(StatDuration));

		// own stats
		arc.ReadRaw("OwnStatId", OwnStatId, sizeof(OwnStatId));
		arc.ReadRaw("OwnStatValue", OwnStatValue, sizeof(OwnStatValue));

		// abilities
		arc.ReadRaw("AbilityId", AbilityId, sizeof(AbilityId));
		arc.ReadRaw("AbilityChance", AbilityChance, sizeof(AbilityChance));
		arc.ReadRaw("AbilityValue", AbilityValue, sizeof(AbilityValue));
		arc.ReadRaw("AbilityDuration", AbilityDuration, sizeof(AbilityDuration));

		Initialize();
	}

	ItemExtension::~ItemExtension() { };
}
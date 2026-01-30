#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	struct ItemRandomRollDescriptor
	{
		ItemExtension* ItemExtensionData;
		const ItemClassDescriptor* ItemClassData;

		const ItemClassKey ItemFullClass;
		const byte ItemType;
		const byte ItemClass;
		const byte ItemSubClass;

		const int ItemPower;
		bool IsValid;
		bool IsMagic;

		int ItemPrototypeTier;
		zSTRING ItemBaseInstance;

		int ItemLevel;
		int ItemRank;
		int ItemQuality;
		int ItemSockets;
		int ItemWeight;

		int ItemStatsCount;
		int ItemStatsCountMin;
		int ItemStatsCountMax;

		int ItemAbilitiesCount;
		int ItemExtraCondCount;

		int ExtraProtectionsCount;
		int SpecialProtectionsCount;

		int ExtraDamagesCount;
		int SpecialDamagesCount;

		int ItemAffixesCount;
		zSTRING ItemAffixesList[3];

		ItemRandomRollDescriptor(int power, ItemClassKey classKey) : 
			ItemExtensionData(Null), ItemPower(power),
			ItemFullClass(classKey),
			ItemClassData(GetItemClassDescriptor(classKey)),  
			ItemType(ItemClassKey_GetType(classKey)), ItemClass(ItemClassKey_GetClass(classKey)), ItemSubClass(ItemClassKey_GetSubClass(classKey)),
			ItemBaseInstance(), ItemAffixesList() 
		{ IsValid = ItemClassData != Null; }
	};

	void InitializeRandomItemRoll(ItemRandomRollDescriptor& desc)
	{
		if (!desc.ItemClassData)
		{
			DEBUG_MSG("InitializeRandomItemRoll - item class descriptor: " + Z ((int)desc.ItemFullClass) + " not found!");
			desc.IsValid = false;
			return;
		}

		desc.ItemPrototypeTier = CalcItemTier(desc.ItemPower, desc.ItemClassData);
		desc.ItemBaseInstance = RollPrototypeInstanceName(desc.ItemPrototypeTier, desc.ItemClassData);
		desc.ItemExtensionData = CreateItemExtension(desc.ItemPower, desc.ItemBaseInstance, desc.ItemClassData);
		if (!desc.ItemExtensionData)
		{
			DEBUG_MSG("InitializeRandomItemRoll - Fail to create ItemExtension data! Item class descriptor: " + Z((int)desc.ItemFullClass) + ". Item prototype: '" + desc.ItemBaseInstance + "'");
			desc.IsValid = false;
			return;
		}
		desc.ItemExtensionData->BaseInstanceName = desc.ItemBaseInstance;

		desc.ItemLevel = RollItemLevel(desc.ItemPower);
		desc.ItemRank = desc.IsMagic ? RollItemRank(desc.ItemPower) : 0;
		desc.ItemQuality = RollItemQuality(desc.ItemPower, desc.ItemRank);
		desc.ItemSockets = desc.ItemClassData->SocketsAllowed ? RollItemMaxSockets(desc.ItemLevel, desc.ItemRank, desc.ItemQuality, 0, desc.ItemClassData) : 0;

		desc.ItemExtraCondCount = desc.ItemClassData->ExtraConditionAllowed ? RollItemExtraConditionsCount(desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;
		desc.ExtraProtectionsCount = desc.ItemClassData->ExtraProtectionAllowed ? RollItemExtraProtectionsCount(desc.ItemQuality, desc.ItemClassData) : 0;
		desc.ExtraDamagesCount = desc.ItemClassData->ExtraDamageAllowed ? RollItemExtraDamagesCount(desc.ItemQuality, desc.ItemClassData) : 0;
		desc.ItemWeight = desc.ItemClassData->ModWeight ? RollItemWeight(desc.ItemClassData) : Invalid;
		
		if (desc.IsMagic)
		{
			const int affixRollChance = static_cast<int>
				((desc.ItemClassData->AffixRollChance + desc.ItemPower) * ItemsGeneratorConfigs.ItemAffixesRollChanceMult);

			desc.ItemAffixesCount = 0;
			for (int i = 0; i < 3; ++i) {
				if (RollItemAffix(affixRollChance, i, desc.ItemAffixesList[i]))
					++desc.ItemAffixesCount;
			}
			if (desc.ItemAffixesCount <= 0) 
			{
				const unsigned int index = StExt_Rand::Index(3U);
				RollItemAffix(1000, index, desc.ItemAffixesList[index]);
				desc.ItemAffixesCount = 1;
			}

			desc.SpecialProtectionsCount = (desc.ItemClassData->SpecialProtectionAllowed && (desc.ItemRank >= ItemsGeneratorConfigs.ItemRankForSpecialProtectionThreshold)) ?
				RollItemSpecialProtectionsCount(desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;

			desc.SpecialDamagesCount = (desc.ItemClassData->SpecialDamageAllowed && (desc.ItemRank >= ItemsGeneratorConfigs.ItemRankForSpecialDamageThreshold)) ?
				RollItemSpecialDamagesCount(desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;

			desc.ItemStatsCount = desc.ItemClassData->StatsAllowed ?
				RollItemStatsCount(desc.ItemStatsCountMin, desc.ItemStatsCountMax, desc.ItemAffixesCount, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;

			desc.ItemAbilitiesCount = (desc.ItemClassData->AbilitiesAllowed && (desc.ItemRank >= ItemsGeneratorConfigs.ItemRankForAbilityThreshold)) ?
				RollItemAbilitiesCount(desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;
		}
		else
		{
			desc.ItemAbilitiesCount = 0;
			desc.ItemAffixesCount = 0;
			desc.ItemStatsCount = desc.ItemClassData->StatsCountMin;
			desc.SpecialProtectionsCount = 0;
		}
		desc.ItemAbilitiesCount = 0;
	}

	void RollItemConditions(ItemRandomRollDescriptor& desc)
	{
		if (desc.ItemExtraCondCount <= 0 || !desc.ItemClassData->ExtraConditionAllowed) return;
		
		Array<int> baseConditions;
		const int baseConditionsCount = desc.ItemExtensionData->GetConditions(baseConditions);
		if (baseConditionsCount >= ItemExtension_Conditions_Max) return;

		int emptySlotsCount = ValidateValueMax(ItemExtension_Conditions_Max - baseConditionsCount, desc.ItemExtraCondCount);
		int failCheck = 0;
		while (emptySlotsCount > 0)
		{
			const int condId = RollItemExtraConditionId(baseConditions, desc.ItemClassData);
			if (condId != Invalid && condId != 0)
			{
				const ExtraStatData* condData = GetExtraConditionDataById(condId);
				if (!condData) 
				{ 
					++failCheck;
					if (failCheck > 4)
						--emptySlotsCount;
					continue;
				}

				const int condValue = RollItemExtraConditionValue(condData, desc.ItemPower, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData);
				desc.ItemExtensionData->AddCondition(condData, condValue);
				baseConditions.InsertEnd(condId);

				if (baseConditions.GetNum() >= ItemExtension_Conditions_Max) break;
			}
			--emptySlotsCount;
		}

		//if none found
		baseConditions.Clear();
		int conditionsCount = desc.ItemExtensionData->GetConditions(baseConditions);
		if (!conditionsCount && !desc.ItemClassData->ConditionsList.IsEmpty())
		{
			const int condId = desc.ItemClassData->ConditionsList[StExt_Rand::Index(desc.ItemClassData->ConditionsList.GetNum())];
			const ExtraStatData* condData = GetExtraConditionDataById(condId);
			if (condData)
			{
				const int condValue = RollItemExtraConditionValue(condData, desc.ItemPower, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData);
				desc.ItemExtensionData->AddCondition(condData, condValue);
			}
		}
	}

	void RollItemExtraProtection(ItemRandomRollDescriptor& desc)
	{
		if ((desc.ExtraProtectionsCount <= 0) || (desc.ItemQuality <= 0) || (!desc.ItemClassData->ExtraProtectionAllowed)) return;
		
		Array<int> protections;
		for (int i = 0; i < 7; ++i) protections.Insert(Invalid);
		StExt_Rand::RandomSequence(protections, 1, 7);

		for (int i = 0; i < desc.ExtraProtectionsCount; ++i)
		{
			if (!protections.GetSafe(i)) continue;
			if ((protections[i] != 0) && (protections[i] != Invalid))
			{
				int value = RollItemExtraProtectionValue(desc.ItemQuality + desc.ItemLevel, desc.ItemClassData);
				if (value > 0)
					desc.ItemExtensionData->AddProtection(value, protections[i]);
			}
		}
	}

	void RollItemSpecialProtection(ItemRandomRollDescriptor& desc)
	{
		if ((desc.SpecialProtectionsCount <= 0) || (!desc.ItemClassData->SpecialProtectionAllowed)) return;

		const uint protectionsCount = ItemsGeneratorConfigs.SpecialProtectionTypesList.GetNum();
		if (protectionsCount == 0U) return;

		for (int i = 0; i < desc.SpecialProtectionsCount; ++i)
		{
			const int id = ItemsGeneratorConfigs.SpecialProtectionTypesList[StExt_Rand::Index(protectionsCount)];
			const int value = RollItemSpecialProtectionValue(desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData);
			if ((id != 0) && (value > 0))
				desc.ItemExtensionData->AddSpecialProtection(value, id);
		}
	}

	void RollItemExtraDamage(ItemRandomRollDescriptor& desc)
	{
		if ((desc.ExtraDamagesCount <= 0) || (desc.ItemQuality <= 0) || (!desc.ItemClassData->ExtraDamageAllowed)) return;

		Array<int> damages;
		for (int i = 0; i < 6; ++i) damages.Insert(Invalid);
		StExt_Rand::RandomSequence(damages, 1, 6);

		for (int i = 0; i < desc.ExtraDamagesCount; ++i)
		{
			if (!damages.GetSafe(i)) continue;
			if ((damages[i] != 0) && (damages[i] != Invalid))
			{
				if (desc.ItemClassData->IncopatibleDamageTypesList.HasEqualSorted(damages[i])) continue;
				int value = RollItemExtraDamageValue(desc.ItemQuality + desc.ItemLevel, desc.ItemClassData);
				if (value > 0)
					desc.ItemExtensionData->AddDamage(value, damages[i]);
			}
		}
	}

	void RollItemSpecialDamage(ItemRandomRollDescriptor& desc)
	{
		if ((desc.SpecialDamagesCount <= 0) || (!desc.ItemClassData->SpecialDamageAllowed)) return;

		const uint damagesCount = ItemsGeneratorConfigs.SpecialDamageTypesList.GetNum();
		if (damagesCount == 0U) return;

		for (int i = 0; i < desc.SpecialDamagesCount; ++i)
		{
			const int id = ItemsGeneratorConfigs.SpecialDamageTypesList[StExt_Rand::Index(damagesCount)];
			const int valueMax = RollItemSpecialDamageValue(desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData);
			if ((id != 0) && (valueMax > 0))
			{
				const int valueMin = ValidateValue(static_cast<int>(StExt_Rand::Range(valueMax * 0.05f, valueMax * 0.50f)), 1, static_cast<int>(valueMax * 0.50f));
				desc.ItemExtensionData->AddSpecialDamage(valueMin, valueMax, id);
			}
		}
	}

	void RollItemStats(ItemRandomRollDescriptor& desc)
	{
		if (desc.ItemStatsCount <= 0 || !desc.ItemClassData->StatsAllowed) return;

		Array<int> baseStats = Array<int>();
		const int baseStatsCount = desc.ItemExtensionData->GetStats(baseStats);
		if (baseStatsCount >= desc.ItemStatsCountMax) return;

		int emptySlotsCount = ValidateValueMax(desc.ItemStatsCount, desc.ItemStatsCountMax - baseStatsCount);
		if (emptySlotsCount <= 0) return;

		Array<int> rolledStats = Array<int>();
		RollItemStatsChunk(rolledStats, baseStats, emptySlotsCount, desc.ItemClassData);
		for (uint i = 0; i < rolledStats.GetNum(); ++i)
		{
			const ExtraStatData* statData = GetExtraStatDataById(rolledStats[i]);
			if (!statData) continue;

			const int statValue = RollItemStatValue(statData, desc.ItemPower, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData);
			const int statDuration = desc.ItemClassData->StatsDurationAllowed ? 
				RollItemStatDuration(statData->Id, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;
			desc.ItemExtensionData->AddStat(statData->Id, statValue, statDuration);
		}
	}

	void RollItemAbilities(ItemRandomRollDescriptor& desc)
	{
		if (desc.ItemAbilitiesCount <= 0 || !desc.ItemClassData->AbilitiesAllowed) return;

		Array<int> baseAbilities = Array<int>();
		const int baseAbilitiesCount = desc.ItemExtensionData->GetAbilities(baseAbilities);
		if (baseAbilitiesCount >= desc.ItemClassData->AbilitiesCountCap) return;

		int emptySlotsCount = ValidateValueMax(desc.ItemAbilitiesCount, desc.ItemClassData->AbilitiesCountCap - baseAbilitiesCount);
		if (emptySlotsCount <= 0) return;

		Array<int> rolledAbilities = Array<int>();
		RollItemAbilitiesChunk(rolledAbilities, baseAbilities, emptySlotsCount, desc.ItemClassData);

		for (uint i = 0; i < rolledAbilities.GetNum(); ++i)
		{
			const ItemAbility* abilityData = GetItemAbility(rolledAbilities[i]);
			if (!abilityData) continue;

			const bool isPassive = HasFlag(abilityData->AbilityFlags, ItemAbilityFlags::IsPassive);

			const int abilityValue = !isPassive && HasFlag(abilityData->AbilityFlags, ItemAbilityFlags::HasValue) ? 
				RollItemAbilitiesValue(abilityData, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;
			const int abilityDuration = !isPassive && HasFlag(abilityData->AbilityFlags, ItemAbilityFlags::HasDuration) ?
				RollItemAbilitiesDuration(abilityData, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;
			const int abilityChance = !isPassive && HasFlag(abilityData->AbilityFlags, ItemAbilityFlags::HasChance) ?
				RollItemAbilitiesChance(abilityData, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;
			const int abilityRange = !isPassive && HasFlag(abilityData->AbilityFlags, ItemAbilityFlags::HasRange) ?
				RollItemAbilitiesRange(abilityData, desc.ItemLevel, desc.ItemRank, desc.ItemQuality, desc.ItemClassData) : 0;

			desc.ItemExtensionData->AddAbility(abilityData->UId, abilityValue, abilityChance, abilityDuration, abilityRange);
		}
	}

	void RollItemBaseStats(ItemRandomRollDescriptor& desc)
	{
		desc.ItemExtensionData->ChangeLevel(desc.ItemLevel);
		desc.ItemExtensionData->ChangeRank(desc.ItemRank);
		desc.ItemExtensionData->ChangeQuality(desc.ItemQuality);

		if(desc.ItemClassData->SocketsAllowed) desc.ItemExtensionData->AddSockets(desc.ItemSockets);

		desc.ItemExtensionData->SetProperty((int)ItemProperty::StatsMax, desc.ItemStatsCountMax);
		desc.ItemExtensionData->SetProperty((int)ItemProperty::AbilitiesMax, desc.ItemClassData->AbilitiesCountCap);

		if (desc.ItemClassData->RangeSpreadAllowed)
		{
			const int baseRange = desc.ItemExtensionData->GetProperty((int)ItemProperty::InitialRange);
			desc.ItemExtensionData->Range = RollItemRange(baseRange, desc.ItemQuality, desc.ItemClassData);
		}
		desc.ItemExtensionData->Weight = desc.ItemWeight;

		RollItemExtraDamage(desc);
		RollItemExtraProtection(desc);
		RollItemStats(desc);
		RollItemConditions(desc);
	}

	void RollItemMagic(ItemRandomRollDescriptor& desc)
	{
		desc.ItemExtensionData->Preffix = desc.ItemAffixesList[0];
		desc.ItemExtensionData->Affix = desc.ItemAffixesList[1];
		desc.ItemExtensionData->Suffix = desc.ItemAffixesList[2];

		RollItemSpecialProtection(desc);
		RollItemSpecialDamage(desc);
		RollItemAbilities(desc);

		if (StExt_Rand::Permille(ItemsGeneratorConfigs.ItemRollAsUndefinedChance))
			desc.ItemExtensionData->SetTag((int)ItemTags::Unidentified, 1);

		// ToDo: apply infusion
	}

	void FinalizeRandomItemRoll(ItemRandomRollDescriptor& desc)
	{
		desc.ItemExtensionData->UpdatePrice();
	}


	ItemExtension* RollSpecificMagicItem(const int power, const ItemClassKey classKey)
	{
		ItemRandomRollDescriptor rollDescriptor = ItemRandomRollDescriptor(power, classKey);

		rollDescriptor.IsMagic = true;
		InitializeRandomItemRoll(rollDescriptor);
		if (!rollDescriptor.IsValid)
		{
			DEBUG_MSG("RollSpecificMagicItem: Item roll failed! [Power: " + Z(power) + "; ClassKey: " + Z((int)classKey) + "]");
			SAFE_DELETE(rollDescriptor.ItemExtensionData);
			return Null;
		}

		RollItemBaseStats(rollDescriptor);
		RollItemMagic(rollDescriptor);
		FinalizeRandomItemRoll(rollDescriptor);
		return rollDescriptor.ItemExtensionData;
	}

	ItemExtension* RollSpecificSimpleItem(const int power, const ItemClassKey classKey)
	{
		ItemRandomRollDescriptor rollDescriptor = ItemRandomRollDescriptor(power, classKey);
		if (!rollDescriptor.IsValid) return Null;
		InitializeRandomItemRoll(rollDescriptor);
		if (!rollDescriptor.IsValid)
		{
			DEBUG_MSG("RollSpecificSimpleItem: Item roll failed! [Power: " + Z(power) + "; ClassKey: " + Z((int)classKey) + "]");
			SAFE_DELETE(rollDescriptor.ItemExtensionData);
			return Null;
		}
		RollItemBaseStats(rollDescriptor);
		FinalizeRandomItemRoll(rollDescriptor);
		return rollDescriptor.ItemExtensionData;
	}
}
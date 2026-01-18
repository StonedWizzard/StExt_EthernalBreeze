#include <JsonParser.h>
#include <StonedExtension.h>

#define FIELD_OFFSET(type, field) static_cast<size_t>(reinterpret_cast<size_t>(&reinterpret_cast<type*>(0)->field))
namespace Gothic_II_Addon
{
	JsonFieldDesc ItemClassDescriptor_Meta[] =
	{
		{ "Name", FIELD_OFFSET(ItemClassDescriptor, Name), JsonFieldType::String },
		{ "Type", FIELD_OFFSET(ItemClassDescriptor, Type), JsonFieldType::Byte },
		{ "Class", FIELD_OFFSET(ItemClassDescriptor, Class), JsonFieldType::Byte },
		{ "SubClass", FIELD_OFFSET(ItemClassDescriptor, SubClass), JsonFieldType::Byte },

		{ "AffixRollChance", FIELD_OFFSET(ItemClassDescriptor, AffixRollChance), JsonFieldType::Int },
		{ "HasExtraTier", FIELD_OFFSET(ItemClassDescriptor, HasExtraTier), JsonFieldType::Bool },
		{ "TierStep", FIELD_OFFSET(ItemClassDescriptor, TierStep), JsonFieldType::Int },

		{ "UpgradeDamageOnLevelChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeDamageOnLevelChange), JsonFieldType::Bool },
		{ "UpgradeProtectionOnLevelChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeProtectionOnLevelChange), JsonFieldType::Bool },
		{ "UpgradeConditionsOnLevelChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeConditionsOnLevelChange), JsonFieldType::Bool },

		{ "UpgradeDamageOnRankChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeDamageOnRankChange), JsonFieldType::Bool },
		{ "UpgradeProtectionOnRankChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeProtectionOnRankChange), JsonFieldType::Bool },
		{ "UpgradeConditionsOnRankChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeConditionsOnRankChange), JsonFieldType::Bool },

		{ "UpgradeDamageOnQualityChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeDamageOnQualityChange), JsonFieldType::Bool },
		{ "UpgradeProtectionOnQualityChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeProtectionOnQualityChange), JsonFieldType::Bool },
		{ "UpgradeConditionsOnQualityChange", FIELD_OFFSET(ItemClassDescriptor, UpgradeConditionsOnQualityChange), JsonFieldType::Bool },

		{ "SocketsAllowed", FIELD_OFFSET(ItemClassDescriptor, SocketsAllowed), JsonFieldType::Bool },
		{ "SocketsCap", FIELD_OFFSET(ItemClassDescriptor, SocketsCap), JsonFieldType::Int },
		{ "SocketsMax", FIELD_OFFSET(ItemClassDescriptor, SocketsMax), JsonFieldType::Int },
		{ "SocketsRankBonus", FIELD_OFFSET(ItemClassDescriptor, SocketsRankBonus), JsonFieldType::Float },
		{ "SocketsLevelBonus", FIELD_OFFSET(ItemClassDescriptor, SocketsLevelBonus), JsonFieldType::Float },
		{ "SocketsQualityBonus", FIELD_OFFSET(ItemClassDescriptor, SocketsQualityBonus), JsonFieldType::Float },

		{ "StatsAllowed", FIELD_OFFSET(ItemClassDescriptor, StatsAllowed), JsonFieldType::Bool },
		{ "StatsDurationAllowed", FIELD_OFFSET(ItemClassDescriptor, StatsDurationAllowed), JsonFieldType::Bool },

		{ "StatsCountCap", FIELD_OFFSET(ItemClassDescriptor, StatsCountCap), JsonFieldType::Int },
		{ "StatsCountMin", FIELD_OFFSET(ItemClassDescriptor, StatsCountMin), JsonFieldType::Int },
		{ "StatsCountMax", FIELD_OFFSET(ItemClassDescriptor, StatsCountMax), JsonFieldType::Int },

		{ "StatsCountMinRankBonus", FIELD_OFFSET(ItemClassDescriptor, StatsCountMinRankBonus), JsonFieldType::Float },
		{ "StatsCountMinLevelBonus", FIELD_OFFSET(ItemClassDescriptor, StatsCountMinLevelBonus), JsonFieldType::Float },
		{ "StatsCountMinQualityBonus", FIELD_OFFSET(ItemClassDescriptor, StatsCountMinQualityBonus), JsonFieldType::Float },

		{ "StatsCountMaxRankBonus", FIELD_OFFSET(ItemClassDescriptor, StatsCountMaxRankBonus), JsonFieldType::Float },
		{ "StatsCountMaxLevelBonus", FIELD_OFFSET(ItemClassDescriptor, StatsCountMaxLevelBonus), JsonFieldType::Float },
		{ "StatsCountMaxQualityBonus", FIELD_OFFSET(ItemClassDescriptor, StatsCountMaxQualityBonus), JsonFieldType::Float },

		{ "StatsPowerMult", FIELD_OFFSET(ItemClassDescriptor, StatsPowerMult), JsonFieldType::Float },
		{ "StatsPowerLevelBonus", FIELD_OFFSET(ItemClassDescriptor, StatsPowerLevelBonus), JsonFieldType::Float },
		{ "StatsPowerRankBonus", FIELD_OFFSET(ItemClassDescriptor, StatsPowerRankBonus), JsonFieldType::Float },
		{ "StatsPowerQualityBonus", FIELD_OFFSET(ItemClassDescriptor, StatsPowerQualityBonus), JsonFieldType::Float },
		{ "PrimaryStatsPowerBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryStatsPowerBonus), JsonFieldType::Float },

		{ "StatsDurationMin", FIELD_OFFSET(ItemClassDescriptor, StatsDurationMin), JsonFieldType::Int },
		{ "StatsDurationMax", FIELD_OFFSET(ItemClassDescriptor, StatsDurationMax), JsonFieldType::Int },
		{ "StatsDurationMult", FIELD_OFFSET(ItemClassDescriptor, StatsDurationMult), JsonFieldType::Float },
		{ "StatsDurationLevelBonus", FIELD_OFFSET(ItemClassDescriptor, StatsDurationLevelBonus), JsonFieldType::Float },
		{ "StatsDurationRankBonus", FIELD_OFFSET(ItemClassDescriptor, StatsDurationRankBonus), JsonFieldType::Float },
		{ "StatsDurationQualityBonus", FIELD_OFFSET(ItemClassDescriptor, StatsDurationQualityBonus), JsonFieldType::Float },
		{ "PrimaryStatsDurationBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryStatsDurationBonus), JsonFieldType::Float },

		{ "AbilitiesAllowed", FIELD_OFFSET(ItemClassDescriptor, AbilitiesAllowed), JsonFieldType::Bool },
		{ "AbilitiesCountCap", FIELD_OFFSET(ItemClassDescriptor, AbilitiesCountCap), JsonFieldType::Int },
		{ "AbilitiesCountMax", FIELD_OFFSET(ItemClassDescriptor, AbilitiesCountMax), JsonFieldType::Int },
		{ "AbilitiesCountRankBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesCountRankBonus), JsonFieldType::Float },
		{ "AbilitiesCountLevelBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesCountLevelBonus), JsonFieldType::Float },
		{ "AbilitiesCountQualityBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesCountQualityBonus), JsonFieldType::Float },

		{ "AbilitiesRollChance", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRollChance), JsonFieldType::Int },
		{ "AbilitiesRollChanceRankBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRollChanceRankBonus), JsonFieldType::Float },
		{ "AbilitiesRollChanceLevelBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRollChanceLevelBonus), JsonFieldType::Float },
		{ "AbilitiesRollChanceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRollChanceQualityBonus), JsonFieldType::Float },

		{ "AbilitiesPowerMult", FIELD_OFFSET(ItemClassDescriptor, AbilitiesPowerMult), JsonFieldType::Float },
		{ "AbilitiesPowerRankBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesPowerRankBonus), JsonFieldType::Float },
		{ "AbilitiesPowerLevelBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesPowerLevelBonus), JsonFieldType::Float },
		{ "AbilitiesPowerQualityBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesPowerQualityBonus), JsonFieldType::Float },
		{ "PrimaryAbilitiesPowerBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryAbilitiesPowerBonus), JsonFieldType::Float },

		{ "AbilitiesChanceMult", FIELD_OFFSET(ItemClassDescriptor, AbilitiesChanceMult), JsonFieldType::Float },
		{ "AbilitiesChanceRankBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesChanceRankBonus), JsonFieldType::Float },
		{ "AbilitiesChanceLevelBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesChanceLevelBonus), JsonFieldType::Float },
		{ "AbilitiesChanceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesChanceQualityBonus), JsonFieldType::Float },
		{ "PrimaryAbilitiesChanceBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryAbilitiesChanceBonus), JsonFieldType::Float },

		{ "AbilitiesDurationMult", FIELD_OFFSET(ItemClassDescriptor, AbilitiesDurationMult), JsonFieldType::Float },
		{ "AbilitiesDurationRankBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesDurationRankBonus), JsonFieldType::Float },
		{ "AbilitiesDurationLevelBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesDurationLevelBonus), JsonFieldType::Float },
		{ "AbilitiesDurationQualityBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesDurationQualityBonus), JsonFieldType::Float },
		{ "PrimaryAbilitiesDurationBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryAbilitiesDurationBonus), JsonFieldType::Float },

		{ "AbilitiesRangeMult", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRangeMult), JsonFieldType::Float },
		{ "AbilitiesRangeRankBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRangeRankBonus), JsonFieldType::Float },
		{ "AbilitiesRangeLevelBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRangeLevelBonus), JsonFieldType::Float },
		{ "AbilitiesRangeQualityBonus", FIELD_OFFSET(ItemClassDescriptor, AbilitiesRangeQualityBonus), JsonFieldType::Float },
		{ "PrimaryAbilitiesRangeBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryAbilitiesRangeBonus), JsonFieldType::Float },

		{ "DamageMult", FIELD_OFFSET(ItemClassDescriptor, DamageMult), JsonFieldType::Float },
		{ "DamageRankBonus", FIELD_OFFSET(ItemClassDescriptor, DamageRankBonus), JsonFieldType::Float },
		{ "DamageLevelBonus", FIELD_OFFSET(ItemClassDescriptor, DamageLevelBonus), JsonFieldType::Float },
		{ "DamageQualityBonus", FIELD_OFFSET(ItemClassDescriptor, DamageQualityBonus), JsonFieldType::Float },
		{ "PrimaryDamageBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryDamageBonus), JsonFieldType::Float },

		{ "ExtraDamageAllowed", FIELD_OFFSET(ItemClassDescriptor, ExtraDamageAllowed), JsonFieldType::Bool },
		{ "ExtraDamageRollChance", FIELD_OFFSET(ItemClassDescriptor, ExtraDamageRollChance), JsonFieldType::Int },
		{ "ExtraDamageRollChanceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraDamageRollChanceQualityBonus), JsonFieldType::Float },

		{ "ExtraDamageQualityMult", FIELD_OFFSET(ItemClassDescriptor, ExtraDamageQualityMult), JsonFieldType::Float },
		{ "ExtraDamageMinQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraDamageMinQualityBonus), JsonFieldType::Float },
		{ "ExtraDamageMaxQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraDamageMaxQualityBonus), JsonFieldType::Float },

		{ "SpecialDamageAllowed", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageAllowed), JsonFieldType::Bool },
		{ "SpecialDamageRollChance", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageRollChance), JsonFieldType::Int },
		{ "SpecialDamageRollChanceRankBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageRollChanceRankBonus), JsonFieldType::Float },
		{ "SpecialDamageRollChanceLevelBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageRollChanceLevelBonus), JsonFieldType::Float },
		{ "SpecialDamageRollChanceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageRollChanceQualityBonus), JsonFieldType::Float },

		{ "SpecialDamageMult", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageMult), JsonFieldType::Float },
		{ "SpecialDamageRankBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageRankBonus), JsonFieldType::Float },
		{ "SpecialDamageLevelBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageLevelBonus), JsonFieldType::Float },
		{ "SpecialDamageQualityBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialDamageQualityBonus), JsonFieldType::Float },

		{ "ProtectionMult", FIELD_OFFSET(ItemClassDescriptor, ProtectionMult), JsonFieldType::Float },
		{ "ProtectionRankBonus", FIELD_OFFSET(ItemClassDescriptor, ProtectionRankBonus), JsonFieldType::Float },
		{ "ProtectionLevelBonus", FIELD_OFFSET(ItemClassDescriptor, ProtectionLevelBonus), JsonFieldType::Float },
		{ "ProtectionQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ProtectionQualityBonus), JsonFieldType::Float },
		{ "PrimaryProtectionBonus", FIELD_OFFSET(ItemClassDescriptor, PrimaryProtectionBonus), JsonFieldType::Float },

		{ "ExtraProtectionAllowed", FIELD_OFFSET(ItemClassDescriptor, ExtraProtectionAllowed), JsonFieldType::Bool },
		{ "ExtraProtectionRollChance", FIELD_OFFSET(ItemClassDescriptor, ExtraProtectionRollChance), JsonFieldType::Int },
		{ "ExtraProtectionRollChanceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraProtectionRollChanceQualityBonus), JsonFieldType::Float },

		{ "ExtraProtectionQualityMult", FIELD_OFFSET(ItemClassDescriptor, ExtraProtectionQualityMult), JsonFieldType::Float },
		{ "ExtraProtectionMinQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraProtectionMinQualityBonus), JsonFieldType::Float },
		{ "ExtraProtectionMaxQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraProtectionMaxQualityBonus), JsonFieldType::Float },

		{ "SpecialProtectionAllowed", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionAllowed), JsonFieldType::Bool },
		{ "SpecialProtectionRollChance", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionRollChance), JsonFieldType::Int },
		{ "SpecialProtectionRollChanceRankBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionRollChanceRankBonus), JsonFieldType::Float },
		{ "SpecialProtectionRollChanceLevelBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionRollChanceLevelBonus), JsonFieldType::Float },
		{ "SpecialProtectionRollChanceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionRollChanceQualityBonus), JsonFieldType::Float },

		{ "SpecialProtectionMult", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionMult), JsonFieldType::Float },
		{ "SpecialProtectionRankBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionRankBonus), JsonFieldType::Float },
		{ "SpecialProtectionLevelBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionLevelBonus), JsonFieldType::Float },
		{ "SpecialProtectionQualityBonus", FIELD_OFFSET(ItemClassDescriptor, SpecialProtectionQualityBonus), JsonFieldType::Float },

		{ "ConditionMult", FIELD_OFFSET(ItemClassDescriptor, ConditionMult), JsonFieldType::Float },
		{ "ConditionRankBonus", FIELD_OFFSET(ItemClassDescriptor, ConditionRankBonus), JsonFieldType::Float },
		{ "ConditionLevelBonus", FIELD_OFFSET(ItemClassDescriptor, ConditionLevelBonus), JsonFieldType::Float },
		{ "ConditionQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ConditionQualityBonus), JsonFieldType::Float },

		{ "ExtraConditionAllowed", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionAllowed), JsonFieldType::Bool },
		{ "ExtraConditionRollChance", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionRollChance), JsonFieldType::Int },
		{ "ExtraConditionRollChanceRankBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionRollChanceRankBonus), JsonFieldType::Float },
		{ "ExtraConditionRollChanceLevelBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionRollChanceLevelBonus), JsonFieldType::Float },
		{ "ExtraConditionRollChanceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionRollChanceQualityBonus), JsonFieldType::Float },

		{ "ExtraConditionMult", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionMult), JsonFieldType::Float },
		{ "ExtraConditionRankBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionRankBonus), JsonFieldType::Float },
		{ "ExtraConditionLevelBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionLevelBonus), JsonFieldType::Float },
		{ "ExtraConditionQualityBonus", FIELD_OFFSET(ItemClassDescriptor, ExtraConditionQualityBonus), JsonFieldType::Float },

		{ "RangeSpreadAllowed", FIELD_OFFSET(ItemClassDescriptor, RangeSpreadAllowed), JsonFieldType::Bool },
		{ "RangeSpread", FIELD_OFFSET(ItemClassDescriptor, RangeSpread), JsonFieldType::Float },
		{ "RangeQualityBonus", FIELD_OFFSET(ItemClassDescriptor, RangeQualityBonus), JsonFieldType::Float },

		{ "PriceMult", FIELD_OFFSET(ItemClassDescriptor, PriceMult), JsonFieldType::Float },
		{ "PriceRankBonus", FIELD_OFFSET(ItemClassDescriptor, PriceRankBonus), JsonFieldType::Float },
		{ "PriceLevelBonus", FIELD_OFFSET(ItemClassDescriptor, PriceLevelBonus), JsonFieldType::Float },
		{ "PriceQualityBonus", FIELD_OFFSET(ItemClassDescriptor, PriceQualityBonus), JsonFieldType::Float },

		{ "ModWeight", FIELD_OFFSET(ItemClassDescriptor, ModWeight), JsonFieldType::Bool },
		{ "WeightMin", FIELD_OFFSET(ItemClassDescriptor, WeightMin), JsonFieldType::Int },
		{ "WeightMax", FIELD_OFFSET(ItemClassDescriptor, WeightMax), JsonFieldType::Int },

		{ "PrimaryStatsList", FIELD_OFFSET(ItemClassDescriptor, PrimaryStatsList), JsonFieldType::ArrayInt },
		{ "PrimaryAbilitiesList", FIELD_OFFSET(ItemClassDescriptor, PrimaryAbilitiesList), JsonFieldType::ArrayInt },
		{ "IncompatibleStatsList", FIELD_OFFSET(ItemClassDescriptor, IncopatibleStatsList), JsonFieldType::ArrayInt },
		{ "IncompatibleAbilitiesList", FIELD_OFFSET(ItemClassDescriptor, IncopatibleAbilitiesList), JsonFieldType::ArrayInt },

		{ "ConditionsList", FIELD_OFFSET(ItemClassDescriptor, ConditionsList), JsonFieldType::ArrayInt },
		{ "PrimaryDamageTypesList", FIELD_OFFSET(ItemClassDescriptor, PrimaryDamageTypesList), JsonFieldType::ArrayInt },
		{ "PrimaryProtectionTypesList", FIELD_OFFSET(ItemClassDescriptor, PrimaryProtectionTypesList), JsonFieldType::ArrayInt },
		{ "IncompatibleDamageTypesList", FIELD_OFFSET(ItemClassDescriptor, IncopatibleDamageTypesList), JsonFieldType::ArrayInt },

		{ "LowTierPrototypes", FIELD_OFFSET(ItemClassDescriptor, LowTierPrototypes), JsonFieldType::ArrayString },
		{ "MedTierPrototypes", FIELD_OFFSET(ItemClassDescriptor, MedTierPrototypes), JsonFieldType::ArrayString },
		{ "TopTierPrototypes", FIELD_OFFSET(ItemClassDescriptor, TopTierPrototypes), JsonFieldType::ArrayString },
		{ "ExtraTierPrototypes", FIELD_OFFSET(ItemClassDescriptor, ExtraTierPrototypes), JsonFieldType::ArrayString },

		{ nullptr, 0, JsonFieldType::Int }
	};

	JsonFieldDesc ItemsGeneratorConfig_Meta[] =
	{
		{ "LookupGeneratedItemCountBufferSize", FIELD_OFFSET(ItemsGeneratorConfig, LookupGeneratedItemCountBufferSize), JsonFieldType::Int },
		{ "LookupGeneratedItemCountThreshold", FIELD_OFFSET(ItemsGeneratorConfig, LookupGeneratedItemCountThreshold), JsonFieldType::Int },
		{ "LookupGeneratedItemChance", FIELD_OFFSET(ItemsGeneratorConfig, LookupGeneratedItemChance), JsonFieldType::Int },
		{ "PlainPrototypeItemGenerateChance", FIELD_OFFSET(ItemsGeneratorConfig, PlainPrototypeItemGenerateChance), JsonFieldType::Int },

		{ "ItemLevelMax", FIELD_OFFSET(ItemsGeneratorConfig, ItemLevelMax), JsonFieldType::Int },
		{ "ItemLevelPowerRatio", FIELD_OFFSET(ItemsGeneratorConfig, ItemLevelPowerRatio), JsonFieldType::Float },
		{ "ItemLevelSpread", FIELD_OFFSET(ItemsGeneratorConfig, ItemLevelSpread), JsonFieldType::Float },

		{ "ItemRankMax", FIELD_OFFSET(ItemsGeneratorConfig, ItemRankMax), JsonFieldType::Int },
		{ "ItemRankGrowthFactor", FIELD_OFFSET(ItemsGeneratorConfig, ItemRankGrowthFactor), JsonFieldType::Float },
		{ "ItemRankThresholds", FIELD_OFFSET(ItemsGeneratorConfig, ItemRankThresholds), JsonFieldType::ArrayInt },

		{ "ItemRollAsUndefinedChance", FIELD_OFFSET(ItemsGeneratorConfig, ItemRollAsUndefinedChance), JsonFieldType::Int },
		{ "ItemRollAsInfusedChance", FIELD_OFFSET(ItemsGeneratorConfig, ItemRollAsInfusedChance), JsonFieldType::Int },

		{ "ItemQualitySoftCap", FIELD_OFFSET(ItemsGeneratorConfig, ItemQualitySoftCap), JsonFieldType::Int },
		{ "ItemQualityHardCap", FIELD_OFFSET(ItemsGeneratorConfig, ItemQualityHardCap), JsonFieldType::Int },
		{ "ItemQualityPowerRatio", FIELD_OFFSET(ItemsGeneratorConfig, ItemQualityPowerRatio), JsonFieldType::Float },
		{ "ItemRankForExtraQualityThreshold", FIELD_OFFSET(ItemsGeneratorConfig, ItemRankForExtraQualityThreshold), JsonFieldType::Int },

		{ "ItemAffixesRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemAffixesRollChanceMult), JsonFieldType::Float },
		{ "ItemStatsCountMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemStatsCountMult), JsonFieldType::Float },
		{ "ItemSocketsCountMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemSocketsCountMult), JsonFieldType::Float },

		{ "ItemRangeMinCap", FIELD_OFFSET(ItemsGeneratorConfig, ItemRangeMinCap), JsonFieldType::Float },
		{ "ItemRangeMaxCap", FIELD_OFFSET(ItemsGeneratorConfig, ItemRangeMaxCap), JsonFieldType::Float },

		{ "ItemConditionValueMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemConditionValueMult), JsonFieldType::Float },
		{ "ItemExtraConditionRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemExtraConditionRollChanceMult), JsonFieldType::Float },

		{ "ItemExtraProtectionMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemExtraProtectionMult), JsonFieldType::Float },
		{ "ItemExtraProtectionRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemExtraProtectionRollChanceMult), JsonFieldType::Float },

		{ "ItemSpecialProtectionMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemSpecialProtectionMult), JsonFieldType::Float },
		{ "ItemSpecialProtectionRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemSpecialProtectionRollChanceMult), JsonFieldType::Float },
		{ "ItemRankForSpecialProtectionThreshold", FIELD_OFFSET(ItemsGeneratorConfig, ItemRankForSpecialProtectionThreshold), JsonFieldType::Int },

		{ "ItemExtraDamageMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemExtraDamageMult), JsonFieldType::Float },
		{ "ItemExtraDamageRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemExtraDamageRollChanceMult), JsonFieldType::Float },

		{ "ItemSpecialDamageMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemSpecialDamageMult), JsonFieldType::Float },
		{ "ItemSpecialDamageRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemSpecialDamageRollChanceMult), JsonFieldType::Float },
		{ "ItemRankForSpecialDamageThreshold", FIELD_OFFSET(ItemsGeneratorConfig, ItemRankForSpecialDamageThreshold), JsonFieldType::Int },

		{ "ItemStatRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemStatRollChanceMult), JsonFieldType::Float },
		{ "ItemPrimaryStatRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryStatRollChanceMult), JsonFieldType::Float },
		{ "ItemStatPowerMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemStatPowerMult), JsonFieldType::Float },
		{ "ItemPrimaryStatPowerMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryStatPowerMult), JsonFieldType::Float },
		{ "ItemStatDurationMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemStatDurationMult), JsonFieldType::Float },
		{ "ItemPrimaryStatDurationMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryStatDurationMult), JsonFieldType::Float },
		{ "ItemStatDurationMin", FIELD_OFFSET(ItemsGeneratorConfig, ItemStatDurationMin), JsonFieldType::Int },
		{ "ItemStatDurationMax", FIELD_OFFSET(ItemsGeneratorConfig, ItemStatDurationMax), JsonFieldType::Int },
		{ "ItemStatPriceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemStatPriceMult), JsonFieldType::Float },

		{ "ItemAbilityCountMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemAbilityCountMult), JsonFieldType::Float },
		{ "ItemAbilityRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemAbilityRollChanceMult), JsonFieldType::Float },
		{ "ItemPrimaryAbilityRollChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryAbilityRollChanceMult), JsonFieldType::Float },

		{ "ItemAbilityValueMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemAbilityValueMult), JsonFieldType::Float },
		{ "ItemPrimaryAbilityValueMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryAbilityValueMult), JsonFieldType::Float },
		{ "ItemAbilityChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemAbilityChanceMult), JsonFieldType::Float },
		{ "ItemPrimaryAbilityChanceMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryAbilityChanceMult), JsonFieldType::Float },
		{ "ItemAbilityDurationMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemAbilityDurationMult), JsonFieldType::Float },
		{ "ItemPrimaryAbilityDurationMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryAbilityDurationMult), JsonFieldType::Float },
		{ "ItemAbilityRangeMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemAbilityRangeMult), JsonFieldType::Float },
		{ "ItemPrimaryAbilityRangeMult", FIELD_OFFSET(ItemsGeneratorConfig, ItemPrimaryAbilityRangeMult), JsonFieldType::Float },
		{ "ItemRankForAbilityThreshold", FIELD_OFFSET(ItemsGeneratorConfig, ItemRankForAbilityThreshold), JsonFieldType::Int },

		{ "SpecialDamageTypesList", FIELD_OFFSET(ItemsGeneratorConfig, SpecialDamageTypesList), JsonFieldType::ArrayInt },
		{ "SpecialProtectionTypesList", FIELD_OFFSET(ItemsGeneratorConfig, SpecialProtectionTypesList), JsonFieldType::ArrayInt },

		{ nullptr, 0, JsonFieldType::Int }
	};
};
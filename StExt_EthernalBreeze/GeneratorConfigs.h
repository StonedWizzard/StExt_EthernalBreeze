#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	struct ItemCondition
	{
		int Id;
		int HardCap;
		float PerLevelMin;
		float PerLevelMax;
		int AllowedItemTypes;
	};

	struct ItemStatOption
	{
		zSTRING StatName;
		int StatId;
		int StatMaxCap;
		int StatMinCap;
		float StatValueMin;
		float StatValueMax;
		int IncopatibleItemTypes;
		int Rarity;
		int Price;
	};

	struct ItemAbilityOption
	{
		int AbilityId;
		int AbilityValueMin;
		int AbilityValueMax;
		int AbilityChanceMin;
		int AbilityChanceMax;
		int AbilityDurationMin;
		int AbilityDurationMax;
		int IncopatibleItemTypes;
		int Rarity;
		int Price;
	};

	struct ItemsGeneratorConfigs
	{
		int CodePage;

		int NextRankOffset;										// How much item levels required for next rank 
		int NextRankOffsetMin;
		float ItemLevelFromPowerRatio;							// How calculate item level from icoming power
		int MedTierSeparator;
		int TopTierSeparator;

		int GetAlreadyGeneratedItemAfterCount;
		float ChanceGetAlreadyGeneratedItem;

		float PriceMultGlobal;
		float PriceMultPerLevel;
		float PriceMultPerRank;

		int MaxItemLevel;
		int MaxItemDamage;
		int MaxItemProtection;

		// Item damage configs
		float DamagePerLevelMin;
		float DamagePerLevelMax;
		float DamagePerRankMult;
		float TwoHandedDamageMult;
		int ExtraDamageTypesCap;
		float ExtraDamageMaxMult;
		float ExtraDamageMinMult;
		float ExtraDamageChanceBase;
		float ExtraDamageChancePerRankMult;
		float ExtraDamageChancePerLevelMult;
		float RangeSpread;
		float OrigDamagePerLevelMult;

		// Item protection configs
		float ProtectionPerLevelMin;
		float ProtectionPerLevelMax;
		float ProtectionPerRankMult;
		float ExtraProtectionMaxMult;
		float ExtraProtectionMinMult;
		float ExtraProtectionChanceBase;
		float ExtraProtectionChancePerRankMult;
		float ExtraProtectionChancePerLevelMult;
		float OrigProtectionPerLevelMult;

		// Item requirements configs
		float ConditionStaticMult;
		float BaseConditionStaticMult;
		float ConditionPerLevelMult;
		float ConditionPerRankMult;
		float BaseConditionPerLevelMult;
		float BaseConditionPerRankMult;
		float ExtraConditionChanceBase;
		float ExtraConditionChancePerRankMult;
		float ExtraConditionChancePerLevelMult;

		// Item magic options
		int StatsMaxCap;
		int StatsMaxPerRank;
		int StatsMinPerRank;
		float ExtraStatsPerLevel;
		float StatPowerPerLevelMult;
		float StatPowerPerRankMult;
		int StatTimedEffectDurationBaseMax;
		int StatTimedEffectDurationBaseMin;
		float StatTimedEffectPowerMult;
		float StatTimedEffectPriceMult;
		float StatDurationPerLevelMult;
		float StatDurationPerRankMult;
		float StatGlobalPowerMult;
		float StatGlobalGetChanceMult;
		float StatGetChanceFromLevelMult;
		float StatGetChanceFromRankMult;
		float StatMaxCapBonusFromLevel;
		float StatMaxCapBonusFromRank;

		// Item magic abilities
		int AbilitiesMaxCap;
		int AbilitiesBeginsOnRank;
		float AbilitiesGetChanceBase;
		float AbilitiesGetChancePerLevelMult;
		float AbilitiesGetChancePerRankMult;		
		float AbilitiesPowerPerLevelMult;
		float AbilitiesPowerPerRankMult;
		float AbilitiesChancePerLevelMult;
		float AbilitiesChancePerRankMult;
		float AbilitiesDurationPerLevelMult;
		float AbilitiesDurationPerRankMult;

		// Item visual effect
		int VisualEffectBeginsOnRank;
		float VisualEffectGetChanceBase;
		float VisualEffectGetChancePerLevelMult;
		float VisualEffectGetChancePerRankMult;
		
		// Sockets
		int SocketsMaxCap;
		float ExtraSocketsPerLevel;
		float SocketBaseChance;
		float ExtraSocketChancePerLevelMult;
		float ExtraSocketChancePerRankMult;
		
		// Corruption
		float CorruptionBaseChance;
		float CorruptionSpreadMult;

		float UndefinedBaseChance;

		float HelmsGlobalPowerMult;
		float ArmorsGlobalPowerMult;
		float TorsoGlobalPowerMult;
		float PantsGlobalPowerMult;
		float BootsGlobalPowerMult;
		float GlovesGlobalPowerMult;

		float BeltsGlobalPowerMult;
		float AmuletsGlobalPowerMult;
		float RingsGlobalPowerMult;
		float ShieldsGlobalPowerMult;

		float Swords1hGlobalPowerMult;
		float Swords2hGlobalPowerMult;
		float Axes1hGlobalPowerMult;
		float Axes2hGlobalPowerMult;
		float Maces1hGlobalPowerMult;
		float Maces2hGlobalPowerMult;
		float MagicSwordsGlobalPowerMult;
		float StaffsGlobalPowerMult;
		float RapiersGlobalPowerMult;
		float DexSwordsGlobalPowerMult;
		float HalleberdsGlobalPowerMult;
		float SpearsGlobalPowerMult;
		float DualsLGlobalPowerMult;
		float DualsRGlobalPowerMult;
		float BowsGlobalPowerMult;
		float CrossBowsGlobalPowerMult;
		float MagicStaffsGlobalPowerMult;

		float PotionsGlobalPowerMult;
		float ScrollsGlobalPowerMult;

		// Database

		Array<ItemCondition> ConditionsList;
		Array<ItemStatOption> StatsOptions;
		Array<ItemAbilityOption> AbilitiesOptions;

		Array<zSTRING> Preffixes;
		Array<zSTRING> Affixes;
		Array<zSTRING> Suffixes;
		Array<zSTRING> VisualEffects;

		Array<zSTRING> HelmsLowPrototypes;
		Array<zSTRING> HelmsMedPrototypes;
		Array<zSTRING> HelmsTopPrototypes;

		Array<zSTRING> ArmorsLowPrototypes;
		Array<zSTRING> ArmorsMedPrototypes;
		Array<zSTRING> ArmorsTopPrototypes;

		Array<zSTRING> TorsoLowPrototypes;
		Array<zSTRING> TorsoMedPrototypes;
		Array<zSTRING> TorsoTopPrototypes;

		Array<zSTRING> PantsLowPrototypes;
		Array<zSTRING> PantsMedPrototypes;
		Array<zSTRING> PantsTopPrototypes;

		Array<zSTRING> BootsLowPrototypes;
		Array<zSTRING> BootsMedPrototypes;
		Array<zSTRING> BootsTopPrototypes;

		Array<zSTRING> GlovesLowPrototypes;
		Array<zSTRING> GlovesMedPrototypes;
		Array<zSTRING> GlovesTopPrototypes;

		Array<zSTRING> BeltsLowPrototypes;
		Array<zSTRING> BeltsMedPrototypes;
		Array<zSTRING> BeltsTopPrototypes;

		Array<zSTRING> AmuletsLowPrototypes;
		Array<zSTRING> AmuletsMedPrototypes;
		Array<zSTRING> AmuletsTopPrototypes;

		Array<zSTRING> RingsLowPrototypes;
		Array<zSTRING> RingsMedPrototypes;
		Array<zSTRING> RingsTopPrototypes;

		Array<zSTRING> ShieldsLowPrototypes;
		Array<zSTRING> ShieldsMedPrototypes;
		Array<zSTRING> ShieldsTopPrototypes;

		Array<zSTRING> Swords1hLowPrototypes;
		Array<zSTRING> Swords1hMedPrototypes;
		Array<zSTRING> Swords1hTopPrototypes;

		Array<zSTRING> Swords2hLowPrototypes;
		Array<zSTRING> Swords2hMedPrototypes;
		Array<zSTRING> Swords2hTopPrototypes;

		Array<zSTRING> Axes1hLowPrototypes;
		Array<zSTRING> Axes1hMedPrototypes;
		Array<zSTRING> Axes1hTopPrototypes;

		Array<zSTRING> Axes2hLowPrototypes;
		Array<zSTRING> Axes2hMedPrototypes;
		Array<zSTRING> Axes2hTopPrototypes;

		Array<zSTRING> Maces1hLowPrototypes;
		Array<zSTRING> Maces1hMedPrototypes;
		Array<zSTRING> Maces1hTopPrototypes;

		Array<zSTRING> Maces2hLowPrototypes;
		Array<zSTRING> Maces2hMedPrototypes;
		Array<zSTRING> Maces2hTopPrototypes;

		Array<zSTRING> MagicSwordsLowPrototypes;
		Array<zSTRING> MagicSwordsMedPrototypes;
		Array<zSTRING> MagicSwordsTopPrototypes;

		Array<zSTRING> StaffsLowPrototypes;
		Array<zSTRING> StaffsMedPrototypes;
		Array<zSTRING> StaffsTopPrototypes;

		Array<zSTRING> RapiersLowPrototypes;
		Array<zSTRING> RapiersMedPrototypes;
		Array<zSTRING> RapiersTopPrototypes;
		
		Array<zSTRING> DexSwordsLowPrototypes;
		Array<zSTRING> DexSwordsMedPrototypes;
		Array<zSTRING> DexSwordsTopPrototypes;

		Array<zSTRING> HalleberdsLowPrototypes;
		Array<zSTRING> HalleberdsMedPrototypes;
		Array<zSTRING> HalleberdsTopPrototypes;

		Array<zSTRING> SpearsLowPrototypes;
		Array<zSTRING> SpearsMedPrototypes;
		Array<zSTRING> SpearsTopPrototypes;

		Array<zSTRING> DualsLLowPrototypes;
		Array<zSTRING> DualsLMedPrototypes;
		Array<zSTRING> DualsLTopPrototypes;

		Array<zSTRING> DualsRLowPrototypes;
		Array<zSTRING> DualsRMedPrototypes;
		Array<zSTRING> DualsRTopPrototypes;

		Array<zSTRING> BowsLowPrototypes;
		Array<zSTRING> BowsMedPrototypes;
		Array<zSTRING> BowsTopPrototypes;

		Array<zSTRING> CrossBowsLowPrototypes;
		Array<zSTRING> CrossBowsMedPrototypes;
		Array<zSTRING> CrossBowsTopPrototypes;

		Array<zSTRING> MagicStaffsLowPrototypes;
		Array<zSTRING> MagicStaffsMedPrototypes;
		Array<zSTRING> MagicStaffsTopPrototypes;

		Array<zSTRING> Potions1Prototypes;
		Array<zSTRING> Potions2Prototypes;
		Array<zSTRING> Potions3Prototypes;
		Array<zSTRING> Potions4Prototypes;

		Array<zSTRING> ScrollsPrototypes;

		ItemsGeneratorConfigs()
		{
			ConditionsList = Array<ItemCondition>();
			StatsOptions = Array<ItemStatOption>();
			AbilitiesOptions = Array<ItemAbilityOption>();

			Preffixes = Array<zSTRING>();
			Affixes = Array<zSTRING>();
			Suffixes = Array<zSTRING>();
			VisualEffects = Array<zSTRING>();

			HelmsLowPrototypes = Array<zSTRING>();
			HelmsMedPrototypes = Array<zSTRING>();
			HelmsTopPrototypes = Array<zSTRING>();

			ArmorsLowPrototypes = Array<zSTRING>();
			ArmorsMedPrototypes = Array<zSTRING>();
			ArmorsTopPrototypes = Array<zSTRING>();

			TorsoLowPrototypes = Array<zSTRING>();
			TorsoMedPrototypes = Array<zSTRING>();
			TorsoTopPrototypes = Array<zSTRING>();

			PantsLowPrototypes = Array<zSTRING>();
			PantsMedPrototypes = Array<zSTRING>();
			PantsTopPrototypes = Array<zSTRING>();

			BootsLowPrototypes = Array<zSTRING>();
			BootsMedPrototypes = Array<zSTRING>();
			BootsTopPrototypes = Array<zSTRING>();

			GlovesLowPrototypes = Array<zSTRING>();
			GlovesMedPrototypes = Array<zSTRING>();
			GlovesTopPrototypes = Array<zSTRING>();

			BeltsLowPrototypes = Array<zSTRING>();
			BeltsMedPrototypes = Array<zSTRING>();
			BeltsTopPrototypes = Array<zSTRING>();

			AmuletsLowPrototypes = Array<zSTRING>();
			AmuletsMedPrototypes = Array<zSTRING>();
			AmuletsTopPrototypes = Array<zSTRING>();

			RingsLowPrototypes = Array<zSTRING>();
			RingsMedPrototypes = Array<zSTRING>();
			RingsTopPrototypes = Array<zSTRING>();

			ShieldsLowPrototypes = Array<zSTRING>();
			ShieldsMedPrototypes = Array<zSTRING>();
			ShieldsTopPrototypes = Array<zSTRING>();

			Swords1hLowPrototypes = Array<zSTRING>();
			Swords1hMedPrototypes = Array<zSTRING>();
			Swords1hTopPrototypes = Array<zSTRING>();

			Swords2hLowPrototypes = Array<zSTRING>();
			Swords2hMedPrototypes = Array<zSTRING>();
			Swords2hTopPrototypes = Array<zSTRING>();

			Axes1hLowPrototypes = Array<zSTRING>();
			Axes1hMedPrototypes = Array<zSTRING>();
			Axes1hTopPrototypes = Array<zSTRING>();

			Axes2hLowPrototypes = Array<zSTRING>();
			Axes2hMedPrototypes = Array<zSTRING>();
			Axes2hTopPrototypes = Array<zSTRING>();

			Maces1hLowPrototypes = Array<zSTRING>();
			Maces1hMedPrototypes = Array<zSTRING>();
			Maces1hTopPrototypes = Array<zSTRING>();

			Maces2hLowPrototypes = Array<zSTRING>();
			Maces2hMedPrototypes = Array<zSTRING>();
			Maces2hTopPrototypes = Array<zSTRING>();

			MagicSwordsLowPrototypes = Array<zSTRING>();
			MagicSwordsMedPrototypes = Array<zSTRING>();
			MagicSwordsTopPrototypes = Array<zSTRING>();

			StaffsLowPrototypes = Array<zSTRING>();
			StaffsMedPrototypes = Array<zSTRING>();
			StaffsTopPrototypes = Array<zSTRING>();

			RapiersLowPrototypes = Array<zSTRING>();
			RapiersMedPrototypes = Array<zSTRING>();
			RapiersTopPrototypes = Array<zSTRING>();

			DexSwordsLowPrototypes = Array<zSTRING>();
			DexSwordsMedPrototypes = Array<zSTRING>();
			DexSwordsTopPrototypes = Array<zSTRING>();

			HalleberdsLowPrototypes = Array<zSTRING>();
			HalleberdsMedPrototypes = Array<zSTRING>();
			HalleberdsTopPrototypes = Array<zSTRING>();

			SpearsLowPrototypes = Array<zSTRING>();
			SpearsMedPrototypes = Array<zSTRING>();
			SpearsTopPrototypes = Array<zSTRING>();

			DualsLLowPrototypes = Array<zSTRING>();
			DualsLMedPrototypes = Array<zSTRING>();
			DualsLTopPrototypes = Array<zSTRING>();

			DualsRLowPrototypes = Array<zSTRING>();
			DualsRMedPrototypes = Array<zSTRING>();
			DualsRTopPrototypes = Array<zSTRING>();

			BowsLowPrototypes = Array<zSTRING>();
			BowsMedPrototypes = Array<zSTRING>();
			BowsTopPrototypes = Array<zSTRING>();

			CrossBowsLowPrototypes = Array<zSTRING>();
			CrossBowsMedPrototypes = Array<zSTRING>();
			CrossBowsTopPrototypes = Array<zSTRING>();

			MagicStaffsLowPrototypes = Array<zSTRING>();
			MagicStaffsMedPrototypes = Array<zSTRING>();
			MagicStaffsTopPrototypes = Array<zSTRING>();

			Potions1Prototypes = Array<zSTRING>();
			Potions2Prototypes = Array<zSTRING>();
			Potions3Prototypes = Array<zSTRING>();
			Potions4Prototypes = Array<zSTRING>();

			ScrollsPrototypes = Array<zSTRING>();
		}
	};
}
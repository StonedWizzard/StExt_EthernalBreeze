#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	//---------------------------------------------------------------------------
	//								CONSTANTS
	//---------------------------------------------------------------------------

	const zSTRING GenerateItemPrefix = "STEXT_GENERATED_";

	const int wear_torso = 1 << 0;
	const int wear_head = 1 << 1;
	const int wear_effect = 1 << 4;

	const int wear_separator = 1 << 6;
	const int wear_gloves = 1 << 7;
	const int wear_boots = 1 << 8;
	const int wear_pants = 1 << 9;
	const int wear_breast = 1 << 10;
	const int wear_earings = 1 << 11;
	const int wear_braclets = 1 << 12;
	const int wear_collar = 1 << 13;
	const int wear_artifact = 1 << 14;

	const int item_swd = 16384;
	const int item_axe = 32768;
	const int item_2hd_swd = 65536;
	const int item_2hd_axe = 131072;
	const int item_bow = 524288;
	const int item_crossbow = 1048576;
	const int item_ring = 2048;
	const int item_amulet = 4194304;
	const int item_belt = 16777216;
	const int item_trophy = 33554432;
	const int item_dag = 65539;
	const int item_shield = 262144;
	const int item_throw = 262147;
	const int item_multi = 2097152;

	const int bit_item_mag_sword = 262144;
	const int bit_item_orc_weapon = 32768;
	const int bit_item_dex_sword = 67108864;
	const int bit_item_hellebarde = 131072;
	const int bit_item_speer = 1024;
	const int bit_item_pierce_damage = 2;
	const int bit_item_questitem = 524288;

	const int item_kat_none = 1;
	const int item_kat_nf = 2;
	const int item_kat_ff = 4;
	const int item_kat_mun = 8;
	const int item_kat_armor = 16;
	const int item_kat_food = 32;
	const int item_kat_docs = 64;
	const int item_kat_potions = 128;
	const int item_kat_light = 256;
	const int item_kat_rune = 512;
	const int item_kat_magic = -(int)2147483648;
	const int item_kat_keys = 1;

	const int ItemExtension_Props_Max = 64;
	const int ItemExtension_Tags_Max = 64;
	const int ItemExtension_Conditions_Max = 3;
	const int ItemExtension_Stats_Max = 32;
	const int ItemExtension_OwnStats_Max = 8;
	const int ItemExtension_Abilities_Max = 4;
	const int ItemExtension_VisualEffectData_Max = 10;
	const int ItemExtension_CraftData_Max = 64;
	const int ItemExtension_CraftFlags_Max = 128;

	typedef unsigned int ItemClassKey;
	const ItemClassKey ItemClassKey_Empty = 0x0U;
	const ItemClassKey ItemClassKey_Error = 0xFFFFFFFFU;

	//---------------------------------------------------------------------------
	//									ENUMS
	//---------------------------------------------------------------------------

	// ---------------------- ItemType ----------------------
	enum class ItemType : byte
	{
		Unknown = 0,
		Armor = 1,
		Weapon = 2,
		Jewelry = 3,
		Consumable = 4,
		Munition = 5,
		Inventory = 6,
		Craft = 7,
		Other = 8
	};

	// ---------------------- ItemClass ----------------------
	enum class ItemClass : byte
	{
		Unknown = 0,

		FullArmor = 1,
		Helm = 2,
		Torso = 3,
		Pants = 4,
		Gloves = 5,
		Boots = 6,
		Pauldrons = 7,
		Shield = 8,

		MeeleWeapon = 11,
		RangeWeapon = 12,
		CastWeapon = 13,

		Amulet = 21,
		Ring = 22,
		Earing = 23,
		Braclets = 24,
		Belt = 25,
		Collar = 26,
		Trophy = 27,

		Potion = 31,
		Scroll = 32,
		Food = 33,

		Charm = 51,

		Jewel = 61,
		Rune = 62,
		SoulEssence = 63,

		Misk = 100
	};

	// ---------------------- ItemSubClass ----------------------
	enum class ItemSubClass : byte
	{
		Unknown = 0,

		Clothing = 1,
		LightArmor = 2,
		MediumArmor = 3,
		HeavyArmor = 4,

		Hood = 5,
		Circlet = 6,
		LightHelm = 7,
		MediumHelm = 8,
		HeavyHelm = 9,

		LightShield = 10,
		HeavyShield = 11,

		Sword1H = 21,
		Axe1H = 22,
		Mace1H = 23,
		Dagger = 24,
		Rapier = 25,
		DexSword = 26,
		MagicSword = 27,

		Sword2H = 28,
		Mace2H = 29,
		Axe2H = 30,
		Staff = 31,
		Spear = 32,
		Halleberd = 33,
		OrcWeapon = 34,

		DualL = 35,
		DualR = 36,
		FistWeapon = 37,
		ThrowableDagger = 38,
		ThrowableAxe = 39,
		ThrowableMace = 40,
		ThrowableSpear = 41,

		LightBow = 42,
		HeavyBow = 43,
		LightCrossBow = 44,
		HeavyCrossBow = 45,

		Scroll = 46,
		Rune = 47,
		Wand = 48,

		WarriorJewelry = 49,
		MageJewelry = 50,
		ArcherJewelry = 51,
	};

	// -----------------------------------------------------------------------------------

	enum class ItemProperty : int
	{
		SocketsMax = 0,
		SocketsUsed = 1,
		StatsMax = 2,
		AbilitiesMax = 3,
		InfusionType = 4,
		AdditionalCost = 5,

		Level = 6,
		Lp = 7,
		ExpNow = 8,
		ExpNext = 9,

		AttachedSoulPower = 10,
		StackCount = 11,

		SpellId = 12,
		SpellFlags = 13,
		SpellCost = 14,
		SpellCharges = 15,
		SpellPower = 16,

		PoisonType = 17,
		PoisonFlags = 18,
		PoisonValue = 19,
		PoisonCharges = 20,

		CriticalDamageChance = 21,
		CriticalDamageMult = 22,
		BleedingDamageChance = 23,
		ScrushingDamageChance = 24,
		EnergyShieldDamageMult = 25,

		InitialPower = 32,
		InitialMainExtraFlags = 33,
		InitialBaseFlags = 34,
		InitialAdditionalFlags = 35,

		InitialSpell = 36,
		InitialRange = 37,
		InitialCost = 38,
		InitialWeight = 39,

		InitialConditionAttributeIndex = 40,
		InitialConditionAttribute1 = 40,
		InitialConditionAttribute2 = 41,
		InitialConditionAttribute3 = 42,

		InitialConditionAttributeValueIndex = 43,
		InitialConditionAttributeValue1 = 43,
		InitialConditionAttributeValue2 = 44,
		InitialConditionAttributeValue3 = 45,

		InitialDamageTypes = 46,
		InitialDamageTotal = 47,
		InitialDamageIndex = 48,
		InitialDamageBarrier = 48,
		InitialDamageBlunt = 49,
		InitialDamageEdge = 50,
		InitialDamageFire = 51,
		InitialDamageFly = 52,
		InitialDamageMagic = 53,
		InitialDamagePoint = 54,
		InitialDamageFall = 55,

		InitialProtectionIndex = 56,
		InitialProtectionBarrier = 56,
		InitialProtectionBlunt = 57,
		InitialProtectionEdge = 58,
		InitialProtectionFire = 59,
		InitialProtectionFly = 60,
		InitialProtectionMagic = 61,
		InitialProtectionPoint = 62,
		InitialProtectionFall = 63,

		Max = ItemExtension_Props_Max
	};

	enum class ItemTags : int
	{
		Unidentified = 0,
		Infused = 1,
		Crafted = 2,
		HasStack = 3,
		IsSmart = 4,
		HasSoul = 5,

		Max = ItemExtension_Tags_Max
	};

	//---------------------------------------------------------------------------
	//							CLASSES DEFENITION
	//---------------------------------------------------------------------------

	struct ItemExtensionsCoreState
	{
		bool IsInitialized;
		uint ItemExtensionsCount;
		uint NextItemExtensionUId;
	};

	struct ItemClassDescriptor
	{
		ItemClassKey ItemClassID;

		// *** SERIALIZEBLE FIELDS ***
		zSTRING Name;
		byte Type;
		byte Class;
		byte SubClass;

		int AffixRollChance;
		bool HasExtraTier;
		int TierStep;

		bool UpgradeDamageOnLevelChange;
		bool UpgradeProtectionOnLevelChange;
		bool UpgradeConditionsOnLevelChange;

		bool UpgradeDamageOnRankChange;
		bool UpgradeProtectionOnRankChange;
		bool UpgradeConditionsOnRankChange;

		bool UpgradeDamageOnQualityChange;
		bool UpgradeProtectionOnQualityChange;
		bool UpgradeConditionsOnQualityChange;


		// * Sockets *
		bool SocketsAllowed;
		int SocketsCap;
		int SocketsMax;
		float SocketsRankBonus;
		float SocketsLevelBonus;
		float SocketsQualityBonus;

		// * Stats *
		bool StatsAllowed;
		bool StatsDurationAllowed;

		int StatsCountCap;
		int StatsCountMin;
		int StatsCountMax;

		float StatsCountMinRankBonus;
		float StatsCountMinLevelBonus;
		float StatsCountMinQualityBonus;

		float StatsCountMaxRankBonus;
		float StatsCountMaxLevelBonus;
		float StatsCountMaxQualityBonus;

		float StatsPowerMult;
		float StatsPowerLevelBonus;
		float StatsPowerRankBonus;
		float StatsPowerQualityBonus;
		float PrimaryStatsPowerBonus;

		int StatsDurationMin;
		int StatsDurationMax;
		float StatsDurationMult;
		float StatsDurationLevelBonus;
		float StatsDurationRankBonus;
		float StatsDurationQualityBonus;
		float PrimaryStatsDurationBonus;

		// * Abilities *
		bool AbilitiesAllowed;
		int AbilitiesCountCap;
		int AbilitiesCountMax;
		float AbilitiesCountRankBonus;
		float AbilitiesCountLevelBonus;
		float AbilitiesCountQualityBonus;

		int AbilitiesRollChance;
		float AbilitiesRollChanceRankBonus;
		float AbilitiesRollChanceLevelBonus;
		float AbilitiesRollChanceQualityBonus;

		float AbilitiesPowerMult;
		float AbilitiesPowerRankBonus;
		float AbilitiesPowerLevelBonus;
		float AbilitiesPowerQualityBonus;
		float PrimaryAbilitiesPowerBonus;

		float AbilitiesChanceMult;
		float AbilitiesChanceRankBonus;
		float AbilitiesChanceLevelBonus;
		float AbilitiesChanceQualityBonus;
		float PrimaryAbilitiesChanceBonus;

		float AbilitiesDurationMult;
		float AbilitiesDurationRankBonus;
		float AbilitiesDurationLevelBonus;
		float AbilitiesDurationQualityBonus;
		float PrimaryAbilitiesDurationBonus;

		float AbilitiesRangeMult;
		float AbilitiesRangeRankBonus;
		float AbilitiesRangeLevelBonus;
		float AbilitiesRangeQualityBonus;
		float PrimaryAbilitiesRangeBonus;

		// * Damage *
		float DamageMult;
		float DamageRankBonus;
		float DamageLevelBonus;
		float DamageQualityBonus;
		float PrimaryDamageBonus;

		bool ExtraDamageAllowed;
		int ExtraDamageRollChance;
		float ExtraDamageRollChanceQualityBonus;

		float ExtraDamageQualityMult;
		float ExtraDamageMinQualityBonus;
		float ExtraDamageMaxQualityBonus;

		bool SpecialDamageAllowed;
		int SpecialDamageRollChance;
		float SpecialDamageRollChanceRankBonus;
		float SpecialDamageRollChanceLevelBonus;
		float SpecialDamageRollChanceQualityBonus;

		float SpecialDamageMult;
		float SpecialDamageRankBonus;
		float SpecialDamageLevelBonus;
		float SpecialDamageQualityBonus;

		// * Protection *
		float ProtectionMult;
		float ProtectionRankBonus;
		float ProtectionLevelBonus;
		float ProtectionQualityBonus;
		float PrimaryProtectionBonus;

		bool ExtraProtectionAllowed;
		int ExtraProtectionRollChance;
		float ExtraProtectionRollChanceQualityBonus;

		float ExtraProtectionQualityMult;
		float ExtraProtectionMinQualityBonus;
		float ExtraProtectionMaxQualityBonus;

		bool SpecialProtectionAllowed;
		int SpecialProtectionRollChance;
		float SpecialProtectionRollChanceRankBonus;
		float SpecialProtectionRollChanceLevelBonus;
		float SpecialProtectionRollChanceQualityBonus;

		float SpecialProtectionMult;
		float SpecialProtectionRankBonus;
		float SpecialProtectionLevelBonus;
		float SpecialProtectionQualityBonus;

		// * Conditions *
		float ConditionMult;
		float ConditionRankBonus;
		float ConditionLevelBonus;
		float ConditionQualityBonus;

		bool ExtraConditionAllowed;
		int ExtraConditionRollChance;
		float ExtraConditionRollChanceRankBonus;
		float ExtraConditionRollChanceLevelBonus;
		float ExtraConditionRollChanceQualityBonus;

		float ExtraConditionMult;
		float ExtraConditionRankBonus;
		float ExtraConditionLevelBonus;
		float ExtraConditionQualityBonus;

		// * Other *
		bool RangeSpreadAllowed;
		float RangeSpread;
		float RangeQualityBonus;

		float PriceMult;
		float PriceRankBonus;
		float PriceLevelBonus;
		float PriceQualityBonus;

		bool ModWeight;
		int WeightMin;
		int WeightMax;
		

		Array<int> PrimaryStatsList;
		Array<int> PrimaryAbilitiesList;
		Array<int> IncopatibleStatsList;
		Array<int> IncopatibleAbilitiesList;

		Array<int> ConditionsList;
		Array<int> PrimaryDamageTypesList;
		Array<int> PrimaryProtectionTypesList;
		Array<int> IncopatibleDamageTypesList;

		Array<zSTRING> LowTierPrototypes;
		Array<zSTRING> MedTierPrototypes;
		Array<zSTRING> TopTierPrototypes;
		Array<zSTRING> ExtraTierPrototypes;
	};

	struct ItemsGeneratorConfig
	{
		int LookupGeneratedItemCountBufferSize;
		int LookupGeneratedItemCountThreshold;
		int LookupGeneratedItemChance;
		int PlainPrototypeItemGenerateChance;

		int ItemLevelMax;
		float ItemLevelPowerRatio;
		float ItemLevelSpread;

		int ItemMaxRank;
		int ItemRankMax;
		int ItemRankBaseStep;
		float ItemRankGrowthFactor;
		Array<int> ItemRankThresholds;

		int ItemRollAsUndefinedChance;
		int ItemRollAsInfusedChance;

		int ItemQualitySoftCap;
		int ItemQualityHardCap;
		float ItemQualityPowerRatio;
		int ItemRankForExtraQualityThreshold;

		float ItemAffixesRollChanceMult;
		float ItemStatsCountMult;
		float ItemSocketsCountMult;

		float ItemRangeMinCap;
		float ItemRangeMaxCap;

		float ItemConditionValueMult;
		float ItemExtraConditionRollChanceMult;

		float ItemExtraProtectionMult;
		float ItemExtraProtectionRollChanceMult;

		float ItemSpecialProtectionMult;
		float ItemSpecialProtectionRollChanceMult;
		int ItemRankForSpecialProtectionThreshold;

		float ItemExtraDamageMult;
		float ItemExtraDamageRollChanceMult;

		float ItemSpecialDamageMult;
		float ItemSpecialDamageRollChanceMult;
		int ItemRankForSpecialDamageThreshold;

		float ItemStatRollChanceMult;
		float ItemPrimaryStatRollChanceMult;
		float ItemStatPowerMult;
		float ItemPrimaryStatPowerMult;
		float ItemStatDurationMult;
		float ItemPrimaryStatDurationMult;
		int ItemStatDurationMin;
		int ItemStatDurationMax;
		float ItemStatPriceMult;

		float ItemAbilityCountMult;
		float ItemAbilityRollChanceMult;
		float ItemPrimaryAbilityRollChanceMult;

		float ItemAbilityValueMult;
		float ItemPrimaryAbilityValueMult;
		float ItemAbilityChanceMult;
		float ItemPrimaryAbilityChanceMult;
		float ItemAbilityDurationMult;
		float ItemPrimaryAbilityDurationMult;
		float ItemAbilityRangeMult;
		float ItemPrimaryAbilityRangeMult;
		int ItemRankForAbilityThreshold;

		Array<int> SpecialDamageTypesList;
		Array<int> SpecialProtectionTypesList;
	};


	class ItemExtension
	{
	public:
		
		byte Type;
		byte Class;
		byte SubClass;

		const ItemClassDescriptor* ItemClassData;

		/* ----- [Serealizeble] ----- */

		unsigned int UId;
		zSTRING InstanceName;
		zSTRING BaseInstanceName;

		ItemClassKey ItemClassID;
		uflag64 Flags;
		byte Tags[ItemExtension_Tags_Max];

		int CraftData[ItemExtension_CraftData_Max];
		byte CraftFlags[ItemExtension_CraftFlags_Max];

		zSTRING OwnName;
		zSTRING Preffix;
		zSTRING Affix;
		zSTRING Suffix;
		zSTRING AttachedSoulName;

		zSTRING VisualEffect;
		int VisualEffectData[ItemExtension_VisualEffectData_Max];

		int Level;
		int Rank;
		int Quality;
		int Properties[ItemExtension_Props_Max];

		int SpecialDamageMin;
		int SpecialDamageMax;
		unsigned int SpecialDamageTypes;

		int SpecialProtection;
		unsigned int SpecialProtectionTypes;

		int ExtraFlags_Main;
		int ExtraFlags_Base;
		int ExtraFlags_Additional;

		int CondAtr[ItemExtension_Conditions_Max];
		int CondValue[ItemExtension_Conditions_Max];

		int DamageTypes;
		int DamageTotal;
		int Damage[oEDamageIndex_MAX];
		int Protection[oEDamageIndex_MAX];
		int Range;
		int Cost;
		int Weight;

		int StatId[ItemExtension_Stats_Max];
		int StatValue[ItemExtension_Stats_Max];
		int StatDuration[ItemExtension_Stats_Max];

		int OwnStatId[ItemExtension_OwnStats_Max];
		int OwnStatValue[ItemExtension_OwnStats_Max];
		
		int AbilityId[ItemExtension_Abilities_Max];
		int AbilityChance[ItemExtension_Abilities_Max];
		int AbilityValue[ItemExtension_Abilities_Max];
		int AbilityDuration[ItemExtension_Abilities_Max];
		int AbilityRange[ItemExtension_Abilities_Max];

		// ----------------------------------------------- //

	private:
		void UpgradeDamage(const int valueDelta, const float mult);
		void UpgradeProtection(const int valueDelta, const float mult);
		void UpgradeCondition(const int valueDelta, const float mult);

	public:

		ItemExtension();
		void Initialize();

		byte GetTag(const int tagId);
		byte GetCraftFlag(const int flagId);
		int GetCraftData(const int dataId);

		void SetTag(const int tagId, const byte tagValue);
		void SetCraftFlag(const int flagId, const byte flagValue);
		void SetCraftData(const int dataId, const int dataValue);

		void ChangeLevel(const int value);
		void ChangeRank(const int value);
		void ChangeQuality(const int value);
		void UpdatePrice();

		int GetProperty(const int propertyId);
		void SetProperty(const int propertyId, const int propertyValue);
		void ChangeProperty(const int propertyId, const int propertyValue);

		void AddDamage(const int damage, const int damType);
		void AddProtection(const int protection, const int protType);
		void AddSpecialDamage(const int damageMin, const int damageMax, const int damType);
		void AddSpecialProtection(const int protection, const int protType);

		int FindStat(const int statId);
		int FindAbility(const int abilityId);
		int FindCondition(const int conditionId);

		int GetStats(Array<int>& buffer);
		int GetAbilities(Array<int>& buffer);
		int GetConditions(Array<int>& buffer);

		void AddStat(const ExtraStatData* statData, const int value, const int duration);
		void AddStat(const int statId, const int value, const int duration);
		void AddAbility(const ItemAbility* abilityData, const int value, const int chance, const int duration, const int range);
		void AddAbility(const int abilityId, const int value, const int chance, const int duration, const int range);

		void AddCondition(const ExtraStatData* conditionData, const int value);
		void AddCondition(const int conditionId, const int value);

		int GetFreeSockets();
		void AddSockets(const int sockets);
		void FillSockets(const int sockets);

		void Infuse(const int infusionId, const int power);
		void AttachSoul(const int soulId, const int power);

		void Archive(zCArchiver& arc);
		void UnArchive(zCArchiver& arc);

		~ItemExtension();
	};

	struct ItemExtensionIndexer
	{
		const ItemExtension* Data;

		const byte Type;
		const byte Class;
		const byte SubClass;
		const uflag64 Flags;

		const int Level;
		const int Rank;
		const int Power;

		const int LevelMax;
		const int PowerMax;

		explicit ItemExtensionIndexer(const ItemExtension* item);
		ItemExtensionIndexer(byte type = (byte)ItemType::Unknown, byte cls = (byte)ItemClass::Unknown, byte subClass = (byte)ItemSubClass::Unknown,
			uflag64 flags = 0ULL, int level = Invalid, int rank = Invalid, int power = Invalid, int levelDelta = 0, int powerDelta = 0);

		bool operator==(const ItemExtensionIndexer& other) const noexcept;
		bool operator!=(const ItemExtensionIndexer& other) const noexcept;
		bool operator<(const ItemExtensionIndexer& other) const noexcept;
		bool operator>(const ItemExtensionIndexer& other) const noexcept;
		bool operator<=(const ItemExtensionIndexer& other) const noexcept;
		bool operator>=(const ItemExtensionIndexer& other) const noexcept;
	};

	class ItemExtensionDataStorage
	{
	private:
		Map<unsigned int, ItemExtension*> Data;
		Array<ItemExtensionIndexer> Indexer;
		StringMap<const ItemExtension*> Indexer_InstanceName;

		inline void InsertIndex(const ItemExtension* itemExtension);

		template <class T>
		inline void GetImpl(Array<T*>& buffer, const ItemExtensionIndexer& indexer, const uint bufferSize);

	public:
		uint ItemsCount;

		ItemExtensionDataStorage();

		ItemExtension* Get(const unsigned int uId);
		ItemExtension* Get(const zSTRING& instanceName);
		ItemExtension* Get(const ItemExtensionIndexer& indexer);
		ItemExtension* Get(oCItem* item);
		void Get(Array<ItemExtension*>& buffer, const ItemExtensionIndexer& indexer, const uint bufferSize = 32U);
		ItemExtension* Get(byte type = (byte)ItemType::Unknown, byte cls = (byte)ItemClass::Unknown, byte subClass = (byte)ItemSubClass::Unknown, uflag64 flags = 0ULL, int level = Invalid, int rank = Invalid, int power = Invalid, int levelDelta = 0, int powerDelta = 0);
		void Get(Array<ItemExtension*>& buffer, const uint bufferSize = 32U, byte type = (byte)ItemType::Unknown, byte cls = (byte)ItemClass::Unknown, byte subClass = (byte)ItemSubClass::Unknown, uflag64 flags = 0ULL, int level = Invalid, int rank = Invalid, int power = Invalid, int levelDelta = 0, int powerDelta = 0);

		const ItemExtension* GetSafe(const unsigned int uId);
		const ItemExtension* GetSafe(const zSTRING& instanceName);
		const ItemExtension* GetSafe(const ItemExtensionIndexer& indexer);
		const ItemExtension* GetSafe(oCItem* item);
		void GetSafe(Array<const ItemExtension*>& buffer, const ItemExtensionIndexer& indexer, const uint bufferSize = 32U);
		void GetSafe(Array<const ItemExtension*>& buffer, const uint bufferSize = 32U, byte type = (byte)ItemType::Unknown, byte cls = (byte)ItemClass::Unknown, byte subClass = (byte)ItemSubClass::Unknown, uflag64 flags = 0ULL, int level = Invalid, int rank = Invalid, int power = Invalid, int levelDelta = 0, int powerDelta = 0);
		const ItemExtension* GetSafe(byte type = (byte)ItemType::Unknown, byte cls = (byte)ItemClass::Unknown, byte subClass = (byte)ItemSubClass::Unknown, uflag64 flags = 0ULL, int level = Invalid, int rank = Invalid, int power = Invalid, int levelDelta = 0, int powerDelta = 0);

		bool Insert(ItemExtension* itemExtension);
		void Clear();

		void Archive(zCArchiver& arc);
		void UnArchive(zCArchiver& arc);

		~ItemExtensionDataStorage();
	};


	//---------------------------------------------------------------------------
	//								GLOBAL VARS
	//---------------------------------------------------------------------------
	extern ItemExtensionDataStorage* ItemsExtensionData;

	extern ItemsGeneratorConfig ItemsGeneratorConfigs;
	extern ItemExtensionsCoreState ItemExtensionsState;
	extern ItemClassDescriptor BaseItemClassDescriptor;

	extern Array<zSTRING> ItemPreffixesList;
	extern Array<zSTRING> ItemAffixesList;
	extern Array<zSTRING> ItemSuffixesList;
	extern Array<zSTRING> ItemNamesList;
	extern Array<zSTRING> ItemRanksList;
	extern Array<zSTRING> ItemDamageTypesList;

	extern Map<int, zSTRING> ItemConditionsText;
	extern Map<byte, zSTRING> ItemTypesText;
	extern Map<byte, zSTRING> ItemClassesText;
	extern Map<byte, zSTRING> ItemSubClassesText;

	extern Map<ItemClassKey, ItemClassDescriptor> ItemsClassData;
	extern StringMap<ItemClassKey> ItemsClassDataIndexer_InstanceName;

	extern zSTRING ItemNameValueString;
	extern zSTRING ItemDamageString;
	extern zSTRING ItemConditionString;
	extern zSTRING ItemRangeString;
	extern zSTRING ItemProtectionString;
	extern zSTRING ItemOrcWeaponTagString;
	extern zSTRING ItemWeightString;


	//-----------------------------------------------------------------
	//							ITEMS FUNCS
	//-----------------------------------------------------------------

	// * Core funcs *

	void InitItemsExtension();
	void ItemExtensions_DefineExternals();
	//void InitCraftEngine();
	//void InitItemGeneratorConfigs();

	extern ItemClassKey ItemClassKey_Create(byte itemType, byte itemClass, byte itemSubClass);
	extern byte ItemClassKey_GetType(ItemClassKey key);
	extern byte ItemClassKey_GetClass(ItemClassKey key);
	extern byte ItemClassKey_GetSubClass(ItemClassKey key);
	extern void ItemClassKey_Unpack(ItemClassKey key, byte& outType, byte& outClass, byte& outSubClass);

	extern bool IsExtendedItem(const oCItem* item);
	extern bool IsExtendedItem(const zSTRING& instanceName);

	extern ItemExtension* GetItemExtension(const oCItem* item);
	extern ItemExtension* GetItemExtension(const unsigned int key);
	extern ItemExtension* GetItemExtension(const zSTRING& instanceName);

	ItemClassKey GetItemClassKey(const zSTRING& instanceName);
	ItemClassKey GetItemClassKey(const oCItem* item);

	extern const ItemClassDescriptor* GetItemClassDescriptor(const ItemClassKey key);
	extern const ItemClassDescriptor* GetItemClassDescriptor(const zSTRING& instanceName);
	extern const ItemClassDescriptor* GetItemClassDescriptor(const oCItem* item);
	
	extern bool RegisterItemExtension(ItemExtension* itemExtension);
	extern void ClearGeneratedItemsData();
	void LoadGeneratedItems();
	void SaveGeneratedItems();
	void UnArchiveAdditionalArmors(oCNpc* npc);


	// * Builder funcs *
	inline unsigned int GetNextItemUId();

	ItemClassKey ParseItemClassKey(const oCItem* item);
	ItemClassKey ParseItemClassKey(zSTRING& instanceName);
	extern int CalcItemTier(const int power, const ItemClassDescriptor* itemClassDescriptor);

	extern int RollItemLevel(const int power);
	extern int RollItemRank(const int power);
	extern int RollItemQuality(const int power, const int rank);
	extern int RollItemMaxSockets(const int level, const int rank, const int quality, const int extraSocket, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemRange(const int baseRange, const int quality, const ItemClassDescriptor* itemClassDescriptor);

	extern int RollItemExtraConditionsCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemExtraConditionId(const Array<int>& baseConditions, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemExtraConditionValue(const ExtraStatData* condData, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemExtraConditionValueById(const int condDataId, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);

	extern int RollItemExtraProtectionsCount(const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemExtraProtectionValue(const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemSpecialProtectionsCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemSpecialProtectionValue(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);

	extern int RollItemExtraDamagesCount(const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemExtraDamageValue(const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemSpecialDamagesCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemSpecialDamageValue(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);

	extern int RollItemStatsCount(int& minStatsCount, int& maxStatsCount, const int extraStatsCount, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern void RollItemStatsChunk(Array<int>& statsBuffer, const Array<int>& currentStats, const int statsCount, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemStatValue(const ExtraStatData* statData, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemStatValue(const int statDataId, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemStatDuration(const int statDataId, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);

	extern int RollItemAbilitiesCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern void RollItemAbilitiesChunk(Array<int>& abilitiesBuffer, const Array<int>& currentAbilities, const int abilitiesCount, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemAbilitiesValue(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemAbilitiesDuration(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemAbilitiesChance(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);
	extern int RollItemAbilitiesRange(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor);

	zSTRING RollPrototypeInstanceName(const int tier, const ItemClassDescriptor* itemClassDescriptor);
	extern bool RollItemAffix(const int rollChance, const int sourceId, zSTRING& affix);
	extern bool RollItemName(const int rollChance, zSTRING& name);
	extern int RollItemWeight(const ItemClassDescriptor* itemClassDescriptor);

	ItemExtension* CreateItemExtension(const int power, const oCItem* item, const ItemClassDescriptor* itemClassDescriptor);
	ItemExtension* CreateItemExtension(const int power, zSTRING& instanceName, const ItemClassDescriptor* itemClassDescriptor);

	ItemExtension* RollSpecificMagicItem(const int power, const ItemClassKey classKey);
	ItemExtension* RollSpecificSimpleItem(const int power, const ItemClassKey classKey);

	const int GenerateNewMagicItem(const int itemClassId, int power);
	const int GenerateNewRegularItem(const int itemClassId, int power);

	void FindPlayerItems(const int itemClassId, Array<const oCItem*>& foundItems);
	void FindPlayerItems(const Array<int>& itemClassIds, Array<const oCItem*>& foundItems);
	void FindPlayerUndefinedItems(Array<const oCItem*>& foundItems);

	void UpdateItemDescriptionText(oCItem* item, const ItemExtension* extension);
	bool SelectItemGeneratorConfigs(const zSTRING& fileName);
	void IdentifyItem(const oCItem* item);

	/*
    Array<oCItem*> GetEnchantedItems(int flags)
    {
        Array<oCItem*> items = Array<oCItem*>();
        if (!player || !player->inventory2.GetContents())
        {
            DEBUG_MSG("GetEnchantedItems - player seems not initialized!");
            return items;
        }

        auto contents = player->inventory2.GetContents();
        int size = contents->GetNumInList();
        int i = 0;
        zSTRING instName;
        while (i < size)
        {
            oCItem* pItem = contents->Get(i);
            if (!pItem) { i++; continue; }
            instName = pItem->GetInstanceName();
            if (!instName.StartWith(GenerateItemPrefix)) { i++; continue; }
            if (pItem->HasFlag(ITM_FLAG_ACTIVE)) { i++; continue; }
            if (flags != 0)
            {
                C_ItemData* data = GetItemData(instName);
                if (!data) { i++; continue; }
                if (!HasFlag(data->Type, flags)) { i++; continue; }
            }
            
            items.Insert(pItem);
            i++;
        }
        DEBUG_MSG("GetEnchantedItems - items found: " + Z (int)items.GetNum());
        return items;
    }

    Array<oCItem*> GetUndefinedItems()
    {
        Array<oCItem*> items = Array<oCItem*>();
        if (!player || !player->inventory2.GetContents())
        {
            DEBUG_MSG("GetUndefinedItems - player seems not initialized!");
            return items;
        }

        auto contents = player->inventory2.GetContents();
        int size = contents->GetNumInList();
        int i = 0;
        zSTRING instName;
        while (i < size)
        {
            oCItem* pItem = contents->Get(i);
            if (!pItem) { i++; continue; }
            instName = pItem->GetInstanceName();
            if (!instName.StartWith(GenerateItemPrefix)) { i++; continue; }

            C_ItemData* data = GetItemData(instName);
            if (!data) { i++; continue; }
            if (!HasFlag(data->Flags, ItemFlag_Undefined)) { i++; continue; }

            items.Insert(pItem);
            i++;
        }
        DEBUG_MSG("GetUndefinedItems - items found: " + Z(int)items.GetNum());
        return items;
    }*/
}
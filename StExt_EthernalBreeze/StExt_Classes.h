#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	//---------------------------------------------------------------------------
	//							CLASSES DEFENITION
	//---------------------------------------------------------------------------

	struct ConfigPresetData
	{
		zSTRING Name;
		zSTRING Text;
		zSTRING TextColor;
		zSTRING OnApply;
	};

	struct ModExtensionInfo
	{
		zSTRING Name;
		zSTRING Author;
		zSTRING Version;
	};

	struct ModStateData
	{
		zSTRING StExt_RandomizeWorld_Wp1_CallbackStr;
		zSTRING StExt_RandomizeWorld_Wp2_CallbackStr;
		zSTRING StExt_RandomizeWorld_Wp3_CallbackStr;

		zSTRING StExt_RandomizeWorld_Wp1;
		zSTRING StExt_RandomizeWorld_Wp2;
		zSTRING StExt_RandomizeWorld_Wp3;

		zSTRING StExt_ArmorVisualBackup;

		zSTRING StExt_CurrentItemGeneratorConfigs;
		zSTRING StExt_CurrentUserConfigs;
	};

	struct CraftInfoData
	{
		int Flags;
		int Price;
		int ExpBonus;
		int CraftBonus;
		int Count;
		int IngredientId[16];
		int IngredientCount[16];
		zSTRING ConditionFunc;
		zSTRING ResultInstance;
	};

	struct WaypointData
	{
		zSTRING Wp;
		int Radius;
		zCWaypoint* wpInst;
	};

	struct AuraData
	{
		int Id;                 // Id of aura
		int Flags;              // Flags
		int ReservedAtr;        // Index of reserved attribute
		int ReservedValue;      // Initial value for reservation
		int Duration;           // Initial duration
		int Chance;             // Base chance to apply effect
		int PowerRatio;         // Moddifier for applied aura power
		int CondAtr[3];         // Index of required stat to equipment
		int CondValue[3];       // Required stat amount
		int StatId[5];          // Index of applied effect. Used for stats system only
		int StatValue[5];       // Initial effect values
		int MasteryId[3];       // Bounded masteries
		zSTRING OnOffence;      // Invoke func on do damage
		zSTRING OnDeffence;     // Invoke func on get damage
		zSTRING OnTick;         // Invoke func on get damage
		zSTRING BoundedItem;    // Item instnce, bounded to this aura
		zSTRING Effect;			// SpellFx effect on activation
	};

	struct MagicInfusionData
	{
		// System values
		int Id;
		int Tier;
		int AffixId;
		int SuffixId;
		int PreffixId;
		// Npc mods
		int NpcFlags;
		float Size;
		int SpeedMod;
		int ExtraLevel;
		int ExtraLevelMult;
		zSTRING Effect;
		// Attribute mods
		int ExtraHp;
		int ExtraEs;
		int ExtraStr;
		int ExtraAgi;
		int ExtraHpMult;
		int ExtraEsMult;
		int ExtraStrMult;
		int ExtraAgiMult;
		// Damage mods
		int ExtraBluntDam;
		int ExtraEdgeDam;
		int ExtraFireDam;
		int ExtraFlyDam;
		int ExtraMagicDam;
		int ExtraPointDam;
		int ExtraPoisonDam;
		int ExtraHitchances;
		// Protection mods
		int ExtraDodgeChance;
		int ExtraBluntProt;
		int ExtraEdgeProt;
		int ExtraFireProt;
		int ExtraFlyProt;
		int ExtraMagicProt;
		int ExtraPointProt;
		int ExtraProtAllMult;
		int ResistEdge;
		int ResistBlunt;
		int ResistPoint;
		int ResistMagic;
		int ResistFire;
		int ResistFly;
		int ResistPois;
		// Abilities
		int LowAbilityId;
		int MedAbilityId;
		int TopAbilityId;
		int SummonAbilityId;
		int PassiveAbilityId;
		int BuffAbilityId;
		int DebuffAbilityId;
		int Abilities[10];
		int LuckBonus;
		int GoldBonus;
	};

	struct ExtraStatData
	{
		int Id;
		int ValueType;
		int StatGroup;
		int SortIndex;
		int MinCap;
		int MaxCap;

		int RollChance;
		int RollMinCap;
		int RollMaxCap;
		float RollMinPower;
		float RollMaxPower;
		float CostPerStat;
	};

	struct UncapedStatData
	{
		int Id;
		int Now;
		int Was;
		int Max;
		int BonusWas;
		int BonusNow;
	};

	struct SoundOverrideData
	{
		zSTRING OrigSound;
		zSTRING NewSound;
	};

	struct SpellInfo
	{
		int SpellId;
		int SpellType;
		int SpellFlags;
		int SpellLevel;

		int IsScroll;
		int IsAoe;
		int IsDot;
		int IsStream;
		int ManaCost;
	};

	struct NpcExtension
	{
		int NpcUid;
		oCNpc* NpcPtr;
		zSTRING InstanceName;//just added

		int Stats[NpcExtension_StatsMax];
	};


	// **************************************************************
	//						NB Hooks
	// **************************************************************

	typedef void (*SncCheckBonusFuncType)(int);

	struct NpcIgnoreEntry
	{
	public:
		oCNpc* pNpc;
		float timeIgnore;

		NpcIgnoreEntry::NpcIgnoreEntry()
		{
			timeIgnore = 0;
			pNpc = NULL;
		}
	};

	class oCNpcEx : public oCNpc {
	public:
		zCLASS_UNION_DECLARATION(oCNpcEx);
		oCItemContainer m_pPocket;
		oCItemContainer m_pInventory2;
		int m_pVARS[400];
		Timer timer;

		// список нпс, которых будет игнорить нпс некоторое время
		zCArray<NpcIgnoreEntry*> tempIgnoreList;

#ifdef INTERACTIVE_ON
		int debuffCount[50];
		int debuffTime[50];
		float debuffVar[50];
		int buffCount[50];
		int buffTime[50];
		float buffVar[50];
#endif

		virtual void Archive(zCArchiver& ar);
		virtual void Unarchive(zCArchiver& ar);
		virtual void ProcessNpc();

		~oCNpcEx();

		void AddTempIgnore(oCNpc* pNpc, int time);
		bool IsIgnoringNpc(oCNpc* pNpc);
		void ProcessIgnoreList();
	};
}
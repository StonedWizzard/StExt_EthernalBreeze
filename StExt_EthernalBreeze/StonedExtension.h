#include "nlohmann/json.hpp"
#include <vector>
#include <C_ModState.h>
#include <C_AuraData.h>
#include <C_MagicInfusion.h>
#include <C_EnchantmentData.h>
#include <C_ItemData.h>
#include <C_CraftData.h>
#include <C_UncapedStat.h>
#include <C_ConfigPreset.h>
#include <DamageInfo.h>
#include <GeneratorConfigs.h>
#include <ModExtensionsInfo.h>

namespace Gothic_II_Addon
{
	const string ModDataRootDir = "EthernalBreeze_Data";
	const string ItemGeneratorConfigsDir = "ItemGeneratorConfigs";
	const string UserExportedConfigsDir = "UserExportedConfigs";
	const zSTRING GenerateItemPrefix = "STEXT_GENERATED_";

	constexpr auto ConfigsExportTemplate = R"(META
{
    After = ConfigsPresets.d;
    Mod = EthernalBreeze.vdf;
};

instance [ConfigName](C_ConfigPreset)
{
    Name = "[ConfigName]";
    Text = "[ConfigText]";
    TextColor = StExt_Color_Header;
    OnApply = "[ConfigApplyFunc]";
};

func event StExt_Evt_OnModLoaded() { StExt_RegistrateConfigsPreset("[ConfigName]"); };	

func void [ConfigApplyFunc]() 
{
[ConfigsList]
};
)";

	#define DebugEnabled false
	#define DebugStackEnabled false

	#if DebugEnabled
		#define DEBUG_MSG(message) DebugMessage(message)
		#define DEBUG_MSG_IF(condition, message) do { if (condition) { DebugMessage(message); } } while(0) 
	#else
		#define DEBUG_MSG(message)
		#define DEBUG_MSG_IF(condition, message)
	#endif

	
	struct WaypointData
	{
		zSTRING Wp;
		int Radius;
	};

	extern ItemsGeneratorConfigs GeneratorConfigs;

	extern CExtraDamage ExtraDamage;
	extern CDotDamage DotDamage;
	extern CExtraDamage ReflectDamage;
	extern CIncomingDamage IncomingDamage;

	extern std::map<int, C_AuraData*> AuraData;
	extern Array<C_MagicInfusionData*> InfusionData_Afixes;
	extern Array<C_MagicInfusionData*> InfusionData_Suffixes;
	extern Array<C_MagicInfusionData*> InfusionData_Preffixes;
	extern Array<WaypointData> ProhibitedWaypoints;
	extern Array<C_ConfigPreset*> GameConfigsPresets;
	extern Array<ModExtensionInfo> ModPluginsInfo;

	extern std::map<int, int> StatsTypeMap;
	extern zSTRING ModVersionString;

	extern int OnPreLoopFunc;
	extern int OnPostLoopFunc;
	//extern int ApplyResistsFunc;
	//extern int ApplyDamageToEsFunc;
	extern int ProcessHpDamageFunc;
	//extern int OnModDamageExpFunc;
	//extern int OnSncDamageFunc;
	extern int PrintDamageFunc;
	extern int OnDamageAfterFunc;
	extern int OnDamageBeginFunc;
	extern int OnPostDamageFunc;
	extern int FxDamageCanBeAppliedFunc;
	extern int IsNpcImmortalFunc;
	extern int IsExtraDamageProhibitedFunc;
	extern int OnArmorEquipFunc;
	extern int GetSpellDamageFlagsFunc;
	extern int GetSpellEffectFlagsFunc;
	extern int OnLoadEndFunc;
	extern int OnLevelChangeFunc;
	extern int NpcGetBarCurEsFunc;
	extern int NpcGetBarMaxEsFunc;
	extern int UpdateFocusNpcInfoFunc;
	extern int IsHeroMovLockedFunc;
	extern int HandleKeyEventFunc;
	extern int HandlePcStatChangeFunc;
	extern int CanCallModMenuFunc;
	extern int SaveParserVarsFunc;
	extern int RestoreParserVarsFunc;
	extern int UpdateUiStatusFunc;
	extern int StExt_CheckConditionStatFunc;

	extern int MaxSpellId;
	extern int StExt_AbilityPrefix;
	extern int ItemCondSpecialSeparator;

	extern int StExt_Config_NpcStats_TopOffset;
	extern int StExt_Config_NpcStats_HideTags;

	extern float ItemBasePriceMult;
	extern float ItemSellPriceMult;

	extern bool IsLevelChanging;
	extern bool IsLoading;
	extern int ShowModMenu;
	extern int ShowPcEs;
	extern int BlockMovement;
	extern int PcEsPosX;
	extern int PcEsPosY;
	extern zSTRING PcEsCurStr;

	extern zSTRING StExt_EsText;
	extern zSTRING* SpellFxNames;

	const zSTRING& StExt_GetFuncNameByAddress(int address);

	void PrintHeroEsBar();
	void UpdateUiStatus();

	void ApplyExtraDamage(oCNpc* atk, oCNpc* target);
	void ApplyDotDamage(oCNpc* atk, oCNpc* target);
	void ApplyReflectDamage(oCNpc* atk, oCNpc* target);
	void ApplyDamages(ULONG type, ULONG* damage, int& total);
	int ValidateIntValue(int value, int min, int max);
	void CreateUniqKeysArray(std::vector<int>& numbers, int maxValue);
	std::vector<int> CreateUniqKeysArray(int maxValue, int size = Invalid);
	int GetRandomRange(int min, int max);
	float GetRandomFloatRange(float value);
	string GetSlotNameByID(int ID);
	void ClearRegisteredNpcs();
	
	C_AuraData* GetAuraById(int id);
	C_MagicInfusionData* CreateInfusion(int tier);
	C_MagicInfusionData* GetRandomInfusion(int type, int tier, int grantInfusion);

	void CreateDebugFile();
	void PrintDebug(zSTRING message);
	void DebugMessage(zSTRING message);
	bool HasFlag(unsigned long flags, unsigned long bits);
	void DebugSymInfo(zCPar_Symbol* ps);

	zSTRING GetFloatStringFromInt(int val);
	zSTRING IntValueToYesNo(int val);

	void InitAuraData();
	void InitNpcAffixes();
	void InitNpcPreffixes();
	void InitNpcSuffixes();
	void StonedExtension_InitModData();
	void StonedExtension_InitUi();
	void StonedExtension_InitUi_StatMenu();
	void StonedExtension_Loop();
	void StonedExtension_Loop_StatMenu();
	void StonedExtension_DefineExternals();
	void StonedExtension_MsgTray_Loop();
	void MsgTray_AddEntry(zSTRING text, zSTRING color);
	zCOLOR ParseHexColor(std::string inputColor);

	void LoadModState();
	void SaveModState();
	void ResetModState();
	zSTRING GetModVersion();

	// Timed effects
	void SaveTimedEffects();
	void LoadTimedEffects();
	void TimedEffectsOnNewGame();
	void TimedEffectsLoop();
	void TimedEffectsFinalizeLoop();

	int GetTimedEffectByIndex();
	int GetTimedEffectByNpc();
	int CreateTimedEffect();
	int GetTimedEffectsCount();
	void C_TimedEffect_RegisterClassOffset();
	void UpdateTimedEffectsNpcUids(int oldUid, int newUid);

	// Npc UId subsystem
	int GetNextNpcUid();
	void RegisterNpc(oCNpc* npc, int npcUid);
	void RegisterNpc(oCNpc* npc);
	int GetNpcUid(oCNpc* npc);
	oCNpc* GetNpcByUid(int npcUid);
	bool IsUidRegistered(int npcUid);
	void MsgTray_Clear();

	// Item generator
	zCPar_Symbol* CopySymbol(const zSTRING origName, zSTRING name, int& index);
	zSTRING BuildEnchantedItemInstanceName(int itemId, int enchntmentId);
	int GetNextEnchntmentId();
	int GetNextItemId();
	int GenerateNewItem(const int itemTypes, int power);
	zSTRING GetRandomPrototype(int& itemType, int power);
	void SetItemAffixes(zSTRING& preffix, zSTRING& affix, zSTRING& suffix);
	void LoadGeneratedItems();
	void SaveGeneratedItems();
	void ClearGeneratedItemsData();
	C_EnchantmentData* GetEnchantmentData(oCItem* item);
	void BuildItemText(C_EnchantmentData* enchantment, oCItem* item);
	Array<oCItem*> GetEnchantedItems(int flags = 0);
	Array<oCItem*> GetUndefinedItems();
	void IdentifyItem(int instanceId, oCItem* item);
	bool IsGeneratedItem(oCItem* item);

	void UnArchiveAdditionalArmors(oCNpc* npc);
	void ThrowItem(ThrowItemDescriptor& itemDescriptor);
	//void StonedExtension_Loop_Inventory();

	void InitStatsUncaper();
	void StartUncaper();
	void StopUncaper();
	void StatsUncaperLoop();
	void inline UpdateUncapedStat(int indx);

	void DrawModInfo();
	void ClearDamageMeta();

	bool SelectItemGeneratorConfigs(const string fileName);
	void InitItemGeneratorConfigs();
	C_ConfigPreset* GetConfigPreset(zSTRING presetName);

	// Item generator constants
	const int ItemType_None = 0;
	const int ItemType_Helm = 1 << 0;
	const int ItemType_Armor = 1 << 1;
	const int ItemType_Pants = 1 << 2;
	const int ItemType_Boots = 1 << 3;
	const int ItemType_Gloves = 1 << 4;
	const int ItemType_Belt = 1 << 5;
	const int ItemType_Amulet = 1 << 6;
	const int ItemType_Ring = 1 << 7;
	const int ItemType_Shield = 1 << 8;
	const int ItemType_Sword1h = 1 << 9;
	const int ItemType_Sword2h = 1 << 10;
	const int ItemType_Axe1h = 1 << 11;
	const int ItemType_Axe2h = 1 << 12;
	const int ItemType_Mace1h = 1 << 13;
	const int ItemType_Mace2h = 1 << 14;
	const int ItemType_MagicSword = 1 << 15;
	const int ItemType_Staff = 1 << 16;
	const int ItemType_Rapier = 1 << 17;
	const int ItemType_DexSword = 1 << 18;
	const int ItemType_Halleberd = 1 << 19;
	const int ItemType_Spear = 1 << 20;
	const int ItemType_DualL = 1 << 21;
	const int ItemType_DualR = 1 << 22;
	const int ItemType_Bow = 1 << 23;
	const int ItemType_CrossBow = 1 << 24;
	const int ItemType_MagicStaff = 1 << 25;
	const int ItemType_Potion = 1 << 26;
	const int ItemType_Scroll = 1 << 27;
	const int ItemType_Torso = 1 << 28; // 268435456

	const int ItemTypesMax = 28;

	const int ItemFlag_None = 0;
	const int ItemFlag_Undefined = 1 << 1;		// Hide properties
	const int ItemFlag_Socketed = 1 << 2;		// Allow sockets system
	const int ItemFlag_Corrupted = 1 << 3;		// Prohibit upgrade
	const int ItemFlag_Crafted = 1 << 4;		// Exclude from search
	const int ItemFlag_Armor = 1 << 5;
	const int ItemFlag_Weapon = 1 << 6;
	const int ItemFlag_Jewelry = 1 << 7;
	const int ItemFlag_Talisman = 1 << 8;
	const int ItemFlag_Consumable = 1 << 9;
	const int ItemFlag_Melee = 1 << 10;
	const int ItemFlag_Range = 1 << 11;
	const int ItemFlag_Staff = 1 << 12;
	const int ItemFlag_LeftHand = 1 << 13;
	const int ItemFlag_RightHand = 1 << 14;
	const int ItemFlag_BothHands = 1 << 15;
	const int ItemFlag_MagicWeapon = 1 << 16;

	const int wear_torso = 1;
	const int wear_head = 2;
	const int wear_effect = 16;
	const int wear_gloves = 128;
	const int wear_boots = 256;
	const int wear_pants = 512;
	const int wear_pauldrons = 1024;

	const int item_swd = 16384;
	const int item_axe = 32768;
	const int item_2hd_swd = 65536;
	const int item_2hd_axe = 131072;
	const int item_bow = 524288;
	const int item_crossbow = 1048576;
	const int item_ring = 2048;
	const int item_amulet = 4194304;
	const int item_belt = 16777216;
	const int item_dag = 65539;
	const int item_shield = 262144;
	const int item_throw = 262147;

	const int bit_item_mag_sword = 262144;
	const int bit_item_orc_weapon = 32768;

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

	const int ThrowableVelocity = 2000;

	// **************************************************************
	//							Constanst
	// **************************************************************
	const int StExt_InfusionType_Affix = 0;
	const int StExt_InfusionType_Preffix = 1;
	const int StExt_InfusionType_Suffix = 2;

	const int StExt_Corruption_Mage = 1;
	const int StExt_Corruption_Warrior = 2;
	const int StExt_Corruption_Ranger = 3;

	const int dam_invalid = 0;
	const int dam_barrier = 1;
	const int dam_blunt = 2;
	const int dam_edge = 4;
	const int dam_fire = 8;
	const int dam_fly = 16;
	const int dam_magic = 32;
	const int dam_point = 64;
	const int dam_fall = 128;
	const int dam_poison = 5;

	const int dam_index_barrier = 0;
	const int dam_index_blunt = 1;
	const int dam_index_edge = 2;
	const int dam_index_fire = 3;
	const int dam_index_fly = 4;
	const int dam_index_magic = 5;
	const int dam_index_point = 6;
	const int dam_index_fall = 7;
	const int dam_index_max = 8;

	const int StExt_DamageType_Unknown = 0;
	const int StExt_DamageType_Melee = 1;
	const int StExt_DamageType_Range = 2;
	const int StExt_DamageType_Spell = 4;
	const int StExt_DamageType_Ability = 8;
	const int StExt_DamageType_Fire = 16;
	const int StExt_DamageType_Ice = 32;
	const int StExt_DamageType_Electric = 64;
	const int StExt_DamageType_Air = 128;
	const int StExt_DamageType_Earth = 256;
	const int StExt_DamageType_Light = 512;
	const int StExt_DamageType_Dark = 1024;
	const int StExt_DamageType_Death = 2048;
	const int StExt_DamageType_Life = 4096;
	const int StExt_DamageType_Poision = 8192;
	const int StExt_DamageType_True = 16384;

	const int StExt_DamageFlag_None = 0;
	const int StExt_DamageFlag_Aura = 1;
	const int StExt_DamageFlag_Dot = 2;
	const int StExt_DamageFlag_Aoe = 4;
	const int StExt_DamageFlag_Heal = 8;
	const int StExt_DamageFlag_Summon = 16;
	const int StExt_DamageFlag_Support = 32;
	const int StExt_DamageFlag_Unlock = 64;
	const int StExt_DamageFlag_Blink = 128;
	const int StExt_DamageFlag_Buff = 256;
	const int StExt_DamageFlag_Debuff = 512;
	const int StExt_DamageFlag_Timed = 1024;
	const int StExt_DamageFlag_Transform = 2048;
	const int StExt_DamageFlag_Golem = 4096;
	const int StExt_DamageFlag_Demon = 8192;
	const int StExt_DamageFlag_Undead = 16384;
	const int StExt_DamageFlag_Reflect = 32768;
	const int StExt_DamageFlag_Human = 65536;
	const int StExt_DamageFlag_Potion = 131072;
	const int StExt_DamageFlag_Orc = 262144;
	const int StExt_DamageFlag_Animal = 524288;
	const int StExt_DamageFlag_Target = 1048576;
	const int StExt_DamageFlag_Totem = 2097152;
	const int StExt_DamageFlag_Single = 4194304;
	const int StExt_DamageFlag_Freeze = 8388608;
	const int StExt_DamageFlag_Stun = 16777216;
	const int StExt_DamageFlag_Roots = 33554432;

	const int DamageDescFlag_ExtraDamage = 65536;
	const int DamageDescFlag_DotDamage = 131072;
	const int DamageDescFlag_ReflectDamage = 262144;
	const int DamageDescFlag_IsAbilityDamage = 524288;

	const int StExt_ArtifactIndex_None = 0;
	const int StExt_ArtifactIndex_Grimoir = 1;
	const int StExt_ArtifactIndex_Dagger = 2;
	const int StExt_ArtifactIndex_Max = 3;

	const int StExt_AiVar_IsRandomized = 351;
	const int StExt_AiVar_Uid = 354;
	const int StExt_AiVar_EsCur = 322;
	const int StExt_AiVar_EsMax = 323;

	const int Value_Type_Default = 0;
	const int Value_Type_Percent = 1;
	const int Value_Type_YesNo = 2;
	const int Value_Type_DefaultPerc = 3;
	const int Value_Type_InvertPerc = 4;

	const int StExt_DamageMessageType_Default = 0;
	const int StExt_DamageMessageType_Dot = 2;
	const int StExt_DamageMessageType_Reflect = 4;

	const int StExt_IncomingDamageFlag_Index_None = 0;
	const int StExt_IncomingDamageFlag_Index_Processed = 1;
	const int StExt_IncomingDamageFlag_Index_HasAttacker = 2;
	const int StExt_IncomingDamageFlag_Index_HasWeapon = 4;
	const int StExt_IncomingDamageFlag_Index_ExtraDamage = 8;

	// **************************************************************
	//					NB Hooks
	// **************************************************************
	//void AB_SNC::checkBonus( int damage ) 
	typedef void (*SncCheckBonusFuncType)(int);

	//extern SncCheckBonusFuncType SncCheckBonusFuncAdr;

	// **************************************************************
	//					NB Npc Definition
	// **************************************************************
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

		virtual void Archive(zCArchiver& ar);      // Вызывается при сохранении NPC в сейв
		virtual void Unarchive(zCArchiver& ar);  // Вызывается при чтении NPC из сейва
		virtual void ProcessNpc();

		~oCNpcEx();

		void AddTempIgnore(oCNpc* pNpc, int time);
		bool IsIgnoringNpc(oCNpc* pNpc);
		void ProcessIgnoreList();
	};
}
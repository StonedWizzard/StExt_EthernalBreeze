#include <UnionAfx.h>
#include <functional>
#include <vector>
#include <algorithm>
#include <functional>
#include <sapi.h>
#include <sphelper.h>

#include <StExt_Debug.h>
#include <StExt_IO.h>
#include <StExt_Constants.h>
#include <StExt_Classes.h>
#include <StExt_Helpers.h>
#include <StExt_Rand.h>
#include <StringCollections.h>

#include <ItemAbilities.h>
#include <ItemExtensions.h>
#include <CraftEngine.h>
#include <TimedEffect.h>
#include <Damage.h>
#include <ModUi.h>


namespace Gothic_II_Addon
{	
	//---------------------------------------------------------------------------
	//								GLOBAL VARS
	//---------------------------------------------------------------------------
	extern bool StExt_ModReady;

	extern Map<int, ExtraStatData> ExtraStatsData;
	extern Map<int, ExtraStatData> ExtraConditionStatsData;
	extern Map<int, zSTRING> ExtraStatsNameData;
	extern Map<int, AuraData> AurasData;

	extern Array<ConfigPresetData> GameConfigsPresets;
	extern Array<ModExtensionInfo> ModPluginsInfo;
	extern Array<WaypointData> ProhibitedWaypoints;

	extern Array<MagicInfusionData> InfusionAffixes;
	extern Array<MagicInfusionData> InfusionSuffixes;
	extern Array<MagicInfusionData> InfusionPreffixes;
	
	extern SpellInfo CurrentSpellInfo;
	extern zSTRING ModVersionString;
	extern int HeroNpcUId;

	extern int OnPreLoopFunc;
	extern int OnPostLoopFunc;
	extern int ProcessHpDamageFunc;
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
	extern int HandleUiButtomEventFunc;
	extern int HandlePcStatChangeFunc;
	extern int CanCallModMenuFunc;
	extern int SaveParserVarsFunc;
	extern int RestoreParserVarsFunc;
	extern int UpdateUiStatusFunc;
	extern int StExt_CheckConditionStatFunc;
	extern int StExt_OnAiStateFunc;
	extern int StExt_NpcToUidFunc;
	extern int StExt_ApplyPotionEffectFunc;
	extern int StExt_ApplyPotionPermEffectFunc;
	extern int StExt_GamePausedFunc;

	extern int StExt_InitializeCraftContextFunc;

	extern int StExt_OnSpellCastFunc;
	extern int StExt_OnSpellPreCastFunc;

	extern int StExt_CurrentDayPart;
	extern int MaxSpellId;
	extern int StExt_AbilityPrefix;
	extern int ItemCondSpecialSeparator;
	extern int MaxStatId;

	extern int StExt_AiVar_IsRandomized;
	extern int StExt_AiVar_Uid;
	extern int StExt_AiVar_EsCur;
	extern int StExt_AiVar_EsMax;

	extern int StExt_Config_NpcStats_TopOffset;
	extern int StExt_Config_NpcStats_HideTags;

	extern float ItemBasePriceMult;
	extern float ItemSellPriceMult;

	extern bool IsLevelChanging;
	extern bool IsLoading;
	extern int CanShowModMenu;
	extern int ShowPcEs;
	extern int BlockMovement;
	extern int PcEsPosX;
	extern int PcEsPosY;

	extern zSTRING PcEsCurStr;
	extern zSTRING StExt_EsText;
	extern zSTRING SecondsSuffixString;
	extern zSTRING CurrentDiffPresetStr;
	extern zSTRING CurrentItemsPresetStr;


	//-----------------------------------------------------------------
	//						GLOBAL FUNCTIONS
	//-----------------------------------------------------------------

	void StonedExtension_InitModData();
	void StonedExtension_Loop();
	void StonedExtension_DefineExternals();

	void LoadModState();
	void SaveModState();
	void ResetModState();	

	void SortZStringArray(Array<zSTRING>& arr);
	uint FindZStringSorted(const Array<zSTRING>& arr, const zSTRING& key);

	// Npc UId subsystem
	int GetNextNpcUid();
	void RegisterNpc(oCNpc* npc, int& npcUid);
	void RegisterNpc(oCNpc* npc);
	int GetNpcUid(oCNpc* npc);
	oCNpc* GetNpcByUid(int npcUid);
	bool IsUidRegistered(int npcUid);
	void ClearRegisteredNpcs();
	void RegisterNearestNpcs();

	void InitStatsUncaper();
	void StartUncaper();
	void StopUncaper();
	void StatsUncaperLoop();
	void inline UpdateUncapedStat(int indx);

	extern const AuraData* GetAuraById(const int id);
	extern const ExtraStatData* GetExtraStatDataById(const int id);
	extern const ExtraStatData* GetExtraConditionDataById(const int id);
	extern const zSTRING& GetExtraStatNameById(const int id);
	MagicInfusionData* CreateInfusion(int tier);
	extern const MagicInfusionData* GetRandomInfusion(int type, int tier, int grantInfusion);
	extern const ConfigPresetData* GetConfigPreset(const zSTRING& presetName);

	bool InitVoiceController();

	void* AllocateDynamicObject(size_t size);
	bool FreeDynamicObject(void* ptr);
	bool IsObjectsTableAllocated(void* ptr);
	bool WriteToDynamicObject(void* basePtr, size_t offset, int value);
	bool ReadFormDynamicObject(void* basePtr, size_t offset, int& outValue);
}
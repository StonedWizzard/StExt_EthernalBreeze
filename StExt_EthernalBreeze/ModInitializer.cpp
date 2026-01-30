#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	zFILE* DebugFile;
	zSTRING ModVersionString;
	bool StExt_ModReady = false;

	int OnPreLoopFunc = Invalid;
	int OnPostLoopFunc = Invalid;
	int ProcessHpDamageFunc = Invalid;
	int OnModDamageExpFunc = Invalid;
	int OnSncDamageFunc = Invalid;
	int PrintDamageFunc = Invalid;
	int OnDamageAfterFunc = Invalid;
	int OnDamageBeginFunc = Invalid;
	int OnPostDamageFunc = Invalid;
	int FxDamageCanBeAppliedFunc = Invalid;
	int IsNpcImmortalFunc = Invalid;
	int OnArmorEquipFunc = Invalid;
	int GetSpellDamageFlagsFunc = Invalid;
	int GetSpellEffectFlagsFunc = Invalid;
	int OnLoadEndFunc = Invalid;
	int OnLevelChangeFunc = Invalid;
	int NpcGetBarCurEsFunc = Invalid;
	int NpcGetBarMaxEsFunc = Invalid;
	int UpdateFocusNpcInfoFunc = Invalid;
	int IsHeroMovLockedFunc = Invalid;
	int HandleKeyEventFunc = Invalid;
	int HandleUiButtomEventFunc = Invalid;
	int HandlePcStatChangeFunc = Invalid;
	int CanCallModMenuFunc = Invalid;
	int SaveParserVarsFunc = Invalid;
	int RestoreParserVarsFunc = Invalid;
	int UpdateUiStatusFunc = Invalid;
	int IsExtraDamageProhibitedFunc = Invalid;
	int StExt_CheckConditionStatFunc = Invalid;
	int StExt_OnAiStateFunc = Invalid;
	int StExt_NpcToUidFunc = Invalid;
	int StExt_ApplyPotionEffectFunc = Invalid;
	int StExt_ApplyPotionPermEffectFunc = Invalid;
	int StExt_GamePausedFunc = Invalid;
	int StExt_InitializeCraftContextFunc = Invalid;
	int StExt_OnSpellCastFunc = Invalid;
	int StExt_OnSpellPreCastFunc = Invalid;

	int StExt_Config_NpcStats_TopOffset;
	int StExt_Config_NpcStats_HideTags;
	int StExt_CurrentDayPart;
	int MaxStatId;

	int StExt_AiVar_IsRandomized;
	int StExt_AiVar_Uid;
	int StExt_AiVar_EsCur;
	int StExt_AiVar_EsMax;

	zSTRING SecondsSuffixString;
	zSTRING StExt_EsText;
	zSTRING CurrentDiffPresetStr;
	zSTRING CurrentItemsPresetStr;
	float ItemBasePriceMult;
	float ItemSellPriceMult;
	bool IsLevelChanging;
	bool IsLoading;


	void CreateDebugFile()
	{
		string root = zoptions->GetDirString(zTOptionPaths::DIR_ROOT);
		string path = string::Combine("%s\\StExt_Debug.log", root);
		DebugFile = new zFILE_VDFS(path);
		if (DebugFile->Exists())
		{
			if (DebugFile->IsOpened()) DebugFile->Close();
			DebugFile->FileDelete();
		}
		DebugFile->Create();
		DEBUG_MSG("StExt - Debug system initialized!");
	}

	void PrintDebug(zSTRING message)
	{
		message = message + "\n";
		cmd << message;
		DebugFile->Write(message);
	}

	void DebugMessage(zSTRING message)
	{
		message = "[EthernalBreeze.dll] -> " + message + "\n";
		cmd << message;
		DebugFile->Write(message);
	}

	bool ParseTextFile(const zSTRING& path, zSTRING& buffer)
	{
		DEBUG_MSG("ParseTextFile: read '" + path + "' ...");

		zFILE_VDFS* originFile = new zFILE_VDFS(path);
		DEBUG_MSG_IF(originFile->IsOpened(), "ParseTextFile: file '" + path + "' is (wtf) opened?!");

		if (!originFile->Exists())
		{
			DEBUG_MSG("ParseTextFile: file '" + path + "' not exist!");
			SAFE_DELETE(originFile);
			return false;
		}

		originFile->Open(false);
		buffer.Clear();
		zSTRING line;
		do
		{
			originFile->Read(line);
			buffer += line;
		} while (!originFile->Eof());
		bool isSuccess = true;

		if (originFile && originFile->IsOpened()) originFile->Close();
		SAFE_DELETE(originFile);
		DEBUG_MSG_IF(!isSuccess, "ParseTextFile: '" + Z path + "' readed!");
		return isSuccess;
	}

	zSTRING GetModVersion()
	{
		zCParser* par = zCParser::GetParser();
		zCPar_Symbol* verSym = par->GetSymbol("StExt_CurrentModVersionString");
		return verSym->stringdata;
	}

	void BuildModVersionString()
	{
		int setVerFunc = parser->GetIndex("StExt_SetModVersionString");
		parser->CallFunc(setVerFunc);
		ModVersionString = Z("Ethernal Breeze mod [" + GetModVersion() + " (Build: 7.0.0)]");
		
#if DebugEnabled 
		ModVersionString += Z(" | [Debug]"); 
#endif
	}


	void InitAuraData()
	{
		zCParser* par = zCParser::GetParser();
		zCPar_Symbol* auraIndxArray = par->GetSymbol("StExt_AurasIndexArray");
		if (!auraIndxArray)
		{
			DEBUG_MSG("InitAuraData: 'StExt_AurasIndexArray' not found!");
			return;
		}

		AurasData = Map<int, AuraData>();
		DEBUG_MSG("Initialize " + Z((int)auraIndxArray->ele) + " aura instances...");
		for (uint i = 0; i < auraIndxArray->ele; ++i)
		{
			zSTRING strIndx = auraIndxArray->stringdata[i];
			int index = par->GetIndex(strIndx);
			if (index == Invalid)
			{
				DEBUG_MSG("InitAuraData: Can't load aura instance '" + strIndx + "'!");
				continue;
			}
			AuraData aura = AuraData();
			parser->CreateInstance(index, &aura);
			AurasData.Insert(aura.Id, aura);
		}
		DEBUG_MSG("InitAuraData: Auras was initialized!");
	}

	void InitExtraMasteriesData()
	{
		DEBUG_MSG("Initialize masteries data...");

		const int corruptionMaxIndex = parser->GetSymbol("StExt_Corruption_Max")->single_intdata;
		const int corruptionPerksMax = parser->GetSymbol("StExt_CorruptionPerk_Max")->single_intdata;
		const zSTRING corruptionsSuffix[] = { zSTRING(""), zSTRING("Mage"), zSTRING("Warrior"), zSTRING("Ranger") };
		for (int i = 1; i < corruptionMaxIndex; ++i)
		{
			ExtraMasteryData data = ExtraMasteryData();

			data.MasteryId = i;
			data.IsCorruption = true;
			data.IsGeneric = false;
			data.PerksCount = corruptionPerksMax;

			data.TitleSymbol = "StExt_Str_CorruptionName";
			data.DescSymbol = "StExt_Str_CorruptionDesc";
			data.RankSymbol = zSTRING();
			data.LevelSymbol = "StExt_CorruptionPath_Level";
			data.ExpSymbol = "StExt_CorruptionPath_ExpNow";
			data.NextExpSymbol = "StExt_CorruptionPath_ExpNext";
			data.LpSymbol = "StExt_CorruptionPath_SkillPoints";

			data.PerkNameSymbol = "StExt_Corruption_Perk_Name_" + corruptionsSuffix[i];
			data.PerkDescSymbol = "StExt_Corruption_Perk_Desc_" + corruptionsSuffix[i];
			data.PerkValueSymbol = "StExt_Corruption_Perk_" + corruptionsSuffix[i];

			ExtraMasteriesData.Insert(data);
		}

		const int masteriesMaxIndex = parser->GetSymbol("StExt_MasteryIndex_Max")->single_intdata;
		const int masteriesPerksMax = parser->GetSymbol("StExt_MasteryPerk_Max")->single_intdata;
		for (int i = 0; i < masteriesMaxIndex; ++i)
		{
			ExtraMasteryData data = ExtraMasteryData();

			data.MasteryId = i;
			data.IsCorruption = false;
			data.IsGeneric = false;
			data.PerksCount = masteriesPerksMax;

			data.TitleSymbol = "StExt_Str_MasteryNames";
			data.DescSymbol = "StExt_Str_MasteryDescription";
			data.RankSymbol = "StExt_Talent_Level";
			data.LevelSymbol = "StExt_Talent_Progression";
			data.ExpSymbol = "StExt_Talent_ExpNow";
			data.NextExpSymbol = "StExt_Talent_ExpNext";
			data.LpSymbol = "StExt_Talent_PerkPoints";

			data.PerkNameSymbol = "StExt_Str_MasteryPerk_Name_" + Z(i);
			data.PerkDescSymbol = "StExt_Str_MasteryPerk_Desc_" + Z(i);
			data.PerkValueSymbol = "StExt_Perk_" + Z(i);

			ExtraMasteriesData.Insert(data);
		}

		const int generalPerksMax = parser->GetSymbol("StExt_Perk_Max")->single_intdata;
		ExtraMasteryData genericData = ExtraMasteryData();

		genericData.MasteryId = 0;
		genericData.IsCorruption = false;
		genericData.IsGeneric = true;
		genericData.PerksCount = generalPerksMax;

		genericData.TitleSymbol = "StExt_Str_GeneralPerks_Header";
		genericData.DescSymbol = zSTRING();
		genericData.RankSymbol = zSTRING();
		genericData.LevelSymbol = zSTRING();
		genericData.ExpSymbol = zSTRING();
		genericData.NextExpSymbol = zSTRING();
		genericData.LpSymbol = zSTRING();

		genericData.PerkNameSymbol = "StExt_Str_Perk_Name";
		genericData.PerkDescSymbol = "StExt_Str_Perk_Desc";
		genericData.PerkValueSymbol = "StExt_Perk";

		ExtraMasteriesData.Insert(genericData);
		DEBUG_MSG("Masteries data was initialized!");
	}

	void InitExtraStatsData(Map<int, ExtraStatData> &statsData, const zSTRING& initArray)
	{
		zCParser* par = zCParser::GetParser();
		zCPar_Symbol* statsIndxArray = par->GetSymbol(initArray);
		if (!statsIndxArray)
		{
			DEBUG_MSG("InitExtraStatsData - '" + initArray + "' not found!");
			return;
		}

		DEBUG_MSG("Initialize " + Z((int)statsIndxArray->ele) + " stat instances...");
		for (unsigned int i = 0; i < statsIndxArray->ele; ++i)
		{
			zSTRING strIndx = statsIndxArray->stringdata[i];
			int index = par->GetIndex(strIndx);
			if (index == Invalid)
			{
				DEBUG_MSG("InitExtraStatsData - Can't load infusion instance '" + strIndx + "'!");
				continue;
			}
			ExtraStatData statData = ExtraStatData();
			parser->CreateInstance(index, &statData);
			statsData.Insert(statData.Id, statData);
		}
		MaxStatId = par->GetSymbol("StExt_PcStats_Index_Max")->single_intdata;
		DEBUG_MSG("InitExtraStatsData - '" + initArray + "' was initialized!");
	}

	void InitExtraStatsData()
	{
		InitExtraStatsData(ExtraStatsData, "StExt_ExtraStatData_IndexArray");
		InitExtraStatsData(ExtraConditionStatsData, "StExt_ExtraConditionStatData_IndexArray");

		InitStatsUncaper();
		ExtraStatsNameData = Map<int, zSTRING>();
		zCPar_Symbol* statsNamesArray = parser->GetSymbol("StExt_PcStats_Desc");
		if (!statsNamesArray)
		{
			DEBUG_MSG("InitExtraStatsData - 'StExt_PcStats_Desc' not found!");
			return;
		}

		for (uint i = 0; i < statsNamesArray->ele; ++i)
			ExtraStatsNameData.Insert(static_cast<int>(i), statsNamesArray->stringdata[i]);
	}

	void InitInfusionData(Array<MagicInfusionData>& InfusionData, const zSTRING& initArray)
	{
		zCParser* par = zCParser::GetParser();
		zCPar_Symbol* infusionIndxArray = par->GetSymbol(initArray);
		if (!infusionIndxArray)
		{
			DEBUG_MSG("InitInfusionData - '" + initArray + "' not found!");
			return;
		}
		DEBUG_MSG("InitInfusionData - Init infusion from: " + initArray + ". size: " + Z((int)infusionIndxArray->ele) + " ...");

		InfusionData = Array<MagicInfusionData>();
		for (uint i = 0; i < infusionIndxArray->ele; ++i)
		{
			zSTRING strIndx = infusionIndxArray->stringdata[i];
			int index = par->GetIndex(strIndx);
			if (index == Invalid)
			{
				DEBUG_MSG("InitInfusionData - Can't load infusion instance '" + strIndx + "'!");
				continue;
			}

			MagicInfusionData infusion = MagicInfusionData();
			parser->CreateInstance(index, &infusion);
			InfusionData.Insert(infusion);
		}
		DEBUG_MSG("InitInfusionData - '" + initArray + "' was initialized!");
	}

	void InitNpcInfusionsData()
	{
		InitInfusionData(InfusionAffixes, "StExt_AffixesIndexArray");
		InitInfusionData(InfusionPreffixes, "StExt_PreffixesIndexArray");
		InitInfusionData(InfusionSuffixes, "StExt_SuffixesIndexArray");
	}


	void InitScriptData()
	{
		DEBUG_MSG("StExt - Initialize scripts data...");

		IsLevelChanging = false;
		IsLoading = false;
		parser->SetInstance("STEXT_NULLNPC", Null);
		parser->SetInstance("STEXT_NULLITEM", Null);
		parser->GetSymbol("StExt_Fps")->SetValue(60, 0);
		parser->GetSymbol("StExt_SizeOf_Int")->SetValue(static_cast<int>(sizeof(int)), 0);

		OnPreLoopFunc = parser->GetIndex("StExt_OnFrameBegin");
		DEBUG_MSG_IF(OnPreLoopFunc == Invalid, "OnPreLoopFunc is null!");

		OnPostLoopFunc = parser->GetIndex("StExt_OnFrameEnd");
		DEBUG_MSG_IF(OnPostLoopFunc == Invalid, "OnPostLoopFunc is null!");

		ProcessHpDamageFunc = parser->GetIndex("StExt_ProcessHpDamage");
		DEBUG_MSG_IF(ProcessHpDamageFunc == Invalid, "ProcessHpDamageFunc is null!");

		PrintDamageFunc = parser->GetIndex("StExt_PrintExtraDamage");
		DEBUG_MSG_IF(PrintDamageFunc == Invalid, "PrintDamageFunc is null!");

		OnDamageAfterFunc = parser->GetIndex("StExt_OnDamageAfter");
		DEBUG_MSG_IF(OnDamageAfterFunc == Invalid, "OnDamageAfterFunc is null!");

		OnDamageBeginFunc = parser->GetIndex("StExt_OnDamageBegin");
		DEBUG_MSG_IF(OnDamageBeginFunc == Invalid, "OnDamageBeginFunc is null!");

		OnPostDamageFunc = parser->GetIndex("StExt_OnPostDamage");
		DEBUG_MSG_IF(OnPostDamageFunc == Invalid, "OnPostDamageFunc is null!");

		FxDamageCanBeAppliedFunc = parser->GetIndex("StExt_FxDamageCanBeApplied");
		DEBUG_MSG_IF(FxDamageCanBeAppliedFunc == Invalid, "FxDamageCanBeAppliedFunc is null!");

		IsNpcImmortalFunc = parser->GetIndex("StExt_IsNpcImmortal_Engine");
		DEBUG_MSG_IF(IsNpcImmortalFunc == Invalid, "IsNpcImmortalFunc is null!");

		OnArmorEquipFunc = parser->GetIndex("StExt_OnArmorEquip");
		DEBUG_MSG_IF(OnArmorEquipFunc == Invalid, "OnArmorEquipFunc is null!");

		GetSpellDamageFlagsFunc = parser->GetIndex("StExt_GetSpellDamageFlags");
		DEBUG_MSG_IF(GetSpellDamageFlagsFunc == Invalid, "GetSpellDamageFlagsFunc is null!");

		GetSpellEffectFlagsFunc = parser->GetIndex("StExt_GetSpellEffectFlags");
		DEBUG_MSG_IF(GetSpellEffectFlagsFunc == Invalid, "GetSpellEffectFlagsFunc is null!");

		OnLoadEndFunc = parser->GetIndex("StExt_OnLoadEnd");
		DEBUG_MSG_IF(OnLoadEndFunc == Invalid, "OnLoadEndFunc is null!");

		OnLevelChangeFunc = parser->GetIndex("StExt_OnLevelChange");
		DEBUG_MSG_IF(OnLevelChangeFunc == Invalid, "OnLevelChangeFunc is null!");

		NpcGetBarCurEsFunc = parser->GetIndex("StExt_Npc_GetBarCurEs");
		DEBUG_MSG_IF(NpcGetBarCurEsFunc == Invalid, "NpcGetBarCurEsFunc is null!");

		NpcGetBarMaxEsFunc = parser->GetIndex("StExt_Npc_GetBarMaxEs");
		DEBUG_MSG_IF(NpcGetBarMaxEsFunc == Invalid, "NpcGetBarMaxEsFunc is null!");

		UpdateFocusNpcInfoFunc = parser->GetIndex("StExt_UpdateFocusNpcInfo");
		DEBUG_MSG_IF(UpdateFocusNpcInfoFunc == Invalid, "UpdateFocusNpcInfoFunc is null!");

		CanCallModMenuFunc = parser->GetIndex("StExt_CanCallModMenu");
		DEBUG_MSG_IF(CanCallModMenuFunc == Invalid, "CanCallModMenuFunc is null!");

		IsHeroMovLockedFunc = parser->GetIndex("StExt_IsHeroMovLocked");
		DEBUG_MSG_IF(IsHeroMovLockedFunc == Invalid, "IsHeroMovLockedFunc is null!");

		HandleKeyEventFunc = parser->GetIndex("StExt_HandleKeyEvent");
		DEBUG_MSG_IF(HandleKeyEventFunc == Invalid, "HandleKeyEventFunc is null!");

		//HandleUiButtomEventFunc
		HandleUiButtomEventFunc = parser->GetIndex("StExt_HandleUiButtom");
		DEBUG_MSG_IF(HandleUiButtomEventFunc == Invalid, "HandleUiButtomEventFunc is null!");

		HandlePcStatChangeFunc = parser->GetIndex("StExt_HandlePcStatChange");
		DEBUG_MSG_IF(HandlePcStatChangeFunc == Invalid, "HandlePcStatChangeFunc is null!");

		SaveParserVarsFunc = parser->GetIndex("StExt_SaveParserVars");
		DEBUG_MSG_IF(SaveParserVarsFunc == Invalid, "SaveParserVarsFunc is null!");

		RestoreParserVarsFunc = parser->GetIndex("StExt_RestoreParserVars");
		DEBUG_MSG_IF(RestoreParserVarsFunc == Invalid, "RestoreParserVarsFunc is null!");

		UpdateUiStatusFunc = parser->GetIndex("StExt_UpdateUiStatus");
		DEBUG_MSG_IF(UpdateUiStatusFunc == Invalid, "UpdateUiStatusFunc is null!");

		IsExtraDamageProhibitedFunc = parser->GetIndex("StExt_UpdateUiStatus");
		DEBUG_MSG_IF(IsExtraDamageProhibitedFunc == Invalid, "IsExtraDamageProhibitedFunc is null!");

		StExt_CheckConditionStatFunc = parser->GetIndex("StExt_CheckConditionStat");
		DEBUG_MSG_IF(StExt_CheckConditionStatFunc == Invalid, "StExt_CheckConditionStatFunc is null!");

		StExt_OnAiStateFunc = parser->GetIndex("StExt_OnAiState");
		DEBUG_MSG_IF(StExt_OnAiStateFunc == Invalid, "StExt_OnAiStateFunc is null!");

		StExt_NpcToUidFunc = parser->GetIndex("StExt_NpcToUid");
		DEBUG_MSG_IF(StExt_NpcToUidFunc == Invalid, "StExt_NpcToUidFunc is null!");

		StExt_ApplyPotionEffectFunc = parser->GetIndex("StExt_ApplyPotionEffect");
		DEBUG_MSG_IF(StExt_ApplyPotionEffectFunc == Invalid, "StExt_ApplyPotionEffectFunc is null!");

		StExt_ApplyPotionPermEffectFunc = parser->GetIndex("StExt_ApplyPotionPermEffect");
		DEBUG_MSG_IF(StExt_ApplyPotionPermEffectFunc == Invalid, "StExt_ApplyPotionPermEffectFunc is null!");
		
		StExt_OnSpellCastFunc = parser->GetIndex("StExt_OnSpellCast");
		DEBUG_MSG_IF(StExt_OnSpellCastFunc == Invalid, "StExt_OnSpellCastFunc is null!");

		StExt_OnSpellPreCastFunc = parser->GetIndex("StExt_OnSpellPreCast");
		DEBUG_MSG_IF(StExt_OnSpellPreCastFunc == Invalid, "StExt_OnSpellPreCastFunc is null!");

		StExt_GamePausedFunc = parser->GetIndex("StExt_GamePaused");
		DEBUG_MSG_IF(StExt_GamePausedFunc == Invalid, "StExt_GamePausedFunc is null!");

		StExt_InitializeCraftContextFunc = parser->GetIndex("StExt_InitializeCraftContext");
		DEBUG_MSG_IF(StExt_InitializeCraftContextFunc == Invalid, "StExt_InitializeCraftContextFunc is null!");
		

		StExt_EsText = parser->GetSymbol("StExt_EsText")->stringdata;
		SecondsSuffixString = parser->GetSymbol("StExt_Str_Seconds")->stringdata;
		ItemCondSpecialSeparator = parser->GetSymbol("StExt_Item_Cond_Separator")->single_intdata;
		StExt_AbilityPrefix = parser->GetSymbol("StExt_AbilityPrefix")->single_intdata;
		CurrentDiffPresetStr = parser->GetSymbol("StExt_Str_ModMenu_CurrentDiffPreset")->stringdata;
		CurrentItemsPresetStr = parser->GetSymbol("StExt_Str_ModMenu_CurrentItemsPreset")->stringdata;

		MaxSpellId = parser->GetSymbol("max_spell")->single_intdata;
		DEBUG_MSG("StExt - MaxSpellId: " + Z MaxSpellId);

		zCPar_Symbol* spellfxinstancenamesSym = parser->GetSymbol("spellfxinstancenames");
		for (int i = 0; i < MaxSpellId; ++i)
			SpellFxNames.Insert(spellfxinstancenamesSym->stringdata[i].Upper(), i);

		StExt_Config_NpcStats_TopOffset = parser->GetSymbol("StExt_Config_NpcStatsUi_TopOffset")->single_intdata;
		StExt_Config_NpcStats_HideTags = parser->GetSymbol("StExt_Config_NpcStatsUi_HideTags")->single_intdata;

		StExt_AiVar_IsRandomized = parser->GetSymbol("StExt_AiVar_IsRandomized")->single_intdata;
		StExt_AiVar_Uid = parser->GetSymbol("StExt_AiVar_Uid")->single_intdata;
		StExt_AiVar_EsCur = parser->GetSymbol("StExt_AiVar_EsCur")->single_intdata;
		StExt_AiVar_EsMax = parser->GetSymbol("StExt_AiVar_EsMax")->single_intdata;

		ItemBasePriceMult = parser->GetSymbol("StExt_ItemBasicPriceMult")->single_floatdata;
		if (ItemBasePriceMult < 0.1f) ItemBasePriceMult = 0.1f;
		if (ItemBasePriceMult > 100.0f) ItemBasePriceMult = 100.0f;

		DEBUG_MSG("StExt - Scripts data initialized!");
	}

	void InitModScriptData()
	{
		InitExtraMasteriesData();
		InitExtraStatsData();
		InitAuraData();
		InitNpcInfusionsData();
		InitItemAbilitiesData();

		ProhibitedWaypoints = Array<WaypointData>();
		parser->CallFunc(parser->GetIndex("StExt_OnModLoaded"));
	}

	void StonedExtension_InitModData()
	{
		DEBUG_MSG("DebugMode - " + zSTRING(parser->GetSymbol("StExt_Config_DebugAlwaysEnabled")->single_intdata));
		BuildModVersionString();		
		DEBUG_MSG(ModVersionString);
		DEBUG_MSG("");

		InitScriptData();
		InitModScriptData();
		InitVoiceController();
		InitItemsExtension();
		DEBUG_MSG("StExt - Mod data initialized!");
	}
}
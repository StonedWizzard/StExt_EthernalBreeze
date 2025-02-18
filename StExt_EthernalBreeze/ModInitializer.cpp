#include <UnionAfx.h>
#include <string> 
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    zFILE* DebugFile;
    static nlohmann::json JsonFile;
    ItemsGeneratorConfigs GeneratorConfigs;

    zSTRING ModVersionString;

    int OnPreLoopFunc = Invalid;
    int OnPostLoopFunc = Invalid;
    //int ApplyResistsFunc = Invalid;
    //int ApplyDamageToEsFunc = Invalid;
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
    int HandlePcStatChangeFunc = Invalid;
    int CanCallModMenuFunc = Invalid;
    int SaveParserVarsFunc = Invalid;
    int RestoreParserVarsFunc = Invalid;
    int UpdateUiStatusFunc = Invalid;
    int IsExtraDamageProhibitedFunc = Invalid;
    int StExt_CheckConditionStatFunc = Invalid;
    int StExt_OnAiStateFunc = Invalid;

    int StExt_Config_NpcStats_TopOffset;
    int StExt_Config_NpcStats_HideTags;

    zSTRING StExt_EsText;
    zSTRING* SpellFxNames;

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

    bool ParseModData(const string path)
    {
        zFILE_VDFS* originFile = new zFILE_VDFS(path);
        if (!originFile->Exists())
        {
            string msg = string::Combine("EthernalBreeze: data file %s not found!", originFile->complete_path);
            Message::Error(msg);
            delete originFile;
            return false;
        }
        originFile->Open(false);

        zSTRING line, buffer;
        do {
            originFile->Read(line);
            buffer += line;
        } while (!originFile->Eof());
        
        try { JsonFile = nlohmann::json::parse(buffer.ToChar()); }
        catch (const std::exception&)
        {
            DEBUG_MSG("Can't load json file - " + Z path);
            return false;
        }
        
        originFile->Close();
        DEBUG_MSG("Json file - '" + Z path + "' parsed!");
        return true;
    }

    astring UTF8_To_ANSI(byte* byteArray, uint strLength, uint codePage) 
    {
        UTF8StringConverter utf8_converter;
        Array<byte>& utf8 = *(Array<byte>*) & utf8_converter;
        ArrayLocator<byte>& allocator = const_cast<ArrayLocator<byte>&>(utf8.GetArrayLocator());
        allocator.PrepareToReserveArray(strLength + 1);
        allocator.ActivateAllocatedMemory();
        memcpy(&utf8[0], byteArray, strLength + 1);
        byteArray[strLength] = 0;
        astring ansi;
        utf8_converter.ToAnsi(ansi, codePage);
        return ansi;
    }

    inline zSTRING ToZString(std::string input, uint codePage = 0)
    {
        zSTRING result = "";
        if (codePage == 0)
            result = new zSTRING(input.c_str());        
        else if (input.length() > 0)
        {
            string ansi = UTF8_To_ANSI((byte*)input.c_str(), input.length(), codePage);
            result = Z ansi;
        }
        return result;
    }

    inline Array<zSTRING> ReadJsonStringArray(std::string name, uint codePage = 0)
    {
        DEBUG_MSG("Parse json string array: '" + Z name.c_str() + "'...");
        Array<zSTRING> result = Array<zSTRING>();
        auto arr = JsonFile[name];

        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            std::string str = it.value().get<std::string>();
            if (codePage == 0)
            {
                zSTRING tmp = new zSTRING(str.c_str());
                result.Insert(tmp);
            }
            else
            {
                string ansi = UTF8_To_ANSI((byte*)str.c_str(), str.length(), codePage);
                result.Insert(Z ansi);
            }
        }
        DEBUG_MSG("Loaded " + Z(int)result.GetNum() + " strings for '" + Z name.c_str() + "'");
        return result;
    }

    inline void InitItemGeneratorConfigs_ItemsConditions()
    {
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' -> read item conditions list...");
        for (auto it = JsonFile["ConditionsList"].begin(); it != JsonFile["ConditionsList"].end(); ++it)
        {
            auto jObj = it.value();
            ItemCondition obj = ItemCondition();

            obj.Id = jObj["Id"].get<int>();
            obj.HardCap = jObj["HardCap"].get<int>();
            obj.PerLevelMin = jObj["PerLevelMin"].get<float>();
            obj.PerLevelMax = jObj["PerLevelMax"].get<float>();
            obj.AllowedItemTypes = jObj["AllowedItemTypes"].get<int>();

            GeneratorConfigs.ConditionsList.Insert(obj);
        }
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' -> loaded " + Z (int)GeneratorConfigs.ConditionsList.GetNum() + " item conditions.");
    }
    inline void InitItemGeneratorConfigs_StatsOptions()
    {
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' -> read item stats list...");
        for (auto it = JsonFile["StatsOptions"].begin(); it != JsonFile["StatsOptions"].end(); ++it)
        {
            auto jObj = it.value();
            ItemStatOption obj = ItemStatOption();

            obj.StatName = ToZString(jObj["StatName"].get<std::string>(), (uint)GeneratorConfigs.CodePage);
            obj.StatId = jObj["StatId"].get<int>();
            obj.StatMaxCap = jObj["StatMaxCap"].get<int>();
            obj.StatMinCap = jObj["StatMinCap"].get<int>();            
            obj.StatValueMin = jObj["StatValueMin"].get<float>();
            obj.StatValueMax = jObj["StatValueMax"].get<float>();
            obj.IncopatibleItemTypes = jObj["IncopatibleItemTypes"].get<int>();
            obj.Rarity = jObj["Rarity"].get<int>();
            obj.Price = jObj["Price"].get<int>();

            GeneratorConfigs.StatsOptions.Insert(obj);
        }
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' -> loaded " + Z(int)GeneratorConfigs.StatsOptions.GetNum() + " item stats.");
    }
    inline void InitItemGeneratorConfigs_AbilitiesOptions()
    {
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' -> read item abilities list...");
        for (auto it = JsonFile["AbilitiesOptions"].begin(); it != JsonFile["AbilitiesOptions"].end(); ++it)
        {
            auto jObj = it.value();
            ItemAbilityOption obj = ItemAbilityOption();

            obj.AbilityId = jObj["AbilityId"].get<int>();
            obj.AbilityValueMin = jObj["AbilityValueMin"].get<int>();
            obj.AbilityValueMax = jObj["AbilityValueMax"].get<int>();
            obj.AbilityChanceMin = jObj["AbilityChanceMin"].get<int>();
            obj.AbilityChanceMax = jObj["AbilityChanceMax"].get<int>();
            obj.AbilityDurationMin = jObj["AbilityDurationMin"].get<int>();
            obj.AbilityDurationMax = jObj["AbilityDurationMax"].get<int>();
            obj.IncopatibleItemTypes = jObj["IncopatibleItemTypes"].get<int>();
            obj.Rarity = jObj["Rarity"].get<int>();
            obj.Price = jObj["Price"].get<int>();

            GeneratorConfigs.AbilitiesOptions.Insert(obj);
        }
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' -> loaded " + Z(int)GeneratorConfigs.AbilitiesOptions.GetNum() + " item abilities.");
    }
    inline void ReadItemGeneratorJsonConfigs()
    {
        GeneratorConfigs.CodePage = JsonFile["CodePage"].get<int>();

        GeneratorConfigs.NextRankOffset = JsonFile["NextRankOffset"].get<int>();
        GeneratorConfigs.NextRankOffsetMin = JsonFile["NextRankOffsetMin"].get<int>();        
        GeneratorConfigs.ItemLevelFromPowerRatio = JsonFile["ItemLevelFromPowerRatio"].get<float>();
        GeneratorConfigs.MedTierSeparator = JsonFile["MedTierSeparator"].get<int>();
        GeneratorConfigs.TopTierSeparator = JsonFile["TopTierSeparator"].get<int>();

        GeneratorConfigs.GetAlreadyGeneratedItemAfterCount = JsonFile["GetAlreadyGeneratedItemAfterCount"].get<int>();
        GeneratorConfigs.ChanceGetAlreadyGeneratedItem = JsonFile["ChanceGetAlreadyGeneratedItem"].get<float>();

        GeneratorConfigs.PriceMultGlobal = JsonFile["PriceMultGlobal"].get<float>();
        GeneratorConfigs.PriceMultPerLevel = JsonFile["PriceMultPerLevel"].get<float>();
        GeneratorConfigs.PriceMultPerRank = JsonFile["PriceMultPerRank"].get<float>();

        GeneratorConfigs.MaxItemLevel = JsonFile["MaxItemLevel"].get<int>();
        GeneratorConfigs.MaxItemDamage = JsonFile["MaxItemDamage"].get<int>();
        GeneratorConfigs.MaxItemProtection = JsonFile["MaxItemProtection"].get<int>();

        GeneratorConfigs.DamagePerLevelMin = JsonFile["DamagePerLevelMin"].get<float>();
        GeneratorConfigs.DamagePerLevelMax = JsonFile["DamagePerLevelMax"].get<float>();
        GeneratorConfigs.DamagePerRankMult = JsonFile["DamagePerRankMult"].get<float>();
        GeneratorConfigs.TwoHandedDamageMult = JsonFile["TwoHandedDamageMult"].get<float>();
        GeneratorConfigs.ExtraDamageTypesCap = JsonFile["ExtraDamageTypesCap"].get<int>();
        GeneratorConfigs.ExtraDamageMaxMult = JsonFile["ExtraDamageMaxMult"].get<float>();
        GeneratorConfigs.ExtraDamageMinMult = JsonFile["ExtraDamageMinMult"].get<float>();
        GeneratorConfigs.ExtraDamageChanceBase = JsonFile["ExtraDamageChanceBase"].get<float>();
        GeneratorConfigs.ExtraDamageChancePerRankMult = JsonFile["ExtraDamageChancePerRankMult"].get<float>();
        GeneratorConfigs.ExtraDamageChancePerLevelMult = JsonFile["ExtraDamageChancePerLevelMult"].get<float>();
        GeneratorConfigs.RangeSpread = JsonFile["RangeSpread"].get<float>();
        GeneratorConfigs.OrigDamagePerLevelMult = JsonFile["OrigDamagePerLevelMult"].get<float>();

        GeneratorConfigs.ProtectionPerLevelMin = JsonFile["ProtectionPerLevelMin"].get<float>();
        GeneratorConfigs.ProtectionPerLevelMax = JsonFile["ProtectionPerLevelMax"].get<float>();
        GeneratorConfigs.ProtectionPerRankMult = JsonFile["ProtectionPerRankMult"].get<float>();
        GeneratorConfigs.ExtraProtectionMaxMult = JsonFile["ExtraProtectionMaxMult"].get<float>();
        GeneratorConfigs.ExtraProtectionMinMult = JsonFile["ExtraProtectionMinMult"].get<float>();
        GeneratorConfigs.ExtraProtectionChanceBase = JsonFile["ExtraProtectionChanceBase"].get<float>();
        GeneratorConfigs.ExtraProtectionChancePerRankMult = JsonFile["ExtraProtectionChancePerRankMult"].get<float>();
        GeneratorConfigs.ExtraProtectionChancePerLevelMult = JsonFile["ExtraProtectionChancePerLevelMult"].get<float>();
        GeneratorConfigs.OrigProtectionPerLevelMult = JsonFile["OrigProtectionPerLevelMult"].get<float>();

        GeneratorConfigs.ConditionStaticMult = JsonFile["ConditionStaticMult"].get<float>();
        GeneratorConfigs.BaseConditionStaticMult = JsonFile["BaseConditionStaticMult"].get<float>();
        GeneratorConfigs.ConditionPerLevelMult = JsonFile["ConditionPerLevelMult"].get<float>();
        GeneratorConfigs.ConditionPerRankMult = JsonFile["ConditionPerRankMult"].get<float>();
        GeneratorConfigs.BaseConditionPerLevelMult = JsonFile["BaseConditionPerLevelMult"].get<float>();
        GeneratorConfigs.BaseConditionPerRankMult = JsonFile["BaseConditionPerRankMult"].get<float>();
        GeneratorConfigs.ExtraConditionChanceBase = JsonFile["ExtraConditionChanceBase"].get<float>();
        GeneratorConfigs.ExtraConditionChancePerRankMult = JsonFile["ExtraConditionChancePerRankMult"].get<float>();
        GeneratorConfigs.ExtraConditionChancePerLevelMult = JsonFile["ExtraConditionChancePerLevelMult"].get<float>();

        GeneratorConfigs.StatsMaxCap = JsonFile["StatsMaxCap"].get<int>();
        GeneratorConfigs.StatsMaxPerRank = JsonFile["StatsMaxPerRank"].get<int>();
        GeneratorConfigs.StatsMinPerRank = JsonFile["StatsMinPerRank"].get<int>();
        GeneratorConfigs.ExtraStatsPerLevel = JsonFile["ExtraStatsPerLevel"].get<float>();
        GeneratorConfigs.StatPowerPerLevelMult = JsonFile["StatPowerPerLevelMult"].get<float>();
        GeneratorConfigs.StatPowerPerRankMult = JsonFile["StatPowerPerRankMult"].get<float>();
        GeneratorConfigs.StatTimedEffectDurationBaseMax = JsonFile["StatTimedEffectDurationBaseMax"].get<int>();
        GeneratorConfigs.StatTimedEffectDurationBaseMin = JsonFile["StatTimedEffectDurationBaseMin"].get<int>();
        GeneratorConfigs.StatTimedEffectPowerMult = JsonFile["StatTimedEffectPowerMult"].get<float>();
        GeneratorConfigs.StatTimedEffectPriceMult = JsonFile["StatTimedEffectPriceMult"].get<float>();
        GeneratorConfigs.StatDurationPerLevelMult = JsonFile["StatDurationPerLevelMult"].get<float>();
        GeneratorConfigs.StatDurationPerRankMult = JsonFile["StatDurationPerRankMult"].get<float>();
        GeneratorConfigs.StatGlobalPowerMult = JsonFile["StatGlobalPowerMult"].get<float>();
        GeneratorConfigs.StatGlobalGetChanceMult = JsonFile["StatGlobalGetChanceMult"].get<float>();
        GeneratorConfigs.StatGetChanceFromLevelMult = JsonFile["StatGetChanceFromLevelMult"].get<float>();
        GeneratorConfigs.StatGetChanceFromRankMult = JsonFile["StatGetChanceFromRankMult"].get<float>();
        GeneratorConfigs.StatMaxCapBonusFromLevel = JsonFile["StatMaxCapBonusFromLevel"].get<float>();
        GeneratorConfigs.StatMaxCapBonusFromRank = JsonFile["StatMaxCapBonusFromRank"].get<float>();

        GeneratorConfigs.AbilitiesMaxCap = JsonFile["AbilitiesMaxCap"].get<int>();
        GeneratorConfigs.AbilitiesBeginsOnRank = JsonFile["AbilitiesBeginsOnRank"].get<int>();
        GeneratorConfigs.AbilitiesGetChanceBase = JsonFile["AbilitiesGetChanceBase"].get<float>();
        GeneratorConfigs.AbilitiesGetChancePerLevelMult = JsonFile["AbilitiesGetChancePerLevelMult"].get<float>();
        GeneratorConfigs.AbilitiesGetChancePerRankMult = JsonFile["AbilitiesGetChancePerRankMult"].get<float>();
        GeneratorConfigs.AbilitiesPowerPerLevelMult = JsonFile["AbilitiesPowerPerLevelMult"].get<float>();
        GeneratorConfigs.AbilitiesPowerPerRankMult = JsonFile["AbilitiesPowerPerRankMult"].get<float>();
        GeneratorConfigs.AbilitiesChancePerLevelMult = JsonFile["AbilitiesChancePerLevelMult"].get<float>();
        GeneratorConfigs.AbilitiesChancePerRankMult = JsonFile["AbilitiesChancePerRankMult"].get<float>();
        GeneratorConfigs.AbilitiesDurationPerLevelMult = JsonFile["AbilitiesDurationPerLevelMult"].get<float>();
        GeneratorConfigs.AbilitiesDurationPerRankMult = JsonFile["AbilitiesDurationPerRankMult"].get<float>();

        GeneratorConfigs.VisualEffectBeginsOnRank = JsonFile["VisualEffectBeginsOnRank"].get<int>();
        GeneratorConfigs.VisualEffectGetChanceBase = JsonFile["VisualEffectGetChanceBase"].get<float>();
        GeneratorConfigs.VisualEffectGetChancePerLevelMult = JsonFile["VisualEffectGetChancePerLevelMult"].get<float>();
        GeneratorConfigs.VisualEffectGetChancePerRankMult = JsonFile["VisualEffectGetChancePerRankMult"].get<float>();

        GeneratorConfigs.SocketsMaxCap = JsonFile["SocketsMaxCap"].get<int>();
        GeneratorConfigs.ExtraSocketsPerLevel = JsonFile["ExtraSocketsPerLevel"].get<float>();
        GeneratorConfigs.SocketBaseChance = JsonFile["SocketBaseChance"].get<float>();
        GeneratorConfigs.ExtraSocketChancePerLevelMult = JsonFile["ExtraSocketChancePerLevelMult"].get<float>();
        GeneratorConfigs.ExtraSocketChancePerRankMult = JsonFile["ExtraSocketChancePerRankMult"].get<float>();

        GeneratorConfigs.CorruptionBaseChance = JsonFile["CorruptionBaseChance"].get<float>();
        GeneratorConfigs.CorruptionSpreadMult = JsonFile["CorruptionSpreadMult"].get<float>();
        GeneratorConfigs.UndefinedBaseChance = JsonFile["UndefinedBaseChance"].get<float>();

        GeneratorConfigs.HelmsGlobalPowerMult = JsonFile["HelmsGlobalPowerMult"].get<float>();
        GeneratorConfigs.ArmorsGlobalPowerMult = JsonFile["ArmorsGlobalPowerMult"].get<float>();
        GeneratorConfigs.TorsoGlobalPowerMult = JsonFile["TorsoGlobalPowerMult"].get<float>();
        GeneratorConfigs.PantsGlobalPowerMult = JsonFile["PantsGlobalPowerMult"].get<float>();
        GeneratorConfigs.BootsGlobalPowerMult = JsonFile["BootsGlobalPowerMult"].get<float>();
        GeneratorConfigs.GlovesGlobalPowerMult = JsonFile["GlovesGlobalPowerMult"].get<float>();

        GeneratorConfigs.BeltsGlobalPowerMult = JsonFile["BeltsGlobalPowerMult"].get<float>();
        GeneratorConfigs.AmuletsGlobalPowerMult = JsonFile["AmuletsGlobalPowerMult"].get<float>();
        GeneratorConfigs.RingsGlobalPowerMult = JsonFile["RingsGlobalPowerMult"].get<float>();
        GeneratorConfigs.ShieldsGlobalPowerMult = JsonFile["ShieldsGlobalPowerMult"].get<float>();

        GeneratorConfigs.Swords1hGlobalPowerMult = JsonFile["Swords1hGlobalPowerMult"].get<float>();
        GeneratorConfigs.Swords2hGlobalPowerMult = JsonFile["Swords2hGlobalPowerMult"].get<float>();
        GeneratorConfigs.Axes1hGlobalPowerMult = JsonFile["Axes1hGlobalPowerMult"].get<float>();
        GeneratorConfigs.Axes2hGlobalPowerMult = JsonFile["Axes2hGlobalPowerMult"].get<float>();
        GeneratorConfigs.Maces1hGlobalPowerMult = JsonFile["Maces1hGlobalPowerMult"].get<float>();
        GeneratorConfigs.Maces2hGlobalPowerMult = JsonFile["Maces2hGlobalPowerMult"].get<float>();
        GeneratorConfigs.MagicSwordsGlobalPowerMult = JsonFile["MagicSwordsGlobalPowerMult"].get<float>();
        GeneratorConfigs.StaffsGlobalPowerMult = JsonFile["StaffsGlobalPowerMult"].get<float>();
        GeneratorConfigs.RapiersGlobalPowerMult = JsonFile["RapiersGlobalPowerMult"].get<float>();
        GeneratorConfigs.DexSwordsGlobalPowerMult = JsonFile["DexSwordsGlobalPowerMult"].get<float>();
        GeneratorConfigs.HalleberdsGlobalPowerMult = JsonFile["HalleberdsGlobalPowerMult"].get<float>();
        GeneratorConfigs.SpearsGlobalPowerMult = JsonFile["SpearsGlobalPowerMult"].get<float>();
        GeneratorConfigs.DualsLGlobalPowerMult = JsonFile["DualsLGlobalPowerMult"].get<float>();
        GeneratorConfigs.DualsRGlobalPowerMult = JsonFile["DualsRGlobalPowerMult"].get<float>();
        GeneratorConfigs.BowsGlobalPowerMult = JsonFile["BowsGlobalPowerMult"].get<float>();
        GeneratorConfigs.CrossBowsGlobalPowerMult = JsonFile["CrossBowsGlobalPowerMult"].get<float>();
        GeneratorConfigs.MagicStaffsGlobalPowerMult = JsonFile["MagicStaffsGlobalPowerMult"].get<float>();

        GeneratorConfigs.PotionsGlobalPowerMult = JsonFile["PotionsGlobalPowerMult"].get<float>();
        GeneratorConfigs.ScrollsGlobalPowerMult = JsonFile["ScrollsGlobalPowerMult"].get<float>();

        InitItemGeneratorConfigs_ItemsConditions();
        InitItemGeneratorConfigs_StatsOptions();
        InitItemGeneratorConfigs_AbilitiesOptions();

        GeneratorConfigs.Preffixes = ReadJsonStringArray("Preffixes", (uint)GeneratorConfigs.CodePage);
        GeneratorConfigs.Affixes = ReadJsonStringArray("Affixes", (uint)GeneratorConfigs.CodePage);
        GeneratorConfigs.Suffixes = ReadJsonStringArray("Suffixes", (uint)GeneratorConfigs.CodePage);
        GeneratorConfigs.VisualEffects = ReadJsonStringArray("VisualEffects");

        GeneratorConfigs.HelmsLowPrototypes = ReadJsonStringArray("HelmsLowPrototypes");
        GeneratorConfigs.HelmsMedPrototypes = ReadJsonStringArray("HelmsMedPrototypes");
        GeneratorConfigs.HelmsTopPrototypes = ReadJsonStringArray("HelmsTopPrototypes");

        GeneratorConfigs.ArmorsLowPrototypes = ReadJsonStringArray("ArmorsLowPrototypes");
        GeneratorConfigs.ArmorsMedPrototypes = ReadJsonStringArray("ArmorsMedPrototypes");
        GeneratorConfigs.ArmorsTopPrototypes = ReadJsonStringArray("ArmorsTopPrototypes");

        GeneratorConfigs.TorsoLowPrototypes = ReadJsonStringArray("TorsoLowPrototypes");
        GeneratorConfigs.TorsoMedPrototypes = ReadJsonStringArray("TorsoMedPrototypes");
        GeneratorConfigs.TorsoTopPrototypes = ReadJsonStringArray("TorsoTopPrototypes");

        GeneratorConfigs.PantsLowPrototypes = ReadJsonStringArray("PantsLowPrototypes");
        GeneratorConfigs.PantsMedPrototypes = ReadJsonStringArray("PantsMedPrototypes");
        GeneratorConfigs.PantsTopPrototypes = ReadJsonStringArray("PantsTopPrototypes");

        GeneratorConfigs.BootsLowPrototypes = ReadJsonStringArray("BootsLowPrototypes");
        GeneratorConfigs.BootsMedPrototypes = ReadJsonStringArray("BootsMedPrototypes");
        GeneratorConfigs.BootsTopPrototypes = ReadJsonStringArray("BootsTopPrototypes");

        GeneratorConfigs.GlovesLowPrototypes = ReadJsonStringArray("GlovesLowPrototypes");
        GeneratorConfigs.GlovesMedPrototypes = ReadJsonStringArray("GlovesMedPrototypes");
        GeneratorConfigs.GlovesTopPrototypes = ReadJsonStringArray("GlovesTopPrototypes");

        GeneratorConfigs.BeltsLowPrototypes = ReadJsonStringArray("BeltsLowPrototypes");
        GeneratorConfigs.BeltsMedPrototypes = ReadJsonStringArray("BeltsMedPrototypes");
        GeneratorConfigs.BeltsTopPrototypes = ReadJsonStringArray("BeltsTopPrototypes");

        GeneratorConfigs.AmuletsLowPrototypes = ReadJsonStringArray("AmuletsLowPrototypes");
        GeneratorConfigs.AmuletsMedPrototypes = ReadJsonStringArray("AmuletsMedPrototypes");
        GeneratorConfigs.AmuletsTopPrototypes = ReadJsonStringArray("AmuletsTopPrototypes");

        GeneratorConfigs.RingsLowPrototypes = ReadJsonStringArray("RingsLowPrototypes");
        GeneratorConfigs.RingsMedPrototypes = ReadJsonStringArray("RingsMedPrototypes");
        GeneratorConfigs.RingsTopPrototypes = ReadJsonStringArray("RingsTopPrototypes");

        GeneratorConfigs.ShieldsLowPrototypes = ReadJsonStringArray("ShieldsLowPrototypes");
        GeneratorConfigs.ShieldsMedPrototypes = ReadJsonStringArray("ShieldsMedPrototypes");
        GeneratorConfigs.ShieldsTopPrototypes = ReadJsonStringArray("ShieldsTopPrototypes");

        GeneratorConfigs.Swords1hLowPrototypes = ReadJsonStringArray("Swords1hLowPrototypes");
        GeneratorConfigs.Swords1hMedPrototypes = ReadJsonStringArray("Swords1hMedPrototypes");
        GeneratorConfigs.Swords1hTopPrototypes = ReadJsonStringArray("Swords1hTopPrototypes");

        GeneratorConfigs.Swords2hLowPrototypes = ReadJsonStringArray("Swords2hLowPrototypes");
        GeneratorConfigs.Swords2hMedPrototypes = ReadJsonStringArray("Swords2hMedPrototypes");
        GeneratorConfigs.Swords2hTopPrototypes = ReadJsonStringArray("Swords2hTopPrototypes");

        GeneratorConfigs.Axes1hLowPrototypes = ReadJsonStringArray("Axes1hLowPrototypes");
        GeneratorConfigs.Axes1hMedPrototypes = ReadJsonStringArray("Axes1hMedPrototypes");
        GeneratorConfigs.Axes1hTopPrototypes = ReadJsonStringArray("Axes1hTopPrototypes");

        GeneratorConfigs.Axes2hLowPrototypes = ReadJsonStringArray("Axes2hLowPrototypes");
        GeneratorConfigs.Axes2hMedPrototypes = ReadJsonStringArray("Axes2hMedPrototypes");
        GeneratorConfigs.Axes2hTopPrototypes = ReadJsonStringArray("Axes2hTopPrototypes");

        GeneratorConfigs.Maces1hLowPrototypes = ReadJsonStringArray("Maces1hLowPrototypes");
        GeneratorConfigs.Maces1hMedPrototypes = ReadJsonStringArray("Maces1hMedPrototypes");
        GeneratorConfigs.Maces1hTopPrototypes = ReadJsonStringArray("Maces1hTopPrototypes");

        GeneratorConfigs.Maces2hLowPrototypes = ReadJsonStringArray("Maces2hLowPrototypes");
        GeneratorConfigs.Maces2hMedPrototypes = ReadJsonStringArray("Maces2hMedPrototypes");
        GeneratorConfigs.Maces2hTopPrototypes = ReadJsonStringArray("Maces2hTopPrototypes");

        GeneratorConfigs.MagicSwordsLowPrototypes = ReadJsonStringArray("MagicSwordsLowPrototypes");
        GeneratorConfigs.MagicSwordsMedPrototypes = ReadJsonStringArray("MagicSwordsMedPrototypes");
        GeneratorConfigs.MagicSwordsTopPrototypes = ReadJsonStringArray("MagicSwordsTopPrototypes");

        GeneratorConfigs.StaffsLowPrototypes = ReadJsonStringArray("StaffsLowPrototypes");
        GeneratorConfigs.StaffsMedPrototypes = ReadJsonStringArray("StaffsMedPrototypes");
        GeneratorConfigs.StaffsTopPrototypes = ReadJsonStringArray("StaffsTopPrototypes");

        GeneratorConfigs.RapiersLowPrototypes = ReadJsonStringArray("RapiersLowPrototypes");
        GeneratorConfigs.RapiersMedPrototypes = ReadJsonStringArray("RapiersMedPrototypes");
        GeneratorConfigs.RapiersTopPrototypes = ReadJsonStringArray("RapiersTopPrototypes");

        GeneratorConfigs.DexSwordsLowPrototypes = ReadJsonStringArray("DexSwordsLowPrototypes");
        GeneratorConfigs.DexSwordsMedPrototypes = ReadJsonStringArray("DexSwordsMedPrototypes");
        GeneratorConfigs.DexSwordsTopPrototypes = ReadJsonStringArray("DexSwordsTopPrototypes");

        GeneratorConfigs.HalleberdsLowPrototypes = ReadJsonStringArray("HalleberdsLowPrototypes");
        GeneratorConfigs.HalleberdsMedPrototypes = ReadJsonStringArray("HalleberdsMedPrototypes");
        GeneratorConfigs.HalleberdsTopPrototypes = ReadJsonStringArray("HalleberdsTopPrototypes");

        GeneratorConfigs.SpearsLowPrototypes = ReadJsonStringArray("SpearsLowPrototypes");
        GeneratorConfigs.SpearsMedPrototypes = ReadJsonStringArray("SpearsMedPrototypes");
        GeneratorConfigs.SpearsTopPrototypes = ReadJsonStringArray("SpearsTopPrototypes");

        GeneratorConfigs.DualsLLowPrototypes = ReadJsonStringArray("DualsLLowPrototypes");
        GeneratorConfigs.DualsLMedPrototypes = ReadJsonStringArray("DualsLMedPrototypes");
        GeneratorConfigs.DualsLTopPrototypes = ReadJsonStringArray("DualsLTopPrototypes");

        GeneratorConfigs.DualsRLowPrototypes = ReadJsonStringArray("DualsRLowPrototypes");
        GeneratorConfigs.DualsRMedPrototypes = ReadJsonStringArray("DualsRMedPrototypes");
        GeneratorConfigs.DualsRTopPrototypes = ReadJsonStringArray("DualsRTopPrototypes");

        GeneratorConfigs.BowsLowPrototypes = ReadJsonStringArray("BowsLowPrototypes");
        GeneratorConfigs.BowsMedPrototypes = ReadJsonStringArray("BowsMedPrototypes");
        GeneratorConfigs.BowsTopPrototypes = ReadJsonStringArray("BowsTopPrototypes");

        GeneratorConfigs.CrossBowsLowPrototypes = ReadJsonStringArray("CrossBowsLowPrototypes");
        GeneratorConfigs.CrossBowsMedPrototypes = ReadJsonStringArray("CrossBowsMedPrototypes");
        GeneratorConfigs.CrossBowsTopPrototypes = ReadJsonStringArray("CrossBowsTopPrototypes");

        GeneratorConfigs.MagicStaffsLowPrototypes = ReadJsonStringArray("MagicStaffsLowPrototypes");
        GeneratorConfigs.MagicStaffsMedPrototypes = ReadJsonStringArray("MagicStaffsMedPrototypes");
        GeneratorConfigs.MagicStaffsTopPrototypes = ReadJsonStringArray("MagicStaffsTopPrototypes");

        GeneratorConfigs.Potions1Prototypes = ReadJsonStringArray("Potions1Prototypes");
        GeneratorConfigs.Potions2Prototypes = ReadJsonStringArray("Potions2Prototypes");
        GeneratorConfigs.Potions3Prototypes = ReadJsonStringArray("Potions3Prototypes");
        GeneratorConfigs.Potions4Prototypes = ReadJsonStringArray("Potions4Prototypes");

        GeneratorConfigs.ScrollsPrototypes = ReadJsonStringArray("ScrollsPrototypes");
    }
    
    void InitItemGeneratorConfigs()
    {
        GeneratorConfigs = ItemsGeneratorConfigs();
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' ...");
        string root = zoptions->GetDirString(zTOptionPaths::DIR_ROOT);
        string path = string::Combine("%s\\%s\\StExt_ItemGeneratorConfigs.json", root, ModDataRootDir);

        if (ParseModData(path)) {
            ReadItemGeneratorJsonConfigs();
        }
        else
        {
            Message::Error("Can't read Item Generator Configs!");
            gameMan->ExitGame();
        }
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' Done!");
        DEBUG_MSG("Item generator configs are initialized!");
        DEBUG_MSG("");
    }

    bool SelectItemGeneratorConfigs(const string fileName)
    {
        if (fileName.IsEmpty())
        {
            DEBUG_MSG("SelectItemGeneratorConfigs: file name is empty!");
            return false;
        }

        ItemsGeneratorConfigs configBackup = GeneratorConfigs;
        GeneratorConfigs = ItemsGeneratorConfigs();
        DEBUG_MSG("Parse config file: " + fileName + "...");
        string root = zoptions->GetDirString(zTOptionPaths::DIR_ROOT);
        string path = string::Combine("%s\\%s\\%s\\%s", root, ModDataRootDir, ItemGeneratorConfigsDir, fileName);
        string fileData;
        if (!fileData.ReadFromVdf(path, VDF_DEFAULT))
        {
            DEBUG_MSG("Config file: '" + path + "' not found!");
            return false;
        }

        if (ParseModData(path)) {
            ReadItemGeneratorJsonConfigs();
        }
        else
        {
            GeneratorConfigs = configBackup;
            DEBUG_MSG("SelectItemGeneratorConfigs: configs update is failed!");
            return false;
        }
        DEBUG_MSG("Parse config file: '" + fileName + "' Done!");
        DEBUG_MSG("Item generator configs updated!");
        return true;
    }
    
    void InitAuraData()
    {
        zCParser* par = zCParser::GetParser();
        zCPar_Symbol* auraIndxArray = par->GetSymbol("StExt_AurasIndexArray");
        if (!auraIndxArray)
        {
            DEBUG_MSG("'StExt_AurasIndexArray' not found!");
            return;
        }

        AuraData = std::map<int, C_AuraData*>();
        zSTRING msg = "Initialize " + zSTRING((int)auraIndxArray->ele) + " auras...";
        DEBUG_MSG(msg);
        for (unsigned int i = 0; i < auraIndxArray->ele; i++)
        {
            zSTRING strIndx = auraIndxArray->stringdata[i];
            int index = par->GetIndex(strIndx);
            if (index == Invalid)
            {
                DEBUG_MSG("Can't load aura instance '" + strIndx + "'");
                continue;
            }
            C_AuraData* aura = new C_AuraData();
            aura->SetByScript(index);
            AuraData.insert({aura->Id, aura});
        }
        DEBUG_MSG("Auras initialized!");
    }

    Array<C_MagicInfusionData*> InitInfusionData(zSTRING initArray)
    {
        zCParser* par = zCParser::GetParser();
        zCPar_Symbol* infusionIndxArray = par->GetSymbol(initArray);
        if (!infusionIndxArray)
        {
            DEBUG_MSG("'" + initArray + "' not found!");
            return Array<C_MagicInfusionData*>();
        }

        Array<C_MagicInfusionData*> InfusionData = Array<C_MagicInfusionData*>();
        zSTRING msg = "Init infusion from: " + initArray + ". size: " + zSTRING((int)infusionIndxArray->ele) + " ...";
        DEBUG_MSG(msg);
        for (unsigned int i = 0; i < infusionIndxArray->ele; i++)
        {
            zSTRING strIndx = infusionIndxArray->stringdata[i];
            zCPar_Symbol* instSym = par->GetSymbol(strIndx);
            int index = par->GetIndex(strIndx);
            C_MagicInfusionData* infusion = new C_MagicInfusionData();
            infusion->SetByScript(index);
            InfusionData.Insert(infusion);
        }
        DEBUG_MSG(" '" + initArray + "' initialized!");
        return InfusionData;
    }
    
    void InitNpcAffixes() { InfusionData_Afixes = InitInfusionData("StExt_AffixesIndexArray"); }
    void InitNpcPreffixes() { InfusionData_Preffixes = InitInfusionData("StExt_PreffixesIndexArray"); }
    void InitNpcSuffixes() { InfusionData_Suffixes = InitInfusionData("StExt_SuffixesIndexArray"); }

    void InitScriptData()
    {
        DEBUG_MSG("StExt - Initialize scripts data...");

        IsLevelChanging = false;
        IsLoading = false;
        parser->SetInstance("STEXT_NULLNPC", Null);
        parser->SetInstance("STEXT_NULLITEM", Null);
        parser->GetSymbol("StExt_Fps")->SetValue(60, 0);

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
        DEBUG_MSG_IF(StExt_CheckConditionStatFunc == Invalid, "StExt_OnAiStateFunc is null!");

        StExt_EsText = parser->GetSymbol("StExt_EsText")->stringdata;
        SpellFxNames = parser->GetSymbol("spellfxinstancenames")->stringdata;
        MaxSpellId = parser->GetSymbol("max_spell")->single_intdata;
        ItemCondSpecialSeparator = parser->GetSymbol("StExt_Item_Cond_Separator")->single_intdata;
        StExt_AbilityPrefix = parser->GetSymbol("StExt_AbilityPrefix")->single_intdata;
        DEBUG_MSG("StExt - MaxSpellId: " + Z MaxSpellId);

        StExt_Config_NpcStats_TopOffset = parser->GetSymbol("StExt_Config_NpcStatsUi_TopOffset")->single_intdata;
        StExt_Config_NpcStats_HideTags = parser->GetSymbol("StExt_Config_NpcStatsUi_HideTags")->single_intdata;

        ItemBasePriceMult = parser->GetSymbol("StExt_ItemBasicPriceMult")->single_floatdata;
        if (ItemBasePriceMult < 0.1f) ItemBasePriceMult = 0.1f;
        if (ItemBasePriceMult > 100.0f) ItemBasePriceMult = 100.0f;

        DEBUG_MSG("StExt - Scripts data initialized!");
    }

    zSTRING GetModVersion()
    {
        zCParser* par = zCParser::GetParser();
        zCPar_Symbol* verSym = par->GetSymbol("StExt_CurrentModVersionString");
        return verSym->stringdata;
    }

    void InitModScriptData()
    {
        ProhibitedWaypoints = Array<WaypointData>();
        parser->CallFunc(parser->GetIndex("StExt_OnModLoaded"));
    }

    void StonedExtension_InitModData()
    {
        DEBUG_MSG("DebugMode - " + zSTRING(parser->GetSymbol("StExt_Config_DebugAlwaysEnabled")->single_intdata));
        int setVerFunc = parser->GetIndex("StExt_SetModVersionString");
        parser->CallFunc(setVerFunc);
        ModVersionString = Z("Ethernal Breeze mod [" + GetModVersion() + " (Build: 6.4.2)]");
        #if DebugEnabled
            ModVersionString += Z(" | [Debug]");
        #endif
        DEBUG_MSG(ModVersionString);
        DEBUG_MSG("");

        try { InitItemGeneratorConfigs(); }
        catch (const std::exception&) { DEBUG_MSG("Loading of item generator configs was failed!"); }  

        InitScriptData();
        InitStatsUncaper();
        InitAuraData();
        InitNpcAffixes();
        InitNpcPreffixes();
        InitNpcSuffixes();
        InitModScriptData();
        InitVoiceController();
        DEBUG_MSG("StExt - Mod data initialized!");
    }
}
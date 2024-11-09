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
    int ApplyResistsFunc = Invalid;
    int ApplyDamageToEsFunc = Invalid;
    int OnModDamageExpFunc = Invalid;
    int OnSncDamageFunc = Invalid;
    int PrintDamageFunc = Invalid;
    int OnDamageAfterFunc = Invalid;
    int OnDamageBeginFunc = Invalid;
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

    bool ParseModData(const string fileName)
    {
        string root = zoptions->GetDirString(zTOptionPaths::DIR_ROOT);
        string path = string::Combine("%s\\%s\\%s", root, ModDataRootDir, fileName);
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
            DEBUG_MSG("Can't load json file - " + Z fileName);
            return false;
        }
        
        DEBUG_MSG("Json file - '" + Z fileName + "' parsed!");
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

            obj.StatId = jObj["StatId"].get<int>();
            obj.StatMaxCap = jObj["StatMaxCap"].get<float>();
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
    
    void InitItemGeneratorConfigs()
    {
        GeneratorConfigs = ItemsGeneratorConfigs();
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' ...");
        if (ParseModData("StExt_ItemGeneratorConfigs.json"))
        {
            GeneratorConfigs.CodePage = JsonFile["CodePage"].get<int>();

            GeneratorConfigs.NextRankOffset = JsonFile["NextRankOffset"].get<int>();
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

            GeneratorConfigs.ConditionPerLevelMult = JsonFile["ConditionPerLevelMult"].get<float>();
            GeneratorConfigs.ConditionPerRankMult = JsonFile["ConditionPerRankMult"].get<float>();
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
        DEBUG_MSG("Parse config file: 'StExt_ItemGeneratorConfigs.json' Done!");
        DEBUG_MSG("Item generator configs are initialized!");
        DEBUG_MSG("");
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
            zCPar_Symbol* instSym = par->GetSymbol(strIndx);
            int index = par->GetIndex(strIndx);
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

    zSTRING GetModVersion()
    {
        zCParser* par = zCParser::GetParser();
        zCPar_Symbol* verSym = par->GetSymbol("StExt_CurrentModVersionString");
        return verSym->stringdata;
    }

    typedef void(__thiscall* checkBonusFunc)(void* this_ptr, int damage);

    void LoadNbFunctions()
    {
        DEBUG_MSG("Load 'Union_abi.dll' ...");
        auto nbModule = GetModuleHandle("UNION_ABI.DLL");

        if (nbModule == Null)
        {
            DEBUG_MSG("Load 'Union_abi.dll' - failed!");
            return;
        }

        //?checkBonus@AB_SNC@Gothic_II_Addon@QEXH@Z
        //checkBonus@AB_SNC@Gothic_II_Addon@@QAEXH@Z
        //?checkBonus@AB_SNC@Gothic_II_Addon@@QAEXH@Z
        auto testFuncAdr = GetProcAddress(nbModule, "?checkBonus@AB_SNC@Gothic_II_Addon@@QAEXH@Z");

        checkBonusFunc checkBonus = (checkBonusFunc)GetProcAddress(nbModule, "?checkBonus@AB_SNC@Gothic_II_Addon@@QAEXH@Z");
        DEBUG_MSG("Load 'Union_abi.dll' func: Gothic_II_Addon::AB_SNC::checkBonus(int) - " + Z(int)checkBonus);
        if (!checkBonus) DEBUG_MSG("Load 'Union_abi.dll' func: Gothic_II_Addon::AB_SNC::checkBonus(int)  - FAIL!");

        DEBUG_MSG("Load 'Union_abi.dll' - compleated!");
    }
 
    void InitWaypoints()
    {
        ProhibitedWaypoints = Array<WaypointData>();
        parser->CallFunc(parser->GetIndex("StExt_InitializeProhibitedWaypoints"));
    }

    void StonedExtension_InitModData()
    {
        DEBUG_MSG("DebugMode - " + zSTRING(parser->GetSymbol("StExt_Config_DebugAlwaysEnabled")->single_intdata));
        int setVerFunc = parser->GetIndex("StExt_SetModVersionString");
        parser->CallFunc(setVerFunc);
        ModVersionString = zSTRING("Ethernal Breeze mod [" + GetModVersion() + "b (Build: 5.0.1)]");
        DEBUG_MSG(ModVersionString);
        DEBUG_MSG("");

        DamageInfo.StopProcess = true;
        parser->SetInstance("StExt_NullNpc", Null);
        parser->GetSymbol("StExt_Fps")->SetValue(60, 0);
        IncomingDamage = CIncomingDamage();

        OnPreLoopFunc = parser->GetIndex("StExt_OnFrameBegin");
        OnPostLoopFunc = parser->GetIndex("StExt_OnFrameEnd");
        ApplyResistsFunc = parser->GetIndex("StExt_ApplyResists");
        ApplyDamageToEsFunc = parser->GetIndex("StExt_ApplyDamageToEs");
        OnModDamageExpFunc = parser->GetIndex("StExt_ProcessDamageExp_Engine");
        OnSncDamageFunc = parser->GetIndex("StExt_AddSncDamage");
        PrintDamageFunc = parser->GetIndex("StExt_PrintExtraDamage");
        OnDamageAfterFunc = parser->GetIndex("StExt_OnDamageAfter");
        OnDamageBeginFunc = parser->GetIndex("StExt_OnDamageBegin");
        FxDamageCanBeAppliedFunc = parser->GetIndex("StExt_FxDamageCanBeApplied");
        IsNpcImmortalFunc = parser->GetIndex("StExt_IsNpcImmortal_Engine");
        OnArmorEquipFunc = parser->GetIndex("StExt_OnArmorEquip");
        GetSpellDamageFlagsFunc = parser->GetIndex("StExt_GetSpellDamageFlags");
        GetSpellEffectFlagsFunc = parser->GetIndex("StExt_GetSpellEffectFlags");
        OnLoadEndFunc = parser->GetIndex("StExt_OnLoadEnd");
        OnLevelChangeFunc = parser->GetIndex("StExt_OnLevelChange");
        NpcGetBarCurEsFunc = parser->GetIndex("StExt_Npc_GetBarCurEs");
        NpcGetBarMaxEsFunc = parser->GetIndex("StExt_Npc_GetBarMaxEs");
        UpdateFocusNpcInfoFunc = parser->GetIndex("StExt_UpdateFocusNpcInfo");
        CanCallModMenuFunc = parser->GetIndex("StExt_CanCallModMenu");
        IsHeroMovLockedFunc = parser->GetIndex("StExt_IsHeroMovLocked");
        HandleKeyEventFunc = parser->GetIndex("StExt_HandleKeyEvent");
        HandlePcStatChangeFunc = parser->GetIndex("StExt_HandlePcStatChange");
        SaveParserVarsFunc = parser->GetIndex("StExt_SaveParserVars");
        RestoreParserVarsFunc = parser->GetIndex("StExt_RestoreParserVars");

        try { InitItemGeneratorConfigs(); }
        catch (const std::exception&) { DEBUG_MSG("Loading of item generator configs was failed!"); }  

        //LoadNbFunctions();

        InitStatsUncaper();
        InitAuraData();
        InitNpcAffixes();
        InitNpcPreffixes();
        InitNpcSuffixes();
        InitWaypoints();
        DEBUG_MSG("StExt - Mod data initialized!");
    }
}
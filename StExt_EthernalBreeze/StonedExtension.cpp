#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    Array<ConfigPresetData> GameConfigsPresets;
    Array<ModExtensionInfo> ModPluginsInfo;
    
    Map<int, ExtraStatData> ExtraStatsData;
    Map<int, ExtraStatData> ExtraConditionStatsData;
    Map<int, zSTRING> ExtraStatsNameData;
    Array<ExtraMasteryData> ExtraMasteriesData;
    StringMap<ExtraConfigData> ExtraConfigsData(256);

    Map<int, oCNpc*> RegisteredNpcs;
    Map<int, AuraData> AurasData;
    Array<WaypointData> ProhibitedWaypoints;

    Array<MagicInfusionData> InfusionAffixes;
    Array<MagicInfusionData> InfusionSuffixes;
    Array<MagicInfusionData> InfusionPreffixes;   

    SpellInfo CurrentSpellInfo;
    zSTRING ResultString;
    TimerBlock FpsTimer;
    int FpsCounter;
    int HeroNpcUId;

    void StonedExtension_Loop()
    {
        if (IsLoading || IsLevelChanging) return;
        HeroNpcUId = parser->GetSymbol("StExt_HeroUid")->single_intdata;

        StonedExtension_Loop_Ui();
        FpsCounter += 1;
        if (FpsTimer.AwaitExact(1000))
        {
            parser->GetSymbol("StExt_Fps")->SetValue(FpsCounter, 0);
            StExt_CurrentDayPart = parser->GetSymbol("StExt_CurrentDayPart")->single_intdata;
            FpsCounter = 0;
        }
    }

    //-----------------------------------------------------------------
    //							MOD FUNCTIONS
    //-----------------------------------------------------------------

    static void StringArraySortImpl(Array<zSTRING>& arr, uint low, uint high) 
    {
        uint i = low;
        uint j = high;
        zSTRING pivot = arr[(low + high) / 2U];

        while (i <= j) 
        {
            while (arr[i] < pivot) i++;
            while (pivot < arr[j]) j--;
            if (i <= j) 
            {
                if (i != j) 
                {
                    zSTRING tmp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = tmp;
                }
                i++; j--;
            }
        }
        if (low < j)  StringArraySortImpl(arr, low, j);
        if (i < high) StringArraySortImpl(arr, i, high);
    }

    void SortZStringArray(Array<zSTRING>& arr) 
    {
        if (arr.GetNum() > 1U)
            StringArraySortImpl(arr, 0U, arr.GetNum() - 1U);
    }

    uint FindZStringSorted(const Array<zSTRING>& arr, const zSTRING& key) 
    {
        uint l = 0;
        uint r = arr.GetNum();
        while (l < r) 
        {
            uint mid = (l + r) / 2;
            if (arr[mid] == key) return mid;

            if (arr[mid] < key) l = mid + 1;
            else r = mid;
        }
        return Invalid;
    }

    //-----------------------------------------------------------------

    string GetSaveSlotNameByID(int ID)
    {
        if (ID > 0) return "savegame" + A ID;
        if (ID == 0) return "quicksave";
        return "current";
    }

    inline const AuraData* GetAuraById(const int id)
    {
        auto auraPair = AurasData.GetSafePair(id);
        if (!auraPair)
        {
            DEBUG_MSG("GetAuraById - incorrect id: " + Z id + "!");
            return Null;
        }
        return &auraPair->GetValue();
    }

    inline const ExtraStatData* GetExtraStatDataById(const int id)
    {
        auto statPair = ExtraStatsData.GetSafePair(id);
        if (!statPair)
        {
            DEBUG_MSG("GetExtraStatDataById - incorrect id: " + Z id + "!");
            return Null;
        }
        return &statPair->GetValue();
    }

    inline const zSTRING& GetExtraStatNameById(const int id)
    {
        auto statPair = ExtraStatsNameData.GetSafePair(id);
        if (!statPair)
        {
            DEBUG_MSG("GetExtraStatNameById - incorrect id: " + Z id + "!");
            return zString_Unknown;
        }
        return statPair->GetValue();
    }

    inline const ExtraStatData* GetExtraConditionDataById(const int id)
    {
        auto statPair = ExtraConditionStatsData.GetSafePair(id);
        if (!statPair)
        {
            DEBUG_MSG("GetExtraConditionDataById - incorrect id: " + Z id + "!");
            return Null;
        }
        return &statPair->GetValue();
    }

    inline const MagicInfusionData* GetRandomInfusion(int type, int tier, int grantInfusion)
    {
        int chance = StExt_Rand::Next(100) + (tier * 5);
        if (!grantInfusion && (chance < 50)) return Null;

        switch (type)
        {
            case StExt_InfusionType_Affix:
                return &InfusionAffixes[StExt_Rand::Index(InfusionAffixes.GetNum())];

            case StExt_InfusionType_Preffix:
                return &InfusionPreffixes[StExt_Rand::Index(InfusionPreffixes.GetNum())];

            case StExt_InfusionType_Suffix:
                return &InfusionSuffixes[StExt_Rand::Index(InfusionSuffixes.GetNum())];

            default:
                return Null;
        }
        return Null;
    }

    MagicInfusionData* CreateInfusion(int tier)
    {
        const MagicInfusionData* prefix = GetRandomInfusion(StExt_InfusionType_Preffix, tier, false);
        const MagicInfusionData* affix = GetRandomInfusion(StExt_InfusionType_Affix, tier, false);
        const MagicInfusionData* suffix = GetRandomInfusion(StExt_InfusionType_Suffix, tier, false);

        if (!suffix && !affix && !prefix)
        {
            if (StExt_Rand::Bool()) prefix = GetRandomInfusion(StExt_InfusionType_Preffix, tier, true);
            else suffix = GetRandomInfusion(StExt_InfusionType_Suffix, tier, true);
        }
        Array<const MagicInfusionData*> infusionData = Array<const MagicInfusionData*>();
        infusionData.Insert(prefix);
        infusionData.Insert(affix);
        infusionData.Insert(suffix);

        MagicInfusionData* result = new MagicInfusionData();
        result->Tier = tier;
        result->PreffixId = prefix ? prefix->Id : -1;
        result->AffixId = affix ? affix->Id : -1;
        result->SuffixId = suffix ? suffix->Id : -1;
        result->Effect = "";
        for (int i = 0; i < 10; ++i)
            result->Abilities[i] = -1;

        Array<zSTRING> effectsData = Array<zSTRING>();
        Array<int> abilitiesData = Array<int>();
        for (unsigned int i = 0; i < infusionData.GetNum(); ++i)
        {
            if (!infusionData[i]) continue;

            // Npc mods
            result->NpcFlags |= infusionData[i]->NpcFlags;
            result->Size += infusionData[i]->Size;
            result->SpeedMod += infusionData[i]->SpeedMod;
            result->ExtraLevel += infusionData[i]->ExtraLevel;
            result->ExtraLevelMult += infusionData[i]->ExtraLevelMult;

            // Attribute mods
            result->ExtraHp += infusionData[i]->ExtraHp;
            result->ExtraEs += infusionData[i]->ExtraEs;
            result->ExtraStr += infusionData[i]->ExtraStr;
            result->ExtraAgi += infusionData[i]->ExtraAgi;
            result->ExtraHpMult += infusionData[i]->ExtraHpMult;
            result->ExtraEsMult += infusionData[i]->ExtraEsMult;
            result->ExtraStrMult += infusionData[i]->ExtraStrMult;
            result->ExtraAgiMult += infusionData[i]->ExtraAgiMult;

            // Damage mods
            result->ExtraBluntDam += infusionData[i]->ExtraBluntDam;
            result->ExtraEdgeDam += infusionData[i]->ExtraEdgeDam;
            result->ExtraFireDam += infusionData[i]->ExtraFireDam;
            result->ExtraFlyDam += infusionData[i]->ExtraFlyDam;
            result->ExtraMagicDam += infusionData[i]->ExtraMagicDam;
            result->ExtraPointDam += infusionData[i]->ExtraPointDam;
            result->ExtraHitchances += infusionData[i]->ExtraHitchances;

            // Protection mods
            result->ExtraDodgeChance += infusionData[i]->ExtraDodgeChance;
            result->ExtraBluntProt += infusionData[i]->ExtraBluntProt;
            result->ExtraEdgeProt += infusionData[i]->ExtraEdgeProt;
            result->ExtraFireProt += infusionData[i]->ExtraFireProt;
            result->ExtraFlyProt += infusionData[i]->ExtraFlyProt;
            result->ExtraMagicProt += infusionData[i]->ExtraMagicProt;
            result->ExtraPointProt += infusionData[i]->ExtraPointProt;
            result->ExtraProtAllMult += infusionData[i]->ExtraProtAllMult;
            result->ResistEdge += infusionData[i]->ResistEdge;
            result->ResistBlunt += infusionData[i]->ResistBlunt;
            result->ResistPoint += infusionData[i]->ResistPoint;
            result->ResistMagic += infusionData[i]->ResistMagic;
            result->ResistFire += infusionData[i]->ResistFire;
            result->ResistFly += infusionData[i]->ResistFly;
            result->ResistPois += infusionData[i]->ResistPois;

            result->GoldBonus += infusionData[i]->GoldBonus;
            result->LuckBonus += infusionData[i]->LuckBonus;

            // Visual effects
            if (!infusionData[i]->Effect.IsEmpty())
                effectsData.Insert(infusionData[i]->Effect);

            // Abilities
            if (!abilitiesData.HasEqual(infusionData[i]->PassiveAbilityId) && (infusionData[i]->PassiveAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->PassiveAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->LowAbilityId) && (infusionData[i]->LowAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->LowAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->MedAbilityId) && (tier >= 2) && (infusionData[i]->MedAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->MedAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->TopAbilityId) && (tier >= 3) && (infusionData[i]->TopAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->TopAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->SummonAbilityId) && (tier >= 2) && (infusionData[i]->SummonAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->SummonAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->BuffAbilityId) && (infusionData[i]->BuffAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->BuffAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->DebuffAbilityId) && (infusionData[i]->DebuffAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->DebuffAbilityId);
        }

        // Visual effects selection
        if (effectsData.GetNum() > 0)
            result->Effect = effectsData[StExt_Rand::Index(effectsData.GetNum())];

        int abilityNum = abilitiesData.GetNum();
        if (abilityNum > 10)
            StExt_Rand::ShuffleArray(abilitiesData);

        for (int i = 0; i < 10; ++i) {
            result->Abilities[i] = ((abilityNum == 0) || (i >= abilityNum)) ? Invalid : abilitiesData[i];
        }

        // Model scale
        result->Size += 1.0f;
        if (result->Size < 0.75f) result->Size = 0.75f;
        if (result->Size > 2.00f) result->Size = 2.00f;

        return result;
    }

    inline const ConfigPresetData* GetConfigPreset(const zSTRING &presetName)
    {
        if (presetName.IsEmpty()) return Null;

        for (uint i = 0; i < GameConfigsPresets.GetNum(); ++i)
        {
            ConfigPresetData* data = &GameConfigsPresets[i];
            if (!data) continue;
            if (data->Name == presetName) { return data; }
        }
        return Null;
    }

    void GetNpcInRadius(zCArray<oCNpc*>& npcList, oCNpc* center, const float radius)
    {
        if (!center) return;

        center->ClearVobList();
        center->CreateVobList(radius);

        zCVob* pVob = NULL;
        oCNpc* npc = NULL;
        int foundNpcs = 0;
        for (int i = 0; i < center->vobList.GetNum(); ++i)
        {
            if (i > 8192)
            {
                DEBUG_MSG("GetNpcInRadius - break loop!");
                break;
            }

            pVob = center->vobList.GetSafe(i);
            if (!pVob) continue;

            npc = zDYNAMIC_CAST<oCNpc>(pVob);
            if (!npc) continue;
            if (npc->IsDead()) continue;

            npcList.Insert(npc);
        }
    }

    inline zVEC3 FindCorrectPosition(oCNpc* npc)
    {
        zVEC3 newPos = npc->GetPositionWorld();
        npc->SetOnFloor(newPos);
        npc->SearchNpcPosition(newPos);
        return newPos;
    }

    inline bool NpcPositionIsInvalid(oCNpc* npc)
    {
        zVEC3 center = npc->bbox3D.GetCenter() + (npc->bbox3D.GetCenter() - npc->bbox3D.GetCenterFloor()) * 0.5f;
        if (!npc->groundPoly || !ogame->GetWorld()->TraceRayFirstHit(center, zVEC3(0, -1000, 0), npc, zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER))
            return true;

        if (ogame->GetWorld()->TraceRayNearestHit(npc->GetPositionWorld(), zVEC3(0, 500, 0), npc, zTRACERAY_POLY_2SIDED | zTRACERAY_POLY_IGNORE_TRANSP | zTRACERAY_VOB_IGNORE_CHARACTER))
            return true;

        if (!npc->SearchNpcPosition(npc->GetPositionWorld()))
            return true;

        return false;
    }


    // ******** Npc UIs subsystem ********

    int GetNextNpcUid()
    {
        zCPar_Symbol* ps = parser->GetSymbol("StExt_NpcUidCounter");
        int result = ps->single_intdata + 1;
        while (IsUidRegistered(result)) result += 1;
        ps->SetValue(result, 0);
        return result;
    }

    bool IsUidRegistered(int npcUid)
    {
        if (npcUid == 0 || npcUid == Invalid) return true;
        return RegisteredNpcs.GetSafePair(npcUid) != Null;
    }

    void RegisterNpc(oCNpc* npc, int& npcUid)
    {
        if (!npc) { return; }
        if (npcUid == 0 || npcUid == Invalid) { npcUid = GetNextNpcUid(); }

        auto pair = RegisteredNpcs.GetSafePair(npcUid);
        if (!pair)
        {
            RegisteredNpcs.Insert(npcUid, npc);
            DEBUG_MSG("RegisterNpc - UId for npc '" + Z npc->name + "' [" + Z npcUid + "] Done!");
            return;
        }

        oCNpc* currentNpc = pair->GetValue();
        oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
        oCNpcEx* currentNpcEx = dynamic_cast<oCNpcEx*>(currentNpc);

        // seems we have a record with such id...
        if (!currentNpc)
        {
            if (npcEx)
                npcEx->m_pVARS[StExt_AiVar_Uid] = npcUid;
            pair->GetValue() = npc;
        }
        if (currentNpc == npc) return;

        // ...and here kind of collision with different npc's with one uniq id
        npcUid = GetNextNpcUid();
        if (npcEx)
            npcEx->m_pVARS[StExt_AiVar_Uid] = npcUid;
        RegisteredNpcs.Insert(npcUid, npc);
    }

    void RegisterNpc(oCNpc* npc)
    {
        if (!npc) return;
        int uid = Invalid;

        oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
        if (npcEx)        
            uid = npcEx->m_pVARS[StExt_AiVar_Uid];
        
        RegisterNpc(npc, uid);
    }

    int GetNpcUid(oCNpc* npc)
    {
        if (npc)
        {
            oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
            if (npcEx)
                return npcEx->m_pVARS[StExt_AiVar_Uid];
            else
            {
                void* slf = parser->GetSymbol("STEXT_SELF")->GetInstanceAdr();
                parser->SetInstance("STEXT_SELF", npc);
                int uid = *(int*)parser->CallFunc(StExt_NpcToUidFunc);
                parser->SetInstance("STEXT_SELF", slf);
                return uid;
            }
        }
        return Invalid;
    }

    oCNpc* GetNpcByUid(int npcUid)
    {
        if (npcUid > 0)
        {
            auto pair = RegisteredNpcs.GetSafePair(npcUid);
            if (pair)
                return dynamic_cast<oCNpc*>((zCVob*)pair->GetValue());
        }
        DEBUG_MSG("GetNpcByUid - npc with id: " + Z npcUid + " not found!");
        return Null;
    }

    void ClearRegisteredNpcs() { RegisteredNpcs.Clear(); }

    void RegisterNearestNpcs()
    {
        DEBUG_MSG("RegisterNearestNpcs - start search...");
        if (!player)
        {
            DEBUG_MSG("RegisterNearestNpcs - player not initialized yet!");
            return;
        }

        zCVob* pVob = Null;
        oCNpc* npc = Null;        

        player->ClearVobList();
        player->CreateVobList(32000.0f);

        int foundNpcs = 0;
        for (int i = 0; i < player->vobList.GetNum(); ++i)
        {
            if (i > 32000)
            {
                DEBUG_MSG("RegisterNearestNpcs - break loop!");
                break;
            }

            pVob = player->vobList.GetSafe(i);
            if (!pVob) continue;
            npc = zDYNAMIC_CAST<oCNpc>(pVob);
            if (!npc) continue;

            ++foundNpcs;
            RegisterNpc(npc);
        }
        DEBUG_MSG("RegisterNearestNpcs - done! Found: " + Z foundNpcs);
    }
    
    //-----------------------------------------------------------------
    //					EXTERNALS (SCRIPT API)
    //-----------------------------------------------------------------

    // ********************** MISK FUNCS **********************

    int __cdecl StExt_Cmd()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING message;
        par->GetParameter(message);
        PrintDebug(message);
        return True;
    }

    int __cdecl StExt_GetInstanceIdByName()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING instName;
        par->GetParameter(instName);
        int index = (instName.IsEmpty() || instName == "") ? Invalid : par->GetIndex(instName);
        if (index == Invalid)
            DEBUG_MSG("Instance '" + instName + "' not found!");
        par->SetReturn(index);
        return True;
    }

    int __cdecl StExt_FloatMult()
    {
        zCParser* par = zCParser::GetParser();
        float m1, m2, result;
        par->GetParameter(m2);
        par->GetParameter(m1);
        result = m1 * m2;
        par->SetReturn(result);
        return True;
    }

    int __cdecl StExt_FloatPow()
    {
        zCParser* par = zCParser::GetParser();
        float m1, m2, result;
        par->GetParameter(m2);
        par->GetParameter(m1);
        result = pow(m1, m2);
        par->SetReturn(result);
        return True;
    }

    int __cdecl StExt_IntPow()
    {
        zCParser* par = zCParser::GetParser();
        int m1, m2, result;
        par->GetParameter(m2);
        par->GetParameter(m1);
        result = static_cast<int>(pow(m1, m2));
        par->SetReturn(result);
        return True;
    }

    int __cdecl StExt_FloatPowAsInt()
    {
        zCParser* par = zCParser::GetParser();
        float m2;
        int m1, result;
        par->GetParameter(m2);
        par->GetParameter(m1);
        result = static_cast<int>(pow(m1, m2));
        par->SetReturn(result);
        return True;
    }

    int __cdecl StExt_GetPercentBasedOnValue()
    {
        zCParser* par = zCParser::GetParser();
        float m1, m2;
        int val, max, result;
        par->GetParameter(max);
        par->GetParameter(val);
        result = 0;
        if (max != 0 && val != 0)
        {
            m1 = static_cast<float>(val);
            m2 = static_cast<float>(max);
            result = static_cast<int>((m1 / m2) * 100.0f);
        }
        par->SetReturn(result);
        return True;
    }

    int __cdecl StExt_TryCallFunc()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING funcName;
        int funcId;
        par->GetParameter(funcName);
        funcId = par->GetIndex(funcName);
        if (funcId == Invalid)
        {
            DEBUG_MSG("StExt_TryCallFunc - fail to call '" + funcName + "'");
            par->SetReturn(false);
            return False;
        }
        parser->CallFunc(funcId);
        par->SetReturn(true);
        return True;
    }

    int __cdecl StExt_GetAuraData()
    {
        zCParser* par = zCParser::GetParser();
        int index;
        parser->GetParameter(index);

        const AuraData* aura = GetAuraById(index);
        if (!aura) 
        {
            par->SetReturn(Null);
            return False;
        }
        par->SetReturn(const_cast<AuraData*>(aura));
        return True;
    }

    int __cdecl StExt_UpdatePcStats()
    {
        zCParser* par = zCParser::GetParser();
        zCPar_Symbol* StExt_PcStatsArray = parser->GetSymbol("StExt_PcStats");
        zCPar_Symbol* StExt_PcStats_PermArray = parser->GetSymbol("StExt_PcStats_Perm");
        zCPar_Symbol* StExt_PcStats_ItemsArray = parser->GetSymbol("StExt_PcStats_Items");
        zCPar_Symbol* StExt_PcStats_TalismansArray = parser->GetSymbol("StExt_PcStats_Talismans");
        zCPar_Symbol* StExt_PcStats_DynamicArray = parser->GetSymbol("StExt_PcStats_Dynamic");
        zCPar_Symbol* StExt_PcStats_OtherArray = parser->GetSymbol("StExt_PcStats_Other");
        zCPar_Symbol* StExt_PcStats_AlchemyArray = parser->GetSymbol("StExt_PcStats_Alchemy");
        zCPar_Symbol* StExt_PcStats_BuffsArray = parser->GetSymbol("StExt_PcStats_Buffs");
        zCPar_Symbol* StExt_PcStats_GrimoirArray = parser->GetSymbol("StExt_PcStats_Grimoir");
        zCPar_Symbol* StExt_PcStats_DaggerArray = parser->GetSymbol("StExt_PcStats_Dagger");
        zCPar_Symbol* StExt_PcStats_AurasArray = parser->GetSymbol("StExt_PcStats_Auras");
        zCPar_Symbol* StExt_PcStats_AbilitiesArray = parser->GetSymbol("StExt_PcStats_Abilities");
        zCPar_Symbol* StExt_ArtifactEquipped = parser->GetSymbol("StExt_ArtifactEquipped");

        for (int i = 0; i < MaxStatId; ++i)
        {
            StExt_PcStatsArray->intdata[i] = StExt_PcStats_PermArray->intdata[i] + StExt_PcStats_ItemsArray->intdata[i] +
                StExt_PcStats_TalismansArray->intdata[i] + StExt_PcStats_DynamicArray->intdata[i] + StExt_PcStats_OtherArray->intdata[i] +
                StExt_PcStats_AlchemyArray->intdata[i] + StExt_PcStats_BuffsArray->intdata[i] + StExt_PcStats_AurasArray->intdata[i] + StExt_PcStats_AbilitiesArray->intdata[i];

            if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Grimoir)
                StExt_PcStatsArray->intdata[i] += StExt_PcStats_GrimoirArray->intdata[i];
            else if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Dagger)
                StExt_PcStatsArray->intdata[i] += StExt_PcStats_DaggerArray->intdata[i];
        }
        return True;
    }

    int __cdecl StExt_ProhibitWp()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING wpId;
        int radius;
        par->GetParameter(radius);
        par->GetParameter(wpId);
        WaypointData wp = WaypointData();
        wp.Radius = radius;
        wp.Wp = wpId;
        wp.wpInst = Null;
        ProhibitedWaypoints.Insert(wp);
        return True;
    }

    int __cdecl StExt_GetRandomWp()
    {
        zCParser* par = zCParser::GetParser();

        ResultString.Clear();
        oCWorld* world = ogame->GetGameWorld();
        if (!world)
        {
            par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
            par->SetReturn(ResultString);
            return False;
        }

        int size = ogame->GetGameWorld()->wayNet->wplist.GetNumInList();
        if (size <= 0)
        {
            DEBUG_MSG("StExt_GetRandomWp - Wp list is empty!");
            par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
            par->SetReturn(ResultString);
            return False;
        }

        uint prohibitedCount = ProhibitedWaypoints.GetNum();
        for (uint i = 0; i < prohibitedCount; ++i)
            ProhibitedWaypoints[i].wpInst = world->wayNet->GetWaypoint(ProhibitedWaypoints[i].Wp);

        int iMax = 128 + (prohibitedCount * 32);
        int i = 0, indx = Invalid;
        bool isfound = false, wpIsValid;
        float dist;
        zCWaypoint* wp;
        do
        {
            wpIsValid = true;
            indx = StExt_Rand::Index(size);
            wp = world->wayNet->wplist.Get(indx);

            if (wp)
            {
                for (uint i = 0; i < prohibitedCount; i++)
                {
                    if (!ProhibitedWaypoints[i].wpInst) continue;

                    dist = (wp->GetPositionWorld() - ProhibitedWaypoints[i].wpInst->GetPositionWorld()).LengthApprox();
                    if (std::isnan(dist)) continue;

                    if ((int)dist <= ProhibitedWaypoints[i].Radius)
                    {
                        wpIsValid = false;
                        break;
                    }
                }

                if (wpIsValid)
                {
                    isfound = true;
                    ResultString = wp->GetName();
                    break;
                }
            }

            ++i;
            if (i >= iMax)
            {
                DEBUG_MSG("StExt_GetRandomWp - Wp not found. Too many attempts: " + Z i + "!");
                break;
            }
        } 
        while ((i < iMax) && !isfound);
        DEBUG_MSG_IF(!isfound, "StExt_GetRandomWp - Wp not found!");

        par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
        par->SetReturn(ResultString);
        return isfound;
    }

    int __cdecl StExt_GetRandomEmptyChest()
    {
        zCParser* par = zCParser::GetParser();

        ResultString.Clear();
        oCWorld* world = ogame->GetGameWorld();
        if (!world)
        {
            par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
            par->SetReturn(ResultString);
            return False;
        }

        Array<oCMobContainer*> chests = Array<oCMobContainer*>();
        auto node = world->voblist;
        while (node)
        {
            zCVob* vob = node->GetData();
            if (vob)
            {
                auto chest = dynamic_cast<oCMobContainer*>(vob);
                if (chest && (!chest->items || !chest->items->contents || chest->items->contents->GetNumInList() == 0))
                    chests.Insert(chest);
            }
            node = node->next;
        }

        if (!chests.IsEmpty())
        {
            uint indx = StExt_Rand::Index(chests.GetNum());
            ResultString = chests[indx]->GetObjectName();

            DEBUG_MSG("StExt_GetRandomEmptyChest: " + ResultString);
            par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
            par->SetReturn(ResultString);
            return False;
        }

        DEBUG_MSG("StExt_GetRandomEmptyChest: Empty!");
        par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
        par->SetReturn(ResultString);
        return False;
    }


    int __cdecl StExt_StartUncaper()
    {
        StartUncaper();
        return True;
    }

    int __cdecl StExt_UpdateUncaper()
    {
        StatsUncaperLoop();
        return True;
    }

    int __cdecl StExt_UpdateUncaperStat()
    {
        zCParser* par = zCParser::GetParser();
        int index; par->GetParameter(index);
        UpdateUncapedStat(index);
        return True;
    }

    int __cdecl StExt_UpdateTradeVars()
    {
        zCParser* par = zCParser::GetParser();
        int sellMulRaw;
        par->GetParameter(sellMulRaw);

        float sellMul = (sellMulRaw > 0) ? sellMulRaw * 0.001f : 0.01f;
        if (sellMul < 0.01f) sellMul = 0.01f;
        if (sellMul > 0.99f) sellMul = 0.99f;
        ItemSellPriceMult = sellMul;

        if (ItemBasePriceMult < 0.1f) ItemBasePriceMult = 0.1f;
        if (ItemBasePriceMult > 100.0f) ItemBasePriceMult = 100.0f;

        //DEBUG_MSG("StExt_UpdateTradeVars - ItemSellPriceMult: " + Z ItemSellPriceMult + " | ItemBasePriceMult: " + Z ItemBasePriceMult);
        return True;
    }

    int __cdecl StExt_RegtisterScriptPlugin()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING modName, modVersion, modAuthor;
        par->GetParameter(modAuthor);
        par->GetParameter(modVersion);
        par->GetParameter(modName);

        ModExtensionInfo info = ModExtensionInfo();
        info.Name = modName;
        info.Version = modVersion;
        info.Author = modAuthor;
        ModPluginsInfo.Insert(info);
        return True;
    }


    int __cdecl StExt_OverrideFunc()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING origFuncName, newFuncName;
        par->GetParameter(newFuncName);
        par->GetParameter(origFuncName);
        DEBUG_MSG("StExt_OverrideFunc - replace '" + origFuncName + "' func to '" + newFuncName + "' ...");

        zCPar_Symbol* origFuncSym;
        zCPar_Symbol* newFuncSym;
        origFuncSym = par->GetSymbol(origFuncName);
        newFuncSym = par->GetSymbol(newFuncName);
        if (!origFuncSym || !newFuncSym)
        {
            DEBUG_MSG("StExt_OverrideFunc - symbols not found!");
            par->SetReturn(false);
            return False;
        }

        if (origFuncSym->type != zPAR_TYPE_FUNC || newFuncSym->type != zPAR_TYPE_FUNC)
        {
            if (origFuncSym->type != zPAR_TYPE_FUNC) { DEBUG_MSG("StExt_OverrideFunc - original symbol is not a func!"); }
            else { DEBUG_MSG("StExt_OverrideFunc - new symbol is not a func!"); }
            par->SetReturn(false);
            return False;
        }

        int oldPos = origFuncSym->single_intdata;
        int newPos = newFuncSym->single_intdata;
        byte* bytecodes = par->stack.stack;

        Array<byte*> referralTokens = Array<byte*>();
        uint index = 0, stackSize = par->stack.GetDynSize();
        while (index < stackSize - 1)
        {
            byte& token = bytecodes[index++];
            switch (token)
            {
            case zPAR_TOK_CALL:
                referralTokens += &token;
                index += 4;
                break;
            }
        }

        int tokensCount = referralTokens.GetNum();
        int overrideCount = 0;
        for (int i = 0; i < tokensCount; ++i)
        {
            byte* bytecode = referralTokens[i];
            uint byteIndx = static_cast<uint>((int_t)bytecode - (int_t)bytecodes);
            if (byteIndx >= stackSize) continue;

            byte& command = bytecode[0];
            int& address = (int&)bytecode[1];

            if (address == oldPos)
            {
                address = newPos;
                ++overrideCount;
            }
        }

        referralTokens.Clear();
        DEBUG_MSG("StExt_OverrideFunc - overriding '" + origFuncName + "' (" + Z overrideCount + ") was DONE!");
        par->SetReturn(true);
        return True;
    }

    int __cdecl StExt_OverrideDialog()
    {
        zCParser* par = zCParser::GetParser();
        int instanceId, actionId, condId;
        zSTRING infoFuncName, condFuncName;
        par->GetParameter(condFuncName);
        par->GetParameter(infoFuncName);
        par->GetParameter(instanceId);

        oCInfo* pInfo = ogame->GetInfoManager()->GetInformation(instanceId);
        if (!pInfo)
        {
            DEBUG_MSG("StExt_OverrideDialog - dialog not found!");
            par->SetReturn(false);
            return False;
        }

        actionId = par->GetIndex(infoFuncName);
        int oldActionId = pInfo->pd.information;
        if (actionId != Invalid && oldActionId != actionId)
        {
            pInfo->pd.information = actionId;
            DEBUG_MSG("StExt_OverrideDialog - override action. Id was/new: " + Z oldActionId + " / " + Z actionId + " | New func name: " + infoFuncName);
        }

        condId = par->GetIndex(condFuncName);
        int oldConditionId = pInfo->pd.conditions;
        if (condId != Invalid && oldConditionId != condId)
        {
            pInfo->pd.conditions = condId;
            DEBUG_MSG("StExt_OverrideDialog - override condition. Id was/new: " + Z oldConditionId + " / " + Z condId + " | New func name: " + condFuncName);
        }

        DEBUG_MSG("StExt_OverrideDialog - overriding '" + pInfo->name + "' was DONE!");
        par->SetReturn(true);
        return True;
    }

    int __cdecl StExt_OverrideConst()
    {
        zCParser* par = zCParser::GetParser();
        int newValue;
        zSTRING constName;
        par->GetParameter(constName);
        par->GetParameter(newValue);

        zCPar_Symbol* sym = par->GetSymbol(constName);
        if (sym)
            sym->SetValue(newValue, 0);
        return True;
    }


    int __cdecl StExt_Struct_Sizeof()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING className;
        par->GetParameter(className);

        int instanceID;
        instanceID = par->GetIndex(className);
        if (instanceID == Invalid)
        {
            DEBUG_MSG("StExt_Struct_Sizeof - Class not found!");
            par->SetReturn(Invalid);
            return False;
        }

        zCPar_Symbol* classInstSym = par->GetSymbol(instanceID);
        uint elementsCount = classInstSym->ele;

        zCPar_Symbol* second = classInstSym;
        uint size = 0, memberId = 0;
        while (second)
        {
            ++memberId;
            size += second->type == zPAR_TYPE_STRING ? sizeof(zSTRING) : 4;
            second = second->next;
            if (memberId > elementsCount) break;
        }
        par->SetReturn(static_cast<int>(size));
        return True;
    }

    int __cdecl StExt_Struct_Alloc()
    {
        zCParser* par = zCParser::GetParser();
        int length;
        par->GetParameter(length);
        void* mem = AllocateDynamicObject(static_cast<size_t>(length));
        if (mem == Null)
        {
            par->SetReturn(Null);
            DEBUG_MSG("StExt_Struct_Alloc - Allocate failed!");
            return False;
        }
        par->SetReturn(mem);
        DEBUG_MSG("StExt_Struct_Alloc - Allocated: " + Z length + " bytes in ObjectsMemPool!");
        return True;
    }

    int __cdecl StExt_Struct_Free()
    {
        zCParser* par = zCParser::GetParser();
        void* mem = par->GetInstance();
        if (!FreeDynamicObject(mem)) return False;
        DEBUG_MSG("StExt_Struct_Free - Allocated memory was cleared!");
        return True;
    }

    int __cdecl StExt_Struct_IsEmpty()
    {
        zCParser* par = zCParser::GetParser();
        void* mem = par->GetInstance();
        bool result = mem && IsObjectsTableAllocated(mem);
        par->SetReturn(result);
        return result;
    }

    //func int StExt_Struct_Write(var C_IntArray ptr, var int offset, var int value);
    int __cdecl StExt_Struct_Write()
    {
        zCParser* par = zCParser::GetParser();
        int value, offset;
        par->GetParameter(value);
        par->GetParameter(offset);
        void* ptr = par->GetInstance();

        bool ok = WriteToDynamicObject(ptr, (size_t)offset, value);
        DEBUG_MSG_IF(!ok, "StExt_Struct_Write - fail to write!");
        par->SetReturn((int)ok);
        return ok;
    }

    //func int StExt_Struct_Read(var C_IntArray ptr, var int offset);
    int __cdecl StExt_Struct_Read()
    {
        zCParser* par = zCParser::GetParser();

        int offset;
        par->GetParameter(offset);
        void* ptr = par->GetInstance();

        int value = 0;
        bool ok = ReadFormDynamicObject((void*)ptr, (size_t)offset, value);
        DEBUG_MSG_IF(!ok, "StExt_Struct_Read - fail to write!");
        par->SetReturn(ok ? value : 0);
        return ok ? value : 0;
    }


    // ********************** NPC FUNCS **********************

    int __cdecl StExt_GetNpc()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* npc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        par->SetReturn(npc);
        return true;
    }

    int __cdecl StExt_SpawnNpcWithFunc()
    {
        zCParser* par = zCParser::GetParser();
        int count, instance, initFuncIndx;
        zSTRING initFunc, point;
        par->GetParameter(initFunc);
        par->GetParameter(point);
        par->GetParameter(count);
        par->GetParameter(instance);

        if (point.IsEmpty())
        {
            DEBUG_MSG("StExt_SpawnNpcWithFunc - wp is empty!");
            return False;
        }

        initFuncIndx = initFunc.IsEmpty() ? Invalid : parser->GetIndex(initFunc);
        for (int i = 0; i < count; ++i)
        {
            oCNpc* pNpc = ogame->GetSpawnManager()->SpawnNpc(instance, point, 0.0f);
            if (!pNpc)
            {
                DEBUG_MSG("StExt_SpawnNpcWithFunc - fail to spawn instanceId: " + Z(instance) + " at '" + point + "'!");
                return False;
            }

            pNpc->respawnOn = false;
            if (rtnMan)
                rtnMan->UpdateSingleRoutine(pNpc);

            if (initFuncIndx != Invalid)
            {
                void* slf = parser->GetSymbol("STEXT_SELF")->GetInstanceAdr();
                parser->SetInstance("STEXT_SELF", pNpc);
                par->CallFunc(initFuncIndx);
                parser->SetInstance("STEXT_SELF", slf);
            }
        }
        return True;
    }

    int __cdecl StExt_ForEachNpcInRadius()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING initFunc, condFunc, execFunc;
        int radius, initFuncIndx, condFuncIndx, execFuncIndx;

        par->GetParameter(execFunc);
        par->GetParameter(condFunc);
        par->GetParameter(initFunc);
        par->GetParameter(radius);
        oCNpc* center = (oCNpc*)par->GetInstance();

        //DEBUG_MSG("StExt_ForEachNpcInRadius - Do FOR(Init = '" + initFunc + "'; Cond = '" + condFunc + "'; Func = '" + execFunc + "')");
        initFuncIndx = par->GetIndex(initFunc);
        condFuncIndx = par->GetIndex(condFunc);
        execFuncIndx = par->GetIndex(execFunc);

        if (radius < 10) radius = 10;
        if (!center || (execFuncIndx == Invalid))
        {
            DEBUG_MSG_IF(!center, "StExt_ForEachNpcInRadius -  center npc is null!");
            DEBUG_MSG_IF(execFuncIndx == Invalid, "StExt_ForEachNpcInRadius -  func not found!");
            return False;
        }

        zCArray<oCNpc*> npcArray;
        GetNpcInRadius(npcArray, center, static_cast<float>(radius));
        if (npcArray.GetNum() <= 0) return True;

        void* slf = par->GetSymbol("STEXT_SELF")->GetInstanceAdr();
        void* oth = par->GetSymbol("STEXT_OTHER")->GetInstanceAdr();

        parser->SetInstance("STEXT_SELF", center);
        if (initFuncIndx != Invalid)
            par->CallFunc(initFuncIndx);

        parser->SetInstance("STEXT_OTHER", center);
        for (int i = 0; i < npcArray.GetNum(); ++i)
        {
            if (npcArray[i] == Null) continue;

            if (condFuncIndx != Invalid)
            {
                int condResult = *(int*)par->CallFunc(condFuncIndx);
                if (!condResult) continue;
            }

            parser->SetInstance("STEXT_SELF", npcArray[i]);
            par->CallFunc(execFuncIndx);
        }

        parser->SetInstance("STEXT_OTHER", slf);
        parser->SetInstance("STEXT_SELF", oth);
    }

    int __cdecl StExt_FindTargetInRadius()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING condFunc;
        int radius, condFuncIndx;

        par->GetParameter(condFunc);
        par->GetParameter(radius);
        oCNpc* center = (oCNpc*)par->GetInstance();

        condFuncIndx = par->GetIndex(condFunc);
        if (condFuncIndx == Invalid)
        {
            parser->SetReturn(false);
            return False;
        }
        if (radius < 10) radius = 10;

        zCArray<oCNpc*> npcArray;
        GetNpcInRadius(npcArray, center, static_cast<float>(radius));
        if (npcArray.GetNum() <= 0)
        {
            parser->SetReturn(false);
            return False;
        }

        for (int i = 0; i < npcArray.GetNum(); ++i)
        {
            if (npcArray[i] == Null) continue;

            parser->SetInstance("STEXT_SELF", npcArray[i]);
            int condResult = *(int*)par->CallFunc(condFuncIndx);
            if (condResult)
            {
                parser->SetInstance("STEXT_OTHER", npcArray[i]);
                parser->SetReturn(true);
                return True;
            };
        }
        parser->SetReturn(false);
        return False;
    }

    int __cdecl StExt_InfuseNpcWithMagic()
    {
        zCParser* par = zCParser::GetParser();
        int tier;
        par->GetParameter(tier);
        oCNpc* pNpc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        if (!pNpc)
        {
            DEBUG_MSG("StExt_InfuseNpcWithMagic - target npc is null!");
            par->SetReturn(Null);
            return False;
        }

        MagicInfusionData* infusion = CreateInfusion(tier);
        par->SetReturn(infusion);
        return True;
    }

    int __cdecl StExt_DisposeNpcInfusion()
    {
        zCParser* par = zCParser::GetParser();
        void* infusionInstance = par->GetInstance();

        if (!infusionInstance)
        {
            DEBUG_MSG("DisposeNpcInfusion - instance is null!");
            return False;
        }
        MagicInfusionData* infusion = reinterpret_cast<MagicInfusionData*>(infusionInstance);
        SAFE_DELETE(infusion);
        par->SetReturn(Null);
        return True;
    }

    int __cdecl StExt_RegisterNpc()
    {
        zCParser* par = zCParser::GetParser();
        int npcUid;

        par->GetParameter(npcUid);
        oCNpc* npc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        if (!npc)
        {
            DEBUG_MSG("StExt_RegisterNpc - Try to register null npc!");
            par->SetReturn(Invalid);
            return False;
        }

        RegisterNpc(npc, npcUid);
        par->SetReturn(npcUid);
        return True;
    }

    int __cdecl StExt_GetNpcByUid()
    {
        zCParser* par = zCParser::GetParser();
        int npcUid;
        par->GetParameter(npcUid);

        if (npcUid <= 0)
        {
            par->SetReturn(Null);
            return False;
        }

        oCNpc* npc = GetNpcByUid(npcUid);
        par->SetReturn(npc);
        return True;
    }

    int __cdecl StExt_IsUidRegistered()
    {
        zCParser* par = zCParser::GetParser();
        int npcUid; par->GetParameter(npcUid);
        par->SetReturn(IsUidRegistered(npcUid));
        return True;
    }

    int __cdecl StExt_GetNextNpcUid()
    {
        zCParser* par = zCParser::GetParser();
        par->SetReturn(GetNextNpcUid());
        return true;
    }


    int __cdecl StExt_ValidateNpcPosition()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* npc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        if (!npc)
            return False;

        if (NpcPositionIsInvalid(npc))
        {
            DEBUG_MSG("StExt_ValidateNpcPosition - fix npc '" + Z npc->name + "' position!");
            zVEC3 pos = FindCorrectPosition(npc);
            npc->SetCollDet(FALSE);             
            npc->SetPositionWorld(pos);
            npc->SetCollDet(TRUE);
            npc->Enable(npc->GetPositionWorld());
            ogame->GetSpawnManager()->SpawnImmediately(FALSE);
            return True;
        }
        return False;
    }

    int __cdecl StExt_TeleportToNpc()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* target = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        oCNpc* atk = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());

        if (!atk || !target)
        {
            DEBUG_MSG("StExt_TeleportToNpc - one of npc is null!");
            return False;
        }

        zMAT4 trafo = target->trafoObjToWorld;
        zVEC3 pos = target->GetPositionWorld() + (target->GetAtVectorWorld() * 200.0);

        atk->SetCollDet(FALSE);
        atk->SetPositionWorld(pos);
        if (NpcPositionIsInvalid(atk))
        {
            DEBUG_MSG("StExt_TeleportToNpc - fix npc position...");
            pos = FindCorrectPosition(atk);
            atk->SetPositionWorld(pos);
        }
        atk->SetCollDet(TRUE);
        atk->Enable(atk->GetPositionWorld());
        ogame->GetSpawnManager()->SpawnImmediately(FALSE);
        return True;
    }

    int __cdecl StExt_IsNpcInProhibitedPlace()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* npc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());

        if (!npc)
        {
            DEBUG_MSG("StExt_IsNpcInProhibitedPlace - npc is null!");
            parser->SetReturn(true);
            return False;
        }
        if (!ogame->GetWorld()) return False;

        for (uint i = 0U; i < ProhibitedWaypoints.GetNum(); ++i)
        {
            zCWaypoint* pWp = ogame->GetWorld()->wayNet->GetWaypoint(ProhibitedWaypoints[i].Wp);
            if (!pWp) continue;

            int dist = static_cast<int>((npc->GetPositionWorld() - pWp->GetPositionWorld()).LengthApprox());
            if (dist < ProhibitedWaypoints[i].Radius)
            {
                parser->SetReturn(true);
                return True;
            }
        }

        parser->SetReturn(false);
        return True;
    }

    int __cdecl StExt_CopyNpcLook()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* originalNpc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        oCNpc* targetNpc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        int armorId = Invalid;

        if (!targetNpc || !originalNpc)
        {
            DEBUG_MSG("StExt_CopyNpcLook - some actor is null!");
            par->SetReturn(false);
            return False;
        }
        DEBUG_MSG("StExt_CopyNpcLook - from '" + originalNpc->name[0] + "' to '" + targetNpc->name[0] + "' ...");

        targetNpc->SetAdditionalVisuals(originalNpc->body_visualName, originalNpc->body_TexVarNr, originalNpc->body_TexColorNr,
            originalNpc->head_visualName, originalNpc->head_TexVarNr, originalNpc->teeth_TexVarNr, Invalid);

        oCItem* armor = originalNpc->GetEquippedArmor();
        if (armor)
        {
            armorId = armor->GetInstance();
            oCItem* pItem = dynamic_cast<oCItem*>(ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, armorId));
            if (pItem)
            {
                pItem->visual_change = armor->visual_change;
                targetNpc->PutInInv(pItem);
                targetNpc->EquipArmor(pItem);
                pItem->Release();
            }
        }

        oCItem* helm = originalNpc->GetSlotItem("ZS_HELMET");
        if (helm)
        {
            armorId = helm->GetInstance();
            oCItem* pItem = dynamic_cast<oCItem*>(ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, armorId));
            if (pItem)
            {
                targetNpc->PutInInv(pItem);
                targetNpc->EquipArmor(pItem);
                pItem->Release();
            }
        }
        par->SetReturn(true);
        return True;
    }


    // ********************** DAMAGE FUNCS **********************

    int __cdecl StExt_InitializeExtraDamage()
    {
        zCParser* par = zCParser::GetParser();
        memset(&ExtraDamage, 0, sizeof ExtraDamage);
        par->SetInstance("STEXT_EXTRADAMAGEINFO", &ExtraDamage);
        return True;
    }

    int __cdecl StExt_InitializeDotDamage()
    {
        zCParser* par = zCParser::GetParser();
        memset(&DotDamage, 0, sizeof DotDamage);
        par->SetInstance("STEXT_DOTDAMAGEINFO", &DotDamage);
        return True;
    }

    int __cdecl StExt_InitializeReflectDamage()
    {
        zCParser* par = zCParser::GetParser();
        memset(&ReflectDamage, 0, sizeof ReflectDamage);
        par->SetInstance("STEXT_REFLECTDAMAGEINFO", &ReflectDamage);
        return True;
    }

    int __cdecl StExt_ApplyDotDamage()
    {
        DEBUG_MSG("StExt_ApplyDotDamage - call");
        zCParser* par = zCParser::GetParser();
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();
        ApplyDotDamage(atk, target);
        return True;
    }

    int __cdecl StExt_ApplyExtraDamage()
    {
        DEBUG_MSG("StExt_ApplyExtraDamage - call");
        zCParser* par = zCParser::GetParser();
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();
        ApplyExtraDamage(atk, target);
        return True;
    }

    int __cdecl StExt_ApplyReflectDamage()
    {
        DEBUG_MSG("StExt_ApplyReflectDamage - call");
        zCParser* par = zCParser::GetParser();
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();
        ApplyReflectDamage(atk, target);
        return True;
    }

    int __cdecl StExt_ApplyDamage()
    {
        DEBUG_MSG("StExt_ApplyDamage - call...");
        zCParser* par = zCParser::GetParser();
        int damType, damage;
        par->GetParameter(damType);
        par->GetParameter(damage);
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();

        if ((!atk) || (!target))
        {
            DEBUG_MSG("StExt_ApplyDamage - one of actors is null!");
            return False;
        }
        if (damage <= 5) damage = 5;
        if (damType == 0) damType = dam_barrier;

        oCNpc::oSDamageDescriptor desc = oCNpc::oSDamageDescriptor();
        ApplyDamages(damType, desc.aryDamage, damage);
        desc.fDamageTotal = static_cast<float>(damage);
        desc.enuModeDamage = static_cast<unsigned long>(damType);
        desc.fDamageMultiplier = 1.0f;
        desc.vecLocationHit = target->GetPositionWorld();
        desc.pNpcAttacker = atk;
        desc.pVobAttacker = atk;
        desc.bOnce = true;
        desc.dwFieldsValid |= oCNpc::oEDamageDescFlag_Attacker | oCNpc::oEDamageDescFlag_Npc |
            oCNpc::oEDamageDescFlag_DamageType | oCNpc::oEDamageDescFlag_Damage | DamageDescFlag_ExtraDamage;
        desc.pVobHit = target;

        DEBUG_MSG("StExt_ApplyDamage - send descriptor. Damage = " + Z desc.fDamageTotal);
        target->OnDamage(desc);
        return True;
    }

    int __cdecl StExt_StunPlayer()
    {
        zCParser* par = zCParser::GetParser();
        int stun = false;
        par->GetParameter(stun);
        // TODO
        return True;
    }

    int __cdecl StExt_ThrowItem()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING throwInstanceName;
        int damage, damageType;

        par->GetParameter(damageType);
        par->GetParameter(damage);
        par->GetParameter(throwInstanceName);
        oCNpc* target = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        oCNpc* attaker = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());

        if (!attaker || !target)
        {
            DEBUG_MSG("StExt_ThrowItem - some actor is null!");
            return False;
        }

        int instanceId = par->GetIndex(throwInstanceName);
        if (instanceId == Invalid)
        {
            DEBUG_MSG("StExt_ThrowItem - throwable item not found!");
            return False;
        }

        ThrowItemDescriptor throwData = ThrowItemDescriptor();
        throwData.attacker = attaker;
        throwData.target = target;
        throwData.throwInstance = instanceId;
        throwData.damage = damage < 5 ? 5 : damage;
        throwData.damageType = damageType <= 0 ? dam_barrier : damageType;

        ThrowItem(throwData);
        return True;
    }

    int __cdecl StExt_ThrowItemEx()
    {
        zCParser* par = zCParser::GetParser();
        int damage, damageType, throwInstance;

        par->GetParameter(damageType);
        par->GetParameter(damage);
        par->GetParameter(throwInstance);
        oCNpc* target = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        oCNpc* attaker = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());

        if (!attaker || !target)
        {
            DEBUG_MSG("StExt_ThrowItem - some actor is null!");
            return False;
        }

        if (throwInstance == Invalid)
        {
            DEBUG_MSG("StExt_ThrowItem - throwable item not found!");
            return False;
        }

        ThrowItemDescriptor throwData = ThrowItemDescriptor();
        throwData.attacker = attaker;
        throwData.target = target;
        throwData.throwInstance = throwInstance;
        throwData.damage = damage < 5 ? 5 : damage;
        throwData.damageType = damageType <= 0 ? dam_barrier : damageType;

        ThrowItem(throwData);
        return True;
    }


    // ********************** ITEMS FUNCS **********************

    int __cdecl StExt_GetItemById()
    {
        zCParser* par = zCParser::GetParser();
        int instanceId; par->GetParameter(instanceId);
        
        if (instanceId == Invalid)
        {
            par->SetReturn(Null);
            return False;
        }

        oCItem* pItem = new oCItem();
        if (par->CreateInstance(instanceId, pItem)) {
            par->SetReturn(pItem);
        }
        par->SetReturn(Null);
        return True;
    }

    int __cdecl StExt_DeleteTempItem()
    {
        zCParser* par = zCParser::GetParser();
        oCItem* pItem = (oCItem*)par->GetInstance();
        pItem->Release();
        return True;
    }

    int __cdecl StExt_GetItemNameById()
    {
        zCParser* par = zCParser::GetParser();
        int instanceId; par->GetParameter(instanceId);

        ResultString.Clear();
        if (instanceId == Invalid)
        {
            par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
            par->SetReturn(ResultString);
            return False;
        }

        oCItem* pItem = new oCItem();
        if (par->CreateInstance(instanceId, pItem)) {
            ResultString = pItem->name;
        }
        par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
        par->SetReturn(ResultString);
        pItem->Release();
        return True;
    }

    int __cdecl StExt_CreateRandomItem()
    {
        zCParser* par = zCParser::GetParser();
        int itemClassId, power;
        par->GetParameter(power);
        par->GetParameter(itemClassId);

        if (power <= 0) power = 1;
        const int itemIndex = GenerateNewMagicItem(itemClassId, power);
        par->SetReturn(itemIndex);
        return True;
    }

    int __cdecl StExt_GetRegularItem()
    {
        zCParser* par = zCParser::GetParser();
        int itemClassId, power;
        par->GetParameter(power);
        par->GetParameter(itemClassId);

        if (power <= 0) power = 1;
        const int itemIndex = GenerateNewRegularItem(itemClassId, power);
        par->SetReturn(itemIndex);
        return True;
    }

    int __cdecl StExt_UseEnchantedItem()
    {
        zCParser* par = zCParser::GetParser();
        zCPar_Symbol* ps = parser->GetSymbol("ITEM");

        oCItem* item = dynamic_cast<oCItem*>((zCVob*)ps->GetInstanceAdr());
        if (!item)
        {
            DEBUG_MSG("StExt_UseEnchantedItem - item is null");
            return False;
        }

        ItemExtension* enchantment = GetItemExtension(item);
        if (!enchantment)
        {
            DEBUG_MSG("StExt_UseEnchantedItem - enchantment is null");
            return False;
        }

        if (enchantment->Type == (byte)ItemType::Consumable)
        {
            for (int i = 0; i < ItemExtension_Stats_Max; ++i) {
                if (enchantment->StatId[i] == Invalid) continue;
                par->CallFunc(StExt_ApplyPotionEffectFunc, enchantment->StatId[i], enchantment->StatValue[i], enchantment->StatDuration[i]);
            }
            return True;

            for (int i = 0; i < ItemExtension_OwnStats_Max; ++i) {
                if (enchantment->OwnStatId[i] == Invalid) continue;
                par->CallFunc(StExt_ApplyPotionPermEffectFunc, enchantment->StatId[i], enchantment->StatValue[i]);
            }
        }
        return False;
    }

    int __cdecl StExt_GetItemInstanceName()
    {
        zCParser* par = zCParser::GetParser();
        oCItem* itm = dynamic_cast<oCItem*>((zCVob*)par->GetInstance());

        ResultString.Clear();
        if (itm)        
            ResultString = itm->GetInstanceName();        
        DEBUG_MSG_IF(!itm, "StExt_GetItemInstanceName - itm is null");
        
        par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
        par->SetReturn(ResultString);
        return True;
    }

    // ********************** TEXT FUNCS **********************

    int __cdecl StExt_StatValueToString()
    {
        zCParser* par = zCParser::GetParser();
        int statId, statVal;
        par->GetParameter(statVal);
        par->GetParameter(statId);

        ResultString.Clear();
        auto statPair = ExtraStatsData.GetSafePair(statId);
        if (statPair)
        {
            UiValueDisplayType displayType = (UiValueDisplayType)statPair->GetValue().ValueType;
            switch (displayType)
            {
                case UiValueDisplayType::Bool: ConvertValueToYesNo(ResultString, statVal); break;
                case UiValueDisplayType::Permille: ConvertValueToPermille(ResultString, statVal); break;
                case UiValueDisplayType::Percent: ResultString = zSTRING(statVal) + "%"; break;
                case UiValueDisplayType::DeciPercent: ResultString = zSTRING(statVal * 10) + "%"; break;
                case UiValueDisplayType::Default:
                default: ResultString = zSTRING(statVal); break;
            }
        }

        par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
        par->SetReturn(ResultString);
        return True;
    }

    int __cdecl StExt_Info_AddChoice()
    {
        zCParser* par = zCParser::GetParser();
        int menu, action;
        zSTRING text, actionName;
        par->GetParameter(actionName);
        par->GetParameter(text);
        par->GetParameter(menu);

        action = par->GetIndex(actionName);
        if (action == Invalid)
        {
            DEBUG_MSG("StExt_Info_AddChoice - action '" + actionName + "' not found!");
            return False;
        }
        if(!ogame || !ogame->GetInfoManager()) return False;

        oCInfo* pInfo = ogame->GetInfoManager()->GetInformation(menu);
        if (pInfo)
        {
            DEBUG_MSG("StExt_Info_AddChoice - menu '" + Z(menu) + "' not found!");
            return False;
        }
        pInfo->AddChoice(text, action);
        return True;
    }

    int __cdecl StExt_PrintMessageToTray()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING text, color;
        par->GetParameter(color);
        par->GetParameter(text);
        MsgTray_AddEntry(text, color);
        return True;
    }

    int __cdecl StExt_TryGetSymbolDescriptionText()
    {        
        zCParser* par = zCParser::GetParser();
        zSTRING optionName;
        par->GetParameter(optionName);

        ResultString.Clear();
        if (!optionName.IsEmpty())
        {
            int pos = optionName.Search(ModSymbolPrefix, 0);
            if (pos != Invalid)
            {
                optionName.Delete(pos, ModSymbolPrefix.Length());
                optionName.Insert(pos, ModSymbolNamePrefix);
                ResultString = optionName;
            }
        }

        par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
        par->SetReturn(ResultString);
        return True;
    }

    int __cdecl StExt_Info_BuildItemGeneratorPresetsChoices()
    {
        zCParser* par = zCParser::GetParser();
        int menu, action;
        zSTRING text, actionName;
        par->GetParameter(actionName);
        par->GetParameter(menu);
        // DISABLED
        return True;
    }

    int __cdecl StExt_Info_BuildConfigPresetsChoices()
    {
        zCParser* par = zCParser::GetParser();
        int menu, action;
        zSTRING text, actionName;

        par->GetParameter(actionName);
        par->GetParameter(menu);
        if (!ogame->GetInfoManager()) return False;

        action = par->GetIndex(actionName);
        oCInfo* pInfo = ogame->GetInfoManager()->GetInformation(menu);
        if ((action == Invalid) || !pInfo) return False;

        for (uint i = 0; i < GameConfigsPresets.GetNum(); i++)
        {
            zSTRING text = GameConfigsPresets[i].Text + " [" + GameConfigsPresets[i].Name + "]";
            pInfo->AddChoice(text, action);
        }
        return True;
    }

    int __cdecl StExt_BuildItemsSellForm()
    {
        zCParser* par = zCParser::GetParser();
        if (!player || !player->inventory2.GetContents())
        {
            DEBUG_MSG("StExt_BuildItemsSellForm - player seems not initialized!");
            return False;
        }

        const int callBackFunc = par->GetIndex("StExt_BuildItemsSellForm_Loop");
        if (callBackFunc == Invalid)
        {
            DEBUG_MSG("StExt_BuildItemsSellForm - func 'StExt_BuildItemsSellForm_Loop' not found!");
            return False;
        }

        Array<int> excludedItems = {};
        zCPar_Symbol* exludedItemsArr = par->GetSymbol("StExt_ItemsSellForm_ExcludedItems");
        if (exludedItemsArr)
        {
            uint arrSize = exludedItemsArr->ele;
            for (uint i = 0; i < arrSize; ++i)
            {
                int indx = par->GetIndex(exludedItemsArr->stringdata[i]);
                if (indx != Invalid) excludedItems.InsertEnd(indx);
            }
        }
        excludedItems.QuickSort();

        int mainFlag = 0, flags = 0, allowGenerated = FALSE, priceMultRaw = 10;
        par->GetParameter(priceMultRaw);
        par->GetParameter(allowGenerated);
        par->GetParameter(flags);
        par->GetParameter(mainFlag);

        const bool hasExcluded = !excludedItems.IsEmpty();
        const float priceMult = ((priceMultRaw * 0.01f) < 0.01f) ? 0.01f : priceMultRaw * 0.01f;
        zCListSort<oCItem>* it = player->inventory2.GetContents()->GetNextInList();
        while (it)
        {
            oCItem* pItem = it->GetData();
            if (pItem)
            {
                bool canSell = true;

                if (canSell && pItem->HasFlag(ITM_FLAG_ACTIVE)) { canSell = false; }
                if (canSell && HasFlag(pItem->hitp, bit_item_questitem)) { canSell = false; }
                if (canSell && hasExcluded && excludedItems.HasEqualSorted(pItem->GetInstance())) { canSell = false; }
                if (canSell && allowGenerated && IsExtendedItem(pItem)) { canSell = false; }

                if (canSell && (mainFlag != 0) && !HasFlag(pItem->mainflag, mainFlag)) { canSell = false; }
                if (canSell && (flags != 0) && !pItem->HasFlag(flags)) { canSell = false; }

                if (canSell)
                {
                    const int sellPrice = static_cast<int>(pItem->value * priceMult);

                    CraftInfoData craftData = CraftInfoData();
                    craftData.Price = sellPrice <= 0 ? 1 : sellPrice;
                    craftData.ResultInstance = pItem->GetInstanceName();
                    par->SetInstance("STEXT_CRAFTINFO", &craftData);
                    par->CallFunc(callBackFunc);
                    par->SetInstance("STEXT_CRAFTINFO", Null);
                }                
            }
            it = it->GetNextInList();
        }
        return True;
    }


    int __cdecl StExt_SetItemGeneratorConfigs()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING configFileName;
        par->GetParameter(configFileName);
        DEBUG_MSG("StExt_SetItemGeneratorConfigs -> read config: " + configFileName + "...");
        if (!SelectItemGeneratorConfigs(configFileName))
        {
            DEBUG_MSG("StExt_SetItemGeneratorConfigs -> ERROR. Item Generator Configs not loaded!");
            par->SetReturn(false);
            return False;
        }
        par->SetReturn(true);
        return True;
    }

    int __cdecl StExt_ResetItemGeneratorConfigs()
    {
        //InitItemGeneratorConfigs();
        return True;
    }

    int __cdecl StExt_RegistrateConfigsPreset()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING presetInstance;
        par->GetParameter(presetInstance);

        int index = par->GetIndex(presetInstance);
        if (index == Invalid)
        {
            DEBUG_MSG("StExt_RegistrateConfigsPreset -> Can't load configs preset instance '" + presetInstance + "'!");
            par->SetReturn(false);
            return FALSE;
        }

        for (uint i = 0; i < GameConfigsPresets.GetNum(); ++i) {
            if (GameConfigsPresets[i].Name == presetInstance)
            {
                DEBUG_MSG("StExt_RegistrateConfigsPreset -> Configs preset instance '" + presetInstance + "' already contains in presets list!");
                par->SetReturn(false);
                return FALSE;
            }
        }

        ConfigPresetData configPreset;
        par->CreateInstance(index, &configPreset);
        GameConfigsPresets.InsertEnd(configPreset);
        DEBUG_MSG("StExt_RegistrateConfigsPreset: " + configPreset.Name);
        par->SetReturn(true);
        return TRUE;
    }

    int __cdecl StExt_GetConfigsPreset()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING presetInstance;
        par->GetParameter(presetInstance);

        if (presetInstance.IsEmpty())
        {
            DEBUG_MSG("StExt_GetConfigsPreset -> Instance name is empty!");
            par->SetReturn(Null);
            return False;
        }

        ConfigPresetData* result = const_cast<ConfigPresetData*>(GetConfigPreset(presetInstance));
        par->SetReturn(result);
        return True;
    }

    int __cdecl StExt_ParsePresetName()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING rawInput;
        par->GetParameter(rawInput);

        ResultString.Clear();
        if (rawInput.IsEmpty())
        {
            DEBUG_MSG("StExt_GetConfigsPreset -> Instance name is empty!");
            par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
            par->SetReturn(ResultString);
            return False;
        }

        int left = rawInput.Search("[", 0);
        if (left != Invalid)
        {
            int right = rawInput.Search("]", left + 1);
            if (right != Invalid && right > left + 1) {
                ResultString = rawInput.Copied(left + 1, right - left - 1);
            }
        }

        par->GetSymbol("STEXT_RETURNSTRING")->SetValue(ResultString, 0);
        par->SetReturn(ResultString);
        return True;
    }

    int __cdecl StExt_RegisterConfig()
    {
        zCParser* par = zCParser::GetParser();

        int isExportable, isEditable, isVisible, configGroup, displayType;
        int maxValue, minValue;
        zSTRING valueSymbol, descSymbol;

        par->GetParameter(isExportable);
        par->GetParameter(isEditable);
        par->GetParameter(isVisible);
        par->GetParameter(displayType);
        par->GetParameter(maxValue);
        par->GetParameter(minValue);
        par->GetParameter(configGroup);
        par->GetParameter(descSymbol);
        par->GetParameter(valueSymbol);

        if (valueSymbol.IsEmpty() || par->GetSymbol(valueSymbol) == Null)
        {
            DEBUG_MSG("StExt_RegisterConfig -> Config value symbol name '" + valueSymbol + "' is empty or incorrect!");
            return False;
        }
        if ((descSymbol.IsEmpty() || par->GetSymbol(descSymbol) == Null) && isVisible) {
            DEBUG_MSG("StExt_RegisterConfig -> Symbol description symbol name '" + descSymbol + "' is empty or incorrect!");
        }

        NormalizeInstanceName(valueSymbol);
        if (ExtraConfigsData.HasKey(valueSymbol))
        {
            DEBUG_MSG("StExt_RegisterConfig -> Config '" + valueSymbol + "' already registered!");
            return False;
        }

        ExtraConfigData data = ExtraConfigData();
        data.ValueSymbol = valueSymbol;
        data.DescSymbol = descSymbol;
        data.ConfigGroup = configGroup;
        data.DisplayType = displayType;
        data.MaxValue = maxValue;
        data.MinValue = minValue;
        data.IsExportable = isExportable;
        data.IsEditable = isEditable;
        data.IsVisible = isVisible;
        data.DisplayOrder = static_cast<int>(ExtraConfigsData.Size());

        ExtraConfigsData.Insert(data.ValueSymbol, data);
        return True;
    }

    int __cdecl StExt_ValidateConfig()
    {
        zCParser* par = zCParser::GetParser();

        zSTRING valueSymbol;
        par->GetParameter(valueSymbol);

        auto pair = ExtraConfigsData.Find(valueSymbol);
        if (!pair) return False;

        ExtraConfigData& data = *pair;
        zCPar_Symbol* valueSym = par->GetSymbol(data.ValueSymbol);

        if (!valueSym) return False;
        switch (valueSym->type)
        {
            case zPAR_TYPE_INT:
            {
                int curValue = valueSym->single_intdata;
                if (curValue < data.MinValue) curValue = data.MinValue;
                if (curValue > data.MaxValue) curValue = data.MaxValue;
                valueSym->SetValue(curValue, 0);
                break;
            }
            case zPAR_TYPE_FLOAT:
            {
                float curValue = valueSym->single_floatdata;
                if (curValue < data.MinValue) curValue = static_cast<float>(data.MinValue);
                if (curValue > data.MaxValue) curValue = static_cast<float>(data.MaxValue);
                valueSym->SetValue(curValue, 0);
                break;
            }
            default:
                DEBUG_MSG("StExt_ValidateConfig - incorrect symbol '" + data.ValueSymbol + "' type!");
                break;
        }
        return True;
    }

    int __cdecl StExt_ValidateConfigs()
    {
        zCParser* par = zCParser::GetParser();
        for (auto& data : ExtraConfigsData)
        {
            zCPar_Symbol* valueSym = par->GetSymbol(data.ValueSymbol);

            if (!valueSym) continue;
            switch (valueSym->type)
            {
                case zPAR_TYPE_INT:
                {
                    int curValue = valueSym->single_intdata;
                    if (curValue < data.MinValue) curValue = data.MinValue;
                    if (curValue > data.MaxValue) curValue = data.MaxValue;
                    valueSym->SetValue(curValue, 0);
                    break;
                }
                case zPAR_TYPE_FLOAT:
                {
                    float curValue = valueSym->single_floatdata;
                    if (curValue < data.MinValue) curValue = static_cast<float>(data.MinValue);
                    if (curValue > data.MaxValue) curValue = static_cast<float>(data.MaxValue);
                    valueSym->SetValue(curValue, 0);
                    break;
                }
                default:
                    DEBUG_MSG("StExt_ValidateConfigs - incorrect symbol '" + data.ValueSymbol + "' type!");
                    break;
            }
        }
        return True;
    }

    int __cdecl StExt_ExportCurrentConfigs()
    {
        zCParser* par = zCParser::GetParser();
        DEBUG_MSG("StExt_ExportCurrentConfigs - export started...");

        const zSTRING configNameTemplate = "EthernalBreeze_ExportedConfig_";
        const zSTRING configTextTemplate = "ExportedConfigs_";
        zSTRING configName, configText, configApplyFunc, configsList;

        int configId = 0;
        do
        {
            ++configId;
            configName = configNameTemplate + FormatNumberPad(configId, 3);
        } 
        while (GetConfigPreset(configName));
        configText = configTextTemplate + FormatNumberPad(configId, 3);
        configApplyFunc = configName + "_OnApply";
        
        DEBUG_MSG("StExt_ExportCurrentConfigs - export config '" + configName + "'...");
        for (auto& data : ExtraConfigsData)
        {
            if (!data.IsExportable) continue;

            zCPar_Symbol* sym = par->GetSymbol(data.ValueSymbol);
            if (!sym)
            {
                DEBUG_MSG("StExt_ExportCurrentConfigs - symbol '" + data.ValueSymbol + "' Not Found!");
                continue;
            }

            zSTRING configLine = data.ValueSymbol + " = ";
            if (sym->type == zPAR_TYPE_INT) configLine += Z(sym->single_intdata);
            else if (sym->type == zPAR_TYPE_FLOAT) configLine += Z(sym->single_floatdata);
            else if (sym->type == zPAR_TYPE_STRING) configLine += "\"" + sym->stringdata[0] + "\"";
            else
            {
                DEBUG_MSG("StExt_ExportCurrentConfigs -> symbol '" + data.ValueSymbol + "' Has unknown type!");
                continue;
            }
            configsList += "\r\n\t" + configLine + ";";
        }

        DEBUG_MSG("StExt_ExportCurrentConfigs - process template...");
        zSTRING result = ConfigsExportTemplate;
        result.Replace("[ConfigName]", configName);
        result.Replace("[ConfigText]", configText);
        result.Replace("[ConfigApplyFunc]", configApplyFunc);
        result.Replace("[ConfigsList]", configsList);

        const zSTRING root = zoptions->GetDirString(zTOptionPaths::DIR_SYSTEM);
        const zSTRING configFileName = configName + ".d";
        const zSTRING path = root + "\\Autorun\\EthernalBreezeConfigs\\" + configFileName;
        DEBUG_MSG("StExt_ExportCurrentConfigs - start save file to " + path + " ...");

        zFILE_FILE* configsFile = new zFILE_FILE(path);
        configsFile->Create(path);
        configsFile->s_physPathString = Z path;
        configsFile->s_virtPathString = Z("\\Autorun\\EthernalBreezeConfigs\\");
        configsFile->Open(path, true);
        configsFile->Write(result);
        configsFile->Close();
        SAFE_DELETE(configsFile);

        DEBUG_MSG("StExt_ExportCurrentConfigs - '" + configName + "' was exported!");
        par->SetReturn(true);
        return True;
    }

    int __cdecl StExt_BuildEnchntedItemsSellForm()
    {
        zCParser* par = zCParser::GetParser();

        int size = 0;
        par->GetParameter(size);
        void* basePtr = par->GetInstance();

        if (!IsObjectsTableAllocated(basePtr))
        {
            DEBUG_MSG("StExt_BuildEnchntedItemsSellForm: Invalid pointer!");
            par->SetReturn(FALSE);
            return False;
        }

        Array<int> itemClasses = Array<int>();
        for (int i = 0; i < size; ++i)
        {
            int value = 0;
            bool ok = ReadFormDynamicObject(basePtr, i * sizeof(int), value);
            if (!ok)
            {
                DEBUG_MSG("StExt_BuildEnchntedItemsSellForm: Read out of bounds at index " + Z(i));
                break;
            }
            itemClasses.Insert(value);
        }

        if (!itemClasses.IsEmpty())
        {
            Array<const oCItem*> foundItems = Array<const oCItem*>();
            FindPlayerItems(itemClasses, foundItems);

            static const int callBackFunc = par->GetIndex("StExt_BuildEnchntedItemsSellForm_BuildLoop");
            const int itemsCount = static_cast<int>(foundItems.GetNum());
            if (foundItems.IsEmpty())
            {
                par->SetReturn(FALSE);
                return False;
            }

            DEBUG_MSG("StExt_BuildEnchntedItemsSellForm - items count: " + Z(itemsCount));
            for (int i = 0; i < itemsCount; ++i)
            {
                CraftInfoData craftData = CraftInfoData();
                craftData.Price = foundItems[i]->value;
                craftData.ResultInstance = GetItemInstanceName(foundItems[i]);
                par->SetInstance("STEXT_CRAFTINFO", &craftData);
                par->CallFunc(callBackFunc);
            }
            par->SetReturn(TRUE);
            return true;
        }

        par->SetReturn(FALSE);
        return False;
    }

    //GetUndefinedItems()
    int __cdecl StExt_GetUndefinedItemsCount()
    {
        zCParser* par = zCParser::GetParser();
        Array<const oCItem*> itms;
        FindPlayerUndefinedItems(itms);
        const int size = static_cast<int>(itms.GetNum());
        par->SetReturn(size);
        return true;
    }

    int __cdecl StExt_IdentifyAllItems()
    {
        Array<const oCItem*> itms;
        FindPlayerUndefinedItems(itms);
        const int size = static_cast<int>(itms.GetNum());
        if (size <= 0) return false;

        for (int i = 0; i < size; ++i) {
            if (itms[i])
                IdentifyItem(itms[i]);
        }
        return true;
    }


    int __cdecl StExt_RegisterItemAbility()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING itemAbility;
        par->GetParameter(itemAbility);
        // ToDo: finalize this shit... when time come
        //InitializeItemAbility(itemAbility);
        return true;
    }

    // foreach invoke of onActive for specific abilities
    int __cdecl StExt_ProcessItemsAbilities()
    {
        zCParser* par = zCParser::GetParser();
        int abilityType;
        int includeFlags = 0;
        int excludeFlags = 0;
        int triggerFlags = 0;
        
        // ToDo: finalize this shit... when time come
        /*
        ItemAbilityRecord* result = Null;
        uint max = EquipedItemAbilitiesData.GetNum();
        for (uint i = 0; i < max; ++i)
        {
            if (!EquipedItemAbilitiesData[i])
            {
                DEBUG_MSG("StExt_ProcessItemsAbilities - ability record is null!");
                continue;
            }

            ItemAbility* ability = EquipedItemAbilitiesData[i]->Ability;
            if (ability->OnApplyFunc == Invalid)
            {
                DEBUG_MSG("StExt_ProcessItemsAbilities - OnApply() handler for '" + ability->InstanceName + "' no found!");
                continue;
            }

            if ((triggerFlags != 0) && !HasFlag(ability->C_ItemAbility.TriggerFlags, triggerFlags)) continue;
            if (!FilterItemAbility(ability, abilityType, includeFlags, excludeFlags)) continue;
            if (!GetItemAbilityChance(ability->C_ItemAbility.Id, EquipedItemAbilitiesData[i]->C_ItemAbilityRecord.Chance)) continue;
            ItemAbilityRecord* abilityRecord = EquipedItemAbilitiesData[i];
            
            // Do funcs invoke, set "enviroment" and etc...

            parser->SetInstance("STEXT_CURRENTITEMABILITY_SELF", Null);
            parser->SetInstance("STEXT_CURRENTITEMABILITY_OTHER", Null);
            parser->SetInstance("STEXT_CURRENTITEMABILITY", &ability->C_ItemAbility);
            parser->SetInstance("STEXT_CURRENTITEMABILITY_STATS", &abilityRecord->C_ItemAbilityRecord);

        }
        */
        return True;
    }


    int __cdecl StExt_Player_ItemBatchUse()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING itemInstanceName;
        int maxItems;

        if (!player || !player->inventory2.GetContents())
        {
            DEBUG_MSG("StExt_Player_ItemBatchUse - player seems not initialized!");
            return FALSE;
        }

        par->GetParameter(maxItems);
        par->GetParameter(itemInstanceName);

        int itemInstanceId = par->GetIndex(itemInstanceName);
        if (itemInstanceId == Invalid) 
        {
            DEBUG_MSG("StExt_Player_ItemBatchUse - item '" + itemInstanceName + "' not found!");
            return FALSE;
        }

        oCItem* pItm = player->inventory2.GetItem(itemInstanceId);
        if (!pItm)
            return FALSE;

        int useFunc = pItm->onState[0];
        int itemsCount = player->inventory2.GetAmount(itemInstanceId);
        if (useFunc == Invalid || itemsCount <= 0) return FALSE;

        if (maxItems <= 0) maxItems = itemsCount;
        else maxItems = (itemsCount > maxItems) ? maxItems : itemsCount;
        if (maxItems <= 0) return FALSE;

        bool isConsumable = HasFlag(pItm->mainflag, item_kat_potions) || HasFlag(pItm->mainflag, item_kat_food);

        void* oldSelf = parser->GetSymbol("SELF")->GetInstanceAdr();
        par->SetInstance("SELF", player);
        par->SetInstance("ITEM", pItm);

        int usedItems = 0;
        do
        {
            par->CallFunc(useFunc);
            ++usedItems;
        } 
        while (usedItems < maxItems);

        if (isConsumable && (usedItems > 0))
            player->RemoveFromInv(itemInstanceId, usedItems);

        parser->SetInstance("SELF", oldSelf);
        return TRUE;
    }

    //-----------------------------------------------------------------
    //							 DEFINE API
    //-----------------------------------------------------------------

    void StonedExtension_DefineExternals()
    {
        parser->DefineExternal("StExt_Cmd", StExt_Cmd, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_FloatMult", StExt_FloatMult, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_FloatPow", StExt_FloatPow, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_IntPow", StExt_IntPow, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_FloatPowAsInt", StExt_FloatPowAsInt, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);        

        parser->DefineExternal("StExt_GetPercentBasedOnValue", StExt_GetPercentBasedOnValue, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_GetInstanceIdByName", StExt_GetInstanceIdByName, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetAuraData", StExt_GetAuraData, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UpdatePcStats", StExt_UpdatePcStats, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_SpawnNpcWithFunc", StExt_SpawnNpcWithFunc, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ForEachNpcInRadius", StExt_ForEachNpcInRadius, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_InfuseNpcWithMagic", StExt_InfuseNpcWithMagic, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_DisposeNpcInfusion", StExt_DisposeNpcInfusion, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        
        parser->DefineExternal("StExt_GetNpc", StExt_GetNpc, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Info_AddChoice", StExt_Info_AddChoice, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);    
        parser->DefineExternal("StExt_Info_BuildItemGeneratorPresetsChoices", StExt_Info_BuildItemGeneratorPresetsChoices, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_SetItemGeneratorConfigs", StExt_SetItemGeneratorConfigs, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ResetItemGeneratorConfigs", StExt_ResetItemGeneratorConfigs, zPAR_TYPE_VOID, zPAR_TYPE_VOID);  
        parser->DefineExternal("StExt_RegistrateConfigsPreset", StExt_RegistrateConfigsPreset, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetConfigsPreset", StExt_GetConfigsPreset, zPAR_TYPE_INSTANCE, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Info_BuildConfigPresetsChoices", StExt_Info_BuildConfigPresetsChoices, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ParsePresetName", StExt_ParsePresetName, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_InitializeExtraDamage", StExt_InitializeExtraDamage, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_InitializeDotDamage", StExt_InitializeDotDamage, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_InitializeReflectDamage", StExt_InitializeReflectDamage, zPAR_TYPE_VOID, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_ApplyExtraDamage", StExt_ApplyExtraDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ApplyDotDamage", StExt_ApplyDotDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ApplyReflectDamage", StExt_ApplyReflectDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_ApplyDamage", StExt_ApplyDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_TryCallFunc", StExt_TryCallFunc, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_OverrideFunc", StExt_OverrideFunc, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_OverrideDialog", StExt_OverrideDialog, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_OverrideConst", StExt_OverrideConst, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_RegtisterScriptPlugin", StExt_RegtisterScriptPlugin, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);     
        
        parser->DefineExternal("StExt_Struct_Sizeof", StExt_Struct_Sizeof, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Struct_Alloc", StExt_Struct_Alloc, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Struct_Free", StExt_Struct_Free, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Struct_IsEmpty", StExt_Struct_IsEmpty, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Struct_Read", StExt_Struct_Read, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Struct_Write", StExt_Struct_Write, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_StunPlayer", StExt_StunPlayer, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        
        parser->DefineExternal("StExt_GetTimedEffectsCount", GetTimedEffectsCount, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetTimedEffectByIndex", GetTimedEffectByIndex, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetTimedEffectByNpc", GetTimedEffectByNpc, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_CreateTimedEffect", CreateTimedEffect, zPAR_TYPE_INSTANCE, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        
        parser->DefineExternal("StExt_GetNpcByUid", StExt_GetNpcByUid, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetNextNpcUid", StExt_GetNextNpcUid, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_IsUidRegistered", StExt_IsUidRegistered, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_RegisterNpc", StExt_RegisterNpc, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GenerateRandomItem", StExt_CreateRandomItem, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetRegularItem", StExt_GetRegularItem, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_FindTargetInRadius", StExt_FindTargetInRadius, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UseEnchantedItem", StExt_UseEnchantedItem, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_StatValueToString", StExt_StatValueToString, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetItemById", StExt_GetItemById, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_DeleteTempItem", StExt_DeleteTempItem, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetItemNameById", StExt_GetItemNameById, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);        

        parser->DefineExternal("StExt_ThrowItem", StExt_ThrowItem, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ThrowItemEx", StExt_ThrowItemEx, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_TeleportToNpc", StExt_TeleportToNpc, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ValidateNpcPosition", StExt_ValidateNpcPosition, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_PrintMessageToTray", StExt_PrintMessageToTray, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_TryGetSymbolDescriptionText", StExt_TryGetSymbolDescriptionText, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        
        parser->DefineExternal("StExt_ProhibitWp", StExt_ProhibitWp, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_RegisterConfig", StExt_RegisterConfig, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ValidateConfigs", StExt_ValidateConfigs, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ValidateConfig", StExt_ValidateConfig, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ExportCurrentConfigs", StExt_ExportCurrentConfigs, zPAR_TYPE_INT, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_IsNpcInProhibitedPlace", StExt_IsNpcInProhibitedPlace, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_BuildEnchntedItemsSellForm", StExt_BuildEnchntedItemsSellForm, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetUndefinedItemsCount", StExt_GetUndefinedItemsCount, zPAR_TYPE_INT, zPAR_TYPE_VOID);   
        parser->DefineExternal("StExt_IdentifyAllItems", StExt_IdentifyAllItems, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_BuildItemsSellForm", StExt_BuildItemsSellForm, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        
        parser->DefineExternal("StExt_StartUncaper", StExt_StartUncaper, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UpdateUncaper", StExt_UpdateUncaper, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UpdateUncaperStat", StExt_UpdateUncaperStat, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetItemInstanceName", StExt_GetItemInstanceName, zPAR_TYPE_STRING, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_CopyNpcLook", StExt_CopyNpcLook, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UpdateTradeVars", StExt_UpdateTradeVars, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);     

        parser->DefineExternal("StExt_GetRandomWp", StExt_GetRandomWp, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetRandomEmptyChest", StExt_GetRandomEmptyChest, zPAR_TYPE_STRING, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_Player_ItemBatchUse", StExt_Player_ItemBatchUse, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);
    }

    //-----------------------------------------------------------------
    //							   HOOKS
    //-----------------------------------------------------------------

    HOOK Hook_oCNpc_Archive PATCH(&oCNpc::Archive, &oCNpc::Archive_StExt);
    void oCNpc::Archive_StExt(zCArchiver& ar)
    {
        THISCALL(Hook_oCNpc_Archive)(ar);
        if (!IsLevelChanging) RegisterNpc(this);
    }

    HOOK Hook_oCNpc_Unarchive PATCH(&oCNpc::Unarchive, &oCNpc::Unarchive_StExt);
    void oCNpc::Unarchive_StExt(zCArchiver& ar)
    {
        THISCALL(Hook_oCNpc_Unarchive)(ar);
        RegisterNpc(this);
        UnArchiveAdditionalArmors(this);
    }

    HOOK Hook_oCNpc_ProcessNpc PATCH(&oCNpc::ProcessNpc, &oCNpc::ProcessNpc_StExt);
    void oCNpc::ProcessNpc_StExt()
    {
        THISCALL(Hook_oCNpc_ProcessNpc)();

        if (this && !this->IsDead() && (StExt_OnAiStateFunc != Invalid))
        {
            parser->SetInstance("STEXT_SELF", this);
            parser->CallFunc(StExt_OnAiStateFunc);
        }
    }

    HOOK Hook_oCNpc_OpenInventory PATCH(&oCNpc::OpenInventory, &oCNpc::OpenInventory_StExt);
    void oCNpc::OpenInventory_StExt(int mode) 
    {
        if (this->HasBodyStateModifier(BS_MOD_BURNING)) {
            this->ModifyBodyState(0, BS_MOD_BURNING);
        }
        THISCALL(Hook_oCNpc_OpenInventory)(mode);
    }

    HOOK Hook_oCInformationManager_OnChoice PATCH(&oCInformationManager::OnChoice, &oCInformationManager::StExt_OnChoice);
    void __fastcall oCInformationManager::StExt_OnChoice(oCInfoChoice* pChoice)
    {
        if (pChoice)
            parser->GetSymbol("StExt_ChoiceName")->SetValue(pChoice->Text, 0);
        THISCALL(Hook_oCInformationManager_OnChoice)(pChoice);
    }

    HOOK Hook_oCItemContainer_GetValueMultiplier PATCH(&oCItemContainer::GetValueMultiplier, &oCItemContainer::GetValueMultiplier_StExt);
    float oCItemContainer::GetValueMultiplier_StExt()
    {
        if (ItemSellPriceMult < 0.01f)
            ItemSellPriceMult = parser->GetSymbol("trade_value_multiplier")->single_floatdata;
        float result = (ItemSellPriceMult >= 0.01f) ? ItemSellPriceMult : 0.1f;
        return result;
    }

    HOOK Hook_oCSpell_Cast PATCH(&oCSpell::Cast, &oCSpell::StExt_Cast);
    int oCSpell::StExt_Cast()
    {
        parser->CallFunc(StExt_OnSpellPreCastFunc);
        const int isCasted = THISCALL(Hook_oCSpell_Cast)();
        if (isCasted)
        {
            memset(&CurrentSpellInfo, 0, sizeof(CurrentSpellInfo));
            CurrentSpellInfo.SpellId = this->spellID;
            CurrentSpellInfo.SpellLevel = this->spellLevel;

            parser->SetInstance("STEXT_SPELLINFO", &CurrentSpellInfo);
            parser->CallFunc(StExt_OnSpellCastFunc);
            parser->SetInstance("STEXT_SPELLINFO", Null);
        }
        return isCasted;
    }

    HOOK Hook_oCSpell_Invest PATCH(&oCSpell::Invest, &oCSpell::StExt_Invest);
    int oCSpell::StExt_Invest()
    {
        parser->CallFunc(StExt_OnSpellPreCastFunc);
        return THISCALL(Hook_oCSpell_Cast)();
    }

}
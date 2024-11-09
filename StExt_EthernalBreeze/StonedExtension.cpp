#include <UnionAfx.h>
#include <string> 
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    std::map<int, oCNpc*> RegisteredNpcs = {};
    std::map<int, C_AuraData*> AuraData = {};
    Array<C_MagicInfusionData*> InfusionData_Afixes = {};
    Array<C_MagicInfusionData*> InfusionData_Suffixes = {};
    Array<C_MagicInfusionData*> InfusionData_Preffixes = {};
    Array<WaypointData> ProhibitedWaypoints = {};
    TimerBlock FpsTimer;
    int FpsCounter;

    string GetSlotNameByID(int ID)
    {
        if (ID > 0)
            return "savegame" + A ID;
        if (ID == 0)
            return "quicksave";
        return "current";
    }

    void ClearRegisteredNpcs() 
    { 
        RegisteredNpcs.clear();
        //RegisteredNpcs = std::map<int, oCNpc*>();        
    }

    int ValidateIntValue(int value, int min, int max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    int GetRandomRange(int min, int max) 
    { 
        if ((min == 0) && (max == 0)) return 0;
        if (max > min)
        {
            int dif = max - min;
            return (rand() % (dif + 1)) + min;
        }
        return rand() % min;
    }

    float GetRandomFloatRange(float value) 
    {
        std::uniform_real_distribution<float> distribution(-value, value);
        std::random_device rd;
        std::mt19937 generator(rd());
        return distribution(generator);
    }

    std::vector<int> CreateUniqKeysArray(int size, int maxValue) 
    {
        std::vector<int> numbers;
        for (int i = 0; i < maxValue; ++i) 
            numbers.push_back(i);
        
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(numbers.begin(), numbers.end(), g);

        size = size > maxValue ? maxValue : size;
        numbers.resize(size);
        return numbers;
    }

    void C_AuraData::SetByScript(int index) { parser->CreateInstance(index, this); }
    void C_MagicInfusionData::SetByScript(int index) { parser->CreateInstance(index, this); }

    C_AuraData* GetAuraById(int id)
    {
        C_AuraData* aura = Null;
        try { aura = AuraData.at(id); }
        catch (const std::out_of_range& e)
        {
            DEBUG_MSG("GetAuraById - incorrect id: " + Z id + "!");
        }
        return aura;
    }

    C_MagicInfusionData* GetRandomInfusion(int type, int tier, int grantInfusion)
    {
        int chance = (rand() % 100) + (tier * 5);
        if (!grantInfusion && (chance < 50))
            return Null;

        int indx = 0;
        if (type == StExt_InfusionType_Affix)
        {
            indx = rand() % InfusionData_Afixes.GetNum();
            return InfusionData_Afixes[indx];
        }
        if (type == StExt_InfusionType_Preffix)
        {
            indx = rand() % InfusionData_Preffixes.GetNum();
            return InfusionData_Preffixes[indx];
        }
        if (type == StExt_InfusionType_Suffix)
        {
            indx = rand() % InfusionData_Suffixes.GetNum();
            return InfusionData_Suffixes[indx];
        }
        return Null;
    }

    void StonedExtension_Loop()
    {
        if (!ogame || !player) return;
        StonedExtension_Loop_StatMenu();
        StonedExtension_MsgTray_Loop();

        FpsCounter += 1;
        if (FpsTimer.AwaitExact(1000))
        {
            //DEBUG_MSG("Curernt fps: " + Z FpsCounter);
            parser->GetSymbol("StExt_Fps")->SetValue(FpsCounter, 0);
            FpsCounter = 0;
        }        
    }

    int GetNextNpcUid()
    {
        //DEBUG_MSG("GetNextNpcUid...");
        zCPar_Symbol* ps = parser->GetSymbol("StExt_NpcUidCounter");
        int result = ps->single_intdata + 1;
        while (IsUidRegistered(result)) result += 1;
        ps->SetValue(result, 0);
        //DEBUG_MSG("GetNextNpcUid - " + Z result);
        return result;
    }

    void RegisterNpc(oCNpc* npc, int npcUid)
    {
        if (!npc)
        {
            DEBUG_MSG("RegisterNpc - Try to register null Npc!");
            return;
        }
        if (npcUid <= 0)
        {
            DEBUG_MSG("StExt_RegisterNpc - Try to register incorrect index to Npc!");
            npcUid = GetNextNpcUid();
        }

        if (RegisteredNpcs.find(npcUid) == RegisteredNpcs.end())
        {
            RegisteredNpcs.insert({ npcUid, npc });
            DEBUG_MSG("RegisterNpc - UId for npc '" + Z npc->name + "' [" + Z npcUid + "] Done!");
        }
        else
        {
            //DEBUG_MSG("RegisterNpc - Cast to oCNpcEx...");
            oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
            if (npcEx)
            {
                if (npcEx->m_pVARS[StExt_AiVar_Uid] == npcUid) return;

                DEBUG_MSG("RegisterNpc - Detected UId's doubling!");
                int newUid = GetNextNpcUid();
                npcEx->m_pVARS[StExt_AiVar_Uid] = newUid;
                RegisteredNpcs.insert({ newUid, npc });
                DEBUG_MSG("RegisterNpc - Change UId for Npc '" + Z npc->name + "' [" + Z npcUid + " -> " + Z newUid + "]");
                UpdateTimedEffectsNpcUids(npcUid, newUid);
            }
            else
                DEBUG_MSG("RegisterNpc - can't cast Npc to oCNpcEx!");
        }
    }

    oCNpc* GetNpcByUid(int npcUid)
    {
        DEBUG_MSG("GetNpcByUid from storage by id " + Z npcUid);
        if (npcUid > 0)
        {            
            oCNpc* result = Null;
            try { result = RegisteredNpcs.at(npcUid); }
            catch (const std::out_of_range& e) { DEBUG_MSG("GetNpcByUid - npc with id: " + Z npcUid + " not found!"); }
            return result;
        }            
        return Null;
    }

    bool IsUidRegistered(int npcUid)
    {
        if (npcUid <= 0) return true;
        if (RegisteredNpcs.find(npcUid) != RegisteredNpcs.end())
            return true;
        return false;
    }

    int GetNpcUid(oCNpc* npc)
    {
        //DEBUG_MSG("GetNpcUid...");
        if (npc)
        {
            int idx = parser->GetIndex("StExt_NpcToUid");
            parser->SetInstance("StExt_Self", npc);
            int uid = *(int*)parser->CallFunc(idx);
            return uid;
        }
        return Invalid;
    }

    void RegisterNpc(oCNpc* npc)
    {
        if (npc)
        {
            int uid = GetNpcUid(npc);
            RegisterNpc(npc, uid);
        }
    }

    // Script API

    zCArray<oCNpc*> GetNpcInRadius(oCNpc* center, float radius)
    {
        center->ClearVobList();
        center->CreateVobList(radius);
        zCArray<zCVob*> vobList = center->vobList;
        zCArray<oCNpc*> npcList;
        zCVob* pVob = NULL;
        oCNpc* npc = NULL;

        for (int i = 0; i < vobList.GetNum(); i++)
        {
            pVob = vobList.GetSafe(i);
            if (!pVob)
                continue;
            npc = zDYNAMIC_CAST<oCNpc>(pVob);
            if (!npc)
                continue;
            if (npc->IsDead())
                continue;
            npcList.Insert(npc);
        }
        return npcList;
    }

    int __cdecl StExt_GetAuraData()
    {
        zCParser* par = zCParser::GetParser();
        int index;
        parser->GetParameter(index);        
        par->SetReturn(GetAuraById(index));
        return true;
    }


    //      Misk funcs
    int __cdecl StExt_Cmd()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING message;
        par->GetParameter(message);
        PrintDebug(message);
        return true;
    }

    int __cdecl StExt_GetInstanceIdByName()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING instName;
        par->GetParameter(instName);
        int index = (instName.IsEmpty() || instName == "") ? Invalid : par->GetIndex(instName);
        if(index == Invalid)
            DEBUG_MSG("Instance '" + instName + "' not found!");
        par->SetReturn(index);
        return true;
    }

    int __cdecl StExt_FloatMult()
    {
        zCParser* par = zCParser::GetParser();
        float m1, m2, result;
        par->GetParameter(m2);
        par->GetParameter(m1);
        result = m1 * m2;
        par->SetReturn(result);
        return true;
    }

    int __cdecl StExt_GetPercentBasedOnValue()
    {
        zCParser* par = zCParser::GetParser();
        float m1, m2, result;
        int val, max;
        par->GetParameter(max);
        par->GetParameter(val);
        result = 0;
        if (max != 0 && val != 0)
        {
            m1 = (float)val;
            m2 = (float)max;
            result = (m1 / m2) * 100;
        }
        par->SetReturn((int)result);
        return true;
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
        zCPar_Symbol* StExt_ArtifactEquipped = parser->GetSymbol("StExt_ArtifactEquipped");

        int size = parser->GetSymbol("StExt_PcStats_Index_Max")->single_intdata;
        for (int i = 0; i < size; i++)
        {
            StExt_PcStatsArray->intdata[i] = StExt_PcStats_PermArray->intdata[i] + StExt_PcStats_ItemsArray->intdata[i] +
                StExt_PcStats_TalismansArray->intdata[i] + StExt_PcStats_DynamicArray->intdata[i] + StExt_PcStats_OtherArray->intdata[i] +
                StExt_PcStats_AlchemyArray->intdata[i] + StExt_PcStats_BuffsArray->intdata[i] + StExt_PcStats_AurasArray->intdata[i];

            if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Grimoir)
                StExt_PcStatsArray->intdata[i] += StExt_PcStats_GrimoirArray->intdata[i];
            else if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Dagger)
                StExt_PcStatsArray->intdata[i] += StExt_PcStats_DaggerArray->intdata[i];
        }
        return true;
    }

    int StExt_Info_AddChoice()
    {
        zCParser* par = zCParser::GetParser();
        int menu, action;
        zSTRING text, actionName;
        par->GetParameter(actionName);
        par->GetParameter(text);
        par->GetParameter(menu);

        action = par->GetIndex(actionName);
        if (action == -1) 
            return 0;
        oCInfo* pInfo = ogame->GetInfoManager()->GetInformation(menu);
        if (pInfo)
            pInfo->AddChoice(text, action);
        return 0;
    }

    int StExt_TryCallFunc()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING funcName;
        int funcId;
        par->GetParameter(funcName);
        funcId = par->GetIndex(funcName);
        if (funcId == Invalid)
        {
            par->SetReturn(false);
            return false;
        }
        parser->CallFunc(funcId);
        par->SetReturn(true);
        return true;
    }

    //      Npc Funcs

    // (instance, count, wp, initFunc)
    int StExt_SpawnNpcWithFunc()
    {
        zCParser* par = zCParser::GetParser();
        int count, instance, initFuncIndx;
        zSTRING initFunc, point;
        par->GetParameter(initFunc);
        par->GetParameter(point);
        par->GetParameter(count);
        par->GetParameter(instance);

        initFuncIndx = initFunc == "" ? -1 : parser->GetIndex(initFunc);
        for (int i = 0; i < count; i++)
        {
            oCNpc* pNpc = ogame->GetSpawnManager()->SpawnNpc(instance, point, 0);
            if (!pNpc)
                return 0;
            pNpc->respawnOn = FALSE;
            rtnMan->UpdateSingleRoutine(pNpc);
            if (initFuncIndx > 0)
            {
                parser->SetInstance("StExt_Self", pNpc);
                par->CallFunc(initFuncIndx);
            }
        }
        return 0;
    }

    // (center, radius, initFunc, condFunc, execFunc)
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

        initFuncIndx = par->GetIndex(initFunc);
        condFuncIndx = par->GetIndex(condFunc);
        execFuncIndx = par->GetIndex(execFunc);
        DEBUG_MSG("StExt_ForEachNpcInRadius-> for(i = '" + initFunc + "'; cond = '" + condFunc + "'; func = '" + execFunc + "')");
        if (!center || (radius <= 0) || (execFuncIndx == Invalid))
        {
            DEBUG_MSG("StExt_ForEachNpcInRadius -> can't execute radius func...");
            return false;
        }

        zCArray<oCNpc*> npcArray = GetNpcInRadius(center, radius);
        if (npcArray.GetNum() <= 0)
        {
            DEBUG_MSG("StExt_ForEachNpcInRadius -> npc's not found...");
            return false;
        }
        parser->SetInstance("StExt_Other", center);
        if (initFuncIndx != Invalid)
            par->CallFunc(initFuncIndx, center->GetInstance());

        int condResult;
        for (int i = 0; i < npcArray.GetNum(); i++)
        {
            if (npcArray[i] == Null) continue;
            parser->SetInstance("StExt_Self", npcArray[i]);
            
            //DEBUG_MSG("StExt_ForEachNpcInRadius -> npc - " + Z npcArray[i]->name);
            if (condFuncIndx != Invalid)
            {
                condResult = *(int*)par->CallFunc(condFuncIndx);
                if (condResult)
                    par->CallFunc(execFuncIndx);
            }
            else 
                par->CallFunc(execFuncIndx);
        }
    }

    // (center, radius, condFunc)
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
            return false;
        }

        zCArray<oCNpc*> npcArray = GetNpcInRadius(center, radius);
        if (npcArray.GetNum() <= 0) return false;

        for (int i = 0; i < npcArray.GetNum(); i++)
        {
            if (npcArray[i] == Null) continue;
            parser->SetInstance("StExt_Self", npcArray[i]);
            int condResult = *(int*)par->CallFunc(condFuncIndx);
            if (condResult)
            {
                parser->SetInstance("StExt_Other", npcArray[i]);
                parser->SetReturn(true);
                return false;
            };
        }
    }

    int __cdecl StExt_GetNpc()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* npc = (oCNpc*)par->GetInstance();
        par->SetReturn(npc);
        return true;
    }

    int __cdecl StExt_InfuseNpcWithMagic()
    {
        zCParser* par = zCParser::GetParser();
        int tier;
        par->GetParameter(tier);
        oCNpc* pNpc = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        if (!pNpc)
        {
            cmd << "StExt_InfuseNpcWithMagic - npc is null!" << "\n";
            return false;
        }

        tier = ValidateIntValue(tier, 1, 5);
        C_MagicInfusionData* infusion = CreateInfusion(tier);
        par->SetReturn(infusion);
        return 0;
    }


    //          Damage Funcs

    int __cdecl StExt_InitializeExtraDamage()
    {
        zCParser* par = zCParser::GetParser();
        ExtraDamage = CExtraDamage();
        memset(&ExtraDamage, 0, sizeof ExtraDamage);
        par->SetInstance("StExt_ExtraDamageInfo", &ExtraDamage);
        return true;
    }

    int __cdecl StExt_InitializeDotDamage()
    {
        zCParser* par = zCParser::GetParser();
        DotDamage = CDotDamage();
        memset(&DotDamage, 0, sizeof DotDamage);
        par->SetInstance("StExt_DotDamageInfo", &DotDamage);
        return true;
    }

    int __cdecl StExt_InitializeReflectDamage()
    {
        zCParser* par = zCParser::GetParser();
        ReflectDamage = CExtraDamage();
        memset(&ReflectDamage, 0, sizeof ReflectDamage);
        par->SetInstance("StExt_ReflectDamageInfo", &ReflectDamage);
        return true;
    }

    int __cdecl StExt_ApplyDotDamage()
    {
        DEBUG_MSG("StExt_ApplyDotDamage - call");
        zCParser* par = zCParser::GetParser();
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();
        ApplyDotDamage(atk, target);
        return true;
    }

    int __cdecl StExt_ApplyExtraDamage()
    {
        DEBUG_MSG("StExt_ApplyExtraDamage - call");
        zCParser* par = zCParser::GetParser();
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();
        ApplyExtraDamage(atk, target);
        return true;
    }

    int __cdecl StExt_ApplyReflectDamage()
    {
        DEBUG_MSG("StExt_ApplyReflectDamage - call");
        zCParser* par = zCParser::GetParser();
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();
        ApplyReflectDamage(atk, target);
        return true;
    }

    int __cdecl StExt_ApplyDamage()
    {
        DEBUG_MSG("StExt_ApplyDamage - call");
        zCParser* par = zCParser::GetParser();
        int damType, damage;
        par->GetParameter(damType);
        par->GetParameter(damage);
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();
        
        if ((!atk) || (!target)) return false;
        if (damage <= 0) return false;
        if (damType == 0) return false;

        oCNpc::oSDamageDescriptor desc = oCNpc::oSDamageDescriptor();
        memset(&desc, 0, sizeof oCNpc::oSDamageDescriptor);

        ApplyDamages(damType, desc.aryDamage, damage);
        desc.enuModeDamage = damType;
        desc.fDamageMultiplier = 1.0;
        desc.vecLocationHit = target->GetPositionWorld();
        desc.pNpcAttacker = atk;
        desc.pVobAttacker = atk;
        desc.bOnce = true;
        desc.dwFieldsValid |= DamageDescFlag_ExtraDamage;
        desc.pVobHit = target;

        DEBUG_MSG("StExt_ApplyDamage call");
        target->OnDamage(desc);
        return true;
    }

    int __cdecl StExt_StunPlayer()
    {
        zCParser* par = zCParser::GetParser();
        int stun = false;
        par->GetParameter(stun);
        if (player)
            player->SetMovLock(stun);
        return true;
    }

    int __cdecl StExt_RegisterNpc()
    {
        zCParser* par = zCParser::GetParser();
        int npcUid;
        par->GetParameter(npcUid);        
        oCNpc* npc = (oCNpc*)par->GetInstance();
        if (!npc || (npcUid <= 0))
        {
            if (!npc)
            {
                DEBUG_MSG("StExt_RegisterNpc - Try to register null npc!");
                return false;
            }
            if (npcUid <= 0)
            {
                DEBUG_MSG("StExt_RegisterNpc - Try to register incorrect index to npc! Index: " + Z npcUid);
                npcUid = GetNextNpcUid();
            }
        }
        RegisterNpc(npc, npcUid);
        return true;
    }

    int __cdecl StExt_GetNpcByUid()
    {
        zCParser* par = zCParser::GetParser();
        int npcUid;
        par->GetParameter(npcUid);
        
        if (npcUid <= 0)
        {
            par->SetReturn(Null);
            return false;
        }

        oCNpc* npc = GetNpcByUid(npcUid);
        par->SetReturn(npc);
        return true;
    }

    int __cdecl StExt_IsUidRegistered()
    {
        zCParser* par = zCParser::GetParser();
        int npcUid, result;
        par->GetParameter(npcUid);
        result = IsUidRegistered(npcUid);
        par->SetReturn(result);
        return true;
    }

    int __cdecl StExt_GetNextNpcUid()
    {
        zCParser* par = zCParser::GetParser();
        int result = GetNextNpcUid();
        par->SetReturn(result);
        return true;
    }

    int __cdecl StExt_CreateRandomItem()
    {
        zCParser* par = zCParser::GetParser();
        int type, power;
        par->GetParameter(power);
        par->GetParameter(type);        
        int newInstance = GenerateNewItem(type, power);
        par->SetReturn(newInstance);
        return true;
    }

    int __cdecl StExt_GetRegularItem()
    {
        zCParser* par = zCParser::GetParser();
        int type, power, itemInstnce, instanceId;
        par->GetParameter(power);
        par->GetParameter(type);

        power = power * 0.75f;
        if (power <= 0) power = 1;

        zSTRING instName;
        instName = GetRandomPrototype(type, power);
        instanceId = parser->GetIndex(instName);

        par->SetReturn(instanceId);
        return true;
    }

    int __cdecl StExt_UseEnchantedItem()
    {
        zCParser* par = zCParser::GetParser();        
        zCPar_Symbol* ps = parser->GetSymbol("ITEM");

        oCItem* item = dynamic_cast<oCItem*>((zCVob*)ps->GetInstanceAdr());
        if (!item)
        {
            DEBUG_MSG("StExt_UseEnchantedItem -> item is null");
            return false;
        }            

        C_EnchantmentData* enchantment = GetEnchantmentData(item);
        if(!enchantment)
        {
            DEBUG_MSG("StExt_UseEnchantedItem -> enchantment is null");
            return false;
        }

        if (HasFlag(enchantment->Type, ItemType_Scroll) || HasFlag(enchantment->Type, ItemType_Potion))
        {
            int useFuncIndx = par->GetIndex("StExt_ApplyPotionEffect");
            for (int i = 0; i < EnchantStatsMax; i++)
            {
                if (enchantment->StatId[i] == Invalid)
                    continue;
                par->CallFunc(useFuncIndx, enchantment->StatId[i], enchantment->StatValue[i], enchantment->StatDuration[i]);
            }
        }
        return true;
    }

    int __cdecl StExt_StatValueToString()
    {
        zCParser* par = zCParser::GetParser();
        int statId, statVal, valType;
        zSTRING result;
        par->GetParameter(statVal);
        par->GetParameter(statId);

        valType = StatsTypeMap[statId];
        if (valType == Value_Type_YesNo) result = IntValueToYesNo(statVal);
        else if (valType == Value_Type_Percent) result = GetFloatStringFromInt(statVal);
        else if (valType == Value_Type_DefaultPerc) result = zSTRING(statVal) + "%";
        else if (valType == Value_Type_InvertPerc) result = zSTRING(statVal * 10) + "%";
        else result = zSTRING(statVal);
        
        par->GetSymbol("StExt_ReturnString")->SetValue(result, 0);
        par->SetReturn(result);
        return true;
    }

    int __cdecl StExt_GetItemById()
    {
        zCParser* par = zCParser::GetParser();
        int instanceId;
        oCItem* pItem;
        par->GetParameter(instanceId);

        pItem = dynamic_cast<oCItem*>(ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, instanceId));
        par->SetReturn(pItem);
        return true;
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
            return false;
        }

        int instanceId = par->GetIndex(throwInstanceName);
        if (instanceId == Invalid)
        {
            DEBUG_MSG("StExt_ThrowItem - throwable item not found!");
            return false;
        }

        ThrowItemDescriptor throwData = ThrowItemDescriptor();
        throwData.attacker = attaker;
        throwData.target = target;
        throwData.throwInstance = instanceId;
        throwData.damage = damage < 5 ? 5 : damage;
        throwData.damageType = damageType <= 0 ? dam_barrier : damageType;
        
        ThrowItem(throwData);
        return true;
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
            return false;
        }

        if (throwInstance == Invalid)
        {
            DEBUG_MSG("StExt_ThrowItem - throwable item not found!");
            return false;
        }

        ThrowItemDescriptor throwData = ThrowItemDescriptor();
        throwData.attacker = attaker;
        throwData.target = target;
        throwData.throwInstance = throwInstance;
        throwData.damage = damage < 5 ? 5 : damage;
        throwData.damageType = damageType <= 0 ? dam_barrier : damageType;

        ThrowItem(throwData);
        return true;
    }

    bool NpcPositionIsInvalid(oCNpc* npc)
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

    zVEC3 FindCorrectPosition(oCNpc* npc)
    {
        zVEC3 newPos = npc->GetPositionWorld();
        npc->SetOnFloor(newPos);
        npc->SearchNpcPosition(newPos);
        return newPos;
    }

    int __cdecl StExt_ValidateNpcPosition()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* npc = (oCNpc*)par->GetInstance();
        if (!npc)
            return false;

        if (NpcPositionIsInvalid(npc))
        {
            DEBUG_MSG("StExt_ValidateNpcPosition " + Z npc->name + " - fix npc position!");
            zVEC3 pos;
            npc->SetCollDet(FALSE);
            pos = FindCorrectPosition(npc);
            npc->SetPositionWorld(pos);
            npc->SetCollDet(TRUE);
            npc->Enable(npc->GetPositionWorld());
            ogame->GetSpawnManager()->SpawnImmediately(FALSE);
            return true;
        }
        return false;
    }

    int __cdecl StExt_TeleportToNpc()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* target = (oCNpc*)par->GetInstance();
        oCNpc* atk = (oCNpc*)par->GetInstance();

        if (!atk && !target)
        {
            DEBUG_MSG("StExt_TeleportToNpc - one of npc is null!");
            return false;
        }

        zVEC3 pos;
        zMAT4 trafo = target->trafoObjToWorld;
        pos = target->GetPositionWorld() + (target->GetAtVectorWorld() * 200.0);

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
        return true;
    }

    int __cdecl StExt_PrintMessageToTray()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING text, color;
        par->GetParameter(color);
        par->GetParameter(text);
        MsgTray_AddEntry(text, color);
        return true;
    }

    int StExt_TryGetSymbolDescriptionText()
    {
        static zSTRING result, optionName;
        zCParser* par = zCParser::GetParser();

        result = zSTRING();
        par->GetParameter(optionName);
        
        if (!optionName.IsEmpty() && optionName != "")
        {
            std::string tmp = optionName.ToChar();
            std::string toReplace = "StExt_";
            std::string replacement = "StExt_Str_";

            size_t pos = tmp.find(toReplace);
            if (pos != std::string::npos)
            {
                tmp.replace(pos, toReplace.length(), replacement);
                result = Z tmp.c_str();
            }
        }

        par->SetReturn(result);
        return 0;
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
        ProhibitedWaypoints.Insert(wp);
        return true;
    }
    
    int __cdecl StExt_IsNpcInProhibitedPlace()
    {
        zCParser* par = zCParser::GetParser();
        oCNpc* npc = (oCNpc*)par->GetInstance();

        if (!npc)
        {
            DEBUG_MSG("StExt_IsNpcInProhibitedPlace - npc is null!");
            parser->SetReturn(true);
            return false;
        }

        for (int i = 0; i < ProhibitedWaypoints.GetNum(); i++)
        {
            zCWaypoint* pWp = ogame->GetWorld()->wayNet->GetWaypoint(ProhibitedWaypoints[i].Wp);
            int dist;
            if (pWp)
            {
                dist = (int)(npc->GetPositionWorld() - pWp->GetPositionWorld()).LengthApprox();
                if (dist < ProhibitedWaypoints[i].Radius)
                {
                    parser->SetReturn(true);
                    return true;
                }
            }
        }
       
        parser->SetReturn(false);
        return true;
    }

    CraftData BuildCraftData()
    {
        CraftData craftData = CraftData();
        memset(&craftData, 0, sizeof craftData);
        return craftData;
    }

    int __cdecl StExt_BuildEnchntedItemsSellForm()
    {
        zCParser* par = zCParser::GetParser();
        int types = 0;
        par->GetParameter(types);
        auto enchantedItems = GetEnchantedItems(types);

        int callBackFunc = par->GetIndex("StExt_BuildEnchntedItemsSellForm_BuildLoop");
        int size = enchantedItems.GetNum();
        DEBUG_MSG("StExt_BuildEnchntedItemsSellForm - items count: " + Z size);
        if (size == 0 || callBackFunc == Invalid) return false;

        for (int i = 0; i < size; i++)
        {
            CraftData craftData = BuildCraftData();
            craftData.Price = enchantedItems[i]->value;
            craftData.ResultInstance = enchantedItems[i]->GetInstanceName();
            par->SetInstance("StExt_CraftInfo", &craftData);
            par->CallFunc(callBackFunc);
        }
        return true;
    }

    //GetUndefinedItems()
    int __cdecl StExt_GetUndefinedItemsCount()
    {
        zCParser* par = zCParser::GetParser();
        auto itms = GetUndefinedItems();
        int size = itms.GetNum();
        par->SetReturn(size);
        return true;
    }

    int __cdecl StExt_IdentifyAllItems()
    {
        auto itms = GetUndefinedItems();
        int size = itms.GetNum();
        if (size <= 0) return false;

        for (int i = 0; i < size; i++)
        {
            if (itms[i])
                IdentifyItem(itms[i]->GetInstance(), itms[i]);
        }
        return true;
    }

    int __cdecl StExt_StartUncaper()
    {
        StartUncaper();
        return true;
    }

    int __cdecl StExt_UpdateUncaper()
    {
        StatsUncaperLoop();
        return true;
    }

    int __cdecl StExt_UpdateUncaperStat()
    {
        zCParser* par = zCParser::GetParser();
        int index = 0;
        par->GetParameter(index);
        UpdateUncapedStat(index);
        return true;
    }

    int __cdecl StExt_GetItemInstanceName()
    {
        zCParser* par = zCParser::GetParser();
        oCItem* itm = (oCItem*)par->GetInstance();
        zSTRING itmInstanceName = zSTRING();
        if (!itm)
        {
            DEBUG_MSG("StExt_GetItemInstanceName - itm is null");
            par->SetReturn(itmInstanceName);
            return false;
        }
        itmInstanceName = itm->GetInstanceName();
        //DEBUG_MSG("StExt_GetItemInstanceName - name: " + itmInstanceName);
        par->GetSymbol("StExt_ReturnString")->SetValue(itmInstanceName, 0);
        par->SetReturn(itmInstanceName);
        return true;
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
            return false;
        }

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
        return true;
    }

    void StonedExtension_DefineExternals()
    {
        parser->DefineExternal("StExt_Cmd", StExt_Cmd, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_FloatMult", StExt_FloatMult, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_FLOAT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetPercentBasedOnValue", StExt_GetPercentBasedOnValue, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_GetInstanceIdByName", StExt_GetInstanceIdByName, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetAuraData", StExt_GetAuraData, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UpdatePcStats", StExt_UpdatePcStats, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_SpawnNpcWithFunc", StExt_SpawnNpcWithFunc, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ForEachNpcInRadius", StExt_ForEachNpcInRadius, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_InfuseNpcWithMagic", StExt_InfuseNpcWithMagic, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetNpc", StExt_GetNpc, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_Info_AddChoice", StExt_Info_AddChoice, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_InitializeExtraDamage", StExt_InitializeExtraDamage, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_InitializeDotDamage", StExt_InitializeDotDamage, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_InitializeReflectDamage", StExt_InitializeReflectDamage, zPAR_TYPE_VOID, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_ApplyExtraDamage", StExt_ApplyExtraDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ApplyDotDamage", StExt_ApplyDotDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ApplyReflectDamage", StExt_ApplyReflectDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);        
        parser->DefineExternal("StExt_ApplyDamage", StExt_ApplyDamage, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_TryCallFunc", StExt_TryCallFunc, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_StunPlayer", StExt_StunPlayer, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetTimedEffectsCount", GetTimedEffectsCount, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetTimedEffectByIndex", GetTimedEffectByIndex, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetTimedEffectByNpc", GetTimedEffectByNpc, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_CreateTimedEffect", CreateTimedEffect, zPAR_TYPE_INSTANCE, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetNpcByUid", StExt_GetNpcByUid, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetNextNpcUid", StExt_GetNextNpcUid, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_IsUidRegistered", StExt_IsUidRegistered, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_RegisterNpc", StExt_RegisterNpc, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GenerateRandomItem", StExt_CreateRandomItem, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetRegularItem", StExt_GetRegularItem, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_FindTargetInRadius", StExt_FindTargetInRadius, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UseEnchantedItem", StExt_UseEnchantedItem, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_StatValueToString", StExt_StatValueToString, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetItemById", StExt_GetItemById, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ThrowItem", StExt_ThrowItem, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ThrowItemEx", StExt_ThrowItemEx, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_TeleportToNpc", StExt_TeleportToNpc, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_ValidateNpcPosition", StExt_ValidateNpcPosition, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_PrintMessageToTray", StExt_PrintMessageToTray, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_TryGetSymbolDescriptionText", StExt_TryGetSymbolDescriptionText, zPAR_TYPE_STRING, zPAR_TYPE_STRING, zPAR_TYPE_VOID);
        
        parser->DefineExternal("StExt_ProhibitWp", StExt_ProhibitWp, zPAR_TYPE_VOID, zPAR_TYPE_STRING, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_IsNpcInProhibitedPlace", StExt_IsNpcInProhibitedPlace, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_BuildEnchntedItemsSellForm", StExt_BuildEnchntedItemsSellForm, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetUndefinedItemsCount", StExt_GetUndefinedItemsCount, zPAR_TYPE_INT, zPAR_TYPE_VOID);   
        parser->DefineExternal("StExt_IdentifyAllItems", StExt_IdentifyAllItems, zPAR_TYPE_VOID, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_StartUncaper", StExt_StartUncaper, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UpdateUncaper", StExt_UpdateUncaper, zPAR_TYPE_VOID, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_UpdateUncaperStat", StExt_UpdateUncaperStat, zPAR_TYPE_VOID, zPAR_TYPE_INT, zPAR_TYPE_VOID);
        parser->DefineExternal("StExt_GetItemInstanceName", StExt_GetItemInstanceName, zPAR_TYPE_STRING, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);

        parser->DefineExternal("StExt_CopyNpcLook", StExt_CopyNpcLook, zPAR_TYPE_INT, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, zPAR_TYPE_VOID);
    }


    HOOK Hook_oCNpc_Archive PATCH(&oCNpc::Archive, &oCNpc::Archive_StExt);
    void oCNpc::Archive_StExt(zCArchiver& ar)
    {
        THISCALL(Hook_oCNpc_Archive)(ar);
        RegisterNpc(this);
    }

    HOOK Hook_oCNpc_Unarchive PATCH(&oCNpc::Unarchive, &oCNpc::Unarchive_StExt);
    void oCNpc::Unarchive_StExt(zCArchiver& ar)
    {
        THISCALL(Hook_oCNpc_Unarchive)(ar);
        RegisterNpc(this);
        UnArchiveAdditionalArmors(this);
    }

    HOOK Ivk_zCParser_CreatePCode PATCH(&zCParser::CreatePCode, &zCParser::CreatePCode_StExt);
    void zCParser::CreatePCode_StExt() 
    {
        if (parser && this == parser)
            C_TimedEffect_RegisterClassOffset();

        THISCALL(Ivk_zCParser_CreatePCode)();
    }

    HOOK Hook_oCNpc_ProcessNpc PATCH(&oCNpc::ProcessNpc, &oCNpc::ProcessNpc_StExt);
    void oCNpc::ProcessNpc_StExt()
    {
        THISCALL(Hook_oCNpc_ProcessNpc)();

        if (this && !this->IsDead())
        {
            int indx = parser->GetIndex("StExt_OnAiState");
            parser->SetInstance("StExt_Self", this);
            parser->CallFunc(indx);
        }
    }

    /*
#ifdef DebugStackEnabled

    HOOK Ivk_zCParser_CallFuncStr PATCH(&zCParser::CallFunc, &zCParser::CallFuncStr_StExt);
    void* zCParser::CallFuncStr_StExt(zSTRING const& str)
    {
        DebugMessage("Parser: Call function name - " + str);
        return THISCALL(Ivk_zCParser_CallFuncStr)(str);
    }

    HOOK Ivk_zCParser_CallFunc PATCH(&zCParser::CallFunc, &zCParser::CallFunc_StExt);
    void* __cdecl zCParser::CallFunc_StExt(int id, ...)
    {
        va_list args;
        va_start(args, id);
        zSTRING str = parser->GetSymbolInfo(id)->name;
        DebugMessage("Parser: Call function name (" + Z id + ")" + str);
        return THISCALL(Ivk_zCParser_CallFunc)(id, args);
        //void* result = THISCALL(Ivk_zCParser_CallFunc)(id, args);
        //va_end(args);
        //return result;
    }

#endif
    */
}
#include "C_TimedEffect.h"

namespace Gothic_II_Addon
{
    static Timer triggerTimer;
    static const int NoNpc = 0;
    static int AfterLoadDelay;

    C_TimedEffect::C_TimedEffect()
    {
        Self = Null;
        Other = Null;
        ZeroMemory(&Parser, sizeof(Parser));
        Parser.Enabled = True;
        TimedEffectScripts.InsertSorted(this);
    }
    C_TimedEffect::~C_TimedEffect() { TimedEffectScripts.RemoveSorted(this); }

    zSTRING GetDebugLine(C_TimedEffect* effect)
    {
        zSTRING result = "";
        if (effect)
        {
            zSTRING slfName = effect->Self ? effect->Self->name : Z "???";
            zSTRING slfIndx = effect->Parser.SelfNpcUid == NoNpc ? "NoUid" : Z effect->Parser.SelfNpcUid;
            zSTRING funcName = effect->FunctionName;
            zSTRING funcIndx = Z effect->Function;
            result = "TimedEffect for npc " + slfName + " [" + slfIndx +
                "] | Func: " + funcName + " (" + funcIndx + ")";
            return result;
        }
        
        return "GetDebugLine -> TimedEffect is null!";
    }

    bool C_TimedEffect::CallTrigger()
    {
        if (Parser.SelfNpcUid <= NoNpc)
        {
            DEBUG_MSG(GetDebugLine(this) + " - No self npc uid!");
            return true;
        }
        if (!Self)
        {
            Self = GetNpcByUid(Parser.SelfNpcUid);
            if (!Self)
            {
                DEBUG_MSG(GetDebugLine(this) + " - Self npc instance not found!");
                return true;
            }
        }

        if ((Parser.OtherNpcUid > NoNpc) && (!Other))
        {
            Other = GetNpcByUid(Parser.OtherNpcUid);
            if (!Other)
            {
                DEBUG_MSG(GetDebugLine(this) + " - Other npc instance is required but not found!");
                return true;
            }
        }
        
        Function = parser->GetIndex(FunctionName);
        if (Function != Invalid && Parser.Enabled)
        {
            DEBUG_MSG(GetDebugLine(this) + " - initialize func call...");

            //parser->CallFunc(SaveParserVarsFunc);
            parser->SetInstance("SELF", Self);
            parser->SetInstance("OTHER", Other);
            this->SetAsInstance("CurrentTimedEffect");

            DEBUG_MSG(GetDebugLine(this) + " - CallFunc...");
            int Ok = *(int*)parser->CallFunc(Function);
            if (Ok) DEBUG_MSG(GetDebugLine(this) + " - Trigger completed!");

            //parser->CallFunc(RestoreParserVarsFunc);
            return Ok ? true : false;
        }
        
        DEBUG_MSG(GetDebugLine(this) + " - Function is invalid or trigger was disabled!");
        return true;
    }

    void C_TimedEffect::SetAsInstance(zSTRING instName) { parser->SetInstance(instName, this); }
    void C_TimedEffect::RegisterClassOffset()
    {
        C_TimedEffect* trigger = Null;
        parser->AddClassOffset(Z "C_TIMEDEFFECT", (int)&trigger->Parser);
    }
    void C_TimedEffect_RegisterClassOffset() { C_TimedEffect::RegisterClassOffset(); }

    void C_TimedEffect::Archive(zCArchiver& arc)
    {
        arc.WriteString("FunctionName", FunctionName);
        arc.WriteRaw("Parser", &Parser, sizeof(Parser));
        arc.WriteInt("Timer", triggerTimer[this].TimeHasPassed());
        arc.WriteInt("Self", Parser.SelfNpcUid == NoNpc ? NoNpc : GetNpcUid(Self));
        arc.WriteInt("Other", Parser.OtherNpcUid == NoNpc ? NoNpc : GetNpcUid(Other));
    }

    void C_TimedEffect::Unarchive(zCArchiver& arc)
    {
        arc.ReadString("FunctionName", FunctionName);
        Function = parser->GetIndex(FunctionName);
        arc.ReadRaw("Parser", &Parser, sizeof(Parser));

        int_t passedTime;
        arc.ReadInt("Timer", passedTime);
        triggerTimer[this].ResetTime().TimeAppend(passedTime);

        int selfUID = 0;
        int OtherUID = 0;
        arc.ReadInt("Self", selfUID);
        arc.ReadInt("Other", OtherUID);
        Self = GetNpcByUid(selfUID);
        Other = GetNpcByUid(OtherUID);
    }

    void UpdateTimedEffectsNpcUids(int oldUid, int newUid)
    {
        for (uint i = 0; i < C_TimedEffect::TimedEffectScripts.GetNum(); i++)
        {
            auto& trigger = C_TimedEffect::TimedEffectScripts[i];
            if (trigger->Parser.SelfNpcUid == oldUid)
            {
                trigger->Parser.SelfNpcUid = newUid;
                trigger->Self = GetNpcByUid(newUid);
            }
            if (trigger->Parser.OtherNpcUid == oldUid)
            {
                trigger->Parser.OtherNpcUid = newUid;
                trigger->Other = GetNpcByUid(newUid);
            }
        }
    }

    C_TimedEffect* C_TimedEffect::CreateTimedEffect(zSTRING funcName, int delay)
    {
        C_TimedEffect* trigger = new C_TimedEffect();
        trigger->FunctionName = funcName;
        trigger->Function = parser->GetIndex(funcName);
        trigger->Parser.Delay = delay;
        return trigger;
    }

    void C_TimedEffect::DoTimedEffectsLoop()
    {
        triggerTimer.ClearUnused();
        bool gamePause = ogame->IsOnPause() ? True : False;
        if (AfterLoadDelay > 0)
        {
            AfterLoadDelay -= 1;
            return;
        }

        bool deleteFlag = false;
        for (uint i = 0; i < TimedEffectScripts.GetNum(); i++)
        {
            auto trigger = TimedEffectScripts[i];
            if (!trigger)
            {
                DEBUG_MSG("TimedEffect loop " + Z (int)i + " is Null!");
                deleteFlag = true;
                continue;
            }
            uint delay = trigger->Parser.Delay;

            // Stop timer processing on the game pause
            triggerTimer[trigger].Suspend(gamePause);
            if (gamePause) continue;

            if (triggerTimer[trigger].AwaitExact(delay))            
                if (trigger->CallTrigger()) {
                    delete trigger; 
                    deleteFlag = true;
                }
        }

        if (deleteFlag)
        {
            DEBUG_MSG("TimedEffect loop - delete empty trigges");
            int outer_i = 0;
            uint max = TimedEffectScripts.GetNum();
            for (uint i = 0; i < max; )
            {
                if (!TimedEffectScripts[i]) TimedEffectScripts.RemoveAt(i);
                else i++;

                outer_i += 1;
                if (outer_i > max * 2)
                {
                    DEBUG_MSG("TimedEffect loop - delete empty trigges is looped?!");
                    break;
                }
            }
        }
    }

    void RegisterNearestNpcs()
    {
        DEBUG_MSG("Register nearest npc's...");
        zCVob* pVob = Null;
        oCNpc* npc = Null;
        if (player)
        {
            player->ClearVobList();
            player->CreateVobList(16000);
            zCArray<zCVob*> vobList = player->vobList;
            for (int i = 0; i < vobList.GetNum(); i++)
            {
                pVob = vobList.GetSafe(i);
                if (!pVob) continue;
                npc = zDYNAMIC_CAST<oCNpc>(pVob);
                if (!npc) continue;

                RegisterNpc(npc);
                if (i > 32000)
                {
                    DEBUG_MSG("Register nearest npc's loop happens!?");
                    break;
                }
            }
        }
        DEBUG_MSG("Register nearest npc's done!");
    }

    void C_TimedEffect::LoadTimedEffects(zCArchiver& arc) 
    {
        int_t triggersNum;
        arc.ReadInt("TriggersNum", triggersNum);
        for (int_t i = 0; i < triggersNum; i++) 
        {
            C_TimedEffect* trigger = new C_TimedEffect();
            trigger->Unarchive(arc);
        }
        DEBUG_MSG("Loaded " + Z triggersNum + " TimedEffects");
        RegisterNearestNpcs();
        AfterLoadDelay = 120;
    }

    void C_TimedEffect::SaveTimedEffects(zCArchiver& arc)
    {
        uint triggersNum = TimedEffectScripts.GetNum();
        arc.WriteInt("TriggersNum", triggersNum);
        for (uint i = 0; i < triggersNum; i++)
            TimedEffectScripts[i]->Archive(arc);
        DEBUG_MSG("Saved " + Z (int)triggersNum + " TimedEffects");
    }

    void C_TimedEffect::ClearTimedEffects()
    {
        if (TimedEffectScripts.GetNum() > 0)
        {
            for (uint i = 0; i < TimedEffectScripts.GetNum(); i++)
                delete TimedEffectScripts[i--];
            TimedEffectScripts.Clear();
        }
    }

    string GetTimedEffectsArchivePath() 
    {
        int slotID = SaveLoadGameInfo.slotID;
        string savesDir = zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES);
        string slotDir = GetSlotNameByID(slotID);
        string archivePath = string::Combine("%s\\%s\\StExt_TimedEffects.sav", savesDir, slotDir);
        return archivePath;
    }

    // Load
    void LoadTimedEffects()
    {
        DEBUG_MSG("Load timed effects...");
        C_TimedEffect::ClearTimedEffects();
        string archiveName = GetTimedEffectsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverRead(archiveName, 0);
        if (ar) 
        {
            C_TimedEffect::LoadTimedEffects(*ar);
            ar->Close();
            ar->Release();
        }
        DEBUG_MSG("Load timed effects done!");
    }

    // Save
    void SaveTimedEffects()
    {
        DEBUG_MSG("Save timed effects...");
        string archiveName = GetTimedEffectsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverWrite(archiveName, zARC_MODE_ASCII, 0, 0);
        if (ar)
        {
            C_TimedEffect::SaveTimedEffects(*ar);
            ar->Close();
            ar->Release();
        }
        DEBUG_MSG("Save timed effects done!");
    }

    void TimedEffectsOnNewGame() { C_TimedEffect::ClearTimedEffects(); }

    void TimedEffectsFinalizeLoop()
    {
        DEBUG_MSG("Finalize TimedEffects...");
        int heroId = parser->GetSymbol("StExt_HeroUid")->single_intdata;

        for (uint i = 0; i < C_TimedEffect::TimedEffectScripts.GetNum(); i++)
        {
            auto& trigger = C_TimedEffect::TimedEffectScripts[i];
            if (!trigger) continue;

            DEBUG_MSG(GetDebugLine(trigger) + " Finalizing... Id: " + Z (int)i);
            if ((trigger->Parser.SelfNpcUid == heroId) && (trigger->Parser.OtherNpcUid <= NoNpc))
            {
                DEBUG_MSG(GetDebugLine(trigger) + " Can be saved. Skip!");
                continue;
            }

            trigger->CallTrigger();
            DEBUG_MSG(GetDebugLine(trigger) + " - Deleting...");
            delete trigger;
        }
        C_TimedEffect::TimedEffectScripts.Clear();
        DEBUG_MSG("Finalizing timed effects done!");
    }

    void TimedEffectsLoop() { C_TimedEffect::DoTimedEffectsLoop(); }

    int GetTimedEffectByNpc()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING funcName;
        int npcUid;
        par->GetParameter(funcName);
        par->GetParameter(npcUid);

        for (uint i = 0; i < C_TimedEffect::TimedEffectScripts.GetNum(); i++)
        {
            if (C_TimedEffect::TimedEffectScripts[i]->Parser.SelfNpcUid != npcUid) continue;
            if (C_TimedEffect::TimedEffectScripts[i]->FunctionName == funcName)
            {
                par->SetReturn(C_TimedEffect::TimedEffectScripts[i]);
                return true;
            }
        }
        par->SetReturn(Null);
        return false;
    }

    int GetTimedEffectByIndex()
    {
        zCParser* par = zCParser::GetParser();
        int index;    
        par->GetParameter(index);

        if ((index >= 0) && (index < C_TimedEffect::TimedEffectScripts.GetNum()))
            par->SetReturn(C_TimedEffect::TimedEffectScripts[index]);

        par->SetReturn(Null);
        return true;
    }

    int CreateTimedEffect()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING funcName;
        int delay;

        oCNpc* Other = (oCNpc*)par->GetInstance();
        oCNpc* Self = (oCNpc*)par->GetInstance();
        par->GetParameter(delay);
        par->GetParameter(funcName);

        C_TimedEffect* trigger = C_TimedEffect::CreateTimedEffect(funcName, delay);
        trigger->Self = Self;
        trigger->Other = Other;
        trigger->Parser.SelfNpcUid = Self ? GetNpcUid(Self) : NoNpc;
        trigger->Parser.OtherNpcUid = Self ? GetNpcUid(Other) : NoNpc;

        par->SetReturn(trigger);
        return True;
    }

    int GetTimedEffectsCount()
    {
        zCParser* par = zCParser::GetParser();
        int result = C_TimedEffect::TimedEffectScripts.GetNum();
        par->SetReturn(result);
        return True;
    }
}
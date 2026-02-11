#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    const int DefaultLoadDelay = 120;

    Array<TimedEffect*> TimedEffects;
    uint CurrentTimedEffectsCount;
    Timer TriggerTimer;
    int AfterLoadDelay;


    inline zSTRING GetTimedEffectDebugLine(TimedEffect* trigger)
    {
        if (trigger)
        {
            zSTRING slfName = trigger->Self ? trigger->Self->name : zString_Unknown;
            zSTRING slfIndx = trigger->ScriptInstance.SelfUId == EmptyNpcUId ? "NoUid" : Z trigger->ScriptInstance.SelfUId;
            return Z("TimedEffect for Npc '" + slfName + "' [" + slfIndx + "] | Func: " + trigger->FunctionName + " (" + Z trigger->Function + ")");
        }
        return "Timed effect is null!";
    }

    inline string GetTimedEffectsArchivePath()
    {
        int slotID = SaveLoadGameInfo.slotID;
        string savesDir = zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES);
        string slotDir = GetSaveSlotNameByID(slotID);
        return string::Combine("%s\\%s\\StExt_TimedEffects.sav", savesDir, slotDir);
    }

    TimedEffect* BuildTimedEffect(zSTRING& funcName, int delay)
    {
        if (delay <= 0) delay = 1000;

        TimedEffect* trigger = new TimedEffect();
        trigger->FunctionName = funcName;
        trigger->Function = parser->GetIndex(funcName);
        trigger->ElapsedTime = 0U;
        trigger->ScriptInstance.Delay = delay;
        trigger->ScriptInstance.Enabled = True;

        if (trigger->Function == Invalid)
        {
            SAFE_DELETE(trigger);
            return Null;
        }

        TimedEffects.Insert(trigger);
        CurrentTimedEffectsCount = TimedEffects.GetNum();
        TriggerTimer[trigger].ResetTime().TimeAppend(trigger->ElapsedTime);
        return trigger;
    }

    void ClearTimedEffects()
    {
        parser->SetInstance("STEXT_CURRENTTIMEDEFFECT", Null);
        for (uint i = TimedEffects.GetNum(); i-- > 0; )
        {
            auto& trigger = TimedEffects[i];
            if (trigger)
                TriggerTimer[trigger].Delete();
            TimedEffects[i] = Null;
            SAFE_DELETE(trigger);
        }
        TimedEffects.Clear();
        CurrentTimedEffectsCount = TimedEffects.GetNum();
    }

    void ProcessTimedEffect(TimedEffect* trigger, bool& requireCleanUp)
    {
        requireCleanUp = false;
        if (!trigger)
        {
            requireCleanUp = true;
            return;
        }

        if (trigger->ScriptInstance.SelfUId == EmptyNpcUId)
        {
            DEBUG_MSG("ProcessTimedEffect - Self Npc UId not set! Details: " + GetTimedEffectDebugLine(trigger));
            requireCleanUp = true;
            return;
        }

        if (!trigger->Self)
        {
            trigger->Self = GetNpcByUid(trigger->ScriptInstance.SelfUId);
            if (!trigger->Self)
            {
                DEBUG_MSG("ProcessTimedEffect - Self npc instance not registered! Details: " + GetTimedEffectDebugLine(trigger));
                requireCleanUp = true;
                return;
            }
        }

        if ((trigger->ScriptInstance.OtherUId != EmptyNpcUId) && (!trigger->Other))
        {
            trigger->Other = GetNpcByUid(trigger->ScriptInstance.OtherUId);
            if (!trigger->Other)
            {
                DEBUG_MSG("ProcessTimedEffect - Other npc instance not registered (but required)! Details: " + GetTimedEffectDebugLine(trigger));
                requireCleanUp = true;
                return;
            }
        }

        if (trigger->Function == Invalid)
        {
            trigger->Function = parser->GetIndex(trigger->FunctionName);
            if (trigger->Function == Invalid)
            {
                DEBUG_MSG("ProcessTimedEffect - TimedEffect function not defined! Details: " + GetTimedEffectDebugLine(trigger));
                requireCleanUp = true;
                return;
            }
        }

        if (trigger->ScriptInstance.Enabled)
        {
            DEBUG_MSG("ProcessTimedEffect - OnTick: " + GetTimedEffectDebugLine(trigger));

            oCNpc* self = dynamic_cast<oCNpc*>((zCVob*)parser->GetSymbol(StExt_Self_SymId)->GetInstanceAdr());
            oCNpc* other = dynamic_cast<oCNpc*>((zCVob*)parser->GetSymbol(StExt_Other_SymId)->GetInstanceAdr());

            parser->SetInstance(StExt_Self_SymId, trigger->Self);
            parser->SetInstance(StExt_Other_SymId, trigger->Other);
            parser->SetInstance("STEXT_CURRENTTIMEDEFFECT", &trigger->ScriptInstance);

            int Ok = *(int*)parser->CallFunc(trigger->Function);
            if (Ok)
            {
                DEBUG_MSG("ProcessTimedEffect - Finished " + GetTimedEffectDebugLine(trigger));
                requireCleanUp = true;
            }
            parser->SetInstance(StExt_Self_SymId, self);
            parser->SetInstance(StExt_Other_SymId, other);
        }
    }


    void TimedEffectsLoop()
    {
        if (AfterLoadDelay > 0) {
            --AfterLoadDelay;
            return;
        }

        CurrentTimedEffectsCount = TimedEffects.GetNum();
        const bool gamePause = ogame->IsOnPause() || *(int*)parser->CallFunc(StExt_GamePausedFunc);
        bool requireCleanUp = false;
        TriggerTimer.ClearUnused();

        for (int i = (int)(CurrentTimedEffectsCount - 1); i >= 0; --i)
        {
            TimedEffect* trigger = TimedEffects[i];
            if (!trigger)
            {
                DEBUG_MSG("TimedEffectsLoop - trigger: " + Z(i) + " is Null!");
                TimedEffects.RemoveAt(i);
                continue;
            }

            const uint delay = static_cast<uint>(trigger->ScriptInstance.Delay);
            TriggerTimer[trigger].Suspend(gamePause);
            trigger->ElapsedTime = TriggerTimer[trigger].TimeHasPassed();

            if (gamePause) continue;
            if (TriggerTimer[trigger].AwaitExact(delay))
            {
                bool deleteFlag = false;
                ProcessTimedEffect(trigger, deleteFlag);

                if (deleteFlag)
                {
                    TriggerTimer[trigger].Delete();
                    SAFE_DELETE(trigger);
                    TimedEffects.RemoveAt(i);
                }
            }
        }
        CurrentTimedEffectsCount = TimedEffects.GetNum();
        parser->SetInstance("STEXT_CURRENTTIMEDEFFECT", Null);
    }

    void FinalizeTimedEffects()
    {
        DEBUG_MSG("FinalizeTimedEffects - start finalization...");
        CurrentTimedEffectsCount = TimedEffects.GetNum();

        uint outer_i = 0U, loopLimit = CurrentTimedEffectsCount * 2U;
        for (uint i = CurrentTimedEffectsCount; i-- > 0;)
        {
            ++outer_i;
            if (outer_i >= loopLimit) break;

            TimedEffect* trigger = TimedEffects[i];
            if (!trigger)
            {
                TimedEffects.RemoveAt(i);
                continue;
            }
            if (trigger->ScriptInstance.SelfUId == HeroNpcUId) continue;

            bool deleteFlag;
            ProcessTimedEffect(trigger, deleteFlag);
            trigger->ElapsedTime = TriggerTimer[trigger].TimeHasPassed();

            if (deleteFlag)
            {
                TriggerTimer[trigger].Delete();
                TimedEffects[i] = Null;
                SAFE_DELETE(trigger);
                TimedEffects.RemoveAt(i);
            }
        }

        AfterLoadDelay = DefaultLoadDelay;
        parser->SetInstance("STEXT_CURRENTTIMEDEFFECT", Null);
        CurrentTimedEffectsCount = TimedEffects.GetNum();
        DEBUG_MSG("Finalizing timed effects done!");
    }


    void LoadTimedEffects()
    {
        DEBUG_MSG("LoadTimedEffects: load effects...");

        ClearTimedEffects();
        const zSTRING archiveName = GetTimedEffectsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverRead(archiveName, 0);
        if (!ar)
        {
            DEBUG_MSG("LoadTimedEffects - fail to create reader for: " + archiveName);
            Message::Error((string)"LoadTimedEffects - archiver is Null!", "Critical error!");
            gameMan->ExitGame();
            return;
        }

        ar->ReadRaw("CurrentTimedEffectsCount", &CurrentTimedEffectsCount, sizeof(uint));
        uint triggersToLoad = CurrentTimedEffectsCount;
        for (uint i = 0U; i < triggersToLoad; ++i)
        {
            TimedEffect* trigger = new TimedEffect();
            trigger->UnArchive(*ar);
            trigger->Self = GetNpcByUid(trigger->ScriptInstance.SelfUId);
            trigger->Other = GetNpcByUid(trigger->ScriptInstance.OtherUId);
            trigger->Function = parser->GetIndex(trigger->FunctionName);
            TimedEffects.Insert(trigger);
            TriggerTimer[trigger].ResetTime().TimeAppend(trigger->ElapsedTime);
        }

        ar->Close();
        ar->Release();
        AfterLoadDelay = DefaultLoadDelay;
        CurrentTimedEffectsCount = TimedEffects.GetNum();
        DEBUG_MSG("LoadTimedEffects - loaded " + Z((int)CurrentTimedEffectsCount) + "/" + Z((int)triggersToLoad) + " TimedEffects.");
    }

    // Save
    void SaveTimedEffects()
    {
        DEBUG_MSG("SaveTimedEffects: save effects...");

        CurrentTimedEffectsCount = TimedEffects.GetNum();
        const string archiveName = GetTimedEffectsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverWrite(archiveName, zARC_MODE_BINARY_SAFE, 0, 0);
        if (!ar)
        {
            DEBUG_MSG("SaveTimedEffects - fail to create writer for: " + archiveName);
            Message::Error((string)"SaveTimedEffects - archiver is Null!", "Critical error!");
            gameMan->ExitGame();
            return;
        }

        uint triggersNum = 0U;
        for (uint i = 0U; i < CurrentTimedEffectsCount; ++i)
            if (TimedEffects[i] && TimedEffects[i]->ScriptInstance.Enabled) ++triggersNum;

        ar->WriteRaw("CurrentTimedEffectsCount", &triggersNum, sizeof(uint));
        for (uint i = 0U; i < CurrentTimedEffectsCount; ++i)
        {
            if (!TimedEffects[i] || !TimedEffects[i]->ScriptInstance.Enabled) continue;
            TimedEffects[i]->Archive(*ar);
        }

        ar->Close();
        ar->Release();
        DEBUG_MSG("SaveTimedEffects - saved " + Z(int)triggersNum + " TimedEffects");
    }


    int __cdecl GetTimedEffectsCount()
    {
        zCParser* par = zCParser::GetParser();
        CurrentTimedEffectsCount = TimedEffects.GetNum();
        par->SetReturn(static_cast<int>(CurrentTimedEffectsCount));
        return True;
    }

    int __cdecl CreateTimedEffect()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING funcName;
        int delay;

        oCNpc* other = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        oCNpc* self = dynamic_cast<oCNpc*>((zCVob*)par->GetInstance());
        par->GetParameter(delay);
        par->GetParameter(funcName);

        TimedEffect* trigger = BuildTimedEffect(funcName, delay);
        if (!trigger)
        {
            DEBUG_MSG("CreateTimedEffect - fail to create timed effect! OnTick func: '" + funcName + "'");
            par->SetReturn(Null);
            return False;
        }

        trigger->Self = self;
        trigger->Other = other;
        trigger->ScriptInstance.SelfUId = self ? GetNpcUid(self) : EmptyNpcUId;
        trigger->ScriptInstance.OtherUId = other ? GetNpcUid(other) : EmptyNpcUId;

        par->SetReturn(&trigger->ScriptInstance);
        return True;
    }

    int __cdecl GetTimedEffectByNpc()
    {
        zCParser* par = zCParser::GetParser();
        zSTRING funcName;
        int npcUid;
        par->GetParameter(funcName);
        par->GetParameter(npcUid);

        oCNpc* npc = GetNpcByUid(npcUid);
        if (!npc)
        {
            par->SetReturn(Null);
            return False;
        }

        CurrentTimedEffectsCount = TimedEffects.GetNum();
        for (uint i = 0U; i < CurrentTimedEffectsCount; ++i)
        {
            if (TimedEffects[i]->ScriptInstance.SelfUId != npcUid) continue;
            if (TimedEffects[i]->FunctionName == funcName)
            {
                par->SetReturn(&TimedEffects[i]->ScriptInstance);
                return True;
            }
        }

        par->SetReturn(Null);
        return False;
    }

    int __cdecl GetTimedEffectByIndex()
    {
        zCParser* par = zCParser::GetParser();
        int index; par->GetParameter(index);

        if (index < 0)
        {
            DEBUG_MSG("GetTimedEffectByIndex - index out of range. Index: " + Z index);
            par->SetReturn(Null);
            return False;
        }

        if (TimedEffects.GetSafe(static_cast<uint>(index)))
        {
            par->SetReturn(&TimedEffects[index]->ScriptInstance);
            return True;
        }

        DEBUG_MSG("GetTimedEffectByIndex - invalid index: " + Z index);
        par->SetReturn(Null);
        return False;
    }
}
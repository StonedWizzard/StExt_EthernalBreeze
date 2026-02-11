// This file added in headers queue
// File: "Sources.h"
#include "resource.h"

namespace Gothic_II_Addon
{
    void Game_Entry() { }
    void Game_Exit() { }

    // Initialize Game (mod) (kind of entry point)
    void Game_Init()
    {
        CreateDebugFile();
        DEBUG_MSG("StExt - Initialize mod...");
        StonedExtension_InitModData();
        StonedExtension_InitUi();
        StExt_ModReady = true;
        DEBUG_MSG("StExt - Mod was initialized!");
        DEBUG_MSG("");
    }

    // Game Loop
    void Game_PreLoop() { StatsUncaperLoop(); }

    void Game_Loop()
    {
        if (ogame && player)
        {
            UpdateUiStatus();
            StonedExtension_Loop();
            TimedEffectsLoop();
            DrawModInfo();
        }
    }

    void Game_PostLoop()
    {
        ClearDamageMeta();
        StatsUncaperLoop();
    }

    void Game_MenuLoop() 
    { 
        UpdateUiStatus();
        DrawModInfo(); 
    }

    // Information about current saving or loading world
    TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

    // Save game
    void Game_SaveBegin() 
    {
        DEBUG_MSG("");
        DEBUG_MSG("-------> START SAVE GAME");
        DEBUG_MSG("");
    }
    void Game_SaveEnd()
    {
        if (!IsLoading && !IsLevelChanging)
        {
            SaveModState();
            SaveNpcExtensions();
            SaveTimedEffects();
            SaveGeneratedItems();
        }
        DEBUG_MSG("");
        DEBUG_MSG("-------> END SAVE GAME");
        DEBUG_MSG("");
    }

    // Load New Game
    void Game_LoadBegin_NewGame()
    {
        DEBUG_MSG("");
        DEBUG_MSG("-------> LoadBegin: NEW GAME");
        DEBUG_MSG("");
        IsLoading = true;
        parser->GetSymbol("StExt_IsLevelChanging")->SetValue(IsLevelChanging, 0);
        parser->GetSymbol("StExt_IsLoading")->SetValue(IsLoading, 0);

        MsgTray_Clear();
        StopUncaper();
        ClearDamageMeta();
        ClearNpcExtensionPointers();
        ClearNpcExtensionData();
        ClearGeneratedItemsData();
        ResetModState();
        ClearTimedEffects();
    }
    void Game_LoadEnd_NewGame() 
    {
        IsLoading = false;
        IsLevelChanging = false;
        parser->GetSymbol("StExt_IsLevelChanging")->SetValue(IsLevelChanging, 0);
        parser->GetSymbol("StExt_IsLoading")->SetValue(IsLoading, 0);
        parser->CallFunc(OnLoadEndFunc);
        DrawModInfo();
        DEBUG_MSG("");
        DEBUG_MSG("-------> LoadEnd: NEW GAME");
        DEBUG_MSG("");
    }

    // Load Save Game
    void Game_LoadBegin_SaveGame() 
    {
        DEBUG_MSG("");
        DEBUG_MSG("-------> LoadBegin: SAVE GAME");
        DEBUG_MSG("");
        IsLoading = true;
        parser->GetSymbol("StExt_IsLevelChanging")->SetValue(IsLevelChanging, 0);
        parser->GetSymbol("StExt_IsLoading")->SetValue(IsLoading, 0);

        MsgTray_Clear();
        StopUncaper();
        ClearDamageMeta();
        ClearTimedEffects();
        ClearNpcExtensionData();
        LoadGeneratedItems();
    }
    void Game_LoadEnd_SaveGame() 
    {
        LoadModState();
        LoadNpcExtensions();
        LoadTimedEffects();

        IsLoading = false;
        IsLevelChanging = false;
        parser->GetSymbol("StExt_IsLoading")->SetValue(IsLoading, 0);
        parser->GetSymbol("StExt_IsLevelChanging")->SetValue(IsLevelChanging, 0);
        parser->CallFunc(OnLoadEndFunc);
        DrawModInfo();
        DEBUG_MSG("");
        DEBUG_MSG("-------> LoadEnd: SAVE GAME");
        DEBUG_MSG("");
    }

    // Load Next Level
    void Game_LoadBegin_Trigger()
    {
        DEBUG_MSG("");
        DEBUG_MSG("-------> LoadBegin: CHANGE LEVEL");
        DEBUG_MSG("");

        IsLevelChanging = true;
        IsLoading = true;
        parser->GetSymbol("StExt_IsLoading")->SetValue(IsLoading, 0);
        parser->GetSymbol("StExt_IsLevelChanging")->SetValue(IsLevelChanging, 0);
        parser->CallFunc(OnLevelChangeFunc);
        MsgTray_Clear();
        ClearDamageMeta();
    }
    void Game_LoadEnd_Trigger() { }
    void Game_LoadBegin_ChangeLevel()
    {
        FinalizeTimedEffects();
        ClearNpcExtensionPointers();
        StopUncaper();
    }
    void Game_LoadEnd_ChangeLevel()
    {
        RegisterWorldNpcs();
        IsLevelChanging = false;
        IsLoading = false;
        parser->GetSymbol("StExt_IsLoading")->SetValue(IsLoading, 0);
        parser->GetSymbol("StExt_IsLevelChanging")->SetValue(IsLevelChanging, 0);
        parser->CallFunc(OnLoadEndFunc);
        DEBUG_MSG("");
        DEBUG_MSG("-------> LoadEnd: CHANGE LEVEL");
        DEBUG_MSG("");
    }

    void Game_Pause() { UpdateUiStatus(); }

    void Game_Unpause() { UpdateUiStatus(); }

    void Game_DefineExternals()
    {
        ArraysOperations_DefineExternals();
        StonedExtension_DefineExternals();
    }

    void Game_ApplyOptions() { }

    /*
    Functions call order on Game initialization:
      - Game_Entry           * Gothic entry point
      - Game_DefineExternals * Define external script functions
      - Game_Init            * After DAT files init

    Functions call order on Change level:
      - Game_LoadBegin_Trigger     * Entry in trigger
      - Game_LoadEnd_Trigger       *
      - Game_Loop                  * Frame call window
      - Game_LoadBegin_ChangeLevel * Load begin
      - Game_SaveBegin             * Save previous level information
      - Game_SaveEnd               *
      - Game_LoadEnd_ChangeLevel   *

    Functions call order on Save game:
      - Game_Pause     * Open menu
      - Game_Unpause   * Click on save
      - Game_Loop      * Frame call window
      - Game_SaveBegin * Save begin
      - Game_SaveEnd   *

    Functions call order on Load game:
      - Game_Pause              * Open menu
      - Game_Unpause            * Click on load
      - Game_LoadBegin_SaveGame * Load begin
      - Game_LoadEnd_SaveGame   *
    */

#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals,
    Enabled( AppDefault ) Game_ApplyOptions
  );
}
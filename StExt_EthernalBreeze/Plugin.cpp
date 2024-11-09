// This file added in headers queue
// File: "Sources.h"
#include "resource.h"

namespace Gothic_II_Addon
{
    

  void Game_Entry() { }
  
  void Game_Init() 
  {
      CreateDebugFile();
      DEBUG_MSG("StExt - Initialize mod...");
      StonedExtension_InitModData();
      StonedExtension_InitUi();      
      DEBUG_MSG("StExt - Mod initialized!");
      DEBUG_MSG("");
  }

  void Game_Exit() { }

  void Game_PreLoop() 
  { 
     /*if (ogame && player && OnPreLoopFunc != Invalid)
          parser->CallFunc(OnPreLoopFunc);*/
      StatsUncaperLoop();
  }

  void Game_Loop() 
  { 
      StonedExtension_Loop(); 
      TimedEffectsLoop();
  }

  void Game_PostLoop() 
  {
      /*if (ogame && player && OnPostLoopFunc != Invalid)
          parser->CallFunc(OnPostLoopFunc);*/
      StatsUncaperLoop();
  }

  void Game_MenuLoop() 
  {
      DrawModInfo();
  }

  // Information about current saving or loading world
  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_SaveBegin() { }

  void Game_SaveEnd() 
  {
      DEBUG_MSG("End save game...");
      DEBUG_MSG("");
      SaveModState();
      SaveTimedEffects();
      SaveGeneratedItems();
  }

  void LoadBegin() 
  { 
      DEBUG_MSG("Begin load game...");
      DEBUG_MSG("");
      StopUncaper();
      ClearRegisteredNpcs();
      LoadGeneratedItems();
  }

  void LoadEnd() 
  {
      DEBUG_MSG("End load game...");
      DEBUG_MSG("");
      LoadModState();
      LoadTimedEffects();
      parser->CallFunc(OnLoadEndFunc);
      DrawModInfo();
      DEBUG_MSG("Game loaded!");
  }

  void Game_LoadBegin_NewGame()
  {
      DEBUG_MSG("Start new game...");
      DEBUG_MSG("");
      LoadBegin();
      ResetModState();
      ClearRegisteredNpcs();
      TimedEffectsOnNewGame();
      ClearGeneratedItemsData();
  }

  void Game_LoadEnd_NewGame() { LoadEnd(); }

  void Game_LoadBegin_SaveGame() { LoadBegin(); }

  void Game_LoadEnd_SaveGame() { LoadEnd(); }

  void Game_LoadBegin_ChangeLevel() { LoadBegin(); }

  void Game_LoadEnd_ChangeLevel()
  {
      LoadEnd();
      parser->GetSymbol("StExt_LevelChanging")->SetValue(false, 0);
      DEBUG_MSG("Level changed!");
      DEBUG_MSG("");
  }

  void Game_LoadBegin_Trigger() 
  {
      DEBUG_MSG("");
      DEBUG_MSG("Start change level...");
      parser->GetSymbol("StExt_LevelChanging")->SetValue(true, 0);
      parser->CallFunc(OnLevelChangeFunc);
      TimedEffectsFinalizeLoop();
      DEBUG_MSG("Level change processed by script!");
      DEBUG_MSG("");
  }
  
  void Game_LoadEnd_Trigger() { }
  
  void Game_Pause() { }
  
  void Game_Unpause() { }
  
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
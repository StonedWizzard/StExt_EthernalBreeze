#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    static ModStateData ModState;

    string GetModStateArchivePath()
    {
        int slotID = SaveLoadGameInfo.slotID;
        string savesDir = zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES);
        string slotDir = GetSaveSlotNameByID(slotID);
        string archivePath = string::Combine("%s\\%s\\StExt_EthernalBreeze.sav", savesDir, slotDir);
        return archivePath;
    }

    void ResetModState() { ModState = ModStateData(); }

    void ArchiveState(zCArchiver& arc)
    {
        arc.WriteString("StExt_RandomizeWorld_Wp1_CallbackStr", ModState.StExt_RandomizeWorld_Wp1_CallbackStr);
        arc.WriteString("StExt_RandomizeWorld_Wp2_CallbackStr", ModState.StExt_RandomizeWorld_Wp2_CallbackStr);
        arc.WriteString("StExt_RandomizeWorld_Wp3_CallbackStr", ModState.StExt_RandomizeWorld_Wp3_CallbackStr);

        arc.WriteString("StExt_RandomizeWorld_Wp1", ModState.StExt_RandomizeWorld_Wp1);
        arc.WriteString("StExt_RandomizeWorld_Wp2", ModState.StExt_RandomizeWorld_Wp2);
        arc.WriteString("StExt_RandomizeWorld_Wp3", ModState.StExt_RandomizeWorld_Wp3);

        arc.WriteString("StExt_ArmorVisualBackup", ModState.StExt_ArmorVisualBackup);

        arc.WriteString("StExt_CurrentItemGeneratorConfigs", ModState.StExt_CurrentItemGeneratorConfigs);
        arc.WriteString("StExt_CurrentUserConfigs", ModState.StExt_CurrentUserConfigs);
    }

    void UnarchiveState(zCArchiver& arc)
    {
        arc.ReadString("StExt_RandomizeWorld_Wp1_CallbackStr", ModState.StExt_RandomizeWorld_Wp1_CallbackStr);
        arc.ReadString("StExt_RandomizeWorld_Wp2_CallbackStr", ModState.StExt_RandomizeWorld_Wp2_CallbackStr);
        arc.ReadString("StExt_RandomizeWorld_Wp3_CallbackStr", ModState.StExt_RandomizeWorld_Wp3_CallbackStr);

        arc.ReadString("StExt_RandomizeWorld_Wp1", ModState.StExt_RandomizeWorld_Wp1);
        arc.ReadString("StExt_RandomizeWorld_Wp2", ModState.StExt_RandomizeWorld_Wp2);
        arc.ReadString("StExt_RandomizeWorld_Wp3", ModState.StExt_RandomizeWorld_Wp3);

        arc.ReadString("StExt_ArmorVisualBackup", ModState.StExt_ArmorVisualBackup);

        arc.ReadString("StExt_CurrentItemGeneratorConfigs", ModState.StExt_CurrentItemGeneratorConfigs);
        arc.ReadString("StExt_CurrentUserConfigs", ModState.StExt_CurrentUserConfigs);
    }

    // Save
    void SaveModState()
    {
        DEBUG_MSG("Save game state...");
        string archiveName = GetModStateArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverWrite(archiveName, zARC_MODE_ASCII, 0, 0);
        if (!ar)
        {
            Message::Error((string)"SaveModState - archiver is Null!", "Critical error!");
            gameMan->ExitGame();
            return;
        }

        ModState.StExt_RandomizeWorld_Wp1_CallbackStr = parser->GetSymbol("StExt_RandomizeWorld_Wp1_CallbackStr")->stringdata;
        ModState.StExt_RandomizeWorld_Wp2_CallbackStr = parser->GetSymbol("StExt_RandomizeWorld_Wp2_CallbackStr")->stringdata;
        ModState.StExt_RandomizeWorld_Wp3_CallbackStr = parser->GetSymbol("StExt_RandomizeWorld_Wp3_CallbackStr")->stringdata;

        ModState.StExt_RandomizeWorld_Wp1 = parser->GetSymbol("StExt_WorldRandimizer_Waypoint")->stringdata[0];
        ModState.StExt_RandomizeWorld_Wp2 = parser->GetSymbol("StExt_WorldRandimizer_Waypoint")->stringdata[1];
        ModState.StExt_RandomizeWorld_Wp3 = parser->GetSymbol("StExt_WorldRandimizer_Waypoint")->stringdata[2];

        ModState.StExt_ArmorVisualBackup = parser->GetSymbol("StExt_ArmorVisualBackup")->stringdata;

        ModState.StExt_CurrentItemGeneratorConfigs = parser->GetSymbol("StExt_CurrentItemGeneratorConfigs")->stringdata;
        ModState.StExt_CurrentUserConfigs = parser->GetSymbol("StExt_CurrentUserConfigs")->stringdata;

        ArchiveState(*ar);
        ar->Close();
        ar->Release();
    }

    // Load
    void LoadModState()
    {
        DEBUG_MSG("Load game state...");

        ResetModState();
        string archiveName = GetModStateArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverRead(archiveName, 0);
        if (!ar)
        {
            Message::Error((string)"LoadModState - archiver is Null!", "Critical error!");
            gameMan->ExitGame();
            return;
        }

        UnarchiveState(*ar);
        ar->Close();
        ar->Release();

        parser->GetSymbol("StExt_RandomizeWorld_Wp1_CallbackStr")->SetValue(ModState.StExt_RandomizeWorld_Wp1_CallbackStr, 0);
        parser->GetSymbol("StExt_RandomizeWorld_Wp2_CallbackStr")->SetValue(ModState.StExt_RandomizeWorld_Wp2_CallbackStr, 0);
        parser->GetSymbol("StExt_RandomizeWorld_Wp3_CallbackStr")->SetValue(ModState.StExt_RandomizeWorld_Wp3_CallbackStr, 0);

        parser->GetSymbol("StExt_WorldRandimizer_Waypoint")->SetValue(ModState.StExt_RandomizeWorld_Wp1, 0);
        parser->GetSymbol("StExt_WorldRandimizer_Waypoint")->SetValue(ModState.StExt_RandomizeWorld_Wp2, 1);
        parser->GetSymbol("StExt_WorldRandimizer_Waypoint")->SetValue(ModState.StExt_RandomizeWorld_Wp3, 2);

        parser->GetSymbol("StExt_ArmorVisualBackup")->SetValue(ModState.StExt_ArmorVisualBackup, 0);

        parser->GetSymbol("StExt_CurrentItemGeneratorConfigs")->SetValue(ModState.StExt_CurrentItemGeneratorConfigs, 0);
        parser->GetSymbol("StExt_CurrentUserConfigs")->SetValue(ModState.StExt_CurrentUserConfigs, 0);

        DEBUG_MSG("Load game state done!");
    }
}
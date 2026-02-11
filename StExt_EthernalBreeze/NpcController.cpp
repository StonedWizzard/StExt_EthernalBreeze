#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    Map<int, NpcExtension*> NpcExtensionData;
    uint NpcExtensionCount;
    int LastNpcUid;


    inline bool IsUidRegistered(const int npcUid) { return NpcExtensionData.GetSafePair(npcUid) != Null; }
    inline bool IsUidValid(const int npcUid) { return (npcUid != 0 && npcUid != Invalid); }
    inline NpcExtension* CreateNpcExtension(const int npcUid, const zSTRING& instanceName, oCNpc* npc)
    {
        NpcExtension* npcExt = new NpcExtension();
        npcExt->NpcPtr = npc;
        npcExt->NpcUid = npcUid;
        npcExt->InstanceName = instanceName;
        memset(&npcExt->Stats, 0, sizeof(npcExt->Stats));
        return npcExt;
    }

    inline void AddNewNpcRecord(const int npcUid, const zSTRING& instanceName, oCNpc* npc)
    {
        NpcExtension* npcExt = CreateNpcExtension(npcUid, instanceName, npc);
        NpcExtensionData.Insert(npcUid, npcExt);
    }

    inline void ClearNpcExtensionData()
    {
        for (auto& pair : NpcExtensionData)
        {
            NpcExtension* npcExt = pair.GetValue();
            SAFE_DELETE(npcExt);
        }
        NpcExtensionData.Clear();
        NpcExtensionCount = NpcExtensionData.GetNum();
    }

    inline void ClearNpcExtensionPointers()
    {
        for (auto& pair : NpcExtensionData) 
        {
            NpcExtension* ext = pair.GetValue();
            if (ext) ext->NpcPtr = Null;
        }
    }

    inline int GetNpcUid(oCNpc* npc)
    {
        oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
        return npcEx ? npcEx->m_pVARS[StExt_AiVar_Uid] : Invalid;
    }

    inline int GetNextNpcUid()
    {
        int result = LastNpcUid + 1;
        while (IsUidRegistered(result))
        {
            ++result;
            if (result == 0 || result == Invalid) ++result;
        }
        LastNpcUid = result;
        return result;
    }

    void RegisterNpc(oCNpc* npc, int& npcUid)
    {
        if (!npc) return;

        oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(npc);
        if (!npcEx) { DEBUG_MSG("RegisterNpc - fail convert '" + npc->name[0] + "' to oCNpcEx!"); return; }

        if (npc->IsSelfPlayer() || npc == player)
        {
            npcUid = 0;
            npcEx->m_pVARS[StExt_AiVar_Uid] = 0;
            auto pair = NpcExtensionData.GetSafePair(0);
            if (!pair)
            {
                AddNewNpcRecord(npcUid, player->GetInstanceName(), player);
                NpcExtensionCount = NpcExtensionData.GetNum();
            }
            else
            {
                NpcExtension* ext = pair->GetValue();
                if (!ext) {
                    pair->GetValue() = CreateNpcExtension(npcUid, player->GetInstanceName(), player);
                }

                ext->NpcPtr = player;
                ext->NpcUid = npcUid;
                ext->InstanceName = player->GetInstanceName();
            }
            return;
        }

        const zSTRING incomingNpcInstanceName = npc->GetInstanceName();
        const int incomingNpcUid = npcEx->m_pVARS[StExt_AiVar_Uid];
        if (IsUidValid(incomingNpcUid))
        {
            auto pair = NpcExtensionData.GetSafePair(incomingNpcUid);
            if (pair)
            {
                NpcExtension* ext = pair->GetValue();
                if (ext)
                {
                    if (ext->InstanceName == incomingNpcInstanceName)
                    {
                        ext->NpcPtr = npc;
                        npcUid = incomingNpcUid;
                        npcEx->m_pVARS[StExt_AiVar_Uid] = npcUid;                        
                        return;
                    }

                    // Collision
                    DEBUG_MSG("RegisterNpc - UId collision! '" + 
                        incomingNpcInstanceName + " [" + npcEx->name[0] + "]' tried to use UID " + Z(incomingNpcUid) + ", already owned by '" + ext->InstanceName + "'");
                }
            }
            else
            {
                npcUid = incomingNpcUid;
                npcEx->m_pVARS[StExt_AiVar_Uid] = npcUid;
                AddNewNpcRecord(npcUid, incomingNpcInstanceName, npc);
                NpcExtensionCount = NpcExtensionData.GetNum();
                return;
            }
        }

        const int newUid = GetNextNpcUid();
        npcUid = npc->IsSelfPlayer() ? 0 : newUid;
        npcEx->m_pVARS[StExt_AiVar_Uid] = newUid;
        AddNewNpcRecord(newUid, npc->GetInstanceName(), npc);
        NpcExtensionCount = NpcExtensionData.GetNum();
        DEBUG_MSG("RegisterNpc - Assigned NEW UId " + Z(newUid) + " to '" + npc->GetInstanceName() + " [" + npc->name[0] + "]'");
    }

    inline oCNpc* GetNpcByUid(const int npcUid)
    {
        auto pair = NpcExtensionData.GetSafePair(npcUid);
        if (!pair)
        {
            if (player && (npcUid == 0))
            {
                int tmpUid = 0;
                RegisterNpc(player, tmpUid);
                return player;
            }
            return Null;
        }
        NpcExtension* npcExt = pair->GetValue();
        if (!npcExt || !npcExt->NpcPtr) return Null;
        return dynamic_cast<oCNpc*>(npcExt->NpcPtr);
    }
        

    void RegisterWorldNpcs()
    {
        DEBUG_MSG("RegisterWorldNpcs - start search npcs...");
        oCWorld* world = ogame->GetGameWorld();
        if (!world)
        {
            DEBUG_MSG("RegisterWorldNpcs - world not initialized!");
            return;
        }

        int foundNpcs = 0;
        int npcUid = Invalid;
        auto it = world->voblist_npcs;
        while (it)
        {
            oCNpc* npc = it->GetData();
            if (npc)
            {
                int npcUid = Invalid;
                RegisterNpc(npc, npcUid);
                ++foundNpcs;
            }
            it = it->GetNextInList();
        }

        if (player)
        {
            int npcUid = 0;
            RegisterNpc(player, npcUid);
        }
        DEBUG_MSG("RegisterWorldNpcs - done! Found: " + Z(foundNpcs) + "/" + Z(static_cast<int>(NpcExtensionCount)));
    }


    //-----------------------------------------------------------------
    //						        API
    //-----------------------------------------------------------------

    inline bool SetNpcExtensionVar(const int uid, const int index, const int value)
    {
        auto pair = NpcExtensionData.GetSafePair(uid);
        if (!pair) return false;
        NpcExtension* npcExt = pair->GetValue();
        if (npcExt && IsIndexInBounds(index, NpcExtension_StatsMax))
        {
            npcExt->Stats[index] = value;
            return true;
        }
        return false;
    }

    inline bool GetNpcExtensionVar(const int uid, const int index, int& value)
    {
        auto pair = NpcExtensionData.GetSafePair(uid);
        if (!pair) return false;
        NpcExtension* npcExt = pair->GetValue();
        if (npcExt && IsIndexInBounds(index, NpcExtension_StatsMax))
        {
            value = npcExt->Stats[index];
            return true;
        }
        return false;
    }

    //-----------------------------------------------------------------
    //						    SERIALIZATION
    //-----------------------------------------------------------------

    string GetNpcExtensionsArchivePath()
    {
        int slotID = SaveLoadGameInfo.slotID;
        string savesDir = zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES);
        string slotDir = GetSaveSlotNameByID(slotID);
        string archivePath = string::Combine("%s\\%s\\StExt_NpcData.sav", savesDir, slotDir);
        return archivePath;
    }

    void SaveNpcExtensions()
    {
        DEBUG_MSG("SaveNpcExtensions: save npcs...");

        string archiveName = GetNpcExtensionsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverWrite(archiveName, zARC_MODE_BINARY_SAFE, 0, 0);
        if (!ar)
        {
            DEBUG_MSG("SaveNpcExtensions: fail to create writer for: " + archiveName);
            Message::Error((string)"SaveNpcExtensions - archiver is Null!", "Critical error!");
            gameMan->ExitGame();
            return;
        }

        uint savedNpcExtension = 0;
        NpcExtensionCount = NpcExtensionData.GetNum();
        ar->WriteRaw("NpcExtensionCount", &NpcExtensionCount, sizeof(NpcExtensionCount));
        ar->WriteInt("LastNpcUid", LastNpcUid);
        for (auto& pair : NpcExtensionData)
        {
            NpcExtension* npcExt = pair.GetValue();
            if (npcExt)
            {
                ar->WriteInt("NpcUid", npcExt->NpcUid);
                ar->WriteString("InstanceName", npcExt->InstanceName);
                ar->WriteRaw("NpcStats", &npcExt->Stats, sizeof(npcExt->Stats));
                ++savedNpcExtension;
            }
        }

        ar->Close();
        ar->Release();
        NpcExtensionCount = NpcExtensionData.GetNum();
        DEBUG_MSG("SaveNpcExtensions: saved " + Z(static_cast<int>(savedNpcExtension)) + "/" + Z(static_cast<int>(NpcExtensionCount)) + " npcs.");
    }

    void LoadNpcExtensions()
    {
        DEBUG_MSG("LoadNpcExtensions: load npcs...");

        const zSTRING archiveName = GetNpcExtensionsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverRead(archiveName, 0);
        if (!ar)
        {
            DEBUG_MSG("LoadNpcExtensions: fail to create reader for: " + archiveName);
            Message::Error((string)"LoadNpcExtensions - archiver is Null!", "Critical error!");
            gameMan->ExitGame();
            return;
        }

        ClearNpcExtensionData();
        uint loadedNpcExtension = 0;
        ar->ReadRaw("NpcExtensionCount", &NpcExtensionCount, sizeof(NpcExtensionCount));
        ar->ReadInt("LastNpcUid", LastNpcUid);
        const uint extensionToLoad = NpcExtensionCount;
        for (uint i = 0; i < extensionToLoad; ++i)
        {
            NpcExtension* npcExt = new NpcExtension();
            ar->ReadInt("NpcUid", npcExt->NpcUid);
            ar->ReadString("InstanceName", npcExt->InstanceName);
            ar->ReadRaw("NpcStats", &npcExt->Stats, sizeof(npcExt->Stats));
            npcExt->NpcPtr = Null;
            NpcExtensionData.Insert(npcExt->NpcUid, npcExt);
            ++loadedNpcExtension;
        }

        ar->Close();
        ar->Release();        
        DEBUG_MSG("LoadNpcExtensions: loaded " + Z(static_cast<int>(loadedNpcExtension)) + "/" + Z(static_cast<int>(extensionToLoad)) + " npcs.");

        NpcExtensionCount = NpcExtensionData.GetNum();
        RegisterWorldNpcs();
    }

    
    //-----------------------------------------------------------------
    //							   HOOKS
    //-----------------------------------------------------------------

    HOOK Hook_oCNpc_InitByScript PATCH(&oCNpc::InitByScript, &oCNpc::InitByScript_StExt);
    void oCNpc::InitByScript_StExt(int instance, int inSaveGame)
    {
        THISCALL(Hook_oCNpc_InitByScript)(instance, inSaveGame);
        if (IsLoading || IsLevelChanging) return;

        oCNpcEx* npcEx = dynamic_cast<oCNpcEx*>(this);
        if (!npcEx)
        {
            DEBUG_MSG("InitByScript_StExt - fail convert '" + this->name[0] + "' to oCNpcEx!");
            return;
        }

        int uid = npcEx->m_pVARS[StExt_AiVar_Uid];
        if (this->IsAPlayer()) int uid = 0;
        RegisterNpc(this, uid);      
    }

    /*
    HOOK Hook_oCNpc_Archive PATCH(&oCNpc::Archive, &oCNpc::Archive_StExt);
    void oCNpc::Archive_StExt(zCArchiver& ar)
    {
        THISCALL(Hook_oCNpc_Archive)(ar);
        if (!IsLevelChanging) RegisterNpc(this);
    }
    */

    HOOK Hook_oCNpc_Unarchive PATCH(&oCNpc::Unarchive, &oCNpc::Unarchive_StExt);
    void oCNpc::Unarchive_StExt(zCArchiver& ar)
    {
        THISCALL(Hook_oCNpc_Unarchive)(ar);
        UnArchiveAdditionalArmors(this);
    }

    HOOK Hook_oCNpc_ProcessNpc PATCH(&oCNpc::ProcessNpc, &oCNpc::ProcessNpc_StExt);
    void oCNpc::ProcessNpc_StExt()
    {
        THISCALL(Hook_oCNpc_ProcessNpc)();

        if (this && !this->IsDead())
        {
            parser->SetInstance(StExt_ModSelf_SymId, this);
            parser->CallFunc(StExt_OnAiStateFunc);
        }
    }

    HOOK Hook_oCNpc_OpenInventory PATCH(&oCNpc::OpenInventory, &oCNpc::OpenInventory_StExt);
    void oCNpc::OpenInventory_StExt(int mode)
    {
        if (this && this->HasBodyStateModifier(BS_MOD_BURNING)) {
            this->ModifyBodyState(0, BS_MOD_BURNING);
        }
        THISCALL(Hook_oCNpc_OpenInventory)(mode);
    }
}
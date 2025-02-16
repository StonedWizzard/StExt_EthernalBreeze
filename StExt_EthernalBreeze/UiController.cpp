#include <UnionAfx.h>
#include <string> 
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    zCOLOR DefaultColor = zCOLOR(235, 235, 235);
    zCOLOR HoveredColor = zCOLOR(255, 255, 235);
    zCOLOR SelectedColor = zCOLOR(240, 240, 0);

    zCOLOR WhiteColor = zCOLOR(250, 250, 250);
    static oCViewStatusBar* esBar = NULL;
    static oCViewStatusBar* npcEsBar = NULL;
    static zCView* esBarTextView;
    static zCView* focusIcon = NULL;
    static oCNpc* FocusNpc = NULL;
    static oCNpcEx* FocusNpcEx = NULL;
    zSTRING EsText;
    zSTRING FocusRankName, FocusFlags, FocusAbilities, FocusStats, FocusExtraStats, FocusUidStr, FocusName;
    zSTRING PcEsCurStr;

    static int updateDalay;
    static int icon_space = 1;    
    static int focusRank, focusYOffset;
    int PcEsPosX;
    int PcEsPosY;
    int ShowPcEs;
    int ShowModMenu;
    int BlockMovement;

    void UpdateUiStatus()
    {
        if (ogame && player && (!IsLevelChanging || !IsLoading))
        {
            parser->CallFunc(UpdateUiStatusFunc);
            ShowPcEs = parser->GetSymbol("StExt_CanShowPcEsBar")->single_intdata;
            ShowModMenu = parser->GetSymbol("StExt_CanShowModMenu")->single_intdata;
            BlockMovement = parser->GetSymbol("StExt_BlockPcMovement")->single_intdata;            
        }
        else
        {
            BlockMovement = false;
            ShowPcEs = false;
            ShowModMenu = false;
        }
    }

    void StonedExtension_InitUi()
    {
        EsText = parser->GetSymbol("StExt_EsText")->stringdata;
        StonedExtension_InitUi_StatMenu();
    }

    void InitHeroEsBar()
    {
        esBar = new oCViewStatusBar();
        esBar->Init(5, 5, 1.0);
        esBar->SetMaxRange(0, 100);
        esBar->SetTextures("BAR_BACK.TGA", "BAR_TEMPMAX.TGA", "BAR_ES.TGA", "BAR_EMPTY.TGA");
        esBar->scale = 1.0f;
        screen->InsertItem(esBar);
        esBar->Render();

        esBarTextView = new zCView(0, 0, 8192, 8192);
        screen->InsertItem(esBarTextView, FALSE);
        esBarTextView->SetPos(0, 0);
        screen->RemoveItem(esBarTextView);

        PcEsCurStr = zSTRING();
    }

    void InitNpcEsBar()
    {
        npcEsBar = new oCViewStatusBar();
        npcEsBar->Init(5, 5, 1.0);
        npcEsBar->SetMaxRange(0, 100);
        npcEsBar->SetTextures("BAR_BACK_NEW.TGA", "BAR_TEMPMAX_NEW.TGA", "BAR_ES_NPC.TGA", "BAR_EMPTY.TGA");       
        npcEsBar->SetSize(512, 180);
        int x, y;
        npcEsBar->GetSize(x, y);
        //npcEsBar->SetPos((8192 - x) * 0.5f, screen->FontY() * (1 + StExt_Config_NpcStats_TopOffset));
        npcEsBar->SetPos((8192 - x) * 0.5f, screen->FontY());
        screen->InsertItem(npcEsBar);
        npcEsBar->Render();
    }

    void PrintHeroEsBar()
    {
        if (screen && ShowPcEs)
        {
            int sx, sy;
            int x, y;
            oCNpcEx* heroEx;

            if (!esBar || !esBarTextView)
                InitHeroEsBar();
            screen->RemoveItem(esBar);
            screen->RemoveItem(esBarTextView);            

            heroEx = dynamic_cast<oCNpcEx*>(player); 
            if (heroEx)
            {
                int esCur = heroEx->m_pVARS[StExt_AiVar_EsCur];
                int esMax = heroEx->m_pVARS[StExt_AiVar_EsMax];
                int defaultPos = parser->GetSymbol("StExt_Config_EsBar_DefaultPos")->single_intdata;

                if (esMax <= 0)
                {
                    esBar->ondesk = false;
                    screen->RemoveItem(esBar);
                    screen->RemoveItem(esBarTextView);
                    return;
                }
                
                ogame->hpBar->GetSize(sx, sy);
                esBar->SetSize(sx, sy);
                
                if (defaultPos)
                {
                    ogame->hpBar->GetPos(x, y);
                    x += sx + 15;
                }                    
                else
                {
                    x = parser->GetSymbol("StExt_Config_EsBar_PosX")->single_intdata * 81.919998f;
                    y = parser->GetSymbol("StExt_Config_EsBar_PosY")->single_intdata * 81.919998f;
                }
                esBar->SetPos(x, y);
                screen->InsertItem(esBar);
                screen->InsertItem(esBarTextView);

                PcEsCurStr = StExt_EsText + Z": " + Z esCur + Z"/" + Z esMax;
                int PcEsPosX = x;
                int PcEsPosY = y - sy + 3;
                esBarTextView->SetFontColor(WhiteColor);
                esBarTextView->Print(PcEsPosX, PcEsPosY, PcEsCurStr);

                esBar->SetMaxRange(0, esMax);
                esBar->SetRange(0, esMax);
                esBar->SetValue(esCur);
            }
            else
            {
                screen->RemoveItem(esBarTextView);
                screen->RemoveItem(esBar);
                return;
            } 
        }
        else
        {
            if (esBarTextView) screen->RemoveItem(esBarTextView);
            if (esBar) screen->RemoveItem(esBar);
        }
    }

    inline void PrintNpcEs()
    {
        if (!npcEsBar)
            InitNpcEsBar();
        screen->RemoveItem(npcEsBar);

        int esCur;
        int esMax;

        if (FocusNpcEx)
        {
            esCur = FocusNpcEx->m_pVARS[StExt_AiVar_EsCur];
            esMax = FocusNpcEx->m_pVARS[StExt_AiVar_EsMax];
        }
        else
        {
            esCur = *(int*)parser->CallFunc(NpcGetBarCurEsFunc);
            esMax = *(int*)parser->CallFunc(NpcGetBarMaxEsFunc);            
        }

        if ((esMax <= 0) || FocusNpc->IsDead())
        {
            npcEsBar->ondesk = false;
            if(npcEsBar)
                screen->RemoveItem(npcEsBar);
            return;
        }                
        screen->InsertItem(npcEsBar);
        npcEsBar->SetMaxRange(0, esMax);
        npcEsBar->SetRange(0, esMax);
        npcEsBar->SetValue(esCur);

        zSTRING esText = StExt_EsText + ": " + zSTRING(esCur) + "/" + zSTRING(esMax);
        focusYOffset = (screen->FontY() * (2 + StExt_Config_NpcStats_TopOffset)) + npcEsBar->pposy + npcEsBar->psizey;
        screen->Print((8192 - screen->FontSize(esText)) * 0.5f, focusYOffset, esText);        
        focusYOffset += screen->FontY();
    }

    inline CArray<zSTRING> SplitAbilitiesString(zSTRING str)
    {
        CArray<zSTRING> result = CArray<zSTRING>();
        std::string source = str.ToChar();
        std::string tmp;
        zSTRING tmpResult = zSTRING();
        if (source.length() == 0)
            return result;

        for (int i = 0, j = 0; i < source.length(); i++)
        {
            if (source[i] == '|')
            {
                tmp = source.substr(j, i - 1);
                j = i + 1;
                tmpResult += tmp.c_str();
                if (screen->FontSize(tmpResult) >= 6144)
                {
                    tmp = tmpResult.ToChar();
                    result.InsertEnd(tmp.c_str());
                    tmpResult = zSTRING();
                }
            }
        }
        return result;
    }

    inline void PrintNpcInfo()
    {
        if (FocusNpc)
        {
            // update npc info
            parser->CallFunc(UpdateFocusNpcInfoFunc);
            if (FocusNpcEx)
                focusRank = FocusNpcEx->m_pVARS[StExt_AiVar_IsRandomized];
            else focusRank = parser->GetSymbol("StExt_FocusNpcRank")->single_intdata;

            zCPar_Symbol* infoArray = parser->GetSymbol("StExt_FocusNpcInfo");
            FocusRankName = infoArray->stringdata[0];
            FocusFlags = infoArray->stringdata[1];
            FocusAbilities = infoArray->stringdata[2];
            FocusStats = infoArray->stringdata[3];
            FocusExtraStats = infoArray->stringdata[4];            

            if (!FocusRankName) FocusRankName = "";
            if (!FocusFlags) FocusFlags = "";
            if (!FocusAbilities) FocusAbilities = "";
            if (!FocusStats) FocusStats = "";
            if (!FocusExtraStats) FocusExtraStats = "";

            if (focusRank > 0)
            {
                if (focusRank == 1)
                    screen->SetFontColor(zCOLOR(200, 128, 255));
                else if (focusRank == 2)
                    screen->SetFontColor(zCOLOR(64, 64, 252));
                else if (focusRank == 3)
                    screen->SetFontColor(zCOLOR(255, 255, 64));
                else if (focusRank == 4)
                    screen->SetFontColor(zCOLOR(255, 128, 0));
                else if (focusRank >= 5)
                    screen->SetFontColor(zCOLOR(255, 32, 32));
                else
                    screen->SetFontColor(zCOLOR(250, 250, 250));

                zSTRING summLine = FocusRankName + " " + FocusFlags;
                int center = (8192 - screen->FontSize(summLine)) * 0.5f;
                screen->Print(center, screen->FontY() + focusYOffset, FocusRankName);
                screen->SetFontColor(zCOLOR(250, 250, 250));
                screen->Print(center + screen->FontSize(FocusRankName + " "), screen->FontY() + focusYOffset, FocusFlags);
                focusYOffset += screen->FontY();
            }
            if (FocusAbilities && (FocusAbilities.Length() > 0))
            {
                screen->SetFontColor(zCOLOR(220, 220, 220));
                int abilitiesWidth = screen->FontSize(FocusAbilities);
                if (abilitiesWidth > 8000)
                {
                    CArray<zSTRING> abilitiesLines = SplitAbilitiesString(FocusAbilities);
                    for (uint i = 0U; i < abilitiesLines.GetNum(); i++)
                    {
                        screen->Print((8192 - screen->FontSize(abilitiesLines[i])) * 0.5f, screen->FontY() + focusYOffset, abilitiesLines[i]);
                        focusYOffset += screen->FontY();
                    }
                }
                else
                {
                    screen->Print((8192 - abilitiesWidth) * 0.5f, screen->FontY() + focusYOffset, FocusAbilities);
                    focusYOffset += screen->FontY();
                }                
            }
            
            focusYOffset += screen->FontY() * 0.5f;
            if (FocusStats && (FocusStats.Length() > 0))
            {
                screen->SetFontColor(zCOLOR(200, 200, 200));
                screen->Print((8192 - screen->FontSize(FocusStats)) * 0.5f, screen->FontY() + focusYOffset, FocusStats);
                focusYOffset += screen->FontY();
            }
            if (FocusExtraStats && (FocusExtraStats.Length() > 0))
            {
                screen->SetFontColor(zCOLOR(200, 200, 200));
                screen->Print((8192 - screen->FontSize(FocusExtraStats)) * 0.5f, screen->FontY() + focusYOffset, FocusExtraStats);
                focusYOffset += screen->FontY();
            }
            if (parser->GetSymbol("StExt_Config_DebugEnabled")->single_intdata)
            {
                screen->SetFontColor(zCOLOR(250, 250, 250));
                FocusUidStr = "'" + FocusName + "' [" + Z GetNpcUid(FocusNpc) + "]";
                screen->Print((8192 - screen->FontSize(FocusUidStr)) * 0.5f, screen->FontY() + focusYOffset, FocusUidStr);
                focusYOffset += screen->FontY();
            }
            screen->SetFontColor(zCOLOR(250, 250, 250));
        }
    }

    inline void ClearFocusNpc()
    {
        FocusNpc = Null;
        FocusNpcEx = Null;
        FocusName = zSTRING();
        if (npcEsBar)
            screen->RemoveItem(npcEsBar);
    }

    HOOK Hook_oCGame_UpdatePlayerStatus PATCH(&oCGame::UpdatePlayerStatus, &oCGame::UpdatePlayerStatus_StExt);
    void oCGame::UpdatePlayerStatus_StExt()
    {
        THISCALL(Hook_oCGame_UpdatePlayerStatus)();
        if (!ogame || !screen || !player) return;

        if (player->focus_vob)
        {
            focusYOffset = screen->FontY() * (1 + StExt_Config_NpcStats_TopOffset);
            FocusNpc = dynamic_cast<oCNpc*>(player->focus_vob);
            if (FocusNpc)
            {
                FocusNpcEx = dynamic_cast<oCNpcEx*>(FocusNpc);
                FocusName = FocusNpc->name;
                parser->SetInstance("STEXT_FOCUSNPC", FocusNpc);
                PrintNpcEs();
                PrintNpcInfo();
                if (FocusNpc->IsDead()) ClearFocusNpc();
            }
            else
                ClearFocusNpc();
        }
        else { ClearFocusNpc(); }        
    }
}
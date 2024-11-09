#include <UnionAfx.h>
#include <string> 
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    static oCViewStatusBar* esBar = NULL;
    static oCViewStatusBar* npcEsBar = NULL;
    static zCView* focusIcon = NULL;
    static oCNpc* FocusNpc = NULL;
    zSTRING EsText;
    zSTRING FocusRankName, FocusFlags, FocusAbilities, FocusStats, FocusExtraStats, FocusUidStr, FocusName;

    static int icon_space = 1;    
    static int focusRank, focusYOffset;

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
        screen->InsertItem(esBar);
        esBar->Render();
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
        npcEsBar->SetPos((8192 - x) * 0.5f, screen->FontY());
        screen->InsertItem(npcEsBar);
        npcEsBar->Render();
    }

    inline void PrintHeroEsBar()
    {
        if (ogame && screen && ogame->hpBar)
        {
            int sx, sy;
            int x, y;
            if (!esBar)
                InitHeroEsBar();
            screen->RemoveItem(esBar);

            if (ogame->hpBar->ondesk)
            {
                zSTRING esStr = parser->GetSymbol("StExt_EsText")->stringdata;
                int esCur = *(int*)parser->CallFunc(NpcGetBarCurEsFunc);
                int esMax = *(int*)parser->CallFunc(NpcGetBarMaxEsFunc);
                int defaultPos = parser->GetSymbol("StExt_Config_EsBar_DefaultPos")->single_intdata;

                if (esMax <= 0)
                {
                    esBar->ondesk = false;
                    screen->RemoveItem(esBar);
                    return;
                }
                
                ogame->hpBar->GetSize(sx, sy);
                esBar->SetSize(sx, sy);
                esBar->scale = ogame->hpBar->scale;
                if (defaultPos)
                {
                    ogame->hpBar->GetPos(x, y);
                    x += sx + 15;
                    esBar->SetPos(x, y);
                    
                }                    
                else
                {
                    x = parser->GetSymbol("StExt_Config_EsBar_PosX")->single_intdata * 81.919998f;
                    y = parser->GetSymbol("StExt_Config_EsBar_PosY")->single_intdata * 81.919998f;
                    esBar->SetPos(x, y);
                }                
                screen->InsertItem(esBar);

                esStr += ": " + Z(int)esCur + "/" + Z(int)esMax;
                screen->Print(x, y - sy + 3, esStr);

                esBar->SetMaxRange(0, esMax);
                esBar->SetRange(0, esMax);
                esBar->SetValue(esCur);
            }
            else screen->RemoveItem(esBar);
        }
    }

    inline void PrintNpcEs()
    {
        if (!npcEsBar)
            InitNpcEsBar();
        screen->RemoveItem(npcEsBar);

        int esCur = *(int*)parser->CallFunc(NpcGetBarCurEsFunc);
        int esMax = *(int*)parser->CallFunc(NpcGetBarMaxEsFunc);

        if ((esMax <= 0) || FocusNpc->IsDead() || !ogame->hpBar->ondesk)
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

        zSTRING esText = EsText + ": " + zSTRING(esCur) + "/" + zSTRING(esMax);
        focusYOffset = (screen->FontY() * 2.0f) + npcEsBar->pposy + npcEsBar->psizey;
        screen->Print((8192 - screen->FontSize(esText)) * 0.5f, focusYOffset, esText);
        focusYOffset += screen->FontY();
    }

    inline CArray<zSTRING> SplitAbilitiesString(zSTRING str)
    {
        CArray<zSTRING> result = CArray<zSTRING>();
        std::string source = str.ToChar();
        std::string tmp;
        zSTRING tmpResult = zSTRING();
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
            focusRank = parser->GetSymbol("StExt_FocusNpcRank")->single_intdata;            
            
            if (FocusNpc->IsDead())
                return;

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
                    for (unsigned int i = 0; i < abilitiesLines.GetNum(); i++)
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

    void ClearFocusNpc()
    {
        FocusNpc = Null;
        FocusName = zSTRING();
        if (npcEsBar)
            screen->RemoveItem(npcEsBar);
    }

    HOOK Hook_oCGame_UpdatePlayerStatus PATCH(&oCGame::UpdatePlayerStatus, &oCGame::UpdatePlayerStatus_StExt);
    void oCGame::UpdatePlayerStatus_StExt()
    {
        THISCALL(Hook_oCGame_UpdatePlayerStatus)();
        if (ogame && screen && player)
        {
            parser->SetInstance("StExt_FocusNpc", player);
            PrintHeroEsBar();
            if (player->focus_vob && ogame->hpBar && ogame->hpBar->ondesk)
            {
                focusYOffset = screen->FontY();
                FocusNpc = dynamic_cast<oCNpc*>(player->focus_vob);
                if (FocusNpc)
                {
                    FocusName = FocusNpc->name;
                    parser->SetInstance("StExt_FocusNpc", FocusNpc);
                    PrintNpcEs();
                    PrintNpcInfo();
                    if (FocusNpc->IsDead())
                        ClearFocusNpc();
                }
                else
                    ClearFocusNpc();
            }
            else
                ClearFocusNpc();
        }
    }
}
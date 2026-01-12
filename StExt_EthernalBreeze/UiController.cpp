#include <StonedExtension.h>

// Here is entry point for whole mod ui part.
namespace Gothic_II_Addon
{
    constexpr int MaxTextWidth = 6200;
    constexpr int MaxTextScreenWidth = 7800;
    const zCOLOR NpcRankColor[] =
    {
        zCOLOR(250, 250, 250),
        zCOLOR(200, 128, 255),
        zCOLOR(64, 64, 252),
        zCOLOR(255, 255, 64),
        zCOLOR(255, 128, 0),
        zCOLOR(255, 32, 32)
    };

    oCViewStatusBar* esBar = Null;
    oCViewStatusBar* npcEsBar = Null;
    oCNpc* FocusNpc = Null;
    oCNpcEx* FocusNpcEx = Null;
    Array<zSTRING> abilitiesLines = Array<zSTRING>();

    zSTRING EsText;
    int PcEsPosX_SymIndex = Invalid, PcEsPosY_SymIndex = Invalid; 
    int EsBarDefaultPos_SymIndex = Invalid, CanShowPcEsBar_SymIndex = Invalid, CanShowModMenu_SymIndex = Invalid, BlockPcMovement_SymIndex = Invalid;
    int FocusNpcRank_SymIndex = Invalid, FocusNpcInfo_SymIndex = Invalid;
   
    int focusRank, focusYOffset;
    zSTRING FocusName;
    zSTRING PcEsCurStr;

    int ShowPcEs;
    int CanShowModMenu;
    int BlockMovement;


    void UpdateUiStatus()
    {
        if (ogame && player && (!IsLevelChanging || !IsLoading))
        {
            parser->CallFunc(UpdateUiStatusFunc);
            ShowPcEs = parser->GetSymbol(CanShowPcEsBar_SymIndex)->single_intdata;
            CanShowModMenu = parser->GetSymbol(CanShowModMenu_SymIndex)->single_intdata;
            BlockMovement = parser->GetSymbol(BlockPcMovement_SymIndex)->single_intdata;
        }
        else
        {
            BlockMovement = false;
            ShowPcEs = false;
            CanShowModMenu = false;
        }
    }

    void StonedExtension_InitUi()
    {
        DEBUG_MSG("StExt - Initialize mod Ui...");
        EsText = parser->GetSymbol("StExt_EsText")->stringdata;
        abilitiesLines = Array<zSTRING>();

        PcEsPosX_SymIndex = parser->GetIndex("StExt_Config_EsBar_PosX");
        DEBUG_MSG_IF(PcEsPosX_SymIndex == Invalid, "PcEsPosX_SymIndex is null!");
        PcEsPosY_SymIndex = parser->GetIndex("StExt_Config_EsBar_PosY");
        DEBUG_MSG_IF(PcEsPosY_SymIndex == Invalid, "PcEsPosY_SymIndex is null!");

        EsBarDefaultPos_SymIndex = parser->GetIndex("StExt_Config_EsBar_DefaultPos");
        DEBUG_MSG_IF(EsBarDefaultPos_SymIndex == Invalid, "EsBarDefaultPos_SymIndex is null!");

        CanShowModMenu_SymIndex = parser->GetIndex("StExt_CanShowModMenu");
        DEBUG_MSG_IF(CanShowModMenu_SymIndex == Invalid, "CanShowModMenu_SymIndex is null!");
        CanShowPcEsBar_SymIndex = parser->GetIndex("StExt_CanShowPcEsBar");
        DEBUG_MSG_IF(CanShowPcEsBar_SymIndex == Invalid, "CanShowPcEsBar_SymIndex is null!");
        BlockPcMovement_SymIndex = parser->GetIndex("StExt_BlockPcMovement");
        DEBUG_MSG_IF(BlockPcMovement_SymIndex == Invalid, "BlockPcMovement_SymIndex is null!");

        FocusNpcRank_SymIndex = parser->GetIndex("StExt_FocusNpcRank");
        DEBUG_MSG_IF(FocusNpcRank_SymIndex == Invalid, "FocusNpcRank_SymIndex is null!");
        FocusNpcInfo_SymIndex = parser->GetIndex("StExt_FocusNpcInfo");
        DEBUG_MSG_IF(FocusNpcInfo_SymIndex == Invalid, "FocusNpcInfo_SymIndex is null!");

        MsgTrayPosX_SymIndex = parser->GetIndex("StExt_Config_MsgTray_PosX");
        DEBUG_MSG_IF(MsgTrayPosX_SymIndex == Invalid, "MsgTrayPosX_SymIndex is null!");
        MsgTrayPosY_SymIndex = parser->GetIndex("StExt_Config_MsgTray_PosY");
        DEBUG_MSG_IF(MsgTrayPosY_SymIndex == Invalid, "MsgTrayPosY_SymIndex is null!");

        StonedExtension_InitUi_Menu();
        DEBUG_MSG("StExt - Mod Ui was initialized!");
    }

    inline void DeleteUiItem(zCView* uiItem)
    {
        if (screen && uiItem)
            screen->RemoveItem(uiItem);
        SAFE_DELETE(uiItem);
    }

    void InitHeroEsBar()
    {
        DeleteUiItem(esBar);

        esBar = new oCViewStatusBar();
        esBar->Init(5, 5, 1.0);
        esBar->SetMaxRange(0, 100);
        esBar->SetTextures("BAR_BACK.TGA", "BAR_TEMPMAX.TGA", "BAR_ES.TGA", "BAR_EMPTY.TGA");
        esBar->scale = 1.0f;
        screen->InsertItem(esBar);
        esBar->Render();

        PcEsCurStr = zString_Empty;
    }

    void InitNpcEsBar()
    {
        DeleteUiItem(npcEsBar);

        npcEsBar = new oCViewStatusBar();
        npcEsBar->Init(5, 5, 1.0);
        npcEsBar->SetMaxRange(0, 100);
        npcEsBar->SetTextures("BAR_BACK_NEW.TGA", "BAR_TEMPMAX_NEW.TGA", "BAR_ES_NPC.TGA", "BAR_EMPTY.TGA");       
        npcEsBar->SetSize(512, 180);
        int x, y;
        npcEsBar->GetSize(x, y);
        npcEsBar->SetPos(static_cast<int>((ScreenVBufferSize - x) * 0.5f), screen->FontY());
        screen->InsertItem(npcEsBar);
        npcEsBar->Render();
    }


    inline const zCOLOR& GetNpcRankColor(const int rank) { return IsIndexInBounds(rank, 6) ? NpcRankColor[rank] : NpcRankColor[0]; }
    inline int GetScreenLineWidth(const zSTRING& txt) { return (screen && screen->font) ? screen->font->GetFontX(txt) : 0; }

    inline void PrintScreenTextContent(const int x, const int y, const zSTRING& txt, const zCOLOR& color)
    {
        if (!screen) return;
        screen->SetFontColor(color);
        screen->Print(x, y, txt);
        screen->SetFontColor(TextColor_Default);
    }

    inline void PrintScreenTextContent(const int y, const zSTRING& txt, const zCOLOR& color)
    {
        if (!screen) return;
        const int x = static_cast<int>((ScreenVBufferSize - GetScreenLineWidth(txt)) * 0.5f);
        PrintScreenTextContent(x, y, txt, color);
    }

    void PrintHeroEsBar()
    {
        int sx, sy;
        int x, y;
        oCNpcEx* heroEx;

        if (!screen || !ShowPcEs)
        {
            if (esBar)
                screen->RemoveItem(esBar);
            return;
        }

        if (!esBar)
            InitHeroEsBar();
        screen->RemoveItem(esBar);

        heroEx = dynamic_cast<oCNpcEx*>(player); 
        if (heroEx)
        {
            int esCur = heroEx->m_pVARS[StExt_AiVar_EsCur];
            int esMax = heroEx->m_pVARS[StExt_AiVar_EsMax];
            int defaultPos = parser->GetSymbol(EsBarDefaultPos_SymIndex)->single_intdata;

            if (esMax <= 0)
            {
                esBar->ondesk = false;
                screen->RemoveItem(esBar);
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
                x = static_cast<int>(parser->GetSymbol(PcEsPosX_SymIndex)->single_intdata * 81.919998f);
                y = static_cast<int>(parser->GetSymbol(PcEsPosY_SymIndex)->single_intdata * 81.919998f);
            }
            esBar->SetPos(x, y);
            screen->InsertItem(esBar);

            PcEsCurStr = StExt_EsText + ": " + Z(esCur) + "/" + Z(esMax);
            PrintScreenTextContent(x, y - sy + 3, PcEsCurStr, TextColor_Default);

            esBar->SetMaxRange(0.0f, static_cast<float>(esMax));
            esBar->SetRange(0.0f, static_cast<float>(esMax));
            esBar->SetValue(static_cast<float>(esCur));
        }
        else
        {
            screen->RemoveItem(esBar);
            return;
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

        if ((esMax <= 0) || (FocusNpc && FocusNpc->IsDead()))
        {
            npcEsBar->ondesk = false;
            if(npcEsBar) screen->RemoveItem(npcEsBar);
            return;
        }                
        screen->InsertItem(npcEsBar);
        npcEsBar->SetMaxRange(0.0, static_cast<float>(esMax));
        npcEsBar->SetRange(0.0, static_cast<float>(esMax));
        npcEsBar->SetValue(static_cast<float>(esCur));

        const zSTRING esText = StExt_EsText + ": " + Z(esCur) + "/" + Z(esMax);
        const int startPos = npcEsBar->pposy + npcEsBar->psizey;
        const int fontY = screen->FontY();

        focusYOffset = startPos + (fontY * (2 + StExt_Config_NpcStats_TopOffset));
        PrintScreenTextContent(focusYOffset, esText, TextColor_Default);
        focusYOffset += fontY;
    }


    inline zSTRING MakeSegmentFromRange(const char* begin, const char* end)
    {
        const size_t len = end - begin;
        if (len == 0) return zString_Empty;

        if (len <= 1023)
        {
            char buf[1024];
            memcpy(buf, begin, len);
            buf[len] = '\0';
            return zSTRING(buf);
        }
        else
        {
            std::string tmp(begin, len);
            return zSTRING(tmp.c_str());
        }
    }
    inline void SplitAbilitiesString(const zSTRING& str, Array<zSTRING>& lines)
    {
        if (str.IsEmpty()) return;

        const char* src = str.ToChar();
        if (!src) return;

        zSTRING currentLine;
        const char* segmentStart = src;
        static const zSTRING tagSeparator(" | ");

        for (const char* p = src; ; ++p)
        {
            if (*p == '|' || *p == '\0')
            {
                zSTRING segment = MakeSegmentFromRange(segmentStart, p);
                if (!segment.IsEmpty())
                {
                    zSTRING testLine = currentLine;
                    if (!testLine.IsEmpty()) testLine += tagSeparator;                    
                    testLine += segment;

                    if (GetScreenLineWidth(testLine) > MaxTextWidth)
                    {
                        if (!currentLine.IsEmpty()) 
                            lines.InsertEnd(currentLine);
                        currentLine = segment;
                    }
                    else
                    {
                        if (!currentLine.IsEmpty()) 
                            currentLine += tagSeparator;
                        currentLine += segment;
                    }
                }
                if (*p == '\0') break;
                segmentStart = p + 1;
            }
        }

        if (!currentLine.IsEmpty())
            lines.InsertEnd(currentLine);
    }

    inline void PrintNpcInfo()
    {
        if (FocusNpc)
        {
            parser->CallFunc(UpdateFocusNpcInfoFunc);
            if (FocusNpcEx) focusRank = FocusNpcEx->m_pVARS[StExt_AiVar_IsRandomized];
            else focusRank = parser->GetSymbol(FocusNpcRank_SymIndex)->single_intdata;

            zCPar_Symbol* infoArray = parser->GetSymbol(FocusNpcInfo_SymIndex);
            const zSTRING& focusRankName = infoArray->stringdata[0];
            const zSTRING& focusFlags = infoArray->stringdata[1];
            const zSTRING& focusAbilities = infoArray->stringdata[2];
            const zSTRING& focusStats = infoArray->stringdata[3];
            const zSTRING& focusExtraStats = infoArray->stringdata[4];

            const int fontY = screen->FontY();

            if (focusRank > 0)
            {
                const zCOLOR& rankColor = GetNpcRankColor(focusRank);
                const zSTRING summLine = focusRankName + " " + focusFlags;
                const int y = fontY + focusYOffset;
                const int x = static_cast<int>((ScreenVBufferSize - GetScreenLineWidth(summLine)) * 0.5f);

                PrintScreenTextContent(x, y, focusRankName, rankColor);
                PrintScreenTextContent(x + GetScreenLineWidth(focusRankName + " "), y, focusFlags, TextColor_Default);
                focusYOffset += fontY;
            }

            if (!focusAbilities.IsEmpty())
            {
                const int abilitiesWidth = GetScreenLineWidth(focusAbilities);
                if (abilitiesWidth > MaxTextScreenWidth)
                {
                    abilitiesLines.Clear();
                    SplitAbilitiesString(focusAbilities, abilitiesLines);
                    for (uint i = 0U; i < abilitiesLines.GetNum(); ++i)
                    {
                        PrintScreenTextContent(fontY + focusYOffset, abilitiesLines[i], TextColor_Regular_Faded);
                        focusYOffset += fontY;
                    }
                }
                else
                {
                    PrintScreenTextContent(fontY + focusYOffset, focusAbilities, TextColor_Regular_Faded);
                    focusYOffset += fontY;
                }                
            }
            
            focusYOffset += static_cast<int>(fontY * 0.5f);
            if (!focusStats.IsEmpty())
            {
                PrintScreenTextContent(fontY + focusYOffset, focusStats, TextColor_Regular_Faded);
                focusYOffset += fontY;
            }

            if (!focusExtraStats.IsEmpty())
            {
                PrintScreenTextContent(fontY + focusYOffset, focusExtraStats, TextColor_Regular_Faded);
                focusYOffset += fontY;
            }

            #if DebugEnabled
            if (parser->GetSymbol("StExt_Config_DebugEnabled")->single_intdata)
            {
                const zSTRING debugStr = "'" + FocusName + "' [" + Z(GetNpcUid(FocusNpc)) + "]";
                PrintScreenTextContent(fontY + focusYOffset, debugStr, TextColor_Warn);
                focusYOffset += fontY;
            }
            #endif
            screen->SetFontColor(TextColor_Default);
        }
    }

    inline void ClearFocusNpc()
    {
        FocusNpc = Null;
        FocusNpcEx = Null;
        FocusName = zString_Empty;
        if (npcEsBar)
            screen->RemoveItem(npcEsBar);
    }

    void StonedExtension_Loop_Ui()
    {
        StonedExtension_Loop_MenuController();
        StonedExtension_Loop_MsgTray();
        PrintHeroEsBar();
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
            else { ClearFocusNpc(); }
        }
        else { ClearFocusNpc(); }        
    }
}
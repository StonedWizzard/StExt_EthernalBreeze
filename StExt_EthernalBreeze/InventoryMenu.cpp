#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    const int BorderXOffset = 128;
    const int CenterX = 8192 * 0.5f;
    const zSTRING StatTextWhiteSpace = " ";

    std::map<int, int> StatsTypeMap =
    {
        {0, Value_Type_Default},
        {1, Value_Type_Default},
        {2, Value_Type_Default},
        {3, Value_Type_Default},
        {4, Value_Type_Default},
        {5, Value_Type_Default},
        {6, Value_Type_Default},
        {7, Value_Type_Default},
        {8, Value_Type_Default},
        {9, Value_Type_DefaultPerc},
        {10, Value_Type_DefaultPerc},
        {11, Value_Type_DefaultPerc},
        {12, Value_Type_DefaultPerc},
        {13, Value_Type_DefaultPerc},
        {14, Value_Type_Percent},
        {15, Value_Type_Percent},
        {16, Value_Type_Percent},
        {17, Value_Type_Percent},
        {18, Value_Type_Percent},
        {19, Value_Type_Percent},
        {20, Value_Type_Percent},
        {21, Value_Type_Percent},
        {22, Value_Type_Percent},
        {23, Value_Type_Default},
        {24, Value_Type_Percent},
        {25, Value_Type_Default},
        {26, Value_Type_Percent},
        {27, Value_Type_Percent},
        {28, Value_Type_Default},
        {29, Value_Type_Default},
        {30, Value_Type_Default},
        {31, Value_Type_Default},
        {32, Value_Type_Default},
        {33, Value_Type_Default},
        {34, Value_Type_Percent},
        {35, Value_Type_Percent},
        {36, Value_Type_Percent},
        {37, Value_Type_Percent},
        {38, Value_Type_Percent},
        {39, Value_Type_Percent},
        {40, Value_Type_Percent},
        {41, Value_Type_Percent},
        {42, Value_Type_Percent},
        {43, Value_Type_Percent},
        {44, Value_Type_Percent},
        {45, Value_Type_Percent},
        {46, Value_Type_Percent},
        {47, Value_Type_Percent},
        {48, Value_Type_Percent},
        {49, Value_Type_Percent},
        {50, Value_Type_Percent},
        {51, Value_Type_Percent},
        {52, Value_Type_Percent},
        {53, Value_Type_Percent},
        {54, Value_Type_Percent},
        {55, Value_Type_Percent},
        {56, Value_Type_Percent},
        {57, Value_Type_Percent},
        {58, Value_Type_Percent},
        {59, Value_Type_Percent},
        {60, Value_Type_Percent},
        {61, Value_Type_Percent},
        {62, Value_Type_YesNo},
        {63, Value_Type_YesNo},
        {64, Value_Type_YesNo},
        {65, Value_Type_Percent},
        {66, Value_Type_Percent},
        {67, Value_Type_Percent},
        {68, Value_Type_Percent},
        {69, Value_Type_Default},
        {70, Value_Type_Default},
        {71, Value_Type_Default},
        {72, Value_Type_Default},
        {73, Value_Type_Percent},
        {74, Value_Type_Percent},
        {75, Value_Type_Percent},
        {76, Value_Type_Percent},
        {77, Value_Type_Default},
        {78, Value_Type_Percent},
        {79, Value_Type_Default},
        {80, Value_Type_Default},
        {81, Value_Type_Default},
        {82, Value_Type_Default},
        {83, Value_Type_Default},
        {84, Value_Type_Default},
        {85, Value_Type_Default},
        {86, Value_Type_Default},
        {87, Value_Type_Default},
        {88, Value_Type_Default},
        {89, Value_Type_Default},
        {90, Value_Type_Default},
        {91, Value_Type_Default},
        {92, Value_Type_Default},
        {93, Value_Type_Default},
        {94, Value_Type_Default},
        {95, Value_Type_Default},
        {96, Value_Type_Default},
        {97, Value_Type_Default},
        {98, Value_Type_Default},
        {99, Value_Type_Percent},
        {100, Value_Type_Percent},
        {101, Value_Type_Percent},
        {102, Value_Type_Percent},
        {103, Value_Type_Percent},
        {104, Value_Type_Percent},
        {105, Value_Type_Percent},
        {106, Value_Type_Percent},
        {107, Value_Type_Percent},
        {108, Value_Type_Percent},
        {109, Value_Type_Percent},
        {110, Value_Type_Percent},
        {111, Value_Type_Percent},
        {112, Value_Type_Percent},
        {113, Value_Type_Percent},
        {114, Value_Type_Percent},
        {115, Value_Type_Percent},
        {116, Value_Type_Percent},
        {117, Value_Type_Percent},
        {118, Value_Type_Percent},
        {119, Value_Type_Percent},
        {120, Value_Type_Percent},
        {121, Value_Type_Percent},
        {122, Value_Type_Percent},
        {123, Value_Type_Percent},
        {124, Value_Type_Percent},
        {125, Value_Type_Percent},
        {126, Value_Type_Percent},
        {127, Value_Type_Percent},
        {128, Value_Type_Percent},
        {129, Value_Type_Percent},
        {130, Value_Type_Percent},
        {131, Value_Type_Percent},
        {132, Value_Type_Percent},
        {133, Value_Type_Percent},
        {134, Value_Type_Percent},
        {135, Value_Type_Percent},
        {136, Value_Type_Percent},
        {137, Value_Type_Percent},
        {138, Value_Type_Percent},
        {139, Value_Type_Percent},
        {140, Value_Type_Percent},
        {141, Value_Type_Percent},
        {142, Value_Type_Percent},
        {143, Value_Type_Percent},
        {144, Value_Type_Percent},
        {145, Value_Type_Percent},
        {146, Value_Type_Default},
        {147, Value_Type_Default},
        {148, Value_Type_Default},
        {149, Value_Type_Default},
        {150, Value_Type_Default},
        {151, Value_Type_Default},
        {152, Value_Type_Default},
        {153, Value_Type_Default},
        {154, Value_Type_Default},
        {155, Value_Type_Default},
        {156, Value_Type_Default},
        {157, Value_Type_Default},
        {158, Value_Type_Default},
        {159, Value_Type_Default},
        {160, Value_Type_Percent},
        {161, Value_Type_Default},
        {162, Value_Type_Percent},
        {163, Value_Type_Percent},
        {164, Value_Type_Percent},
        {165, Value_Type_Percent},
        {166, Value_Type_Percent},
        {167, Value_Type_Percent},
        {168, Value_Type_DefaultPerc},
        {169, Value_Type_DefaultPerc},
        {170, Value_Type_DefaultPerc},
        {171, Value_Type_DefaultPerc},
        {172, Value_Type_Percent},
        {173, Value_Type_Default},
        {174, Value_Type_Percent},
        {175, Value_Type_Percent},
        {176, Value_Type_Percent},
        {177, Value_Type_Percent},
        {178, Value_Type_Percent},
        {179, Value_Type_Default},
        {180, Value_Type_Default},
        {181, Value_Type_Default},
        {182, Value_Type_Default},
        {183, Value_Type_Default},
        {184, Value_Type_Default},
        {185, Value_Type_Percent},
        {186, Value_Type_Default},
        {187, Value_Type_Default},
        {188, Value_Type_Default},
        {189, Value_Type_Default},
        {190, Value_Type_Default},
        {191, Value_Type_Default},
        {192, Value_Type_Default},
        {193, Value_Type_Percent},
        {194, Value_Type_Percent},
        {195, Value_Type_Percent},
        {196, Value_Type_Percent},
        {197, Value_Type_Percent},
        {198, Value_Type_Percent},
        {199, Value_Type_Percent},
        {200, Value_Type_Default},
        {201, Value_Type_Default},
        {202, Value_Type_Default},
        {203, Value_Type_Default},
        {204, Value_Type_Default},
        {205, Value_Type_Default},
        {206, Value_Type_Default},
        {207, Value_Type_Default},
        {208, Value_Type_Percent},
        {209, Value_Type_Percent},
        {210, Value_Type_Percent},
        {211, Value_Type_Percent},
        {212, Value_Type_Percent},
        {213, Value_Type_Percent},
        {214, Value_Type_Percent},
        {215, Value_Type_Percent},
        {216, Value_Type_Percent},
        {217, Value_Type_Percent},
        {219, Value_Type_Percent},
        {220, Value_Type_Percent},
        {221, Value_Type_Percent},
        {222, Value_Type_Percent},
        {223, Value_Type_Percent},
        {224, Value_Type_Percent},
        {225, Value_Type_Percent},
        {226, Value_Type_Percent},
        {227, Value_Type_Percent},
        {228, Value_Type_Percent},
        {229, Value_Type_Percent},
        {230, Value_Type_Percent},
        {231, Value_Type_Percent},
        {232, Value_Type_Percent},
        {233, Value_Type_Default},
        {234, Value_Type_Default},
        {235, Value_Type_Default},
        {236, Value_Type_Default},
        {237, Value_Type_Default},
        {238, Value_Type_Default},
        {239, Value_Type_Percent},
        {240, Value_Type_Percent},
        {241, Value_Type_Percent},
        {242, Value_Type_Percent},
        {243, Value_Type_DefaultPerc},
        {244, Value_Type_Percent},
        {245, Value_Type_Percent},
        {246, Value_Type_DefaultPerc},
        {247, Value_Type_Default},
        {248, Value_Type_Percent},
        {249, Value_Type_Default},
        {250, Value_Type_Percent},
        {251, Value_Type_Percent},
        {252, Value_Type_Percent},
        // reserved
        {253, Value_Type_Default},
        {254, Value_Type_Default},
        {255, Value_Type_Default},
        {256, Value_Type_Default},
        {257, Value_Type_Default},
        {258, Value_Type_Default},
        {259, Value_Type_Default},
        {260, Value_Type_Default},
        {261, Value_Type_Default},
        {262, Value_Type_Default},
        {263, Value_Type_Default},
        {264, Value_Type_Default},
        {265, Value_Type_Default},
        {266, Value_Type_Default},
        {267, Value_Type_Default},
        {268, Value_Type_Default},
        {269, Value_Type_Default},
        {270, Value_Type_Default},
        {271, Value_Type_Default},
        {272, Value_Type_Default},
        {273, Value_Type_Default},
        {274, Value_Type_Default},
        {275, Value_Type_Default},
        {276, Value_Type_Default},
        {277, Value_Type_Default},
        {278, Value_Type_Default},
        {279, Value_Type_Default},
        {280, Value_Type_Default},
        {281, Value_Type_Default},
        {282, Value_Type_Default},
        {283, Value_Type_Default},
        {284, Value_Type_Default},
        {285, Value_Type_Default},
        {286, Value_Type_Default},
        {287, Value_Type_Default},
        {288, Value_Type_Default},
        {289, Value_Type_Default},
        {290, Value_Type_Default},
        {291, Value_Type_Default},
        {292, Value_Type_Default},
        {293, Value_Type_Default},
        {294, Value_Type_Default},
        {295, Value_Type_Default},
        {296, Value_Type_Default},
        {297, Value_Type_Default},
        {298, Value_Type_Default},
        {299, Value_Type_Default},
    };
    
    std::map<int, zCOLOR> DamageTypeColorMap =
    {
        {0, zCOLOR(220,220,220)},
        {1, zCOLOR(220,220,220)},
        {2, zCOLOR(220,220,220)},
        {3, zCOLOR(220,220,220)},
        {4, zCOLOR(255, 69, 0)},
        {5, zCOLOR(0, 191, 255)},
        {6, zCOLOR(255, 255, 0)},
        {7, zCOLOR(135, 206, 250)},
        {8, zCOLOR(214, 130, 71)},
        {9, zCOLOR(255, 255, 255)},
        {10, zCOLOR(149, 8, 209)},
        {11, zCOLOR(171, 7, 119)},
        {12, zCOLOR(255, 20, 122)},
        {13, zCOLOR(34, 139, 34)},
        {14, zCOLOR(200, 200, 200)},
        {15, zCOLOR(140, 0, 0)},
    };

    static zCView* ExtraItemInfo;
    static int ItemInstance;
    static int RefreshTime;
    static bool IsExtraItemInfoDisplay;

    static zSTRING ItemName;
    static Array<zSTRING> ItemTags;
    static Array<zSTRING> ItemOwnProps;
    static Array<zSTRING> ItemStatsProps;

    static int ExtraItemInfoWidth;
    static int ExtraItemInfoHeight;
    static int ExtraItemInfoX;
    static int ExtraItemInfoY;
    static int ExtraItemInfoCenter;

    void CloseExtraItemInfo()
    {
        if (ExtraItemInfo)
        {
            ExtraItemInfo->Blit();
            ExtraItemInfo->ClrPrintwin();
            screen->RemoveItem(ExtraItemInfo);
        }
        SAFE_DELETE(ExtraItemInfo);
        ExtraItemInfo = Null;
    }

    void OpenExtraItemInfo()
    {
        CloseExtraItemInfo();
        ExtraItemInfo = new zCView(0, 0, 8192, 8192);
        screen->InsertItem(ExtraItemInfo, FALSE);
        ExtraItemInfo->InsertBack(Z "STEXT_EXTRAITEMINFO.TGA");
        ExtraItemInfo->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
        ExtraItemInfo->SetTransparency(180);
        screen->RemoveItem(ExtraItemInfo);
    }

    inline void SetMaxWidth(zSTRING str, int& width) 
    {
        int size = ExtraItemInfo->FontSize(str);
        width = width < size ? size : width;
    }

    void AjustExtraItemInfo()
    {
        int lines = 5;
        int maxWidth = 512;

        if (!ItemName.IsEmpty())
        {
            lines += 1;
            SetMaxWidth(ItemName, maxWidth);
        }  
        for (int i = 0; i < ItemOwnProps.GetNum(); i++)
        {
            lines += 1;
            SetMaxWidth(ItemOwnProps[i], maxWidth);
        }
        for (int i = 0; i < ItemStatsProps.GetNum(); i++)
        {
            lines += 1;
            SetMaxWidth(ItemStatsProps[i], maxWidth);
        }

        int x = parser->GetSymbol("StExt_Config_ItemMenu_PosX")->single_intdata;
        int y = parser->GetSymbol("StExt_Config_ItemMenu_PosY")->single_intdata;

        ExtraItemInfoX = 8192 * (x * 0.01f);
        ExtraItemInfoY = 8192 * (y * 0.01f);

        ExtraItemInfoWidth = maxWidth < 512 ? 512 : maxWidth;
        ExtraItemInfoWidth += BorderXOffset;
        ExtraItemInfoHeight = (lines * ExtraItemInfo->FontY());

        ExtraItemInfo->SetPos(ExtraItemInfoX, ExtraItemInfoY);
        ExtraItemInfo->SetSize(ExtraItemInfoWidth, ExtraItemInfoHeight);
    }

    inline zCOLOR GetRankColor(int rank)
    {
        if (rank == 1) return zCOLOR(0, 128, 255);
        else if (rank == 2) return zCOLOR(128, 255, 0);
        else if (rank == 3) return zCOLOR(255, 128, 64);
        else if (rank == 4) return zCOLOR(255, 128, 255);
        else if (rank >= 5) return zCOLOR(128, 0, 255);
        return zCOLOR(250, 250, 250);
    }

    inline void AppendTag(zSTRING& line, zSTRING tag)
    {
        if (!line.IsEmpty()) line += " | ";
        line += tag;
    }

    inline zSTRING ProcessStatValue(int statVal, int valType)
    {
        zSTRING statValStr;
        if (valType == Value_Type_YesNo) statValStr = IntValueToYesNo(statVal);
        else if (valType == Value_Type_Percent) statValStr = GetFloatStringFromInt(statVal);
        else if (valType == Value_Type_DefaultPerc) statValStr = zSTRING(statVal) + "%";
        else if (valType == Value_Type_InvertPerc) statValStr = zSTRING(statVal * 10) + "%";
        else statValStr = zSTRING(statVal);
        return statValStr;
    }

    void ProcessEnchantmentData(C_EnchantmentData* enchantment, oCItem* item)
    {
        ItemOwnProps = Array<zSTRING>();
        ItemStatsProps = Array<zSTRING>();
        zSTRING tags = parser->GetSymbol("StExt_Str_ItemRank")->stringdata[enchantment->Rank];
        ItemName = item->name;

        // tags        
        if (HasFlag(enchantment->Flags, ItemFlag_Socketed))
            AppendTag(tags, parser->GetSymbol("StExt_Str_ItemFlag_Socketed")->stringdata);
        if (HasFlag(enchantment->Flags, ItemFlag_Corrupted))
            AppendTag(tags, parser->GetSymbol("StExt_Str_ItemFlag_Corrupted")->stringdata);
        if (HasFlag(enchantment->Flags, ItemFlag_Crafted))
            AppendTag(tags, parser->GetSymbol("StExt_Str_ItemFlag_Crafted")->stringdata);

        ItemOwnProps.InsertEnd(tags);
        
        //own stats    
        zSTRING itemLevel = parser->GetSymbol("StExt_Str_ItemLevel")->stringdata + Z enchantment->Level;
        ItemOwnProps.InsertEnd(itemLevel);
        if (HasFlag(enchantment->Flags, ItemFlag_Socketed))
        {
            zSTRING sockets = parser->GetSymbol("StExt_Str_ItemSockets")->stringdata + Z enchantment->SocketsUsed + "/" +Z  enchantment->SocketsMax;
            ItemOwnProps.InsertEnd(sockets);
        }

        //item stats
        if (HasFlag(enchantment->Flags, ItemFlag_Undefined))
        {
            zSTRING undefined = parser->GetSymbol("StExt_Str_Undefined")->stringdata;
            ItemOwnProps.InsertEnd("");
            ItemOwnProps.InsertEnd(undefined);
            return;
        }

        zSTRING SecondsStr = parser->GetSymbol("StExt_Str_Seconds")->stringdata;
        for (int i = 0; i < EnchantStatsMax; i++)
        {
            int statId = enchantment->StatId[i];
            if (statId <= Invalid) continue;

            zSTRING statLine = parser->GetSymbol("StExt_PcStats_Desc")->stringdata[statId] + StatTextWhiteSpace;
            statLine += "+" + ProcessStatValue(enchantment->StatValue[i], StatsTypeMap[statId]);
            if (HasFlag(enchantment->Type, ItemType_Potion) || HasFlag(enchantment->Type, ItemType_Scroll))
                statLine += " | " + Z enchantment->StatDuration[i] + SecondsStr;
            ItemStatsProps.InsertEnd(statLine);
        }
    }

    void ProcessArtifactData(oCItem* item, int type)
    {
        ItemOwnProps = Array<zSTRING>();
        ItemStatsProps = Array<zSTRING>();
        ItemName = item->name;

        int expNow;
        int expNext;
        int level;
        int lp;
        zCPar_Symbol* statsArray = Null;

        if (type == 1)              // grimoir
        {
            expNow = parser->GetSymbol("StExt_Grimoir_ExpNow")->single_intdata;
            expNext = parser->GetSymbol("StExt_Grimoir_ExpNext")->single_intdata;
            level = parser->GetSymbol("StExt_Grimoir_Level")->single_intdata;
            lp = parser->GetSymbol("StExt_Grimoir_Lp")->single_intdata;
            statsArray = parser->GetSymbol("StExt_PcStats_Grimoir");
        }
        else if(type == 2)          // dagger
        {
            expNow = parser->GetSymbol("StExt_Dagger_ExpNow")->single_intdata;
            expNext = parser->GetSymbol("StExt_Dagger_ExpNext")->single_intdata;
            level = parser->GetSymbol("StExt_Dagger_Level")->single_intdata;
            lp = parser->GetSymbol("StExt_Dagger_Lp")->single_intdata;
            statsArray = parser->GetSymbol("StExt_PcStats_Dagger");
        }

        //own stats    
        zSTRING itemLevel = parser->GetSymbol("StExt_Str_Level")->stringdata + Z level;
        ItemOwnProps.InsertEnd(itemLevel);

        zSTRING itemExp = parser->GetSymbol("StExt_Str_Exp")->stringdata + Z expNow + "/" + Z expNext;
        ItemOwnProps.InsertEnd(itemExp);

        zSTRING itemLp = parser->GetSymbol("StExt_Str_Lp")->stringdata + Z lp;
        ItemOwnProps.InsertEnd(itemLp);

        //item stats
        int maxStatsId = parser->GetSymbol("StExt_PcStats_Index_Max")->single_intdata;
        for (int i = 0; i < maxStatsId; i++)
        {
            if (statsArray->intdata[i] > 0)
            {
                zSTRING statLine = parser->GetSymbol("StExt_PcStats_Desc")->stringdata[i] + StatTextWhiteSpace;
                statLine += "+" + ProcessStatValue(statsArray->intdata[i], StatsTypeMap[i]);
                ItemStatsProps.InsertEnd(statLine);
            }
        }
    }

    inline int GetArtifactType(oCItem* item)
    {
        int result = Invalid;
        if (item)
        {
            if (item->GetInstanceName() == Z "itut_stext_magicbook") result = 1;
            if (item->GetInstanceName() == Z "itut_stext_magicdagger") result = 2;
        }
        return result;
    }

    inline void PrintLine(int& y, zSTRING text)
    {
        int x = CenterX - (ExtraItemInfo->FontSize(text) * 0.5f);
        ExtraItemInfo->Print(x, y, text);
        y += ExtraItemInfo->FontY();
    }

    void DrawExtraItemInfo(oCItem* item)
    {
        IsExtraItemInfoDisplay = false;
        if (ExtraItemInfo && !item)
        {
            CloseExtraItemInfo();
            ItemInstance = Invalid;
            return;
        }

        if (ItemInstance == item->GetInstance())
            return;

        if (!Gothic_II_Addon::HasFlag(item->mainflag, item_kat_rune))
        {
            C_EnchantmentData* enchantment = GetEnchantmentData(item);
            if (enchantment)
            {
                IsExtraItemInfoDisplay = true;
                ProcessEnchantmentData(enchantment, item);

                OpenExtraItemInfo();
                screen->InsertItem(ExtraItemInfo);
                AjustExtraItemInfo();
                int fontY = ExtraItemInfo->FontY();
                int y = fontY;

                ExtraItemInfo->SetFontColor(GetRankColor(enchantment->Rank));
                PrintLine(y, ItemName);

                y += fontY;
                if (HasFlag(enchantment->Flags, ItemFlag_Undefined))
                    ExtraItemInfo->SetFontColor(zCOLOR(250, 0, 0));
                else ExtraItemInfo->SetFontColor(zCOLOR(200, 200, 200));
                for (int i = 0; i < ItemOwnProps.GetNum(); i++)
                    PrintLine(y, ItemOwnProps[i]);

                y += fontY;
                ExtraItemInfo->SetFontColor(GetRankColor(2));
                for (int i = 0; i < ItemStatsProps.GetNum(); i++)
                    PrintLine(y, ItemStatsProps[i]);

                BuildItemText(enchantment, item);
            }
            else
            {
                int artType = GetArtifactType(item);
                if (artType == 1 || artType == 2)
                {
                    IsExtraItemInfoDisplay = true;
                    ProcessArtifactData(item, artType);

                    OpenExtraItemInfo();
                    screen->InsertItem(ExtraItemInfo);
                    AjustExtraItemInfo();
                    int fontY = ExtraItemInfo->FontY();

                    int y = fontY;
                    ExtraItemInfo->SetFontColor(GetRankColor(5));
                    PrintLine(y, ItemName);

                    y += fontY;
                    ExtraItemInfo->SetFontColor(zCOLOR(255, 220, 0));
                    for (int i = 0; i < ItemOwnProps.GetNum(); i++)
                        PrintLine(y, ItemOwnProps[i]);

                    y += fontY;
                    ExtraItemInfo->SetFontColor(GetRankColor(2));
                    for (int i = 0; i < ItemStatsProps.GetNum(); i++)
                        PrintLine(y, ItemStatsProps[i]);
                }
            }
        }
        else if (Gothic_II_Addon::HasFlag(item->mainflag, item_kat_rune))
        {
            IsExtraItemInfoDisplay = true;

            OpenExtraItemInfo();
            screen->InsertItem(ExtraItemInfo);
            
            zCPar_Symbol* damageTypeNames = parser->GetSymbol("StExt_Str_ExtraDamageType");
            zCPar_Symbol* damageFlagsNames = parser->GetSymbol("StExt_Str_ExtraDamageFlags");

            int damageTags = *(int*)parser->CallFunc(GetSpellDamageFlagsFunc, item->spell);
            int damageFlags = *(int*)parser->CallFunc(GetSpellEffectFlagsFunc, item->spell);

            ItemName = item->description;
            int maxWidth = 512;
            int lines = 5;            

            SetMaxWidth(ItemName, maxWidth);
            for (int i = 0; i < 16; i++)
            {
                int flag = 1 << i;
                if (Gothic_II_Addon::HasFlag(damageTags, flag))
                {
                    lines += 1;
                    SetMaxWidth(damageTypeNames->stringdata[i], maxWidth);
                }
            }

            zSTRING dmgFlags = zSTRING();
            for (int i = 0; i < 27; i++)
            {
                int flag = 1 << i;
                if (Gothic_II_Addon::HasFlag(damageFlags, flag))
                    AppendTag(dmgFlags, damageFlagsNames->stringdata[i]);
            }
            lines += 1;
            SetMaxWidth(dmgFlags, maxWidth);           

            int px = parser->GetSymbol("StExt_Config_ItemMenu_PosX")->single_intdata;
            int py = parser->GetSymbol("StExt_Config_ItemMenu_PosY")->single_intdata;

            ExtraItemInfoX = 8192 * (px * 0.01f);
            ExtraItemInfoY = 8192 * (py * 0.01f);

            ExtraItemInfoWidth = maxWidth < 512 ? 512 : maxWidth;
            ExtraItemInfoWidth += BorderXOffset;
            ExtraItemInfoHeight = (lines * ExtraItemInfo->FontY());

            ExtraItemInfo->SetPos(ExtraItemInfoX, ExtraItemInfoY);
            ExtraItemInfo->SetSize(ExtraItemInfoWidth, ExtraItemInfoHeight);

            int fontY = ExtraItemInfo->FontY();
            int y = fontY;

            //y += fontY;
            ExtraItemInfo->SetFontColor(GetRankColor(1));
            PrintLine(y, ItemName);

            y += fontY;
            for (int i = 0; i < 16; i++)
            {
                int flag = 1 << i;
                if (Gothic_II_Addon::HasFlag(damageTags, flag))
                {
                    zCOLOR color = DamageTypeColorMap[i];
                    zSTRING dmgName = damageTypeNames->stringdata[i];
                    ExtraItemInfo->SetFontColor(color);
                    PrintLine(y, dmgName);
                }
            }

            y += fontY;            
            ExtraItemInfo->SetFontColor(zCOLOR(250, 250, 250));
            PrintLine(y, dmgFlags);
        }

        CloseExtraItemInfo();
        ItemInstance = Invalid;
    }

    // Hooks
    HOOK Hook_oCItemContainer_DrawItemInfo PATCH(&oCItemContainer::DrawItemInfo, &oCItemContainer::DrawItemInfo_StExt);
    void oCItemContainer::DrawItemInfo_StExt(oCItem* item, zCWorld* world)
    {
        THISCALL(Hook_oCItemContainer_DrawItemInfo)(item, world);
        DrawExtraItemInfo(item);
    }

    HOOK Hook_oCItemContainer_Close PATCH(&oCItemContainer::Close, &oCItemContainer::Close_StExt);
    void oCItemContainer::Close_StExt()
    {
        CloseExtraItemInfo();
        THISCALL(Hook_oCItemContainer_Close)();
        IsExtraItemInfoDisplay = false;
    }

    /*
    HOOK Hook_oCItemContainer_OpenPassive PATCH(&oCItemContainer::OpenPassive, &oCItemContainer::OpenPassive_StExt);
    void oCItemContainer::OpenPassive_StExt(int x, int y, int mode)
    {
        THISCALL(Hook_oCItemContainer_OpenPassive)(x, y, mode);
        OpenExtraItemInfo();
    }
    
    */
}

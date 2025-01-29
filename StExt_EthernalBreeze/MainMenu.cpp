#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    const int Center = 8192 * 0.5f;
    static zCView* ModInfoView;
    static int PosX, PosY;
    static int TextPosX, TextPosY;

    void DrawModInfo()
    {
        if (!ogame && !screen) return;

        if (player && ModInfoView)
        {
            screen->RemoveItem(ModInfoView);
            return;
        }
        if (ModVersionString.IsEmpty()) return;

        int screenFontY = screen->FontY();
        int pluginCount = ModPluginsInfo.GetNum();
        if (!ModInfoView)
        {
            ModInfoView = new zCView(0, 0, 8192, 8192);
            screen->InsertItem(ModInfoView, FALSE);

            ModInfoView->InsertBack(Z "STEXT_MODINFO_BG.tga");
            ModInfoView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
            ModInfoView->SetTransparency(240);

            PosX = 0;
            PosY = 8192 - (screenFontY * (7 + pluginCount));
            ModInfoView->SetPos(PosX, PosY);            
            screen->RemoveItem(ModInfoView);
        }

        screen->InsertItem(ModInfoView);
        ModInfoView->SetSize(8192 * 0.255f, (screenFontY * (3 + pluginCount)));
        TextPosX = screenFontY * 4.1f;
        if (pluginCount > 0)
        {
            int line = 1;
            int lineOffset = ModInfoView->FontY();
            TextPosY = line * lineOffset;
            ModInfoView->SetFontColor(zCOLOR(128, 64, 240));
            ModInfoView->Print(TextPosX, TextPosY, ModVersionString);

            for (int i = 0; i < pluginCount; i++)
            {
                line += 1;
                TextPosY = line * lineOffset;
                zSTRING modInfo = Z(ModPluginsInfo[i].Name + " [" + ModPluginsInfo[i].Version + "] by " + ModPluginsInfo[i].Author);
                ModInfoView->Print(TextPosX, TextPosY, modInfo);
            }
        }
        else
        {
            TextPosY = Center - (ModInfoView->FontY() * 0.5f);
            ModInfoView->SetFontColor(zCOLOR(128, 64, 240));
            ModInfoView->Print(TextPosX, TextPosY, ModVersionString);
        }        
    }
}
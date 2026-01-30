#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    zCView* ModInfoView;
    Array<zSTRING> Content;
    int PosX, PosY;
    int SizeX, SizeY;
    int ContentOffset, WidestContentSize, WidestContentSizeIndex;
    bool DisplayNow;
    zCOLOR ContentColor = zCOLOR(130, 80, 250);

    inline void UpdateModInfoPanel()
    {
        const int fontY = ModInfoView ? ModInfoView->FontY() : screen->FontY();
        const float fontYOffset = player ? 2.5f : 3.75f;
        const uint contentSize = Content.GetNum();

        ContentOffset = 512;
        SizeY = static_cast<int>((contentSize + 2) * (fontY * 1.25f));
        SizeX = static_cast<int>(((WidestContentSize * 1.20f) * ScreenVBufferSize) / zrenderer->vid_xdim);
        PosX = player ? static_cast<int>(ScreenHalfVBufferSize - (SizeX * 0.5f)) : 0;
        PosY = static_cast<int>(ScreenVBufferSize - (fontY * fontYOffset) - SizeY);

        if (contentSize > 1) SizeY += static_cast<int>(fontY * 0.25f);
        else SizeY -= static_cast<int>(fontY * 0.25f);

        bool displayBefore = DisplayNow;
        DisplayNow = !player || ogame->IsOnPause() && !IsMenuWindowOpen();
        if (ModInfoView && (DisplayNow != displayBefore))
        {
            ModInfoView->SetSize(SizeX, SizeY);
            ModInfoView->SetPos(PosX, PosY);
        }
    }

    inline void InitModInfoPanel()
    {
        if (ModInfoView)
        {
            screen->RemoveItem(ModInfoView);
            SAFE_DELETE(ModInfoView);
        }
        ModInfoView = new zCView(0, 0, ScreenVBufferSize, ScreenVBufferSize);

        const zSTRING modInfoHeader = ModVersionString.IsEmpty() ? "Ethernal Breeze mod" : ModVersionString;
        WidestContentSize = ModInfoView->font->GetFontX(modInfoHeader);
        WidestContentSizeIndex = 0;
        Content.InsertEnd(modInfoHeader);

        const uint pluginsCount = ModPluginsInfo.GetNum();
        if (pluginsCount > 0) {
            for (uint i = 0; i < pluginsCount; ++i)
            {
                const zSTRING pluginInfo = Z(ModPluginsInfo[i].Name + " [" + ModPluginsInfo[i].Version + "] by " + ModPluginsInfo[i].Author);
                const int pluginInfoSize = ModInfoView->font->GetFontX(pluginInfo);
                Content.InsertEnd(pluginInfo);
                if (pluginInfoSize > WidestContentSize)
                {
                    WidestContentSizeIndex = static_cast<int>(i + 1);
                    WidestContentSize = pluginInfoSize;
                }
            }
        }

        screen->InsertItem(ModInfoView, FALSE);
        ModInfoView->InsertBack(Z "STEXT_MODINFO_BG.tga");
        ModInfoView->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
        ModInfoView->SetTransparency(245);
        
        UpdateModInfoPanel();
        screen->RemoveItem(ModInfoView);
    }

    void DrawModInfo()
    {
        if (!ogame || !screen || !zrenderer) return;
        if (!ModInfoView)
        {
            InitModInfoPanel();
            return;
        }

        if (!screen->childs.IsIn(ModInfoView)) 
            screen->InsertItem(ModInfoView, FALSE);

        UpdateModInfoPanel();
        if (!DisplayNow)
        {
            if (screen->childs.IsIn(ModInfoView))
                screen->RemoveItem(ModInfoView);
            return;
        }
        ModInfoView->ClrPrintwin();
        ModInfoView->SetFontColor(ContentColor);

        const uint contentSize = Content.GetNum();
        const int screenFontY = ModInfoView->FontY();
        const int screenFontYOffset = static_cast<int>(screenFontY * 0.25f);
        int y = screenFontY + screenFontYOffset;
        int x = ContentOffset;
        for (uint i = 0; i < contentSize; ++i)
        {
            x = !player ? ContentOffset : 
                static_cast<int>(ScreenHalfVBufferSize - (ModInfoView->FontSize(Content[i]) * 0.5f));
            ModInfoView->Print(x, y, Content[i]);
            if (i == 0U) y += screenFontYOffset;
            y += screenFontY + screenFontYOffset;
        }  
    }
}
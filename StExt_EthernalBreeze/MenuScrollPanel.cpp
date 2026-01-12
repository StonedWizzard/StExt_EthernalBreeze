#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    MenuScrollPanel::MenuScrollPanel() { }

    void MenuScrollPanel::Init()
    {
        ScrollOffset = 0;
        ScrollOffsetBefore = Invalid;
        ScrollCanvasSize = 0;

        BaseMenuPanel::Init();
        UpdateCanvasSize();
    }

    void MenuScrollPanel::Resize() { BaseMenuPanel::Resize(); }

    void MenuScrollPanel::Draw() { BaseMenuPanel::Draw(); }

    void MenuScrollPanel::Update()
    {
        BaseMenuElement::Update();
        ItemsCount = Items.GetNum();

        const int panelTop = GlobalPosY;
        const int panelBottom = panelTop + GlobalSizeY;
        const float scrollOffsetAbs = ScrollOffset * ScreenToRelativePixDelta;

        for (uint i = 0; i < ItemsCount; ++i)
        {
            BaseMenuElement* item = Items[i];
            if (!item) continue;

            if (ScrollOffset != ScrollOffsetBefore)
            {
                /*float lastPosY = item->PosY + (ScrollOffsetBefore * ScreenToRelativePixDelta);
                item->PosY = lastPosY - scrollOffsetAbs;*/
                item->PosY = item->InitialPosY - scrollOffsetAbs;
                item->Resize();

                int itemTop = item->GlobalPosY;
                int itemBottom = itemTop + item->GlobalSizeY;
                item->IsVisible = (itemTop >= panelTop) && (itemBottom <= panelBottom);

                if (!item->IsVisible && item->View)
                {
                    item->View->Blit();
                    item->View->ClrPrintwin();
                    if (View)
                        View->RemoveItem(item->View);
                }
                
                /*
                DEBUG_MSG("Update ScrollPanel Item[" + Z((int)i) + "] '" + Z(item->Name.c_str()) +
                    "' rect: [x=" + Z(item->GlobalPosX) + ";y=" + Z(item->GlobalPosY) + ";w=" + Z(item->GlobalSizeX) + ";h=" + Z(item->GlobalSizeY) + "]" +
                    " Hiden: " + Z((int)item->IsVisible));
                DEBUG_MSG("Update ScrollPanel Item[" + Z((int)i) + "] '" + Z(item->Name.c_str()) +
                    "' rrect: [x=" + Z(item->PosX) + ";y=" + Z(item->PosY) + ";w=" + Z(item->SizeX) + ";h=" + Z(item->SizeY) + "]" +
                    " Parent: " + Z((int)(item->Parent != Null)));
                */
            }
            item->Update();
        }
        ScrollOffsetBefore = ScrollOffset;
    }

    void MenuScrollPanel::UpdateCanvasSize()
    {
        if (ItemsCount == 0)
        {
            ScrollCanvasSize = GlobalSizeY;
            return;
        }

        float maxBottom = 0.0f;
        for (uint i = 0; i < Items.GetNum(); ++i)
        {
            BaseMenuElement* item = Items[i];
            if (!item) continue;

            float bottom = item->InitialPosY + item->InitialSizeY;
            if (bottom > maxBottom) maxBottom = bottom;
        }

        ScrollCanvasSize = static_cast<int>((maxBottom * ScreenVBufferSize) - (GlobalSizeY * 0.5f));
        if (ScrollCanvasSize < GlobalSizeY)
            ScrollCanvasSize = GlobalSizeY;
    }

    void MenuScrollPanel::Scroll(int delta)
    {        
        UpdateCanvasSize();
        ScrollOffset += delta * -1;
        if (ScrollOffset < 0) ScrollOffset = 0;
        if (ScrollOffset > ScrollCanvasSize) ScrollOffset = ScrollCanvasSize;
    }

    bool MenuScrollPanel::HandleMouse(const UiMouseEventArgs& args)
    {
        if (!IsEnabled || !IsVisible) return false;

        const bool isHit = (args.X > GlobalRectLeft && args.X < GlobalRectRight) && (args.Y > GlobalRectTop && args.Y < GlobalRectBottom);
        if (isHit && args.Action == UiMouseEnum::Scroll)
        {
            int scrollDelta = static_cast<int>((args.ScrollDelta * ModMenuWindow_ScrollShiftMult) * ModMenuWindow_ScrollMult);
            Scroll(scrollDelta);
        }

        return BaseMenuPanel::HandleMouse(args);
    }

    bool MenuScrollPanel::HandleKey(const UiKeyEventArgs& args)
    {
        
        return false;
    }

    MenuScrollPanel::~MenuScrollPanel() { }
}
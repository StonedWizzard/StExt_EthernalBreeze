#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	MenuListView::MenuListView() { }

	void MenuListView::Resize() { BaseMenuPanel::Resize(); }

	void MenuListView::Init()
	{
		ItemHeight = 0.05f;
		ItemWidth = 0.95f;
		ItemMargin = 0.01f;
		ScrollOffset = 0;
		ScrollOffsetBefore = Invalid;
		ScrollStep = 2048;
		ScrollCanvasSize = ScreenVBufferSize;

		BaseMenuPanel::Init();
	}

	void MenuListView::Draw() { BaseMenuPanel::Draw(); }

	void MenuListView::Update()
	{
		BaseMenuElement::Update();

		ItemsCount = Items.GetNum();
		ScrollStep = static_cast<int>(ScreenVBufferSize * ItemHeight);
		ScrollCanvasSize = ItemsCount > 0 ? 
			static_cast<int>(ItemsCount * (ScrollStep + (ItemMargin * ScreenVBufferSize))) : ScrollStep;
		const int maxScrollOffset = ScrollCanvasSize - GlobalSizeY;

		if (ScrollOffset > maxScrollOffset) ScrollOffset = maxScrollOffset;
		if (ScrollOffset < 0) ScrollOffset = 0;

		const int panelTop = GlobalPosY;
		const int panelBottom = panelTop + GlobalSizeY;
		const float xOffset = (1.0f - ItemWidth) * 0.5f;
		const float yOffsetStep = ItemHeight + ItemMargin;

		float yOffset = (ScrollOffset <= 0) ? 0.0f : static_cast<float>(-ScrollOffset) * ScreenToRelativePixDelta;
		yOffset += ItemMargin;
		for (uint i = 0; i < ItemsCount; ++i)
		{
			BaseMenuElement* item = Items[i];
			if (!item) continue;

			if (ScrollOffset != ScrollOffsetBefore)
			{
				item->SizeX = ItemWidth;
				item->SizeY = ItemHeight;
				item->PosX = xOffset;
				item->PosY = yOffset;
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
				yOffset += yOffsetStep;

				/*
				DEBUG_MSG("Update ListItem[" + Z((int)i) + "] '" + Z(item->Name.c_str()) +
					"' | yOffset: " + Z(yOffset) +
					" | itemTop: " + Z(itemTop) + " itemBottom: " + Z(itemBottom) +
					" | GlobalSizeY: " + Z(item->GlobalSizeY) +
					" | Hiden: " + Z((int)item->IsHiden));*/
			}
			item->Update();
		}
		ScrollOffsetBefore = ScrollOffset;
	}
	
	void MenuListView::Scroll(int delta)
	{
		ScrollOffset += delta * -1;
		const int maxScrollOffset = ScrollCanvasSize - GlobalSizeY;
		if (ScrollOffset < 0) ScrollOffset = 0;
		if (ScrollOffset > maxScrollOffset) ScrollOffset = maxScrollOffset;

		uint newFocusIndex = static_cast<uint>(ScrollOffset / ItemHeight);
		if (newFocusIndex >= ItemsCount) newFocusIndex = ItemsCount - 1U;
		if (newFocusIndex < 0U) newFocusIndex = 0U;
		FocusItem(Items[newFocusIndex]);
	}

	bool MenuListView::HandleKey(const UiKeyEventArgs& args)
	{
		//return (CanInteract() && OnKeyEvent) ? OnKeyEvent(this, args) : false; 
		return false;
	}

	bool MenuListView::HandleMouse(const UiMouseEventArgs& args)
	{
		if (!IsEnabled || !IsVisible) return false;

		const bool isHit = (args.X > GlobalRectLeft && args.X < GlobalRectRight) && (args.Y > GlobalRectTop && args.Y < GlobalRectBottom);
		if (isHit && args.Action == UiMouseEnum::Scroll) {
			int scrollDelta = static_cast<int>((args.ScrollDelta * ModMenuWindow_ScrollShiftMult) * ModMenuWindow_ScrollMult);
			DEBUG_MSG("StExt - mod Ui: scrollDelta - " + Z(scrollDelta));
			Scroll(scrollDelta);
		}
		return BaseMenuPanel::HandleMouse(args);
	}

	MenuListView::~MenuListView() {}
}
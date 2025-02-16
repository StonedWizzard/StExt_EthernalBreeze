#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	void MenuListView::Init(float posX, float posY, float sizeX, float sizeY)
	{
		BaseMenuPanel::Init(posX, posY, sizeX, sizeY);
		ItemHeight = 0.05f;
		ItemWidth = 0.95f;
		ScrollOffset = 0;
		ScrollStep = 2048;
	}

	void MenuListView::Draw()
	{
		ParentView->RemoveItem(View);
		if (!IsVisible) return;
		ParentView->InsertItem(View);

		if (ItemsCount == 0) return;
		
		int yStep = ScreenVBufferSize * ItemHeight;
		int x = (ScreenVBufferSize - (ScreenVBufferSize * ItemWidth)) * 0.5f;
		int y = -yStep;

		int startIdx = (ScrollOffset - yStep) / yStep;
		if (startIdx < 0)
		{
			startIdx = 0;
			y = 0;
		}

		int endIdx = startIdx + 1 + (ScreenVBufferSize / yStep);
		if (endIdx >= ItemsCount) endIdx = ItemsCount;

		for (int i = startIdx; i < endIdx; ++i) 
		{
			Items[i]->View->SetPos(x, y);
			Items[i]->Draw();
			if (!Items[i]->IsVisible)
			{
				++endIdx;
				if (endIdx >= ItemsCount) break;
				continue;
			}			
			y += yStep;
		}
	}

	void MenuListView::Update()
	{
		BaseMenuPanel::Update();
		int yStep = ScreenVBufferSize * ItemHeight;
		ScrollStep = yStep;
		ScrollCanvasSize = (ItemsCount * yStep) - yStep;
		if (ScrollOffset > ScrollCanvasSize) ScrollOffset = ScrollCanvasSize;
	}
	
	void MenuListView::Scroll(int delta)
	{
		ScrollOffset += delta;
		if (ScrollOffset < 0) ScrollOffset = 0;
		if (ScrollOffset < ScrollCanvasSize) ScrollOffset = ScrollCanvasSize;

		int newFocusIndex = ScrollOffset / ItemHeight;
		if (newFocusIndex >= ItemsCount) newFocusIndex = ItemsCount - 1;
		if (newFocusIndex < 0) newFocusIndex = 0;
		FocusItem(Items[newFocusIndex]);
	}

	bool MenuListView::HandleKey(int key)
	{
		float scrollMult = 1.0f;
		bool isHandled = false;
		if (zKeyPressed(KEY_LSHIFT)) scrollMult = 10.0f;

		if (key == KEY_A || key == KEY_LEFTARROW || key == KEY_ESCAPE || key == KEY_TAB)
		{ 
			if (SelectedItem)
			{
				RemoveSelection();
				isHandled = true;
			}
		}
		if (key == KEY_D || key == KEY_RIGHTARROW || key == KEY_RETURN || key == KEY_NUMPADENTER)
		{
			if (FocusedItem)
			{
				SelectItem(FocusedItem);
				isHandled = true;
			}
			else if (SelectedItem) isHandled = SelectedItem->HandleKey(key);
		}
		if (key == KEY_W || key == KEY_UPARROW) 
		{ 
			if (SelectedItem) isHandled = SelectedItem->HandleKey(key);
			else
			{
				Scroll(-ScrollStep * scrollMult);
				isHandled = true;
			}			
		}
		if (key == KEY_S || key == KEY_DOWNARROW) 
		{ 
			if (SelectedItem) isHandled = SelectedItem->HandleKey(key);
			else
			{
				Scroll(ScrollStep * scrollMult);
				isHandled = true;
			}
		}
		if (SelectedItem && !isHandled) isHandled = SelectedItem->HandleKey(key);
		return isHandled;
	}
}
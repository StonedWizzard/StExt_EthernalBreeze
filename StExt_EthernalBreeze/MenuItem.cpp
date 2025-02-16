#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	void MenuItem::Init(float posX, float posY, float sizeX, float sizeY)
	{
		BaseMenuItem::Init(posX, posY, sizeX, sizeY);
		AllowSelection = false;
		AllowHover = false;
		AllowInteraction = false;
		bool IsSelected = false;
		bool IsHovered = false;
	}
	void MenuItem::Init() { BaseMenuItem::Init(); }

	void MenuItem::Draw()
	{
		BaseMenuItem::Draw();
		if (!IsVisible || Text.Length() == 0) return;

		int y = ScreenHalfVBufferSize - (View->FontY() * 0.5f);
		int x = ScreenHalfVBufferSize - (View->FontSize(Text) * 0.5f);
		zCOLOR* color = &DefaultColor;
		if (IsSelected && AllowSelection) color = &SelectedColor;
		if (IsHovered && AllowHover) color = &HoveredColor;

		View->SetFontColor(*color);
		View->Print(x, y, Text);
	}

	void MenuItem::Update() { BaseMenuItem::Update(); }

	bool MenuItem::HandleKey(int key)
	{
		if (OnHandleKeyCallback && AllowInteraction)
		{
			OnHandleKeyCallback(this, key);
			return true;
		}
		return false;
	}
}
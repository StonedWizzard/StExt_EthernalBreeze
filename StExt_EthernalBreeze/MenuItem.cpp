#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	MenuItem::MenuItem() 
	{ 
		Text = zSTRING();
		Font = zSTRING(MenuFont_Default);
		Data = Null;
		OwnsData = false;
	}

	void MenuItem::Resize() { BaseMenuElement::Resize(); }

	void MenuItem::Init() 
	{
		Font = MenuFont_Default;

		TextColor_Default = TextColor_Regular_Default;
		TextColor_Hovered = TextColor_Regular_Hovered;
		TextColor_Selected = TextColor_Regular_Selected;
		TextColor_Disabled = TextColor_Regular_Disabled;

		HorizontalAlign = UiContentAlignEnum::Center;
		VerticalAlign = UiContentAlignEnum::Center;
		
		BaseMenuElement::Init();
		if (Font.Length() == 0)
			Font = MenuFont_Default;
	}

	void MenuItem::Draw()
	{
		BaseMenuElement::Draw();
		if (!IsVisible || IsHiden || !Parent) return;

		if (View && Text.Length() > 0)
		{
			int x, y;
			if (HorizontalAlign == UiContentAlignEnum::Begin) x = 0;
			else if (HorizontalAlign == UiContentAlignEnum::End) x = ScreenVBufferSize - View->FontSize(Text);
			else x = static_cast<int>(ScreenHalfVBufferSize - (View->FontSize(Text) * 0.5f));

			if (VerticalAlign == UiContentAlignEnum::Begin) y = 0;
			else if (VerticalAlign == UiContentAlignEnum::End) y = ScreenVBufferSize - View->FontY();
			else y = static_cast<int>(ScreenHalfVBufferSize - (View->FontY() * 0.5f));

			zCOLOR color = TextColor_Default;
			if (IsSelected && HasBehavior(UiElementBehaviorFlags::Selectable)) color = TextColor_Selected;
			if (IsHovered && HasBehavior(UiElementBehaviorFlags::Hoverable)) color = TextColor_Hovered;
			if (!IsEnabled) color = TextColor_Disabled;

			View->SetFontColor(color);
			View->Print(x, y, Text);
		}
	}

	void MenuItem::Update() { BaseMenuElement::Update(); }

	bool MenuItem::HandleMouse(const UiMouseEventArgs& args) 
	{ 
		//DEBUG_MSG("StExt - Mod Ui::HandleMouse() '" + Z(Name.c_str()) + "'...");
		bool isHit = this == FindElementAt(args.X, args.Y);

		if (!isHit) return false;
		if (CanInteract() && (args.Action == UiMouseEnum::LeftClick || args.Action == UiMouseEnum::MiddleClick || args.Action == UiMouseEnum::RightClick)) {
			return OnMouseEvent ? OnMouseEvent(this, args) : true;
		}
		return true;
	}
	bool MenuItem::HandleKey(const UiKeyEventArgs& args) { return (CanInteract() && OnKeyEvent) ? OnKeyEvent(this, args) : false; }

	inline void MenuItem::ClearData()
	{
		if (OwnsData && Data) {
			delete static_cast<char*>(Data);
		}
		Data = Null;
		OwnsData = false;
	}

	MenuItem::~MenuItem() { ClearData(); }
}
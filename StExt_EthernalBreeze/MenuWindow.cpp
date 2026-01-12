#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	MenuWindow::MenuWindow() { }

	// Window size managed by menu controller
	void MenuWindow::Resize() 
	{
		SizeX = ModMenuWindow_SizeX;
		SizeY = ModMenuWindow_SizeY;
		PosX = ModMenuWindow_PosX;
		PosY = ModMenuWindow_PosY;
		BaseMenuPanel::Resize(); 
	}

	void MenuWindow::Init()	
	{
		BgTexture = UiElement_PanelTexture;
		SizeX = ModMenuWindow_SizeX;
		SizeY = ModMenuWindow_SizeY;
		PosX = ModMenuWindow_PosX;
		PosY = ModMenuWindow_PosY;
		HideCursor = false;
		BaseMenuPanel::Init(); 
	}

	void MenuWindow::Draw() 
	{
		if (!screen) return;
		if (!View)
		{
			View = zNEW(zCView)(0, 0, ScreenVBufferSize, ScreenVBufferSize);
			Resize();
			View->InsertBack(BgTexture);
			DEBUG_MSG("StExt - Mod Ui::Draw() '" + Name + "' Texture - '" + BgTexture + "'");
		}

		View->Blit();
		View->ClrPrintwin();
		screen->RemoveItem(View);
		if (IsVisible)
		{
			screen->InsertItem(View);
			View->Render();
			ForEachItem([](uint i, BaseMenuElement* item) { item->Draw(); });

			if (OnDraw) OnDraw(this);
		}
		else
		{
			// handle child elements?
		}
	}

	void MenuWindow::Update() { BaseMenuPanel::Update(); }

	bool MenuWindow::HandleMouse(const UiMouseEventArgs& args) { return BaseMenuPanel::HandleMouse(args); }
	bool MenuWindow::HandleKey(const UiKeyEventArgs& args) 
	{ 
		if (args.Action == UiKeyEnum::Escape)
		{
			CloseMenuWindow();
			return true;
		}
		return BaseMenuPanel::HandleKey(args); 
	}

	MenuWindow::~MenuWindow() {}
}
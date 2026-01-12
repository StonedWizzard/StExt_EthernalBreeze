#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	BaseUiElement::BaseUiElement()
	{
		Parent = Null;
		View = Null;
		BgTexture = zSTRING(UiElement_EmptyTexture);
	}

	void BaseUiElement::Resize()
	{
		//DEBUG_MSG_IF(!View, "StExt - Mod Ui::Resize() '" + Z(Name.c_str()) + "' - View is NULL!");

		LocalSizeX = static_cast<int>(SizeX * ScreenVBufferSize);
		LocalSizeY = static_cast<int>(SizeY * ScreenVBufferSize);
		LocalPosX = static_cast<int>(PosX * ScreenVBufferSize);
		LocalPosY = static_cast<int>(PosY * ScreenVBufferSize);
		
		if (View) {
			View->SetSize(LocalSizeX, LocalSizeY);
			View->SetPos(LocalPosX, LocalPosY);
		}

		if (!Parent)
		{
			GlobalPosX = LocalPosX;
			GlobalPosY = LocalPosY;
			GlobalSizeX = LocalSizeX;
			GlobalSizeY = LocalSizeY;
		}
		else
		{
			GlobalPosX = Parent->GlobalPosX + static_cast<int>(PosX * Parent->GlobalSizeX);
			GlobalPosY = Parent->GlobalPosY + static_cast<int>(PosY * Parent->GlobalSizeY);
			GlobalSizeX = static_cast<int>(SizeX * Parent->GlobalSizeX);
			GlobalSizeY = static_cast<int>(SizeY * Parent->GlobalSizeY);
		}

		GlobalRectLeft = GlobalPosX;
		GlobalRectRight = GlobalPosX + GlobalSizeX;
		GlobalRectTop = GlobalPosY;
		GlobalRectBottom = GlobalPosY + GlobalSizeY;
	}

	void BaseUiElement::Init()
	{
		Name = UiElement_EmptyName;
		IsVisible = true;

		PosX = 0.0f;
		PosY = 0.0f;
		SizeX = 1.0f;
		SizeY = 1.0f;

		if (OnInit) OnInit(this);

		InitialSizeX = SizeX;
		InitialSizeY = SizeY;
		InitialPosX = PosX;
		InitialPosY = PosY;
	}

	void BaseUiElement::Draw()
	{
		//DEBUG_MSG("StExt - Mod Ui::Draw() '" + Z(Name.c_str()) + "'...");

		if (!Parent) return;
		if (!View)
		{
			View = zNEW(zCView)(0, 0, ScreenVBufferSize, ScreenVBufferSize);
			Resize();
			View->InsertBack(BgTexture);

			MenuItem* item = dynamic_cast<MenuItem*>(this);
			if (item) {
				View->SetFont(item->Font);
			}
		}

		View->ClrPrintwin();
		View->Blit();
		Parent->View->RemoveItem(View);
		if (!IsVisible || IsHiden) return;
		
		Parent->View->InsertItem(View);
		//View->Render();
		if (OnDraw) OnDraw(this);
	}

	void BaseUiElement::Update() 
	{
		//DEBUG_MSG("StExt - Mod Ui::Update() '" + Z(Name.c_str()) + "'...");
		if (OnUpdate) OnUpdate(this);
	}

	BaseUiElement::~BaseUiElement()
	{
		if (View)
		{
			View->ClrPrintwin();
			View->Blit();
			if (Parent)
				Parent->View->RemoveItem(View);
		}
		SAFE_DELETE(View);
		View = Null;
	}
}
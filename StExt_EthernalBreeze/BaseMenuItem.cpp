#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	void BaseMenuItem::Resize()
	{
		int sizeX = ScreenVBufferSize * SizeX;
		int sizeY = ScreenVBufferSize * SizeY;
		int posX = ScreenVBufferSize * PosX;
		int posY = ScreenVBufferSize * PosY;

		View->SetSize(sizeX, sizeY);
		View->SetPos(posX, posY);
	}

	void BaseMenuItem::Init(float posX, float posY, float sizeX, float sizeY)
	{
		View = zNEW(zCView)(0, 0, ScreenVBufferSize, ScreenVBufferSize);
		BgTexture = "";

		PosX = posX;
		PosY = posY;
		SizeX = sizeX;
		SizeY = sizeY;

		IsVisible = true;
		if (!ParentView) ParentView = screen;

		Resize();
		if (BgTexture.Length() > 0) View->InsertBack(BgTexture);
	}	
	void BaseMenuItem::Init() { Init(0.0f, 0.0f, 1.0f, 1.0f); }

	void BaseMenuItem::Draw()
	{
		ParentView->RemoveItem(View);
		if (!IsVisible) return;
		ParentView->InsertItem(View);
	}

	void BaseMenuItem::Update()
	{
		if (OnUpdateCallback)
			OnUpdateCallback(this);
	}

	BaseMenuItem::~BaseMenuItem() 
	{
		OnUpdateCallback = Null;
		if (View)
		{
			View->Blit();
			View->ClrPrintwin();
			if(ParentView)
				ParentView->RemoveItem(View);
		}
		SAFE_DELETE(View);
		View = Null;		
	}
}
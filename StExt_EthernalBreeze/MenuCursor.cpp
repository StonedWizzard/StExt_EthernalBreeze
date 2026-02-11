#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	// -----------------------------------------------
	//				Cursor controller
	// -----------------------------------------------

	inline void ClampCoords(float& x, float& y)
	{
		if (x < 0.0f) x = 0.0f;
		else if (x > 1.0f) x = 1.0f;

		if (y < 0.0f) y = 0.0f;
		else if (y > 1.0f) y = 1.0f;
	}
	inline void ClampCoords(int& x, int& y)
	{
		if (x < 0) x = 0;
		else if (x > ScreenVBufferSize) x = ScreenVBufferSize;

		if (y < 0) y = 0;
		else if (y > ScreenVBufferSize) y = ScreenVBufferSize;
	}

	void MenuCursorController::Init()
	{
		DEBUG_MSG("StExt - MenuCursorController::Init...");

		ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		ScreenRatioX = static_cast<float>(ScreenVBufferSize) / (float)ScreenWidth;
		ScreenRatioY = static_cast<float>(ScreenVBufferSize) / (float)ScreenHeight;

		MouseSensitivity = parser->GetSymbol("StExt_Config_Ui_Mouse_MoveSensitivity")->single_floatdata;
		ScrollSensitivity = parser->GetSymbol("StExt_Config_Ui_Mouse_ScrollSensitivity")->single_floatdata;

		MouseState = UiMouseEventArgs();
		MouseState.X = ScreenHalfVBufferSize;
		MouseState.Y = ScreenHalfVBufferSize;
		MouseState.ScrollDelta = 0;
		MouseState.Action = UiMouseEnum::Empty;

		IsInitialized = true;
	}

	void MenuCursorController::Update()	
	{
		if (!zinput) return;

		float fx = 0.0f, fy = 0.0f, fz = 0.0f;
		POINT pt; ::GetCursorPos(&pt);
		zinput->GetMousePos(fx, fy, fz);

		MouseState.X = static_cast<int>(pt.x * ScreenRatioX);
		MouseState.Y = static_cast<int>(pt.y * ScreenRatioY);
		ClampCoords(MouseState.X, MouseState.Y);

		bool lDown = zKeyPressed(MOUSE_BUTTONLEFT);
		if (lDown && !LeftBtnPressed) LeftBtnPressed = true;		
		else if (!lDown && LeftBtnPressed)
		{
			LeftBtnPressed = false;
			MouseState.Action = UiMouseEnum::LeftClick;
			HasEvent = true;
		}

		bool rDown = zKeyPressed(MOUSE_BUTTONRIGHT);
		if (rDown && !RightBtnPressed) RightBtnPressed = true;
		else if (!rDown && RightBtnPressed)
		{
			RightBtnPressed = false;
			MouseState.Action = UiMouseEnum::RightClick;
			HasEvent = true;
		}

		if (zKeyPressed(MOUSE_WHEELUP))
		{
			MouseState.ScrollDelta = static_cast<int>(fz * ScrollSensitivity);
			MouseState.Action = UiMouseEnum::Scroll;
			HasEvent = true;
		}
		if (zKeyPressed(MOUSE_WHEELDOWN))
		{
			MouseState.ScrollDelta = static_cast<int>(fz * ScrollSensitivity);
			MouseState.Action = UiMouseEnum::Scroll;
			HasEvent = true;
		}
	}

	void MenuCursorController::CreateMouseDescriptor(UiMouseEventArgs& desc)
	{
		/*
		DEBUG_MSG("CreateMouseDescriptor() - MouseInfo((X: " + Z(MouseState.X) + " | Y: " + Z(MouseState.Y) +
			"); Action:" + Z((int)MouseState.Action) + " ; HasEvent:" + Z(HasEvent) + ")");
		*/
		desc.X = MouseState.X;
		desc.Y = MouseState.Y;
		desc.ScrollDelta = MouseState.ScrollDelta;
		if (zinput)
		{
			desc.IsShiftPressed = (zKeyPressed(KEY_LSHIFT) || zKeyPressed(KEY_RSHIFT));
			desc.IsAltPressed = (zKeyPressed(KEY_LALT) || zKeyPressed(KEY_RALT));
		}
		else
		{
			desc.IsShiftPressed = false;
			desc.IsAltPressed = false;
		}
		desc.Action = MouseState.Action;

		if (HasEvent)
		{
			LeftBtnPressed = LeftBtnReleased = false;
			RightBtnPressed = RightBtnReleased = false;
			MiddleBtnPressed = MiddleBtnReleased = false;
			MouseState.Action = UiMouseEnum::Empty;
			MouseState.ScrollDelta = 0;
			HasEvent = false;
		}
	}

	MenuCursorController::~MenuCursorController() { }

	// -----------------------------------------------
	//					Cursor view
	// -----------------------------------------------

	void MenuCursor::SetController(MenuCursorController* controller)
	{
		if (!Controller && controller)
			Controller = controller;
	}

	void MenuCursor::Init()	
	{
		Parent = Null;
		Controller = Null;
		BaseUiElement::Init();
		IsHiden = true;
	}

	void MenuCursor::Resize() 
	{ 
		//BaseUiElement::Resize(); 

		LocalPosX = static_cast<int>(PosX * ScreenVBufferSize);
		LocalPosY = static_cast<int>(PosY * ScreenVBufferSize);
		LocalSizeX = static_cast<int>(SizeX * ScreenVBufferSize);
		LocalSizeY = static_cast<int>(SizeY * ScreenVBufferSize);

		if (Controller)
		{
			LocalSizeX = static_cast<int>(LocalSizeX * Controller->ScreenRatioX);
			LocalSizeY = static_cast<int>(LocalSizeY * Controller->ScreenRatioY);
		}

		if (View)
		{
			View->SetSize(LocalSizeX, LocalSizeY);
			View->SetPos(LocalPosX, LocalPosY);
		}

		GlobalPosX = LocalPosX;
		GlobalPosY = LocalPosY;
		GlobalSizeX = LocalSizeX;
		GlobalSizeY = LocalSizeY;
	}

	void MenuCursor::Draw() 
	{
		/*
		DEBUG_MSG(
			"Cursor::Draw() - View: " + Z((int)View) +
			" PosX: " + Z(PosX) +
			" PosY: " + Z(PosY) +
			" SizeX: " + Z(SizeX) +
			" SizeY: " + Z(SizeY) +
			" IsVisible: " + Z(IsVisible) +
			" IsHiden: " + Z(IsHiden) +
			" BgTexture: " + BgTexture
		);
*/
		if (!screen) return;
		if (!View)
		{
			Resize();
			View = zNEW(zCView)(GlobalPosX, GlobalPosY, GlobalSizeX, GlobalSizeY);			
			View->InsertBack(BgTexture);
		}

		View->Blit();
		View->ClrPrintwin();
		screen->RemoveItem(View);
		if (IsVisible && !IsHiden)
		{
			screen->InsertItem(View);
			View->Render();
		}
	}

	void MenuCursor::Update() 
	{ 
		if (Controller)
		{
			PosX = Controller->MouseState.X * ScreenToRelativePixDelta;
			PosY = Controller->MouseState.Y * ScreenToRelativePixDelta;
			ClampCoords(PosX, PosY);
			//DEBUG_MSG("MouseState (Controller) = [X: " + Z(Controller->MouseState.X) + "; Y: " + Z(Controller->MouseState.Y) + "]");
			//DEBUG_MSG("MouseState = [X: " + Z(PosX) + "; Y: " + Z(PosY) + "]");
			Resize();
		}
		BaseUiElement::Update(); 
	}

	MenuCursor::~MenuCursor() {}
}
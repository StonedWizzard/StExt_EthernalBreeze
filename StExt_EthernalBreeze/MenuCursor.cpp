#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	// -----------------------------------------------
	//				Cursor controller
	// -----------------------------------------------

	MenuCursorController* MenuCursorController::Instance = Null;
	HHOOK MenuCursorController::MouseHook = Null;

	inline void ClampCoords(float& x, float& y)
	{
		if (x < 0.0f) x = 0.0f;
		else if (x > 1.0f) x = 1.0f;

		if (y < 0.0f) y = 0.0f;
		else if (y > 1.0f) y = 1.0f;
	}
	inline void ClampCoords(long& x, long& y)
	{
		if (x < 0) x = 0;
		else if (x > ScreenVBufferSize) x = ScreenVBufferSize;

		if (y < 0) y = 0;
		else if (y > ScreenVBufferSize) y = ScreenVBufferSize;
	}
	inline void ClampCoords(int& x, int& y)
	{
		if (x < 0) x = 0;
		else if (x > ScreenVBufferSize) x = ScreenVBufferSize;

		if (y < 0) y = 0;
		else if (y > ScreenVBufferSize) y = ScreenVBufferSize;
	}

	// -----------------------------------------------
	//					Cursor HOOKS
	// -----------------------------------------------

	// Low-level mouse hook callback
	LRESULT CALLBACK MenuCursorController::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode >= 0 && Instance)
		{
			MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
			auto& st = Instance->MouseState;

			switch (wParam)
			{
				case WM_MOUSEMOVE:
				{
					st.X = static_cast<int>((long)p->pt.x * Instance->ScreenRatioX);
					st.Y = static_cast<int>((long)p->pt.y * Instance->ScreenRatioY);
					ClampCoords(st.X, st.Y);
					break;
				}

				case WM_LBUTTONDOWN:
					Instance->LeftBtnPressed = true;
					break;
				case WM_LBUTTONUP:
					if (Instance->LeftBtnPressed) {
						Instance->LeftBtnReleased = true;
						st.Action = UiMouseEnum::LeftClick;
						Instance->HasEvent = true;
					}
					break;

				case WM_RBUTTONDOWN:
					Instance->RightBtnPressed = true;
					break;
				case WM_RBUTTONUP:
					if (Instance->RightBtnPressed) {
						Instance->RightBtnReleased = true;
						st.Action = UiMouseEnum::RightClick;
						Instance->HasEvent = true;
					}
					break;

				case WM_MBUTTONDOWN:
					Instance->MiddleBtnPressed = true;
					break;
				case WM_MBUTTONUP:
					if (Instance->MiddleBtnPressed) {
						Instance->MiddleBtnReleased = true;
						st.Action = UiMouseEnum::MiddleClick;
						Instance->HasEvent = true;
					}
					break;

				case WM_MOUSEWHEEL:
				{
					short delta = GET_WHEEL_DELTA_WPARAM(p->mouseData);
					st.ScrollDelta = delta;
					st.Action = UiMouseEnum::Scroll;
					Instance->HasEvent = true;
					break;
				}
			}
		}

		//DEBUG_MSG("MouseState (Hook) = [X: " + Z(Instance->MouseState.X) + "; Y: " + Z(Instance->MouseState.Y) + "]");
		return CallNextHookEx(MenuCursorController::MouseHook, nCode, wParam, lParam);
	}

	void MenuCursorController::Init()
	{
		DEBUG_MSG("StExt - MenuCursorController::Init...");
		HWND hwnd = hWndApp;
		if (!hwnd || !IsWindow(hwnd))
		{
			DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Fail to initialize mouse handler (window handler not found)!...");
			return;
		}

		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = hwnd;

		if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
			DEBUG_MSG("StExt - Initialize mod Ui: menu framework -  Fail to initialize window mouse handler (fail to register raw input device)");
			return;
		}

		if (!MouseHook)
		{
			HINSTANCE hInst = GetModuleHandle(NULL);
			//MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInst, 0);
			MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
			if (!MouseHook)
			{
				DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Fail to set global mouse hook!");
				return;
			}
		}
		Instance = this;		

		ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		ScreenRatioX = static_cast<float>(ScreenVBufferSize) / (float)ScreenWidth;
		ScreenRatioY = static_cast<float>(ScreenVBufferSize) / (float)ScreenHeight;
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Screen size: " + Z(ScreenWidth) + "x" + Z(ScreenHeight));

		MouseState = UiMouseEventArgs();
		MouseState.X = ScreenHalfVBufferSize;
		MouseState.Y = ScreenHalfVBufferSize;
		MouseState.ScrollDelta = 0;
		MouseState.Action = UiMouseEnum::Empty;

		IsInitialized = true;
	}

	void MenuCursorController::Update()	
	{
		if (!IsHookChecked || !MouseHook)
		{
			if (MouseHook)
			{
				UnhookWindowsHookEx(MouseHook);
				MouseHook = NULL;
			}

			HHOOK newHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
			if (newHook)
			{
				MouseHook = newHook;
				IsHookChecked = true;
				DEBUG_MSG("MenuCursorController - Mouse hook (re)initialized successfully.");
			}
			else { DEBUG_MSG("MenuCursorController - Mouse hook (re)initialization failed."); }
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
		desc.IsShiftPressed = zinput ? (zKeyPressed(KEY_LSHIFT) || zKeyPressed(KEY_RSHIFT)) : false;
		desc.IsAltPressed = zinput ? (zKeyPressed(KEY_LALT) || zKeyPressed(KEY_RALT)) : false;
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

	MenuCursorController::~MenuCursorController()
	{
		if (MouseHook)
		{
			UnhookWindowsHookEx(MouseHook);
			MouseHook = Null;
		}

		if (Instance == this)
			Instance = Null;
	}

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
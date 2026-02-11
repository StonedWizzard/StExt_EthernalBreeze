//#include <UnionAfx.h>
#include <StonedExtension.h>

// Controller manage all mods windows and cursor for them
namespace Gothic_II_Addon
{
	StringMap<MenuWindow*> MenuWindows(16);
	MenuWindow* CurrentMenu;

	MenuCursorController* CursorController;
	MenuCursor* Cursor;
	ItemInfoPanel* ExtraItemInfoPanel;

	bool ModMenuControllerInitialized;
	bool ModMenuCursorControllerInitialized;

	const float EPS = 0.0001f;
	bool ModMenuWindowRequireResize;
	float ModMenuWindow_PosX, ModMenuWindow_PosY;
	float ModMenuWindow_SizeX, ModMenuWindow_SizeY;
	float ModMenuWindow_OldSizeX, ModMenuWindow_OldSizeY;
	float ModMenuWindow_ScrollMult;
	float ModMenuWindow_ScrollShiftMult;
	float ExtraItemInfoPanel_PosX, ExtraItemInfoPanel_PosY;
	float ModMenuItem_EmdedPanelsScaleMult = 1.0f;

	int StatsWindowKey;
	int CraftWindowKey;
	int ConfigsWindowKey;
	int DoPauseOnWindow;

	zCOLOR TextColor_Default = zCOLOR(250, 250, 250, 255);
	zCOLOR TextColor_Warn = zCOLOR(250, 200, 0, 255);
	const zCOLOR MasteriesRankColor[] = 
	{
		zCOLOR(250, 100, 100, 255),
		zCOLOR(250, 200, 0, 255),
		zCOLOR(200, 230, 0, 255),
		zCOLOR(170, 240, 0, 255),
		zCOLOR(130, 250, 0, 255),
		zCOLOR(100, 250, 0, 255)
	};

	zCOLOR TextColor_Yes = zCOLOR(0, 250, 0, 255);
	zCOLOR TextColor_No = zCOLOR(250, 0, 0, 255);

	zCOLOR TextColor_Regular_Default = zCOLOR(235, 235, 235, 255);
	zCOLOR TextColor_Regular_Faded = zCOLOR(200, 200, 200, 255);
	zCOLOR TextColor_Regular_Hovered = zCOLOR(0, 180, 250, 255);
	zCOLOR TextColor_Regular_Selected = zCOLOR(0, 250, 0, 255);
	zCOLOR TextColor_Regular_Disabled = zCOLOR(120, 120, 120, 250);

	zCOLOR TextColor_Header_Default = zCOLOR(255, 215, 0, 255);
	zCOLOR TextColor_Header_Hovered = TextColor_Regular_Hovered;
	zCOLOR TextColor_Header_Selected = zCOLOR(218, 165, 32, 255);
	zCOLOR TextColor_Header_Disabled = zCOLOR(128, 110, 40, 255);

	zCOLOR TextColor_HeaderTab_Default = zCOLOR(250, 240, 180, 255);
	zCOLOR TextColor_HeaderTab_Hovered = TextColor_Regular_Hovered;
	zCOLOR TextColor_HeaderTab_Selected = zCOLOR(250, 220, 0, 255);
	zCOLOR TextColor_HeaderTab_Disabled = zCOLOR(120, 120, 120, 250);

	zSTRING MenuFont_Header;
	zSTRING MenuFont_Default;
	zSTRING MenuFont_Symbol_Default;
	zSTRING MenuFont_Symbol_Header;
	zSTRING MenuFont_Sys_Header;
	zSTRING MenuFont_Sys_Default;	

	zSTRING MenuStr_Yes;
	zSTRING MenuStr_No;

	zSTRING MenuStr_Learned_Yes;
	zSTRING MenuStr_Learned_No;

	zSTRING MenuStr_On;
	zSTRING MenuStr_Off;


	void ConvertValueToYesNo(zSTRING& str, int val) { str = (val == 0) ? MenuStr_No : MenuStr_Yes; }
	void ConvertValueToSkill(zSTRING& str, int val) { str = (val == 0) ? MenuStr_Learned_No : MenuStr_Learned_Yes; }
	void ConvertValueToPermille(zSTRING& str, int val)
	{
		if (val == 0) str = zSTRING("0.0%");
		else
		{
			float tmp = val * 0.1f;
			str = zSTRING(tmp) + "%";
		}
	}
	void ConvertValueToMasteryRank(zSTRING& str, int val) 
	{ 
		if (val < 0) val = 0;
		if (val > 6) val = 6;			
		str = parser->GetSymbol("StExt_Str_MasteryLevels")->stringdata[val];
	}

	void BoolValueColorFormatter(zCOLOR& color, int value) { color = (value != 0) ? TextColor_Yes : TextColor_No; }
	void SkillValueColorFormatter(zCOLOR& color, int value)
	{
		if (value < 0) value = 0;
		if (value >= 5) value = 5;
		color = MasteriesRankColor[value];
	}

	MenuItem* BuildHSeparator(const float posY)
	{
		MenuItem* separator = new MenuItem();
		separator->OnInit = [posY](BaseUiElement* item)
		{
			item->Name = "HSeparator";
			item->SizeX = 0.98f;
			item->SizeY = 0.005f;
			item->PosX = 0.01f;
			item->PosY = posY;
			item->BgTexture = UiElement_HSeparatorTexture;
		};
		separator->SizeY = 0.005f;
		separator->SizeX = 0.98f;
		return separator;
	}

	MenuItem* BuildVSeparator(const float posX)
	{
		MenuItem* separator = new MenuItem();
		separator->OnInit = [posX](BaseUiElement* item)
		{
			item->Name = "VSeparator";
			item->SizeX = 0.005f;
			item->SizeY = 0.98f;
			item->PosX = posX;
			item->PosY = 0.01f;
			item->BgTexture = UiElement_VSeparatorTexture;
		};
		separator->SizeX = 0.005f;
		separator->SizeY = 0.98f;
		return separator;
	}

	void UpdateCheckButtonStyle(MenuItem* button, zSTRING& valueSym)
	{
		if (!button) return;

		zCPar_Symbol* sym = parser->GetSymbol(valueSym);
		if (!sym) return;

		bool isOn = false;
		switch (sym->type)
		{
			case zPAR_TYPE_STRING:
				isOn = !sym->stringdata->IsEmpty();
				break;

			case zPAR_TYPE_FLOAT:
				isOn = sym->single_floatdata != 0;
				break;

			case zPAR_TYPE_INT:
				isOn = sym->single_intdata != 0;
				break;
		}

		if (isOn)
		{
			button->Text = MenuStr_Off;
			button->TextColor_Hovered = TextColor_No;
		}
		else
		{
			button->Text = MenuStr_On;
			button->TextColor_Hovered = TextColor_Yes;
		}
	}


	inline unsigned char HexToBytePair(const char* s)
	{
		auto hexToByte = [](char c) -> unsigned char 
		{ 
			return (c >= '0' && c <= '9') ? (c - '0') :
				(c >= 'a' && c <= 'f') ? (c - 'a' + 10) :
				(c >= 'A' && c <= 'F') ? (c - 'A' + 10) : 0;
		};
		return (hexToByte(s[0]) << 4) | hexToByte(s[1]);
	}
	inline void ParseHexColor(const zSTRING& stringColor, zCOLOR& color)
	{
		color = TextColor_Default;
		if (stringColor.IsEmpty() || stringColor.Length() != 7) return;

		const char* s = stringColor.ToChar();
		if (s[0] != '#') return;
		color.SetRGB(HexToBytePair(s + 1), HexToBytePair(s + 3), HexToBytePair(s + 5));
	}

	//----------------------------------------------------------------------
	//								LOGIC
	//----------------------------------------------------------------------

	void UpdateWindowConfigs()
	{
		StatsWindowKey = parser->GetSymbol("StExt_Config_ModMenuKey")->single_intdata;
		CraftWindowKey = parser->GetSymbol("StExt_Config_ModCraftKey")->single_intdata;
		ConfigsWindowKey = parser->GetSymbol("StExt_Config_ModOptionsKey")->single_intdata;
		DoPauseOnWindow = parser->GetSymbol("StExt_Config_MenuPauseOnShow")->single_intdata;

		ExtraItemInfoPanel_PosX = parser->GetSymbol("StExt_Config_ItemMenu_PosX")->single_intdata * 0.01f;
		ExtraItemInfoPanel_PosY = parser->GetSymbol("StExt_Config_ItemMenu_PosY")->single_intdata * 0.01f;

		ModMenuWindow_ScrollMult = parser->GetSymbol("StExt_Config_MenuScrollMult")->single_intdata * 0.01f;
		if (zinput)
			ModMenuWindow_ScrollShiftMult = (zKeyPressed(KEY_LSHIFT) || zKeyPressed(KEY_RSHIFT)) ? 10.0f : 1.0f;
		else ModMenuWindow_ScrollShiftMult = 1.0f;

		int scaleX = parser->GetSymbol("StExt_Config_MenuScaleX")->single_intdata;
		int scaleY = parser->GetSymbol("StExt_Config_MenuScaleY")->single_intdata;
		
		scaleX = ValidateValue(scaleX, 30, 99);
		scaleY = ValidateValue(scaleY, 30, 99);

		ModMenuWindow_SizeX = scaleX * 0.01f;
		ModMenuWindow_SizeY = scaleY * 0.01f;

		constexpr float EPS = 0.0001f;
		ModMenuWindowRequireResize =
			(fabsf(ModMenuWindow_OldSizeX - scaleX) > EPS) || (fabsf(ModMenuWindow_OldSizeY - scaleY) > EPS);

		ModMenuWindow_PosX = (1.0f - ModMenuWindow_SizeX) * 0.5f;
		ModMenuWindow_PosY = (1.0f - ModMenuWindow_SizeY) * 0.5f;

		ModMenuWindow_OldSizeX = ModMenuWindow_SizeX;
		ModMenuWindow_OldSizeY = ModMenuWindow_SizeY;
	}

	void InitializeMouse()
	{
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Initialize Mouse...");
		CursorController = new MenuCursorController();
		CursorController->Init();

		ModMenuCursorControllerInitialized = (CursorController && CursorController->IsInitialized);
		Cursor = new MenuCursor();
		Cursor->OnInit = [](BaseUiElement* item) {
			item->Name = "Cursor";
			item->BgTexture = UiElement_CursorTexture;
			item->SizeX = CursorSize;
			item->SizeY = CursorSize;
		};

		Cursor->Init();
		Cursor->SetController(CursorController);
		Cursor->IsVisible = ModMenuCursorControllerInitialized;
		Cursor->IsHiden = true;

		DEBUG_MSG_IFELSE(!ModMenuCursorControllerInitialized, 
			"StExt - Mod Ui: menu framework - Mouse initialization failed!",
			"StExt - Mod Ui: menu framework - Mouse initialization done!");
	}

	void InitializeWindows()
	{
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Initialize Windows...");

		MenuFont_Header = parser->GetSymbol("font_screen")->stringdata;
		MenuFont_Default = parser->GetSymbol("font_screensmall")->stringdata;
		MenuFont_Symbol_Default = parser->GetSymbol("font_prolog")->stringdata;
		MenuFont_Symbol_Header = parser->GetSymbol("font_prologmain")->stringdata;
		MenuFont_Sys_Header = parser->GetSymbol("text_font_20")->stringdata;
		MenuFont_Sys_Default = parser->GetSymbol("text_font_10")->stringdata;
		ModMenuItem_EmdedPanelsScaleMult = parser->GetSymbol("StExt_Config_Ui_EmbdedPanelsScale")->single_floatdata;
		UpdateWindowConfigs();

		TestWindow* testWnd = new TestWindow();
		testWnd->Init();
		MenuWindows.Insert(testWnd->Name, testWnd);

		StatsWindow* statsWnd = new StatsWindow();
		statsWnd->Init();
		MenuWindows.Insert(statsWnd->Name, statsWnd);

		ConfigsWindow* configsWnd = new ConfigsWindow();
		configsWnd->Init();
		MenuWindows.Insert(configsWnd->Name, configsWnd);

		/*
		CraftWindow* craftWnd = new CraftWindow();
		craftWnd->Init();
		MenuWindows.Insert(craftWnd->Name, craftWnd);
		*/

		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Initialize Windows done!");
	}

	void InitializeExtraItemInfoPanel()
	{
		ExtraItemInfoPanel = new ItemInfoPanel();
		ExtraItemInfoPanel->OnInit = [](BaseUiElement* item) 
		{
			item->Name = "ExtraItemInfoPanel";
			item->BgTexture = UiElement_ExtraItemInfoPanelTexture;
			item->SizeX = 0.1f;
			item->SizeY = 0.1f;
		};
		ExtraItemInfoPanel->Init();
		ExtraItemInfoPanel->IsVisible = false;
		ExtraItemInfoPanel->IsHiden = false;
	}

	void StonedExtension_InitUi_Menu()
	{
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework...");

		MenuStr_Yes = parser->GetSymbol("StExt_Str_Yes")->stringdata;
		MenuStr_No = parser->GetSymbol("StExt_Str_No")->stringdata;

		MenuStr_Learned_Yes = parser->GetSymbol("StExt_Str_Learned_Yes")->stringdata;
		MenuStr_Learned_No = parser->GetSymbol("StExt_Str_Learned_No")->stringdata;
		
		MenuStr_On = parser->GetSymbol("StExt_Str_On")->stringdata;
		MenuStr_Off = parser->GetSymbol("StExt_Str_Off")->stringdata;
		
		InitializeMouse();
		InitializeWindows();
		InitializeExtraItemInfoPanel();

		CurrentMenu = Null;
		ModMenuWindowRequireResize = false;
		ModMenuControllerInitialized = true;
		
		DEBUG_MSG("StExt - Mod Ui: menu framework was initialized!");
	}

	void CreateKeyDescriptor(const int key, UiKeyEventArgs& desc)
	{
		desc.KeyId = key;
		desc.IsShiftPressed = (zKeyPressed(KEY_LSHIFT) || zKeyPressed(KEY_RSHIFT));
		desc.IsAltPressed = (zKeyPressed(KEY_LALT) || zKeyPressed(KEY_RALT));

		switch (key)
		{
			case KEY_ESCAPE: desc.Action = UiKeyEnum::Escape; break;
			case KEY_TAB: desc.Action = UiKeyEnum::Escape; break;
			case KEY_RETURN: desc.Action = UiKeyEnum::Enter; break;
			case KEY_NUMPADENTER: desc.Action = UiKeyEnum::Enter; break;
			case KEY_BACKSPACE: desc.Action = UiKeyEnum::Back; break;

			case KEY_A: desc.Action = UiKeyEnum::Left; break;
			case KEY_LEFTARROW: desc.Action = UiKeyEnum::Left; break;
			case KEY_D: desc.Action = UiKeyEnum::Right; break;
			case KEY_RIGHTARROW: desc.Action = UiKeyEnum::Right; break;
			case KEY_W: desc.Action = UiKeyEnum::Up; break;
			case KEY_UPARROW: desc.Action = UiKeyEnum::Up; break;
			case KEY_S: desc.Action = UiKeyEnum::Down; break;
			case KEY_DOWNARROW: desc.Action = UiKeyEnum::Down; break;

			default: desc.Action = UiKeyEnum::Empty; break;
		}
	}

	inline void CheckPcInput()
	{
		if (!player) return;

		bool doBlock = CurrentMenu != Null;
		player->SetMovLock(BlockMovement || doBlock);
		if (DoPauseOnWindow && CurrentMenu) ogame->Pause();
	}

	int HandleModKey(const int key)
	{
		bool isHandled = false;
		UiKeyEventArgs desc = UiKeyEventArgs();
		CreateKeyDescriptor(key, desc);
		int isModDialog = parser->GetSymbol("StExt_DisplayModMenu")->single_intdata;

		if (CurrentMenu) isHandled = CurrentMenu->HandleKey(desc);
		else
		{
			isHandled = *(int*)parser->CallFunc(HandleKeyEventFunc, desc.KeyId);
			if (!isModDialog && !isHandled && CanShowModMenu)
			{
				if (desc.KeyId == StatsWindowKey && desc.IsShiftPressed) { isHandled = OpenMenuWindow("StatsWindow"); }
				else if (desc.KeyId == CraftWindowKey && desc.IsAltPressed) { isHandled = OpenMenuWindow("CraftWindow"); }
				else if (desc.KeyId == ConfigsWindowKey && desc.IsShiftPressed) { isHandled = OpenMenuWindow("ConfigsWindow"); }				
			}
		}
		return isHandled;
	}

	int HandleModMouse()
	{
		if (!CurrentMenu || !ModMenuControllerInitialized || !ModMenuCursorControllerInitialized) return false;

		bool isHandled = false;
		UiMouseEventArgs desc = UiMouseEventArgs();
		CursorController->CreateMouseDescriptor(desc);
		isHandled = CurrentMenu->HandleMouse(desc);
		
		return (int)isHandled;
	}

	void StonedExtension_Loop_MenuController()
	{
		if (!ogame || !player || !screen) return;
		if (!ModMenuControllerInitialized) return;
		
		// cursor handling
		bool hasCursor = (Cursor && CursorController) && ModMenuCursorControllerInitialized;
		if (hasCursor)
		{
			CursorController->Update();
			Cursor->Update();
		}

		// menu handling
		UpdateWindowConfigs();
		if (CurrentMenu)
		{
			if (hasCursor && !CurrentMenu->HideCursor) {
				HandleModMouse();
			}

			if (ModMenuWindowRequireResize)
			{
				CurrentMenu->Resize();
				ModMenuWindowRequireResize = false;				
			}
			CurrentMenu->Update();
			CurrentMenu->Draw();
		}
		else 
		{ 
			ModMenuWindowRequireResize = false; 
			if (Cursor)
				Cursor->IsHiden = true;
		}

		// draw cursor
		if (hasCursor) { Cursor->Draw(); }
		CheckPcInput();
	}

	inline bool IsMenuWindowOpen() { return CurrentMenu != Null; }

	bool OpenMenuWindow(const zSTRING& name)
	{
		if (CurrentMenu)
		{
			DEBUG_MSG("OpenMenuWindow - Can't open window, while another one is visible.");
			return false;
		}

		auto wnd = MenuWindows.Find(name);
		if (!wnd)
		{
			DEBUG_MSG("OpenMenuWindow - such window not registered: '" + name + "!'");
			CurrentMenu = Null;
			return false;
		}

		UpdateWindowConfigs();
		CurrentMenu = *wnd;
		CurrentMenu->IsVisible = true;
		if (CurrentMenu->OnOpen)
			CurrentMenu->OnOpen(CurrentMenu);

		if (Cursor)
		{
			Cursor->IsVisible = ModMenuCursorControllerInitialized;
			Cursor->IsHiden = CurrentMenu->HideCursor;
		}
		if (DoPauseOnWindow) ogame->Pause();
		return true;
	}

	void CloseMenuWindow() 
	{
		if (!CurrentMenu) return;
		if (CurrentMenu->OnClose)
			CurrentMenu->OnClose(CurrentMenu);

		CurrentMenu->IsVisible = false;
		CurrentMenu->Draw();
		CurrentMenu = Null;

		if (Cursor)
		{
			Cursor->IsVisible = ModMenuCursorControllerInitialized;
			Cursor->IsHiden = true;
		}
		ogame->Unpause();
	}

	inline void UpdateExtraItemInfoPanelState(const oCItem* item)
	{
		if (!ExtraItemInfoPanel) return;
		ExtraItemInfoPanel->IsVisible = item != Null;
		ExtraItemInfoPanel->SetDisplayItem(item);
		ExtraItemInfoPanel->Update();
		ExtraItemInfoPanel->Draw();
	}
	
	HOOK Hook_oCItemContainer_DrawItemInfo PATCH(&oCItemContainer::DrawItemInfo, &oCItemContainer::DrawItemInfo_StExt);
	void oCItemContainer::DrawItemInfo_StExt(oCItem* item, zCWorld* world)
	{
		THISCALL(Hook_oCItemContainer_DrawItemInfo)(item, world);
		UpdateExtraItemInfoPanelState(item);
	}

	HOOK Hook_oCItemContainer_Close PATCH(&oCItemContainer::Close, &oCItemContainer::Close_StExt);
	void oCItemContainer::Close_StExt()
	{
		THISCALL(Hook_oCItemContainer_Close)();
		UpdateExtraItemInfoPanelState(Null);
	}

	HOOK Hook_oCNpcInventory_Close PATCH(&oCNpcInventory::Close, &oCNpcInventory::Close_StExt);
	void oCNpcInventory::Close_StExt()
	{
		THISCALL(Hook_oCNpcInventory_Close)();
		UpdateExtraItemInfoPanelState(Null);
	}

	HOOK ivk_oCGame_HandleEvent PATCH(&oCGame::HandleEvent, &oCGame::HandleEvent_StExt);
	int oCGame::HandleEvent_StExt(int key) 
	{
		if (IsLoading || IsLevelChanging) return true;
		bool canProcess = ogame && player && screen && !GetWorld()->csPlayer->GetPlayingGlobalCutscene();

		if (canProcess)
		{
			int isHandled = HandleModKey(key);
			if (isHandled) return true;
		}
		return THISCALL(ivk_oCGame_HandleEvent) (key);
	}

	HOOK ivk_zCViewDialogChoice_HandleEvent PATCH(&zCViewDialogChoice::HandleEvent, &zCViewDialogChoice::HandleEvent_StExt);
	int zCViewDialogChoice::HandleEvent_StExt(int key)
	{
		int isModMenu = parser->GetSymbol("StExt_DisplayModMenu")->single_intdata;
		bool isDirKeys = (key == KEY_LEFTARROW || key == KEY_RIGHTARROW || key == KEY_UPARROW || key == KEY_DOWNARROW);
		if (isModMenu && isDirKeys)
		{
			int isHandled = *(int*)parser->CallFunc(HandleUiButtomEventFunc, key);
			if (isHandled) return True;
		}
		return THISCALL(ivk_zCViewDialogChoice_HandleEvent) (key);
	}
}
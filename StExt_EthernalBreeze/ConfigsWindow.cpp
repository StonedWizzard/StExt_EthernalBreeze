#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	ConfigsWindow* Instance = Null;

	struct ConfigPanelArgs
	{
		float PosY;
		int DisplayType;
		bool IsEditable;
		zSTRING TitleSymbol;
		zSTRING ValueSymbol;
	};

	//----------------------------------------------------------------------
	//								HANDLERS
	//----------------------------------------------------------------------

	bool ConfigsWindow_OnTabButtonClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;

		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			int index = *itm->GetData<int>();
			if (index < 0) index = 0;
			if (index >= Instance->TabIndexMax) index = Instance->TabIndexMax - 1;
			Instance->TabIndex = index;
		}
		return true;
	}

	bool ConfigsWindow_OnConfigCatItemClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;

		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			int index = *itm->GetData<int>();
			if (index < 0) index = 0;
			if (index >= Instance->ConfigPanelIndexMax) index = Instance->ConfigPanelIndexMax - 1;
			Instance->ConfigPanelIndex = index;
		}
		return true;
	}

	void ChangeConfigValue(zSTRING& valueName, float changeValue)
	{
		zCPar_Symbol* sym = parser->GetSymbol(valueName);
		auto pair = ExtraConfigsData.GetSafePair(valueName);

		if (!sym || !pair) return;
		ExtraConfigData data = pair->GetValue();

		float currentValue;
		if (sym->type == zPAR_TYPE_FLOAT) currentValue = sym->single_floatdata;		
		else if (sym->type == zPAR_TYPE_INT) currentValue = static_cast<float>(sym->single_intdata);
		else { return; }

		currentValue += changeValue;
		if (currentValue > static_cast<float>(data.MaxValue)) currentValue = static_cast<float>(data.MaxValue);
		if (currentValue < static_cast<float>(data.MinValue)) currentValue = static_cast<float>(data.MinValue);

		if (sym->type == zPAR_TYPE_FLOAT) sym->SetValue(currentValue, 0);		
		else sym->SetValue(static_cast<int>(currentValue), 0);
	}

	bool ConfigsWindow_OnConfigToggleButtonClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;

		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			ConfigPanelArgs* data = itm->GetData<ConfigPanelArgs>();
			if (!data) return true;

			zCPar_Symbol* sym = parser->GetSymbol(data->ValueSymbol);
			if (!sym) return true;

			bool currentValue = false;
			switch (sym->type)
			{
				case zPAR_TYPE_INT:
					currentValue = sym->single_intdata != 0;
					sym->SetValue(currentValue ? 0 : 1, 0);
					break;

				case zPAR_TYPE_FLOAT:
					currentValue = sym->single_floatdata != 0;
					sym->SetValue(currentValue ? 0.0f : 1.0f, 0);
					break;
			}
		}
		return true;
	}
	bool ConfigsWindow_OnConfigPlusButtonClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;
		
		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			ConfigPanelArgs* data = itm->GetData<ConfigPanelArgs>();
			if (!data) return true;

			float changeValue = args.IsAltPressed ? 1000.0f : 1.0f;
			if (args.IsShiftPressed) changeValue *= 10.0f;
			
			ChangeConfigValue(data->ValueSymbol, changeValue);
		}
		return true;
	}
	bool ConfigsWindow_OnConfigMinusButtonClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;

		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			ConfigPanelArgs* data = itm->GetData<ConfigPanelArgs>();
			if (!data) return true;

			float changeValue = args.IsAltPressed ? -1000.0f : -1.0f;
			if (args.IsShiftPressed) changeValue *= 10.0f;

			ChangeConfigValue(data->ValueSymbol, changeValue);
		}
		return true;
	}

	void ConfigsWindow_OnConfigToggleButtonUpdate(BaseUiElement* button)
	{
		if (!button || !Instance) return;

		MenuItem* itm = static_cast<MenuItem*>(button);
		if (itm)
		{
			ConfigPanelArgs* data = itm->GetData<ConfigPanelArgs>();
			if (!data) return;

			UpdateCheckButtonStyle(itm, data->ValueSymbol);
			if (!data->IsEditable)
			{
				itm->IsEnabled = data->IsEditable;
				return;
			}			
		}
	}
	void ConfigsWindow_OnConfigPlusButtonUpdate(BaseUiElement* button)
	{
		if (!button || !Instance) return;

		MenuItem* itm = static_cast<MenuItem*>(button);
		if (itm)
		{
			ConfigPanelArgs* argsData = itm->GetData<ConfigPanelArgs>();
			if (!argsData) return;

			if (!argsData->IsEditable)
			{
				itm->IsEnabled = argsData->IsEditable;
				return;
			}

			zCPar_Symbol* sym = parser->GetSymbol(argsData->ValueSymbol);
			auto pair = ExtraConfigsData.GetSafePair(argsData->ValueSymbol);

			if (!sym || !pair) return;
			ExtraConfigData data = pair->GetValue();

			float currentValue;
			switch (sym->type)
			{
				case zPAR_TYPE_INT:
					currentValue = static_cast<float>(sym->single_intdata);
					break;

				case zPAR_TYPE_FLOAT:
					currentValue = sym->single_floatdata;
					break;

				default:
					return;
			}

			itm->IsEnabled = currentValue < data.MaxValue;
		}
	}
	void ConfigsWindow_OnConfigMinusButtonUpdate(BaseUiElement* button)
	{
		if (!button || !Instance) return;

		MenuItem* itm = static_cast<MenuItem*>(button);
		if (itm)
		{
			ConfigPanelArgs* argsData = itm->GetData<ConfigPanelArgs>();
			if (!argsData) return;

			if (!argsData->IsEditable)
			{
				itm->IsEnabled = argsData->IsEditable;
				return;
			}

			zCPar_Symbol* sym = parser->GetSymbol(argsData->ValueSymbol);
			auto pair = ExtraConfigsData.GetSafePair(argsData->ValueSymbol);

			if (!sym || !pair) return;
			ExtraConfigData data = pair->GetValue();

			float currentValue;
			switch (sym->type)
			{
			case zPAR_TYPE_INT:
				currentValue = static_cast<float>(sym->single_intdata);
				break;

			case zPAR_TYPE_FLOAT:
				currentValue = sym->single_floatdata;
				break;

			default:
				return;
			}

			itm->IsEnabled = currentValue > data.MinValue;
		}
	}

	//----------------------------------------------------------------------
	//								TABS PANEL
	//----------------------------------------------------------------------

	void ConfigsWindow::InitTabs()
	{
		TabPanel = new BaseMenuPanel();
		TabPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "TabsPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.08f;
			item->PosX = 0.01f;
			item->PosY = 0.01f;
			item->BgTexture = UiElement_ButtonNoBorderTexture;
		};
		AddItem(TabPanel);

		const int tabCount = parser->GetSymbol("StExt_ConfigsMenu_TabName")->ele;
		const float margin = 0.025f;
		const float gap = 0.01f;
		const float tabWidth = 0.25f;
		const float totalTabsWidth = tabCount * tabWidth + (tabCount - 1) * gap;

		TabIndexMax = tabCount;
		float x = 0.5f - totalTabsWidth * 0.5f;
		for (int i = 0; i < tabCount; ++i)
		{
			MenuItem* tabItem = new MenuItem();
			tabItem->OnInit = [i, x, tabWidth, margin](BaseUiElement* item)
			{
				item->Name = "Tab_" + Z(i);
				item->SizeX = tabWidth;
				item->SizeY = 1.0f - (margin * 2.0f);
				item->PosX = x;
				item->PosY = margin;
				item->BgTexture = UiElement_ButtonTexture;

				if (auto* itm = static_cast<MenuItem*>(item))
				{
					itm->IsSelected = i == 0;
					itm->Text = parser->GetSymbol("StExt_ConfigsMenu_TabName")->stringdata[i];
					itm->Font = MenuFont_Header;
					itm->TextColor_Default = TextColor_HeaderTab_Default;
					itm->TextColor_Hovered = TextColor_HeaderTab_Hovered;
					itm->TextColor_Selected = TextColor_HeaderTab_Selected;
					itm->TextColor_Disabled = TextColor_HeaderTab_Disabled;
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable | UiElementBehaviorFlags::Checkable;
					itm->SetOwnedData<int>(i);
					itm->OnMouseEvent = ConfigsWindow_OnTabButtonClick;
				}
			};

			TabPanel->AddItem(tabItem);
			x += tabWidth + gap;
		}

		MenuItem* tabPanelSeparator = BuildHSeparator(0.09f);
		AddItem(tabPanelSeparator);

		TabPanel->Init();
		tabPanelSeparator->Init();
	}

	//----------------------------------------------------------------------
	//							CONFIGS PANEL
	//----------------------------------------------------------------------

	inline BaseMenuPanel* BuildConfigPanel(const ConfigPanelArgs& args)
	{
		BaseMenuPanel* panel = new BaseMenuPanel();
		panel->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "ConfigPanel_" + args.ValueSymbol;
			item->SizeX = 0.98f;
			item->SizeY = 0.045f;
			item->PosX = 0.01f;
			item->PosY = args.PosY; 

			if (auto* itm = static_cast<BaseMenuPanel*>(item)) {
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
			}
		};
		panel->SizeY = 0.05f;


		MenuItem* titleItem = new MenuItem();
		titleItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "ConfigPanel_Title_" + args.ValueSymbol;
			item->SizeX = 0.69f;
			item->SizeY = 0.98f;
			item->PosX = 0.01f;
			item->PosY = 0.01f;

			if (auto* itm = static_cast<MenuItem*>(item))
			{
				zCPar_Symbol* nameSym = parser->GetSymbol(args.TitleSymbol);
				itm->Text = nameSym ? nameSym->stringdata : 
					!args.TitleSymbol.IsEmpty() ? args.TitleSymbol : zString_Unknown;
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
				itm->HorizontalAlign = UiContentAlignEnum::Begin;
			}
		};		
		

		if (args.DisplayType == Value_Type_YesNo)
		{
			MenuItem* toggleButton = new MenuItem();
			toggleButton->OnInit = [args](BaseUiElement* item)
			{
				item->Name = "ConfigPanel_ToggleButton_" + args.ValueSymbol;
				item->SizeX = 0.10f;
				item->SizeY = 0.95f;
				item->PosX = 0.72f;
				item->PosY = 0.025f;
				item->BgTexture = UiElement_ButtonTexture;

				if (auto* itm = static_cast<MenuItem*>(item))
				{
					itm->Text = "";
					itm->Font = MenuFont_Sys_Default;
					itm->IsEnabled = args.IsEditable;
					itm->SetOwnedData<ConfigPanelArgs>(args);
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable;
					itm->OnUpdate = ConfigsWindow_OnConfigToggleButtonUpdate;
					itm->OnMouseEvent = ConfigsWindow_OnConfigToggleButtonClick;
				}
			};
			panel->AddItem(toggleButton);
		}
		else
		{
			MenuItem* minusButton = new MenuItem();
			minusButton->OnInit = [args](BaseUiElement* item)
			{
				item->Name = "ConfigPanel_MinusButton_" + args.ValueSymbol;
				item->SizeX = 0.05f;
				item->SizeY = 0.95f;
				item->PosX = 0.72f;
				item->PosY = 0.025f;
				item->BgTexture = UiElement_ButtonSquareTexture;

				if (auto* itm = static_cast<MenuItem*>(item))
				{
					itm->Text = "-";
					itm->Font = MenuFont_Symbol_Default;
					itm->TextColor_Hovered = TextColor_No;
					itm->SetOwnedData<ConfigPanelArgs>(args);
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable;
					itm->OnUpdate = ConfigsWindow_OnConfigMinusButtonUpdate;
					itm->OnMouseEvent = ConfigsWindow_OnConfigMinusButtonClick;
				}
			};

			MenuItem* plusButton = new MenuItem();
			plusButton->OnInit = [args](BaseUiElement* item)
			{
				item->Name = "ConfigPanel_PlusButton_" + args.ValueSymbol;
				item->SizeX = 0.05f;
				item->SizeY = 0.95f;
				item->PosX = 0.77f;
				item->PosY = 0.025f;
				item->BgTexture = UiElement_ButtonSquareTexture;

				if (auto* itm = static_cast<MenuItem*>(item))
				{
					itm->Text = "+";
					itm->Font = MenuFont_Symbol_Default;
					itm->TextColor_Hovered = TextColor_Yes;
					itm->SetOwnedData<ConfigPanelArgs>(args);
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable;
					itm->OnUpdate = ConfigsWindow_OnConfigPlusButtonUpdate;
					itm->OnMouseEvent = ConfigsWindow_OnConfigPlusButtonClick;
				}
			};

			panel->AddItem(minusButton);
			panel->AddItem(plusButton);
		}


		MenuValueItem* valueItem = new MenuValueItem();
		valueItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "ConfigPanel_Value_" + args.ValueSymbol;
			item->SizeX = 0.15f;
			item->SizeY = 0.98f;
			item->PosX = 0.84f;
			item->PosY = 0.01f;

			if (auto* itm = static_cast<MenuValueItem*>(item))
			{
				itm->PrimaryValueArrayIndex = 0;
				itm->PrimaryValueName = args.ValueSymbol;
				itm->PrimaryValueDisplayType = (UiValueDisplayType)args.DisplayType;
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
			}
		};


		panel->AddItem(titleItem);
		panel->AddItem(valueItem);
		return panel;
	}


	void ConfigsWindow::InitConfigsTab()
	{
		ConfigsTabPanel = new BaseMenuPanel();
		ConfigsTabPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "ConfigsTabPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};
		AddItem(ConfigsTabPanel);

		MenuListView* configsCatListView = new MenuListView();
		configsCatListView->OnInit = [](BaseUiElement* item)
		{
			item->Name = "ConfigsCatListView";
			item->SizeX = 0.18f;
			item->SizeY = 0.98f;
			item->PosX = 0.01f;
			item->PosY = 0.01f;
			item->BgTexture = UiElement_ListBorderTexture;
			if (auto* itm = static_cast<MenuListView*>(item))
			{
				itm->ItemHeight = 0.03f;
				itm->ItemMargin = 0.02f;
			}
		};
		ConfigsTabPanel->AddItem(configsCatListView);

		zCPar_Symbol* configCatatNameArraySym = parser->GetSymbol("StExt_Str_Config_Category_Name");
		ConfigPanelIndexMax = configCatatNameArraySym->ele;

		for (int catIndex = 0; catIndex < ConfigPanelIndexMax; ++catIndex)
		{
			MenuItem* configsCatListItem = new MenuItem();
			configsCatListItem->OnInit = [catIndex](BaseUiElement* item)
			{
				item->Name = "ConfigsCatListItem_" + Z(catIndex);
				item->SizeX = 0.98f;
				item->SizeY = 0.05f;
				item->PosX = 0.01f;
				item->PosY = 0.01f;

				if (auto* itm = static_cast<MenuItem*>(item))
				{
					itm->IsSelected = catIndex == 0;
					itm->Text = parser->GetSymbol("StExt_Str_Config_Category_Name")->stringdata[catIndex];
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable | UiElementBehaviorFlags::Checkable;
					itm->SetOwnedData<int>(catIndex);
					itm->OnMouseEvent = ConfigsWindow_OnConfigCatItemClick;
				}
			};
			configsCatListView->AddItem(configsCatListItem);

			MenuScrollPanel* configsCatPanel = new MenuScrollPanel();
			configsCatPanel->OnInit = [catIndex](BaseUiElement* item)
			{
				item->Name = "ConfigsCatPanel_" + Z(catIndex);
				item->SizeX = 0.79f;
				item->SizeY = 0.98f;
				item->PosX = 0.20f;
				item->PosY = 0.01f;
				item->IsHiden = true;
			};
			ConfigsPanels.InsertEnd(configsCatPanel);
			ConfigsTabPanel->AddItem(configsCatPanel);

			Array<ExtraConfigData*> configsForCategory;
			for (auto& configPair : ExtraConfigsData)
			{
				ExtraConfigData& configData = configPair.GetValue();
				if (configData.ConfigGroup != catIndex)
					continue;

				configsForCategory.InsertEnd(&configData);
			}

			std::sort(configsForCategory.begin(), configsForCategory.end(), 
				[](const ExtraConfigData* a, const ExtraConfigData* b) { return a->DisplayOrder < b->DisplayOrder; });

			float yOffset = 0.01f;
			for (ExtraConfigData* configData : configsForCategory)
			{
				ConfigPanelArgs configArgs = ConfigPanelArgs();
				configArgs.TitleSymbol = configData->DescSymbol;
				configArgs.ValueSymbol = configData->ValueSymbol;
				configArgs.DisplayType = configData->DisplayType;
				configArgs.IsEditable = configData->IsEditable;
				configArgs.PosY = yOffset;

				BaseMenuPanel* configPanel = BuildConfigPanel(configArgs);
				yOffset += configPanel->SizeY + 0.005f;
				configsCatPanel->AddItem(configPanel);
			}
		}
		ConfigsTabPanel->Init();
	}

	//----------------------------------------------------------------------
	//								MODINFO
	//----------------------------------------------------------------------

	void ConfigsWindow::InitModInfoTab()
	{
		ModInfoPanel = new MenuScrollPanel();
		ModInfoPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "ModInfoPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};
		AddItem(ModInfoPanel);


		ModInfoPanel->Init();
	}

	//----------------------------------------------------------------------
	//								WINDOW
	//----------------------------------------------------------------------


	ConfigsWindow::ConfigsWindow() 
	{
		ConfigsPanels = Array<MenuScrollPanel*>();
		TabPanel = Null;
		ConfigsTabPanel = Null;
		ModInfoPanel = Null;
	}

	inline BaseMenuPanel* ConfigsWindow::GetTabPanel(const int index)
	{
		switch (index)
		{
			case 0: return ConfigsTabPanel;
			case 1: return ModInfoPanel;
		}
		return Null;
	}

	void ConfigsWindow::Resize() { MenuWindow::Resize(); }

	void ConfigsWindow::Init()
	{
		OnInit = [](BaseUiElement* item) {
			item->Name = "ConfigsWindow";
			item->BgTexture = UiElement_PanelTexture;
			item->Resize();
		};
		MenuWindow::Init();

		InitTabs();
		InitConfigsTab();
		InitModInfoTab();

		Instance = this;
	}

	void ConfigsWindow::Draw() { MenuWindow::Draw(); }

	void ConfigsWindow::Update() 
	{ 
		MenuWindow::Update();

		for (int i = 0; i < TabIndexMax; ++i)
		{
			BaseMenuPanel* panel = GetTabPanel(i);
			if (panel == Null) continue;
			panel->IsHiden = TabIndex != i;
		}

		BaseMenuPanel* currentTabPanel = GetTabPanel(TabIndex);
		if (!currentTabPanel) return;

		if (currentTabPanel == ConfigsTabPanel)
		{
			for (int i = 0; i < ConfigPanelIndexMax; ++i)
			{
				BaseMenuPanel* panel = ConfigsPanels[i];
				if (panel == Null) continue;
				panel->IsHiden = ConfigPanelIndex != i;
			}
		}
	}


	ConfigsWindow::~ConfigsWindow()
	{
		ConfigsPanels.Clear();
		TabPanel = Null;
		ConfigsTabPanel = Null;
		ModInfoPanel = Null;
	}
}
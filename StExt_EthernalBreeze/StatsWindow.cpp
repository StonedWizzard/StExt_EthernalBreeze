#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	StatsWindow* Instance = Null;

	const int StatsSectionOrderMax = 7;
	const int StatsSectionOrder[] =
	{
		StatGroup_Characteristics,
		StatGroup_Attack,
		StatGroup_Protection,
		StatGroup_Skills,
		StatGroup_Auras,
		StatGroup_Summons,
		StatGroup_Other
	};

	struct SkillPanelArgs
	{
		int SkillId;
		int MasteryId;
		float PosY;
		bool IsCorruptionMastery;
		bool IsGenericMastery;
		zSTRING NamePostfix;
		zSTRING TitleSymbol;
		zSTRING DescSymbol;
		zSTRING ValueSymbol;
	};

	struct SkillHeaderPanelArgs
	{
		int MasteryId;
		float PosY;
		bool IsCorruptionMastery;
		zSTRING NamePostfix;
		zSTRING TitleSymbol;
		zSTRING DescSymbol;
		zSTRING LevelSymbol;
		zSTRING RankSymbol;
		zSTRING ExpSymbol;
		zSTRING NextExpSymbol;
		zSTRING LpSymbol;
	};

	//----------------------------------------------------------------------
	//								HANDLERS
	//----------------------------------------------------------------------

	bool StatsWindow_OnTabButtonClick(BaseMenuElement* item, const UiMouseEventArgs& args)
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

	bool StatsWindow_OnMasteryItemClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;

		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			int index = *itm->GetData<int>();
			if (index < 0) index = 0;
			if (index >= Instance->MasteryIndexMax) index = Instance->MasteryIndexMax - 1;
			Instance->MasteryIndex = index;
		}
		return true;
	}

	//----------------------------------------------------------------------
	//								TABS PANEL
	//----------------------------------------------------------------------

	void StatsWindow::InitTabs()
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

		zCPar_Symbol* tabNameArraySym = parser->GetSymbol("StExt_StatsMenu_TabName");
		float totalWidth = 1.0f;
		float margin = 0.025f;
		float gap = 0.01f;

		const int tabCount = tabNameArraySym->ele;
		const float availableWidth = totalWidth - (margin * 2) - (gap * (tabCount - 1));
		const float tabWidth = availableWidth / tabCount;

		TabIndexMax = tabCount;
		float x = margin;
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
					itm->IsEnabled = i <= 2; // Temp silution. Remove later on prod
					itm->Text = parser->GetSymbol("StExt_StatsMenu_TabName")->stringdata[i];
					itm->Font = MenuFont_Header;
					itm->TextColor_Default = TextColor_HeaderTab_Default;
					itm->TextColor_Hovered = TextColor_HeaderTab_Hovered;
					itm->TextColor_Selected = TextColor_HeaderTab_Selected;
					itm->TextColor_Disabled = TextColor_HeaderTab_Disabled;
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable | UiElementBehaviorFlags::Checkable;
					itm->SetOwnedData<int>(i);
					itm->OnMouseEvent = StatsWindow_OnTabButtonClick;
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
	//								GENERAL PANEL
	//----------------------------------------------------------------------

	void StatsWindow::InitGeneralTab()
	{
		GeneralTabPanel = new MenuScrollPanel();
		GeneralTabPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "GeneralTabPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};
		AddItem(GeneralTabPanel);


		GeneralTabPanel->Init();
	}

	void StatsWindow::InitProfesionTab()
	{
		ProfesionTabPanel = new MenuScrollPanel();
		ProfesionTabPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "GeneralTabPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};
		AddItem(ProfesionTabPanel);


		ProfesionTabPanel->Init();
	}

	void StatsWindow::InitBonusTab()
	{
		BonusTabPanel = new MenuScrollPanel();
		BonusTabPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "BonusTabPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};
		AddItem(BonusTabPanel);


		BonusTabPanel->Init();
	}

	//----------------------------------------------------------------------
	//								STATS PANEL
	//----------------------------------------------------------------------

	inline int GetStatGroupOrder(int statGroup) 
	{
		for (int i = 0; i < StatsSectionOrderMax; ++i) 
		{
			if (StatsSectionOrder[i] == statGroup)
				return static_cast<int>(i);
		}
		return INT_MAX;
	}

	inline void SortStats(std::vector<int>& ids)
	{
		ids.reserve(ExtraStatsData.GetNum());
		for (const auto& kv : ExtraStatsData)
			ids.push_back(kv.GetKey());

		std::sort(ids.begin(), ids.end(), [](int a, int b)
		{
			auto pA = ExtraStatsData.GetSafePair(a);
			auto pB = ExtraStatsData.GetSafePair(b);

			if (!pA || !pB) return false;

			const auto& ea = pA->GetValue();
			const auto& eb = pB->GetValue();

			int ga = GetStatGroupOrder(ea.StatGroup);
			int gb = GetStatGroupOrder(eb.StatGroup);
			if (ga != gb)
				return ga < gb;

			int sa = (ea.SortIndex == Invalid ? INT_MAX : ea.SortIndex);
			int sb = (eb.SortIndex == Invalid ? INT_MAX : eb.SortIndex);
			if (sa != sb)
				return sa < sb;

			return ea.Id < eb.Id;
		});
	}

	inline BaseMenuPanel* BuildStatPanel(const int statId, const float posY, const float sizeY)
	{
		const ExtraStatData* statData = GetExtraStatDataById(statId);
		if (!statData)
		{
			DEBUG_MSG("StExt Mod Ui - Fail to build stats panel. StatId = " + Z(statId));
			return Null;
		}
		zSTRING statTitle = GetExtraStatNameById(statId);

		BaseMenuPanel* panel = new BaseMenuPanel();
		panel->OnInit = [statId, posY, sizeY](BaseUiElement* item)
		{
			item->Name = "StatPanel_" + Z(statId);
			item->SizeX = 0.98f;
			item->SizeY = sizeY;
			item->PosX = 0.01f;
			item->PosY = posY;
			if (auto* itm = static_cast<BaseMenuPanel*>(item)) {
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
			}
		};

		MenuItem* titleItem = new MenuItem();		
		titleItem->OnInit = [statId, statTitle](BaseUiElement* item)
		{
			item->Name = "StatPanel_Title_" + Z(statId);
			item->SizeX = 0.80f;
			item->SizeY = 0.95f;
			item->PosX = 0.025f;
			item->PosY = 0.025f;

			if (auto* itm = static_cast<MenuItem*>(item))
			{
				itm->Text = statTitle;
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
				itm->HorizontalAlign = UiContentAlignEnum::Begin;
				itm->SetOwnedData<int>(statId);
			}
		};

		MenuValueItem* valueItem = new MenuValueItem();
		UiValueDisplayType displayType = (UiValueDisplayType)statData->ValueType;
		valueItem->OnInit = [statId, displayType](BaseUiElement* item)
		{
			item->Name = "StatPanel_Value_" + Z(statId);
			item->SizeX = 0.15f;
			item->SizeY = 0.95f;
			item->PosX = 0.825f;
			item->PosY = 0.25f;

			if (auto* itm = static_cast<MenuValueItem*>(item))
			{
				itm->IsArray = true;
				itm->PrimaryValueArrayIndex = statId;
				itm->PrimaryValueName = "StExt_PcStats";
				itm->PrimaryValueDisplayType = displayType;
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
				itm->SetOwnedData<int>(statId);
			}
		};

		panel->AddItem(titleItem);
		panel->AddItem(valueItem);
		return panel;
	}

	inline MenuItem* BuildStatHeader(const int groupId, const float posY, const float sizeY)
	{
		MenuItem* headerItem = new MenuItem();
		headerItem->OnInit = [groupId, posY, sizeY](BaseUiElement* item)
		{
			item->Name = "StatPanel_Header_" + Z(groupId);
			item->SizeX = 0.95f;
			item->SizeY = sizeY;
			item->PosX = 0.025f;
			item->PosY = posY;
			item->BgTexture = UiElement_ButtonNoBorderTexture;

			if (auto* itm = static_cast<MenuItem*>(item))
			{
				itm->Text = parser->GetSymbol("StExt_PcStats_SectionDesc")->stringdata[groupId];
				itm->TextColor_Default = TextColor_Header_Default;
			}
		};
		return headerItem;
	}

	void StatsWindow::InitStatsTab()
	{
		StatsTabPanel = new MenuScrollPanel();
		StatsTabPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "StatsTabPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};
		AddItem(StatsTabPanel);

		std::vector<int> ids;
		SortStats(ids);

		const float statPanelSizeY = 0.035f;
		const float statHeaderSizeY = 0.08f;
		const float itemMargin = 0.005f;
		const float groupMargin = 0.05f;

		float yOffset = 0.03f;
		int currentGroup = Invalid;
		for (int statId : ids)
		{
			ExtraStatData statData;
			auto pair = ExtraStatsData.GetSafePair(statId);

			if (!pair)
			{
				DEBUG_MSG("StExt Mod Ui StatsWindow::InitStatsTab() - Fail to find stat StatId = " + Z(statId));
				continue;
			}
			statData = pair->GetValue();

			if (statData.StatGroup != currentGroup)
			{
				yOffset += groupMargin;
				currentGroup = statData.StatGroup;
				MenuItem* headerItem = BuildStatHeader(currentGroup, yOffset, statHeaderSizeY);				
				StatsTabPanel->AddItem(headerItem);
				yOffset += statHeaderSizeY + (itemMargin * 2.5f);
			}

			BaseMenuPanel* statPanel = BuildStatPanel(statId, yOffset, statPanelSizeY);
			if (statPanel == Null)
			{
				DEBUG_MSG("StExt Mod Ui StatsWindow::InitStatsTab() - Fail to build stat panel! StatId = " + Z(statId));
				continue;
			}
			StatsTabPanel->AddItem(statPanel);
			yOffset += statPanelSizeY + itemMargin;
		}
		StatsTabPanel->Init();
	}

	//----------------------------------------------------------------------
	//								SKILLS PANEL
	//----------------------------------------------------------------------

	inline void BuildSkillPostfixName(zSTRING& postfix, const int masteryId, const int skillId, const bool isCorruption, const bool isGeneric) { postfix = Z((int)isCorruption) + Z((int)isGeneric) + "_" + Z(masteryId) + "_" + Z(skillId); }

	inline BaseMenuPanel* BuildSkillPanel(const SkillPanelArgs& args)
	{
		BaseMenuPanel* panel = new BaseMenuPanel();
		panel->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillPanel_" + args.NamePostfix;
			item->SizeX = 0.98f;
			item->SizeY = 0.10f;
			item->PosX = 0.01f;
			item->PosY = args.PosY;
			if (auto* itm = static_cast<BaseMenuPanel*>(item)) {
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
			}
		};
		panel->SizeY = 0.10f;

		MenuItem* titleItem = new MenuItem();
		titleItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillPanel_Title_" + args.NamePostfix;
			item->SizeX = 0.98f;
			item->SizeY = 0.15f;
			item->PosX = 0.01f;
			item->PosY = 0.05f;

			if (auto* itm = static_cast<MenuItem*>(item))
			{
				itm->Text = parser->GetSymbol(args.TitleSymbol)->stringdata[args.SkillId];
				itm->TextColor_Default = TextColor_Header_Default;
				itm->TextColor_Hovered = TextColor_Regular_Selected;
				itm->TextColor_Selected = TextColor_Regular_Selected;
				itm->TextColor_Disabled = TextColor_Header_Disabled;
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
			}
		};

		MenuTextItem* descriptionItem = new MenuTextItem();
		descriptionItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillPanel_Description_" + args.NamePostfix;
			item->SizeX = 0.80f;
			item->SizeY = 0.76f;
			item->PosX = 0.01f;
			item->PosY = 0.23f;

			if (auto* itm = static_cast<MenuTextItem*>(item))
			{
				itm->HorizontalAlign = UiContentAlignEnum::Begin;
				itm->Text = parser->GetSymbol(args.DescSymbol)->stringdata[args.SkillId];
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
			}
		};

		MenuValueItem* valueItem = new MenuValueItem();
		valueItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillPanel_Value_" + args.NamePostfix;
			item->SizeX = 0.17f;
			item->SizeY = 0.76f;
			item->PosX = 0.82f;
			item->PosY = 0.23f;

			if (auto* itm = static_cast<MenuValueItem*>(item))
			{
				itm->IsArray = true;
				itm->PrimaryValueArrayIndex = args.SkillId;
				itm->PrimaryValueName = args.ValueSymbol;
				itm->PrimaryValueDisplayType = UiValueDisplayType::Bool;
				itm->PrimarySpecialValueFormatter = ConvertValueToSkill;
				itm->SpecialColorFormatter = BoolValueColorFormatter;
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
			}
		};

		panel->AddItem(titleItem);
		panel->AddItem(descriptionItem);
		panel->AddItem(valueItem);
		return panel;
	}

	inline BaseMenuPanel* BuildSkillHeaderPanel(const SkillHeaderPanelArgs& args)
	{
		BaseMenuPanel* panel = new BaseMenuPanel();
		panel->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_" + args.NamePostfix;
			item->SizeX = 0.98f;
			item->SizeY = 0.25f;
			item->PosX = 0.01f;
			item->PosY = args.PosY;
			item->BgTexture = UiElement_ButtonTexture;
		};
		panel->SizeY = 0.25f;

		MenuItem* titleItem = new MenuItem();
		titleItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_Title_" + args.NamePostfix;
			item->SizeX = 0.98f;
			item->SizeY = 0.14f;
			item->PosX = 0.01f;
			item->PosY = 0.05f;

			if (auto* itm = static_cast<MenuItem*>(item))
			{
				itm->Font = MenuFont_Header;
				itm->Text = parser->GetSymbol(args.TitleSymbol)->stringdata[args.MasteryId];
				itm->TextColor_Default = TextColor_Header_Default;
				itm->TextColor_Disabled = TextColor_Header_Disabled;
			}
		};

		MenuItem* levelTitleItem = new MenuItem();
		MenuItem* expTitleItem = new MenuItem();
		MenuItem* lpTitleItem = new MenuItem();

		levelTitleItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_LevelTitle_" + args.NamePostfix;
			item->SizeX = 0.35f;
			item->SizeY = 0.10f;
			item->PosX = 0.10f;
			item->PosY = 0.21f;

			if (auto* itm = static_cast<MenuItem*>(item)) {
				itm->Text = parser->GetSymbol("StExt_Str_Talents_Progress")->stringdata;
				itm->HorizontalAlign = UiContentAlignEnum::End;
			}
		};
		expTitleItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_ExpTitle_" + args.NamePostfix;
			item->SizeX = 0.35f;
			item->SizeY = 0.10f;
			item->PosX = 0.10f;
			item->PosY = 0.32f;

			if (auto* itm = static_cast<MenuItem*>(item)) {
				itm->Text = parser->GetSymbol("StExt_Str_Talents_Exp")->stringdata;
				itm->HorizontalAlign = UiContentAlignEnum::End;
			}
		};
		lpTitleItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_LpTitle_" + args.NamePostfix;
			item->SizeX = 0.35f;
			item->SizeY = 0.10f;
			item->PosX = 0.10f;
			item->PosY = 0.43f;

			if (auto* itm = static_cast<MenuItem*>(item)) {
				itm->Text = parser->GetSymbol("StExt_Str_Talents_Lp")->stringdata;
				itm->HorizontalAlign = UiContentAlignEnum::End;
			}
		};

		MenuValueItem* levelValueItem = new MenuValueItem();
		MenuValueItem* expValueItem = new MenuValueItem();
		MenuValueItem* lpValueItem = new MenuValueItem();

		levelValueItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_LevelValue_" + args.NamePostfix;
			item->SizeX = 0.35f;
			item->SizeY = 0.10f;
			item->PosX = 0.55f;
			item->PosY = 0.21f;

			if (auto* itm = static_cast<MenuValueItem*>(item))
			{
				itm->IsArray = !args.IsCorruptionMastery;
				itm->HorizontalAlign = UiContentAlignEnum::Begin;				

				itm->PrimaryValueName = args.LevelSymbol;
				itm->PrimaryValueArrayIndex = args.MasteryId;
				itm->PrimaryValueDisplayType = UiValueDisplayType::Default;

				if (!args.IsCorruptionMastery)
				{
					itm->IsRanged = true;
					itm->RangeValueSeparator = " | ";
					itm->SecondaryValueName = args.RankSymbol;
					itm->SecondaryValueArrayIndex = args.MasteryId;
					itm->SecondaryValueDisplayType = UiValueDisplayType::Bool;
					itm->SecondarySpecialValueFormatter = ConvertValueToMasteryRank;
				}
			}
		};

		expValueItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_ExpValue_" + args.NamePostfix;
			item->SizeX = 0.35f;
			item->SizeY = 0.10f;
			item->PosX = 0.55f;
			item->PosY = 0.32f;

			if (auto* itm = static_cast<MenuValueItem*>(item))
			{
				itm->IsArray = !args.IsCorruptionMastery;
				itm->IsRanged = true;
				itm->HorizontalAlign = UiContentAlignEnum::Begin;

				itm->PrimaryValueName = args.ExpSymbol;
				itm->PrimaryValueArrayIndex = args.MasteryId;
				itm->PrimaryValueDisplayType = UiValueDisplayType::Default;

				itm->SecondaryValueName = args.NextExpSymbol;
				itm->SecondaryValueArrayIndex = args.MasteryId;
				itm->SecondaryValueDisplayType = UiValueDisplayType::Default;
			}
		};

		lpValueItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_LpValue_" + args.NamePostfix;
			item->SizeX = 0.35f;
			item->SizeY = 0.10f;
			item->PosX = 0.55f;
			item->PosY = 0.43f;

			if (auto* itm = static_cast<MenuValueItem*>(item))
			{
				itm->IsArray = !args.IsCorruptionMastery;
				itm->HorizontalAlign = UiContentAlignEnum::Begin;

				itm->PrimaryValueName = args.LpSymbol;
				itm->PrimaryValueArrayIndex = args.MasteryId;
				itm->PrimaryValueDisplayType = UiValueDisplayType::Default;
			}
		};

		MenuTextItem* descriptionItem = new MenuTextItem();
		descriptionItem->OnInit = [args](BaseUiElement* item)
		{
			item->Name = "SkillHeaderPanel_Description_" + args.NamePostfix;
			item->SizeX = 0.98f;
			item->SizeY = 0.54f;
			item->PosX = 0.01f;
			item->PosY = 0.45f;

			if (auto* itm = static_cast<MenuTextItem*>(item)) {
				itm->Text = parser->GetSymbol(args.DescSymbol)->stringdata[args.MasteryId];
			}
		};

		panel->AddItem(titleItem);
		panel->AddItem(levelTitleItem);
		panel->AddItem(expTitleItem);
		panel->AddItem(lpTitleItem);
		panel->AddItem(levelValueItem);
		panel->AddItem(expValueItem);
		panel->AddItem(lpValueItem);
		panel->AddItem(descriptionItem);
		
		return panel;
	}

	void StatsWindow::InitSkillsTab()
	{
		SkillsTabPanel = new BaseMenuPanel();
		SkillsTabPanel->OnInit = [](BaseUiElement* item)
		{
			item->Name = "SkillsTabPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};
		AddItem(SkillsTabPanel);

		MenuListView* masteriesListView = new MenuListView();
		masteriesListView->OnInit = [](BaseUiElement* item)
		{
			item->Name = "MasteriesListView";
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
		SkillsTabPanel->AddItem(masteriesListView);

		int index = 0;
		MasteryIndexMax = ExtraMasteriesData.GetNum();
		for (auto masteryData : ExtraMasteriesData)
		{
			MenuItem* masteriesListItem = new MenuItem();
			masteriesListItem->OnInit = [index, masteryData](BaseUiElement* item)
			{
				item->Name = "MasteriesListViewItem_" + Z(index);
				item->SizeX = 0.98f;
				item->SizeY = 0.05f;
				item->PosX = 0.01f;
				item->PosY = 0.01f;

				if (auto* itm = static_cast<MenuItem*>(item))
				{
					itm->IsSelected = index == 0;
					itm->Text = parser->GetSymbol(masteryData.TitleSymbol)->stringdata[masteryData.MasteryId];
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable | UiElementBehaviorFlags::Checkable;
					itm->SetOwnedData<int>(index);
					itm->OnMouseEvent = StatsWindow_OnMasteryItemClick;
				}
			};
			masteriesListView->AddItem(masteriesListItem);

			MenuScrollPanel* masteriesPanel = new MenuScrollPanel();
			masteriesPanel->OnInit = [index](BaseUiElement* item)
			{
				item->Name = "MasteriesPanel_" + Z(index);
				item->SizeX = 0.79f;
				item->SizeY = 0.98f;
				item->PosX = 0.20f;
				item->PosY = 0.01f;
				item->IsHiden = true;
			};
			SkillPanels.InsertEnd(masteriesPanel);
			SkillsTabPanel->AddItem(masteriesPanel);

			float yOffset = 0.01f;
			zSTRING namePostfix = zString_Empty;

			if (!masteryData.IsGeneric)
			{
				SkillHeaderPanelArgs skillsHeaderPanelArgs = SkillHeaderPanelArgs();
				BuildSkillPostfixName(namePostfix, masteryData.MasteryId, 0, masteryData.IsCorruption, masteryData.IsGeneric);
				
				skillsHeaderPanelArgs.MasteryId = masteryData.MasteryId;
				skillsHeaderPanelArgs.PosY = yOffset;
				skillsHeaderPanelArgs.IsCorruptionMastery = masteryData.IsCorruption;
				skillsHeaderPanelArgs.NamePostfix = namePostfix;

				skillsHeaderPanelArgs.TitleSymbol = masteryData.TitleSymbol;
				skillsHeaderPanelArgs.DescSymbol = masteryData.DescSymbol;
				skillsHeaderPanelArgs.LevelSymbol = masteryData.LevelSymbol;
				skillsHeaderPanelArgs.RankSymbol = masteryData.RankSymbol;
				skillsHeaderPanelArgs.ExpSymbol = masteryData.ExpSymbol;
				skillsHeaderPanelArgs.NextExpSymbol = masteryData.NextExpSymbol;
				skillsHeaderPanelArgs.LpSymbol = masteryData.LpSymbol;

				BaseMenuPanel* skillsHeaderPanel = BuildSkillHeaderPanel(skillsHeaderPanelArgs);
				masteriesPanel->AddItem(skillsHeaderPanel);
				yOffset += skillsHeaderPanel->SizeY + 0.05f;
			}

			for (int perkId = 0; perkId < masteryData.PerksCount; ++perkId)
			{
				SkillPanelArgs skillPanelArgs = SkillPanelArgs();
				BuildSkillPostfixName(namePostfix, masteryData.MasteryId, perkId, masteryData.IsCorruption, masteryData.IsGeneric);

				skillPanelArgs.SkillId = perkId;
				skillPanelArgs.MasteryId = masteryData.MasteryId;
				skillPanelArgs.PosY = yOffset;
				skillPanelArgs.IsCorruptionMastery = masteryData.IsCorruption;
				skillPanelArgs.IsGenericMastery = masteryData.IsGeneric;
				skillPanelArgs.NamePostfix = namePostfix;
				skillPanelArgs.TitleSymbol = masteryData.PerkNameSymbol;
				skillPanelArgs.DescSymbol = masteryData.PerkDescSymbol;
				skillPanelArgs.ValueSymbol = masteryData.PerkValueSymbol;

				BaseMenuPanel* skillPanel = BuildSkillPanel(skillPanelArgs);
				yOffset += skillPanel->SizeY + 0.01f;
				MenuItem* skillSeparator = BuildHSeparator(yOffset);
				yOffset += skillSeparator->SizeY + 0.03f;
				
				masteriesPanel->AddItem(skillPanel);
				masteriesPanel->AddItem(skillSeparator);
			}

			++index;
		}

		SkillsTabPanel->Init();
	}

	//----------------------------------------------------------------------
	//								WINDOW
	//----------------------------------------------------------------------

	inline BaseMenuPanel* StatsWindow::GetTabPanel(const int index)
	{
		switch (index)
		{
			case 0: return GeneralTabPanel;
			case 1: return StatsTabPanel;
			case 2: return SkillsTabPanel;
			case 3: return ProfesionTabPanel;
			case 4: return BonusTabPanel;
		}
		return Null;
	}

	StatsWindow::StatsWindow() 
	{
		SkillPanels = Array<MenuScrollPanel*>();
		TabPanel = Null;
		SkillsTabPanel = Null;
		GeneralTabPanel = Null;
		StatsTabPanel = Null;
		ProfesionTabPanel = Null;
		BonusTabPanel = Null;
	}

	void StatsWindow::Resize() { MenuWindow::Resize(); }

	void StatsWindow::Init()
	{
		OnInit = [](BaseUiElement* item) {
			item->Name = "StatsWindow";
			item->BgTexture = UiElement_PanelTexture;
			item->Resize();
		};
		MenuWindow::Init();

		InitTabs();
		InitGeneralTab();
		InitStatsTab();
		InitSkillsTab();
		InitProfesionTab();
		InitBonusTab();

		Instance = this;
	}

	void StatsWindow::Draw() { MenuWindow::Draw(); }

	void StatsWindow::Update() 
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

		if (currentTabPanel == SkillsTabPanel)
		{
			for (int i = 0; i < MasteryIndexMax; ++i)
			{
				BaseMenuPanel* panel = SkillPanels[i];
				if (panel == Null) continue;
				panel->IsHiden = MasteryIndex != i;
			}
		}
	}

	StatsWindow::~StatsWindow() 
	{
		SkillPanels.Clear();
		TabPanel = Null;
		SkillsTabPanel = Null;
		GeneralTabPanel = Null;
		StatsTabPanel = Null;
		ProfesionTabPanel = Null;
		BonusTabPanel = Null;
	}
}
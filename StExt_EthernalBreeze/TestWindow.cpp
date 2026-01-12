#include <StonedExtension.h>
#include <random>

namespace Gothic_II_Addon
{
	TestWindow* Instance = Null;

	bool OnTabButtonClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;

		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			Instance->TabIndex = *itm->GetData<int>();
			Instance->UpdateText();
		}
		return true;
	}

	bool OnListItemClick(BaseMenuElement* item, const UiMouseEventArgs& args)
	{
		if (!item || !Instance) return false;
		if (args.Action != UiMouseEnum::LeftClick) return false;

		MenuItem* itm = static_cast<MenuItem*>(item);
		if (itm)
		{
			Instance->ItemIndex = *itm->GetData<int>();
			Instance->UpdateText();
		}
		return true;
	}



	TestWindow::TestWindow() { }

	void TestWindow::Resize() { MenuWindow::Resize(); }

	void TestWindow::Init()
	{
		OnInit = [](BaseUiElement* item) {
			item->Name = "TestWindow";
			item->BgTexture = UiElement_PanelTexture;
			item->Resize();
		};
		MenuWindow::Init();

		// Init childs
		ItemsListView = new MenuListView();
		TabPanel = new BaseMenuPanel();
		Description = new MenuItem();
		TestButton = new MenuItem();
		BaseMenuElement* tabPanelSeparator = new MenuItem();
		MenuScrollPanel* scrollPanel = new MenuScrollPanel();
		TabIndex = 0;

		AddItem(ItemsListView);
		AddItem(TabPanel);
		AddItem(tabPanelSeparator);
		AddItem(scrollPanel);
		AddItem(Description);
		AddItem(TestButton);

		TabPanel->OnInit = [](BaseUiElement* item) {
			item->Name = "TabPanel";
			item->SizeX = 0.98f;
			item->SizeY = 0.08f;
			item->PosX = 0.01f;
			item->PosY = 0.01f;
			item->BgTexture = UiElement_ButtonNoBorderTexture;
		};
		tabPanelSeparator->OnInit = [](BaseUiElement* item) {
			item->Name = "TabPanelSeparator";
			item->SizeX = 0.98f;
			item->SizeY = 0.004f;
			item->PosX = 0.01f;
			item->PosY = 0.095f;
			item->BgTexture = UiElement_HSeparatorTexture;
		};


		ItemsListView->OnInit = [](BaseUiElement* item) {
			item->Name = "ItemsListView";
			item->SizeX = 0.20f;
			item->SizeY = 0.88f;
			item->PosX = 0.01f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelBorderTexture;
		};
		scrollPanel->OnInit = [](BaseUiElement* item) {
			item->Name = "ScrollTestPanel";
			item->SizeX = 0.78f;
			item->SizeY = 0.79f;
			item->PosX = 0.21f;
			item->PosY = 0.11f;
			item->BgTexture = UiElement_PanelNoBorderTexture;
		};

		Description->OnInit = [](BaseUiElement* item) {
			item->Name = "Description";
			item->SizeX = 0.57f;
			item->SizeY = 0.08f;
			item->PosX = 0.21f;
			item->PosY = 0.91f;
		};

		TestButton->OnInit = [](BaseUiElement* item) {
			item->Name = "TestButton";
			item->SizeX = 0.20f;
			item->SizeY = 0.08f;
			item->PosX = 0.79f;
			item->PosY = 0.91f;
			item->BgTexture = UiElement_ButtonTexture;

			MenuItem* itm = static_cast<MenuItem*>(item);
			if (itm)
			{
				itm->Text = parser->GetSymbol("StExt_Str_Menu_CraftButtonText")->stringdata;
				itm->Font = MenuFont_Header;
				itm->TextColor_Default = TextColor_Header_Default;
				itm->TextColor_Hovered = TextColor_Regular_Selected;
				itm->TextColor_Selected = TextColor_Regular_Selected;
				itm->TextColor_Disabled = TextColor_Header_Disabled;				
				itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable;
			}
		};


		float totalWidth = 1.0f;
		float marginLeft = 0.01f;
		float marginRight = 0.01f;
		float gap = 0.01f;
		int tabCount = 5;

		float availableWidth = totalWidth - marginLeft - marginRight - (gap * (tabCount - 1));
		float tabWidth = availableWidth / tabCount;

		float x = marginLeft;
		MenuItem* firstTabItem = Null;
		for (int i = 0; i < tabCount; ++i)
		{
			MenuItem* tabItem = new MenuItem();
			tabItem->OnInit = [i, x, tabWidth](BaseUiElement* item)
			{
				item->Name = "Tab_" + Z(i);
				item->SizeX = tabWidth;
				item->SizeY = 0.98f;
				item->PosX = x;
				item->PosY = 0.01f;
				item->BgTexture = UiElement_ButtonTexture;

				if (auto* itm = static_cast<MenuItem*>(item))
				{
					itm->Text = "<< Tab " + Z(i) + " >>";
					itm->Font = MenuFont_Header;
					itm->TextColor_Default = TextColor_HeaderTab_Default;
					itm->TextColor_Hovered = TextColor_HeaderTab_Hovered;
					itm->TextColor_Selected = TextColor_HeaderTab_Selected;
					itm->TextColor_Disabled = TextColor_HeaderTab_Disabled;
					itm->BehaviorFlags = UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Interactable;
					itm->SetOwnedData<int>(i);
					itm->OnMouseEvent = OnTabButtonClick;
				}
			};

			if (i == 0)
			{
				firstTabItem = tabItem;
			}
			TabPanel->AddItem(tabItem);
			x += tabWidth + gap;
		}

		// fill scroll panel
		std::mt19937 rng(static_cast<unsigned>(1257841457));
		std::uniform_real_distribution<float> xDist(0.0f, 0.8f);
		std::uniform_real_distribution<float> wDist(0.10f, 0.20f);
		std::uniform_real_distribution<float> hDist(0.05f, 0.20f);

		float currentY = 0.0f;
		for (int i = 0; i < 32; ++i)
		{
			MenuItem* listItem = new MenuItem();
			// генерируем случайные размеры
			float width = wDist(rng);
			float height = hDist(rng);
			float posX = xDist(rng);

			if (i > 0)
				currentY += height + 0.01f;

			//DEBUG_MSG("Init ScrollPanel Item[" + Z((int)i) + "] Rect: [x=" + Z(listItem->PosX) + "; y=" + Z(listItem->PosY) + "; w=" + Z(listItem->SizeX) + "; h=" + Z(listItem->SizeY) + "]");
			listItem->OnInit = [i, posX, currentY, width, height](BaseUiElement* item)
			{
				item->Name = "ContainerItem_" + Z(i);
				item->PosX = posX;
				item->PosY = currentY;
				item->SizeX = width;
				item->SizeY = height;

				MenuItem* itm = static_cast<MenuItem*>(item);
				if (itm)
				{
					itm->Text = "ScrollContainer Item " + Z(i);
					itm->SetOwnedData<int>(i);
					itm->BehaviorFlags = UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
				}
			};
			scrollPanel->AddItem(listItem);
		}

		TabPanel->Init();
		Description->Init();
		TestButton->Init();
		tabPanelSeparator->Init();
		scrollPanel->Init();

		for (int i = 0; i < 64; ++i)
		{
			MenuItem* listItem = new MenuItem();
			listItem->OnInit = [i](BaseUiElement* item)
			{
				item->Name = "Item_" + Z(i);

				MenuItem* itm = static_cast<MenuItem*>(item);
				if (itm)
				{
					itm->Text = "Item " + Z(i);
					itm->SetOwnedData<int>(i);
					itm->BehaviorFlags = UiElementBehaviorFlags::Selectable | UiElementBehaviorFlags::Hoverable | UiElementBehaviorFlags::Interactable;
					itm->OnMouseEvent = OnListItemClick;
				}
			};
			ItemsListView->AddItem(listItem);
		}
		ItemsListView->Init();
		ItemsListView->PosX = firstTabItem->PosX;

		Instance = this;
		TabPanel->SelectItem(firstTabItem);		
	}

	void TestWindow::Draw() { MenuWindow::Draw(); }

	void TestWindow::Update() { MenuWindow::Update(); }

	void TestWindow::UpdateText()
	{
		if (!Description) return;
		MenuItem* itm = static_cast<MenuItem*>(Description);
		if (itm)
		{
			zSTRING str = "Tab - " + Z(TabIndex) + " SelectedItem: " + Z(ItemIndex);
			itm->Text = str;
		}
	}

	TestWindow::~TestWindow() {}
}
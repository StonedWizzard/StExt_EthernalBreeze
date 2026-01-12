#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	CraftWindow::CraftWindow() { }

	void CraftWindow::Resize() { MenuWindow::Resize(); }

	void CraftWindow::Init()
	{
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 0");
		// Init this first
		OnInit = [](BaseUiElement* item) {
			item->Name = "CraftWindow";			
			item->Resize();
		};
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 1");
		MenuWindow::Init();
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 2");

		// Init childs
		ReceiptsListView = new MenuListView();
		ReceiptsFiltersPanel = new BaseMenuPanel();
		ReceiptDescription = new MenuItem();
		CraftButton = new MenuItem();

		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 3");
		AddItem(ReceiptsListView);
		AddItem(ReceiptsFiltersPanel);
		AddItem(ReceiptDescription);
		AddItem(CraftButton);
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 4");

		ReceiptsListView->OnInit = [](BaseUiElement* item) {
			item->Name = "ReceiptsListView";
			item->SizeX = 0.34f;
			item->SizeY = 0.98f;
			item->PosX = 0.01f;
			item->PosY = 0.01f;
		};
		ReceiptsFiltersPanel->OnInit = [](BaseUiElement* item) {
			item->Name = "ReceiptsFiltersPanel";
			item->SizeX = 0.63f;
			item->SizeY = 0.10f;
			item->PosX = 0.36f;
			item->PosY = 0.01f; 
		};
		ReceiptDescription->OnInit = [](BaseUiElement* item) {
			item->Name = "ReceiptDescription";
			item->SizeX = 0.63f;
			item->SizeY = 0.27f;
			item->PosX = 0.36f;
			item->PosY = 0.12f;
		};
		CraftButton->OnInit = [](BaseUiElement* item) {
			item->Name = "CraftButton";
			item->SizeX = 0.30f;
			item->SizeY = 0.10f;
			item->PosX = 0.69f;
			item->PosY = 0.89f;

			MenuItem* itm = static_cast<MenuItem*>(item);
			if (itm)
			{
				itm->Text = parser->GetSymbol("StExt_Str_Menu_CraftButtonText")->stringdata;
				itm->TextColor_Default = TextColor_Header_Default;
				itm->TextColor_Hovered = TextColor_Header_Hovered;
				itm->TextColor_Selected = TextColor_Header_Selected;
				itm->TextColor_Disabled = TextColor_Header_Disabled;
			}
		};

		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 5");
		// build slots
		float slotY = 0.40f;
		for (int i = 0; i < ReceiptInfo_Slots_Max; ++i)
		{
			ReceiptItemsSlots[i] = new MenuItem();
			AddItem(ReceiptItemsSlots[i]);
			ReceiptItemsSlots[i]->OnInit = [i, slotY](BaseUiElement* item) {
				item->Name = "ReceiptSlot_" + Z(i);
				item->SizeX = 0.63f;
				item->SizeY = 0.05f;
				item->PosX = 0.69f;
				item->PosY = slotY;
			};
			slotY += 0.06f;
			ReceiptItemsSlots[i]->Init();
		}
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 6");

		// build type tabs
		int tabTypesMax = parser->GetSymbol("StExt_ReceiptType_Max")->single_intdata;
		float tabXOffset = 0.01f;
		float tabGaps = tabTypesMax *0.01f;
		float tabWidth = (0.98f - tabGaps) / (tabTypesMax + 1);

		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 7");
		MenuItem* allReceiptsTab = new MenuItem();
		allReceiptsTab->OnInit = [tabXOffset, tabWidth](BaseUiElement* item)
		{
			item->Name = "CraftType_Tab_All";
			item->SizeX = tabWidth;
			item->SizeY = 0.98f;
			item->PosX = tabXOffset;
			item->PosY = 0.01f;

			MenuItem* itm = static_cast<MenuItem*>(item);
			if (itm)
			{
				itm->Text = parser->GetSymbol("StExt_Str_Menu_CraftType_TabText_All")->stringdata;
				itm->TextColor_Default = TextColor_Header_Default;
				itm->TextColor_Hovered = TextColor_Header_Hovered;
				itm->TextColor_Selected = TextColor_Header_Selected;
				itm->TextColor_Disabled = TextColor_Header_Disabled;
				itm->SetOwnedData<int>(Invalid);
			}
		};
		ReceiptsFiltersPanel->AddItem(allReceiptsTab);
		tabXOffset += tabWidth + 0.01f;
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 8");

		for (int i = 0; i < tabTypesMax; ++i)
		{
			MenuItem* tabItem = new MenuItem();
			tabItem->OnInit = [i, tabXOffset, tabWidth](BaseUiElement* item)
			{
				item->Name = "CraftType_Tab_" + Z(i);
				item->SizeX = tabWidth;
				item->SizeY = 0.98f;
				item->PosX = tabXOffset;
				item->PosY = 0.01f;

				MenuItem* itm = static_cast<MenuItem*>(item);
				if (itm)
				{
					itm->Text = parser->GetSymbol("StExt_Str_Menu_CraftType_TabsText")->stringdata[i];
					itm->TextColor_Default = TextColor_Header_Default;
					itm->TextColor_Hovered = TextColor_Header_Hovered;
					itm->TextColor_Selected = TextColor_Header_Selected;
					itm->TextColor_Disabled = TextColor_Header_Disabled;
					itm->SetOwnedData<int>(i);
				}
			};
			ReceiptsFiltersPanel->AddItem(tabItem);
			tabXOffset += tabWidth + 0.01f;
		}
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 9");

		ReceiptsFiltersPanel->Init();
		ReceiptDescription->Init();
		CraftButton->Init();

		CurrentReceiptsType = Invalid;
		BuildReceiptsList();
		ReceiptsListView->Init();
		DEBUG_MSG("StExt - Initialize mod Ui: menu framework - Craft Windows 10");
	}

	void CraftWindow::Draw() { MenuWindow::Draw(); }

	void CraftWindow::Update() { MenuWindow::Update(); }

	void CraftWindow::BuildReceiptsList()
	{
		// ToDo: Fix this shit... somehow...
		ReceiptsListView->ClearItems();
		for (auto& pair : ReceiptsData)
		{
			int id = pair.GetKey();
			ReceiptInfo* receipt = pair.GetValue();

			if (receipt && (receipt->ScriptInstance.Type == CurrentReceiptsType || CurrentReceiptsType == Invalid))
			{
				MenuItem* receiptItem = new MenuItem();
				receiptItem->OnInit = [receipt](BaseUiElement* item)
				{
					item->Name = "CraftReceipt_" + Z(receipt->ScriptInstance.Id);
					MenuItem* itm = static_cast<MenuItem*>(item);
					if (itm)
					{
						itm->Text = receipt->ScriptInstance.Name;
						itm->SetPtrData(receipt);
					}
				};
				ReceiptsListView->AddItem(receiptItem);
				receiptItem->Init();
			}
		}
	}

	CraftWindow::~CraftWindow() {}
}
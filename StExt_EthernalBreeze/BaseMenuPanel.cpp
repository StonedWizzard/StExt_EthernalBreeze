#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	BaseMenuPanel::BaseMenuPanel() 
	{
		Items = Array<BaseMenuElement*>();
		SelectedItem = Null;
		FocusedItem = Null;
	}

	inline void BaseMenuPanel::ClearItem(BaseMenuElement* item, bool fromArray = true)
	{
		if (!item) return;
		if (SelectedItem == item) RemoveSelection();
		if (FocusedItem == item) RemoveFocus();
		if (fromArray) Items.Remove(item);
		delete item;
	}

	void BaseMenuPanel::ClearItems()
	{
		uint elementsCount = Items.GetNum();
		for (uint i = 0; i < elementsCount; i++) { ClearItem(Items[i], false); }
		Items.Clear();
	}

	BaseMenuElement* BaseMenuPanel::AddItem(BaseMenuElement* item)
	{
		if (!item) return Null;

		Items.InsertEnd(item);
		item->Parent = this;
		return item;
	}

	void BaseMenuPanel::RemoveItem(BaseMenuElement* item)
	{
		if (!item) return;
		for (uint i = 0; i < Items.GetNum(); ++i) {
			if (Items[i] == item)
			{
				ClearItem(item, false);
				Items.RemoveAt(i);
				return;
			}
		}
	}

	BaseMenuElement* BaseMenuPanel::FindElementAt(int globalX, int globalY)
	{
		if (!IsVisible || IsHiden) return Null;

		const bool isHit = (globalX > GlobalRectLeft && globalX < GlobalRectRight) && (globalY > GlobalRectTop && globalY < GlobalRectBottom);
		if (!isHit) return Null;

		for (int i = Items.GetNum() - 1; i >= 0; --i)
		{
			BaseMenuElement* child = Items[i];
			if (!child || child == this) continue;

			BaseMenuElement* hit = child->FindElementAt(globalX, globalY);
			if (hit) return hit;
		}
		return this;
	}

	BaseMenuElement* BaseMenuPanel::FindTopElementAt(int globalX, int globalY)
	{
		if (!IsVisible || IsHiden) return Null;

		bool isHit = (globalX > GlobalRectLeft && globalX < GlobalRectRight) && (globalY > GlobalRectTop && globalY < GlobalRectBottom);
		if (!isHit) return Null;

		for (uint i = 0; i < Items.GetNum(); ++i)
		{
			BaseMenuElement* item = Items[i];
			if (!item || item == this || !item->IsVisible || item->IsHiden) continue;

			isHit = (globalX > item->GlobalRectLeft && globalX < item->GlobalRectRight) && (globalY > item->GlobalRectTop && globalY < item->GlobalRectBottom);
			if (isHit)
				return item;
		}
		return Null;
	}

	void BaseMenuPanel::Resize() 
	{ 
		BaseMenuElement::Resize();
		ForEachItem([](uint i, BaseMenuElement* item) { item->Resize(); });
	}

	void BaseMenuPanel::Init() 
	{
		BaseMenuElement::Init(); 
		ForEachItem([](uint i, BaseMenuElement* item) { item->Init(); });
	}

	void BaseMenuPanel::Draw()
	{
		BaseMenuElement::Draw();
		if (!IsVisible || IsHiden) return;
		ForEachItem([](uint i, BaseMenuElement* item) { item->Draw(); });
	}

	void BaseMenuPanel::Update()
	{
		BaseMenuElement::Update();

		ItemsCount = Items.GetNum();
		--MouseCounter;
		if (MouseCounter == 1)
		{
			ForEachItem([](uint i, BaseMenuElement* item)
			{				
				item->IsHovered = false;
				if (!item->HasBehavior(UiElementBehaviorFlags::Checkable)) item->IsSelected = false;
			});
		}
		if (MouseCounter < 0) MouseCounter = 0;
		ForEachItem([](uint i, BaseMenuElement* item) { item->Update(); });
	}

	bool BaseMenuPanel::HandleMouse(const UiMouseEventArgs& args)
	{ 
		if (!IsEnabled || !IsVisible || IsHiden) return false;

		BaseMenuElement* target = FindTopElementAt(args.X, args.Y);
		bool handled = false;

		if (!target) 
		{
			RemoveFocus();
			return handled;
		}
		BaseMenuPanel* targetContainer = dynamic_cast<BaseMenuPanel*>(target);
		
		if (targetContainer && targetContainer != this)
		{
			if (targetContainer->HandleMouse(args))
				return true;
		}

		//DEBUG_MSG("StExt - Mod Ui try focus on '" + Z(target->Name.c_str()) + "' flags: " + Z((int)target->BehaviorFlags));
		if (target->HasBehavior(UiElementBehaviorFlags::Hoverable)) {
			if (target != FocusedItem) FocusItem(target);
		}
		else { RemoveFocus(); }

		if (target->HasBehavior(UiElementBehaviorFlags::Selectable) && (args.Action == UiMouseEnum::LeftClick)) {
			if (target != SelectedItem) SelectItem(target);
		}		

		MouseCounter = 8;
		handled = target->HandleMouse(args);
		return handled;
	}

	bool BaseMenuPanel::HandleKey(const UiKeyEventArgs& args)
	{ 
		//return (CanInteract() && OnKeyEvent) ? OnKeyEvent(this, args) : false; 
		return false;
	}

	void BaseMenuPanel::SelectItem(BaseMenuElement* item)
	{
		if (SelectedItem) {
			SelectedItem->IsSelected = false;
		}

		RemoveSelection();
		if (!item || !item->HasBehavior(UiElementBehaviorFlags::Selectable)) return;

		item->IsSelected = true;
		SelectedItem = item;

		if (OnItemSelectCallback)
			OnItemSelectCallback(item);

		if (auto nestedPanel = dynamic_cast<BaseMenuPanel*>(item))
		{
			if (nestedPanel->SelectedItem)
				nestedPanel->SelectItem(nestedPanel->SelectedItem);
		}
	}

	void BaseMenuPanel::FocusItem(BaseMenuElement* item)
	{
		if (FocusedItem) {
			FocusedItem->IsHovered = false;
		}

		RemoveFocus();
		if (!item || !item->HasBehavior(UiElementBehaviorFlags::Hoverable)) return;

		item->IsHovered = true;
		FocusedItem = item;

		if (this->HasBehavior(UiElementBehaviorFlags::Hoverable) && 
			this->HasBehavior(UiElementBehaviorFlags::Interactable)) { ForEachItem([](uint i, BaseMenuElement* item) { item->IsHovered = true; }); }

		if (OnItemFocusCallback)
			OnItemFocusCallback(item);
	}

	void BaseMenuPanel::RemoveSelection() 
	{
		if (SelectedItem && OnItemRemoveSelectionCallback)
			OnItemRemoveSelectionCallback(SelectedItem);

		ForEachItem([](uint i, BaseMenuElement* item)
		{
			item->IsSelected = false;
			if (auto panel = dynamic_cast<BaseMenuPanel*>(item)) {
				panel->RemoveSelection();
			}
		});
		SelectedItem = Null;
	}

	void BaseMenuPanel::RemoveFocus()
	{
		if (FocusedItem && OnItemRemoveFocusCallback)
			OnItemRemoveFocusCallback(FocusedItem);
				
		ForEachItem([](uint i, BaseMenuElement* item)
		{
			item->IsHovered = false;
			if (auto panel = dynamic_cast<BaseMenuPanel*>(item))
				panel->RemoveFocus();
		});
		FocusedItem = Null;
	}

	BaseMenuPanel::~BaseMenuPanel()
	{
		ClearItems();
		SelectedItem = Null;
		FocusedItem = Null;
	}
}
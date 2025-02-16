#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	void BaseMenuPanel::Init(float posX, float posY, float sizeX, float sizeY)
	{
		BaseMenuItem::Init(posX, posY, sizeX, sizeY);
		Items = Array<MenuItem*>();
		SelectedItem = Null;
		FocusedItem = Null;
	}

	void BaseMenuPanel::Draw()
	{
		BaseMenuItem::Draw();
		if (!IsVisible) return;

		size_t elementsCount = Items.GetNum();
		for (size_t i = 0; i < elementsCount; i++) {
			Items[i]->Draw();
		}
	}

	void BaseMenuPanel::Update()
	{
		BaseMenuItem::Update();
		size_t elementsCount = Items.GetNum();
		ItemsCount = elementsCount;
		for (size_t i = 0; i < elementsCount; i++) {
			Items[i]->Update();
		}
	}

	void BaseMenuPanel::AddItem(std::function<void(MenuItem*)> callback) 
	{
		MenuItem* item = new MenuItem();
		Items.InsertEnd(item);
		item->ParentView = this->View;
		item->Init();
		if (callback)
			callback(item);
		Update();
	}

	void BaseMenuPanel::RemoveItem(MenuItem* item) 
	{
		if (!item) return;

		size_t elementsCount = Items.GetNum();
		for (size_t i = 0; i < elementsCount; i++) 
		{
			if (Items[i] == item) 
			{
				delete Items[i];
				Items.RemoveAt(i);
				Update();
				return;
			}
		}
	}

	void BaseMenuPanel::SelectItem(MenuItem* item) 
	{		
		if (SelectedItem && OnItemRemoveSelectionCallback)
			OnItemRemoveSelectionCallback(SelectedItem);

		SelectedItem = Null;
		size_t elementsCount = Items.GetNum();
		for (size_t i = 0; i < elementsCount; i++)
		{
			Items[i]->IsSelected = false;
			if (item && Items[i]->AllowSelection && (Items[i] == item))
			{
				Items[i]->IsSelected = true;
				SelectedItem = Items[i];
				if (OnItemSelectCallback)
					OnItemSelectCallback(Items[i]);
			}
		}
	}

	void BaseMenuPanel::FocusItem(MenuItem* item)
	{
		if (FocusedItem && OnItemRemoveFocusCallback)
			OnItemRemoveFocusCallback(FocusedItem);

		FocusedItem = Null;
		size_t elementsCount = Items.GetNum();
		for (size_t i = 0; i < elementsCount; i++)
		{
			Items[i]->IsHovered = false;
			if (item && Items[i]->AllowHover && (Items[i] == item))
			{
				Items[i]->IsHovered = true;
				FocusedItem = Items[i];
				if (OnItemFocusCallback)
					OnItemFocusCallback(Items[i]);
			}
		}
	}

	void BaseMenuPanel::RemoveSelection() 
	{
		if (SelectedItem)
		{
			if (OnItemRemoveSelectionCallback)
				OnItemRemoveSelectionCallback(SelectedItem);
			SelectedItem->IsSelected = false;
		}
		SelectedItem = Null;
	}

	void BaseMenuPanel::RemoveFocus()
	{
		if (FocusedItem)
		{
			if (OnItemRemoveFocusCallback)
				OnItemRemoveFocusCallback(FocusedItem);
			FocusedItem->IsHovered = false;
		}
		FocusedItem = Null;
	}

	void BaseMenuPanel::ClearItems() 
	{
		size_t elementsCount = Items.GetNum();
		for (size_t i = 0; i < elementsCount; i++)
		{
			if (Items[i] == SelectedItem) RemoveSelection();
			delete Items[i];
		}
		Items.Clear();
		Update();
	}
}
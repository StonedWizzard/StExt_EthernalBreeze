#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	void BuildReciptsList(MenuListView* list);
	void BuildChoicesList(MenuListView* list);

	//(0.1f, 0.1f, 0.8f, 0.8f);
	void CraftMenu::Init(float posX, float posY, float sizeX, float sizeY)
	{
		BaseMenuPanel::Init(posX, posY, sizeX, sizeY);
		
		RecieptsList = new MenuListView();
		ChoicesList = new MenuListView();

		RecieptsList->Init(0.0f, 0.0f, 0.3f, 1.0f);
		ChoicesList->Init(0.3f, 0.0f, 0.7f, 1.0f);
	}

	void CraftMenu::Draw()
	{
		BaseMenuPanel::Draw();
		if (!IsVisible) return;

		size_t elementsCount = Items.GetNum();
		for (size_t i = 0; i < elementsCount; i++) {
			Items[i]->Draw();
		}
	}

	void CraftMenu::Update()
	{
		BaseMenuPanel::Update();
		size_t elementsCount = Items.GetNum();
		ItemsCount = elementsCount;
		for (size_t i = 0; i < elementsCount; i++) {
			Items[i]->Update();
		}
	}

	bool CraftMenu::HandleKey(int key)
	{

	}
}
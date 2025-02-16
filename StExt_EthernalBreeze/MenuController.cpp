#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	CraftMenu* CraftWindow;
	BaseMenuPanel* CurrentMenu;
	bool ModMenuControllerInitialized;

	void InitModMenu()
	{

	}

	void ModMenuLoop()
	{
		if (!ogame || !player || !screen)
			return;
		if (!ModMenuControllerInitialized) return;

		if (ogame->IsOnPause()) { }

		if (CurrentMenu)
		{
			BlockMovement = true;
			player->SetMovLock(BlockMovement);
			CurrentMenu->Draw();
		}
		else if (BlockMovement)
			player->SetMovLock(true);
		else
			player->SetMovLock(false);
	}

	bool HandleModKey(int key)
	{

	}
}
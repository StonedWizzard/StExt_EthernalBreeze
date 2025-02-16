#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	const int ScreenVBufferSize = 8192;
	const int ScreenHalfVBufferSize = 4096;

	extern zCOLOR DefaultColor;
	extern zCOLOR HoveredColor;
	extern zCOLOR SelectedColor;

	class BaseMenuItem
	{
	public:
		zCView* View;
		zCView* ParentView;

		bool IsVisible;

		float SizeX;
		float SizeY;
		float PosX;
		float PosY;

		zSTRING BgTexture;

		std::function<void(BaseMenuItem*)> OnUpdateCallback;

		virtual void Init();
		virtual void Init(float posX, float posY, float sizeX, float sizeY);
		virtual void Resize();
		virtual void Draw();
		virtual void Update();
		virtual ~BaseMenuItem();
	};

	class MenuItem : public BaseMenuItem
	{
	public:
		bool AllowSelection;
		bool AllowHover;
		bool AllowInteraction;
		bool IsSelected;
		bool IsHovered;

		void* Data;
		zSTRING Text;
		zSTRING BgTexture;

		std::function<void(MenuItem*, int)> OnHandleKeyCallback;

		virtual void Init() override;
		virtual void Init(float posX, float posY, float sizeX, float sizeY) override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual bool HandleKey(int key);
	};

	class BaseMenuPanel : public BaseMenuItem
	{
	public:
		unsigned int ItemsCount;
		Array<MenuItem*> Items;
		MenuItem* SelectedItem;
		MenuItem* FocusedItem;

		std::function<void(MenuItem*)> OnItemFocusCallback;
		std::function<void(MenuItem*)> OnItemSelectCallback;
		std::function<void(MenuItem*)> OnItemRemoveFocusCallback;
		std::function<void(MenuItem*)> OnItemRemoveSelectionCallback;

		virtual void AddItem(std::function<void(MenuItem*)>);
		virtual void RemoveItem(MenuItem* item);
		virtual void SelectItem(MenuItem* item);
		virtual void FocusItem(MenuItem* item);
		virtual void RemoveSelection();
		virtual void RemoveFocus();
		virtual void ClearItems();

		virtual void Init(float posX, float posY, float sizeX, float sizeY) override;
		virtual void Draw() override;
		virtual void Update() override;
		virtual bool HandleKey(int key) = 0;
	};

	class MenuListView : public BaseMenuPanel
	{
	public:
		float ItemHeight;
		float ItemWidth;
		int ScrollOffset;
		int ScrollCanvasSize;
		int ScrollStep;

		virtual void Init(float posX, float posY, float sizeX, float sizeY) override;
		virtual void Draw() override;
		virtual void Update() override;
		virtual bool HandleKey(int key) override;
		virtual void Scroll(int delta);
	};

	class CraftMenu : BaseMenuPanel
	{
	public:
		MenuListView* RecieptsList;
		MenuListView* ChoicesList;

		virtual void Init(float posX, float posY, float sizeX, float sizeY) override;
		virtual void Draw() override;
		virtual void Update() override;
		virtual bool HandleKey(int key) override;
	};
}
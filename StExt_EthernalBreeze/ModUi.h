#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	const int Value_Type_Default = 0;
	const int Value_Type_Percent = 1;
	const int Value_Type_YesNo = 2;
	const int Value_Type_DefaultPerc = 3;
	const int Value_Type_InvertPerc = 4;
	const int Value_Type_Approx = 5;


	struct ExtraMasteryData
	{
		int MasteryId;
		bool IsCorruption;
		bool IsGeneric;
		int PerksCount;
		zSTRING TitleSymbol;
		zSTRING DescSymbol;
		zSTRING LevelSymbol;
		zSTRING RankSymbol;
		zSTRING ExpSymbol;
		zSTRING NextExpSymbol;
		zSTRING LpSymbol;
		zSTRING PerkNameSymbol;
		zSTRING PerkDescSymbol;
		zSTRING PerkValueSymbol;
	};

	struct ExtraConfigData
	{
		zSTRING ValueSymbol;
		zSTRING DescSymbol;
		int ConfigGroup;
		int MinValue;
		int MaxValue;
		int DisplayType;
		int IsVisible;
		int IsEditable;
		int IsExportable;
		int DisplayOrder;
	};

	extern Map<zSTRING, ExtraConfigData> ExtraConfigsData;
	extern Array<ExtraMasteryData> ExtraMasteriesData;

	const zSTRING UiElement_EmptyName = "NO_NAME";
	constexpr const char* UiElement_EmptyTexture = "";

	constexpr const char* UiElement_CursorTexture = "STEXT_UI_CURSOR.TGA";
	constexpr const char* UiElement_PanelTexture = "STEXT_STATSMENU.TGA";
	constexpr const char* UiElement_PanelBorderTexture = "STEXT_UI_PANEL.TGA";
	constexpr const char* UiElement_PanelNoBorderTexture = "STEXT_UI_PANEL_PLAIN.TGA";
	constexpr const char* UiElement_ButtonTexture = "STEXT_UI_BUTTON.TGA";
	constexpr const char* UiElement_ButtonNoBorderTexture = "STEXT_UI_BUTTON_PLAIN.TGA";
	constexpr const char* UiElement_ButtonSquareTexture = "STEXT_UI_BUTTON_SQUARE.TGA";
	constexpr const char* UiElement_ButtonSquareNoBorderTexture = "STEXT_UI_BUTTON_SQUARE_PLAIN.TGA";
	constexpr const char* UiElement_VSeparatorTexture = "STEXT_UI_V_SEP.TGA";
	constexpr const char* UiElement_HSeparatorTexture = "STEXT_UI_H_SEP.TGA";
	constexpr const char* UiElement_ListBorderTexture = "STEXT_UI_LIST.TGA";
	constexpr const char* UiElement_ExtraItemInfoPanelTexture = "STEXT_EXTRAITEMINFO.TGA";

	constexpr const char* MenuStr_ApproximalPrefix = "(±) ";

	const int ScreenVBufferSize = 8192;
	const int ScreenHalfVBufferSize = ScreenVBufferSize / 2;
	const float ScreenToRelativePixDelta = 1.0f / ScreenVBufferSize;
	const float CursorSize = 64.0f / ScreenVBufferSize;
	const int TextWrapIndex = 8000;

	extern zCOLOR TextColor_Default;
	extern zCOLOR TextColor_Warn;
	extern zCOLOR TextColor_Yes;
	extern zCOLOR TextColor_No;

	extern zCOLOR TextColor_Regular_Default;
	extern zCOLOR TextColor_Regular_Faded;
	extern zCOLOR TextColor_Regular_Hovered;
	extern zCOLOR TextColor_Regular_Selected;
	extern zCOLOR TextColor_Regular_Disabled;

	extern zCOLOR TextColor_Header_Default;
	extern zCOLOR TextColor_Header_Hovered;
	extern zCOLOR TextColor_Header_Selected;
	extern zCOLOR TextColor_Header_Disabled;

	extern zCOLOR TextColor_HeaderTab_Default;
	extern zCOLOR TextColor_HeaderTab_Hovered;
	extern zCOLOR TextColor_HeaderTab_Selected;
	extern zCOLOR TextColor_HeaderTab_Disabled;

	extern zSTRING MenuFont_Header;
	extern zSTRING MenuFont_Default;
	extern zSTRING MenuFont_Symbol_Default;
	extern zSTRING MenuFont_Symbol_Header;
	extern zSTRING MenuFont_Sys_Header;
	extern zSTRING MenuFont_Sys_Default;

	extern float ModMenuWindow_PosX;
	extern float ModMenuWindow_PosY;
	extern float ModMenuWindow_SizeX;
	extern float ModMenuWindow_SizeY;
	extern float ModMenuWindow_ScrollMult;
	extern float ModMenuWindow_ScrollShiftMult;
	extern float ExtraItemInfoPanel_PosX;
	extern float ExtraItemInfoPanel_PosY;

	extern int MsgTrayPosX_SymIndex;
	extern int MsgTrayPosY_SymIndex;

	enum class UiElementBehaviorFlags : uint
	{
		Empty = 0U,
		Interactable = 1 << 0U,
		Selectable = 1 << 1U,
		Hoverable = 1 << 2U,
		Checkable = 1 << 3U
	};

	inline UiElementBehaviorFlags operator|(UiElementBehaviorFlags a, UiElementBehaviorFlags b)	{
		return static_cast<UiElementBehaviorFlags>(static_cast<int>(a) | static_cast<int>(b));
	}
	inline UiElementBehaviorFlags& operator|=(UiElementBehaviorFlags& a, UiElementBehaviorFlags b) { a = a | b; return a; }

	// Refined (from raw key id) key action
	enum class UiKeyEnum : int
	{
		Empty,
		Enter,
		Escape,
		Up,
		Down,
		Left,
		Right,
		Back,
	};

	// Mouse action enum
	enum class UiMouseEnum : int
	{
		Empty,
		LeftClick,
		RightClick,
		MiddleClick,
		Scroll
	};

	enum class UiContentAlignEnum : int
	{
		Center,
		Begin,
		End
	};

	enum class UiValueDisplayType : int
	{
		Default = Value_Type_Default,
		Bool = Value_Type_YesNo,
		Permille = Value_Type_Percent,
		Percent = Value_Type_DefaultPerc,
		DeciPercent = Value_Type_InvertPerc,
	};

	struct UiMouseEventArgs
	{
		int X;
		int Y;
		int ScrollDelta;
		bool IsShiftPressed;
		bool IsAltPressed;
		UiMouseEnum Action;
	};

	struct UiKeyEventArgs
	{
		int KeyId;
		bool IsShiftPressed;
		bool IsAltPressed;
		UiKeyEnum Action;
	};

	//----------------------------------------------------
	//						Classes						//
	//----------------------------------------------------

	// Abstract element for all new Ui Menu system
	class BaseUiElement
	{
	public:
		zCView* View;
		BaseUiElement* Parent;

		zSTRING Name;
		bool IsVisible;
		bool IsHiden;
		zSTRING BgTexture;

		float SizeX;
		float SizeY;
		float PosX;
		float PosY;

		float InitialSizeX;
		float InitialSizeY;
		float InitialPosX;
		float InitialPosY;

		int LocalSizeX;
		int LocalSizeY;
		int LocalPosX;
		int LocalPosY;

		int GlobalSizeX;
		int GlobalSizeY;
		int GlobalPosX;
		int GlobalPosY;

		int GlobalRectLeft;
		int GlobalRectRight;
		int GlobalRectTop;
		int GlobalRectBottom;

		std::function<void(BaseUiElement*)> OnInit;
		std::function<void(BaseUiElement*)> OnResize;
		std::function<void(BaseUiElement*)> OnDraw;
		std::function<void(BaseUiElement*)> OnUpdate;

		BaseUiElement();

		virtual void Init();
		virtual void Resize();
		virtual void Draw();
		virtual void Update();

		virtual ~BaseUiElement();
	};

	// Abstract class for all menu elements
	class BaseMenuElement : public BaseUiElement
	{
	public:
		UiElementBehaviorFlags BehaviorFlags;
		bool IsSelected;
		bool IsHovered;
		bool IsEnabled;

		BaseMenuElement();
		inline bool HasBehavior(UiElementBehaviorFlags flag);

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual inline bool CanInteract();
		virtual BaseMenuElement* FindElementAt(int globalX, int globalY);

		virtual bool HandleMouse(const UiMouseEventArgs& args) = 0;
		virtual bool HandleKey(const UiKeyEventArgs& args) = 0;

		virtual ~BaseMenuElement();
	};

	// Generic menu element. Can work as button or simple text label
	class MenuItem : public BaseMenuElement
	{
	public:
		zSTRING Text;
		zSTRING Font;

		zCOLOR TextColor_Default;
		zCOLOR TextColor_Hovered;
		zCOLOR TextColor_Selected;
		zCOLOR TextColor_Disabled;
		
		UiContentAlignEnum HorizontalAlign;
		UiContentAlignEnum VerticalAlign;

		void* Data;
		bool OwnsData;

		std::function<bool(BaseMenuElement*, const UiMouseEventArgs& args)> OnMouseEvent;
		std::function<bool(BaseMenuElement*, const UiKeyEventArgs& args)> OnKeyEvent;

		MenuItem();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual bool HandleMouse(const UiMouseEventArgs& args) override;
		virtual bool HandleKey(const UiKeyEventArgs& args) override;

		template<typename T, typename... Args>
		inline void SetOwnedData(Args&&... args);

		template<typename T>
		inline void SetPtrData(T* ptr);

		template<typename T>
		inline T* GetData() const;

		virtual inline void ClearData();
		virtual ~MenuItem();
	};

	class MenuTextItem : public MenuItem
	{
	protected:
		Array<zSTRING> Lines;
	public:
		MenuTextItem();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		void RebuildLines();

		virtual ~MenuTextItem();
	};

	class MenuValueItem : public MenuItem
	{
	protected:
		inline bool ReadValue(zSTRING& valueName, zSTRING& value, int index, bool isSecondary, int& iValRef);
		inline void FormatValue(zSTRING& output, int value, bool isSecondary);
	public:
		bool IsArray;
		bool IsRanged;
		bool IsApproximal;
		bool IsSecondaryColorFormat;
		zSTRING RangeValueSeparator;

		zSTRING PrimaryValueName;
		int PrimaryValueArrayIndex;
		UiValueDisplayType PrimaryValueDisplayType;
		std::function<void(zSTRING& str, int val)> PrimarySpecialValueFormatter;

		zSTRING SecondaryValueName;
		int SecondaryValueArrayIndex;
		UiValueDisplayType SecondaryValueDisplayType;
		std::function<void(zSTRING& str, int val)> SecondarySpecialValueFormatter;
		
		std::function<void(zCOLOR& color, int val)> SpecialColorFormatter;

		MenuValueItem();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual bool HandleMouse(const UiMouseEventArgs& args) override;
		virtual bool HandleKey(const UiKeyEventArgs& args) override;

		virtual ~MenuValueItem();
	};

	// Class represent panel, elements group or container
	// with list of MenuItem's in it.
	class BaseMenuPanel : public BaseMenuElement
	{
	protected:
		int MouseCounter;

		inline virtual void ClearItem(BaseMenuElement* item, bool fromArray);
	public:
		unsigned int ItemsCount;
		Array<BaseMenuElement*> Items;
		BaseMenuElement* SelectedItem;
		BaseMenuElement* FocusedItem;

		std::function<void(BaseMenuElement*)> OnItemFocusCallback;
		std::function<void(BaseMenuElement*)> OnItemSelectCallback;
		std::function<void(BaseMenuElement*)> OnItemRemoveFocusCallback;
		std::function<void(BaseMenuElement*)> OnItemRemoveSelectionCallback;

		BaseMenuPanel();

		template<typename Func>
		void ForEachItem(Func&& func);

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual bool HandleMouse(const UiMouseEventArgs& args) override;
		virtual bool HandleKey(const UiKeyEventArgs& args) override;

		virtual BaseMenuElement* AddItem(BaseMenuElement* item);
		virtual void RemoveItem(BaseMenuElement* item);
		virtual void ClearItems();

		virtual BaseMenuElement* FindElementAt(int globalX, int globalY) override;
		virtual BaseMenuElement* FindTopElementAt(int globalX, int globalY);

		virtual void SelectItem(BaseMenuElement* item);
		virtual void FocusItem(BaseMenuElement* item);
		virtual void RemoveSelection();
		virtual void RemoveFocus();

		virtual ~BaseMenuPanel();
	};

	// Just scrollable panel
	class MenuScrollPanel : public BaseMenuPanel
	{
	protected:
		int ScrollCanvasSize;
		int ScrollOffset;
		int ScrollOffsetBefore;

		void UpdateCanvasSize();

	public:
		MenuScrollPanel();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual void Scroll(int delta);
		virtual bool HandleMouse(const UiMouseEventArgs& args) override;
		virtual bool HandleKey(const UiKeyEventArgs& args) override;

		virtual ~MenuScrollPanel();
	};

	// Strict vertical list
	class MenuListView : public BaseMenuPanel
	{
	protected:
		int ScrollCanvasSize;
		int ScrollOffset;
		int ScrollStep;
		int ScrollOffsetBefore;
	public:
		float ItemHeight;
		float ItemWidth;
		float ItemMargin;

		MenuListView();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual bool HandleMouse(const UiMouseEventArgs& args) override;
		virtual bool HandleKey(const UiKeyEventArgs& args) override;
		virtual void Scroll(int delta);

		virtual ~MenuListView();
	};

	// Class represent window kind of root container with elements.
	class MenuWindow : public BaseMenuPanel
	{
	public:
		bool HideCursor;

		MenuWindow();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual bool HandleMouse(const UiMouseEventArgs& args) override;
		virtual bool HandleKey(const UiKeyEventArgs& args) override;

		virtual ~MenuWindow();
	};

	class CraftWindow : public MenuWindow
	{
	public:
		MenuListView* ReceiptsListView;
		BaseMenuPanel* ReceiptsFiltersPanel;
		MenuItem* ReceiptDescription;
		MenuItem* ReceiptItemsSlots[ReceiptInfo_Slots_Max];
		MenuItem* CraftButton;

		int CurrentReceiptsType;

		CraftWindow();

		void BuildReceiptsList();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual ~CraftWindow();
	};

	class TestWindow : public MenuWindow
	{
	public:
		MenuListView* ItemsListView;
		BaseMenuPanel* TabPanel;
		MenuItem* Description;
		MenuItem* TestButton;

		int TabIndex;
		int ItemIndex;

		TestWindow();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		void UpdateText();

		virtual ~TestWindow();
	};

	class ConfigsWindow : public MenuWindow
	{
	protected:
		void InitTabs();
		void InitConfigsTab();
		void InitModInfoTab();

		inline BaseMenuPanel* GetTabPanel(const int index);

	public:
		BaseMenuPanel* TabPanel;
		BaseMenuPanel* ConfigsTabPanel;
		Array<MenuScrollPanel*> ConfigsPanels;
		MenuScrollPanel* ModInfoPanel;

		int TabIndex;
		int TabIndexMax;
		int ConfigPanelIndex;
		int ConfigPanelIndexMax;

		ConfigsWindow();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual ~ConfigsWindow();
	};

	class StatsWindow : public MenuWindow
	{
	protected:
		void InitTabs();
		void InitGeneralTab();
		void InitStatsTab();
		void InitSkillsTab();
		void InitProfesionTab();
		void InitBonusTab();

		inline BaseMenuPanel* GetTabPanel(const int index);
	public:
		BaseMenuPanel* TabPanel;
		BaseMenuPanel* SkillsTabPanel;

		MenuScrollPanel* GeneralTabPanel;
		MenuScrollPanel* StatsTabPanel;
		MenuScrollPanel* ProfesionTabPanel;
		MenuScrollPanel* BonusTabPanel;
		Array<MenuScrollPanel*> SkillPanels;

		int TabIndex;
		int TabIndexMax;

		int MasteryIndex;
		int MasteryIndexMax;

		StatsWindow();

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual ~StatsWindow();
	};

	class MenuCursorController
	{
	protected:
		static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
		static MenuCursorController* Instance;
		static HHOOK MouseHook;

		bool LeftBtnPressed = false, LeftBtnReleased = false;
		bool RightBtnPressed = false, RightBtnReleased = false;
		bool MiddleBtnPressed = false, MiddleBtnReleased = false;

		bool IsHookChecked;
	public:
		int ScreenWidth;
		int ScreenHeight;
		float ScreenRatioX;
		float ScreenRatioY;

		bool HasEvent;
		bool IsInitialized;
		UiMouseEventArgs MouseState;

		void Init();
		void Update();
		void CreateMouseDescriptor(UiMouseEventArgs& desc);

		~MenuCursorController();
	};

	// Cursor view
	class MenuCursor : public BaseUiElement
	{
	protected:
		MenuCursorController* Controller;
	public:
		void SetController(MenuCursorController* controller);

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual ~MenuCursor();
	};


	struct ItemInfoLine
	{
		zSTRING Text;
		zCOLOR TextColor;
		UiContentAlignEnum TextAllignMode;

		ItemInfoLine(const zSTRING& txt, const zCOLOR& color, const UiContentAlignEnum alignMode = UiContentAlignEnum::Center);
	};

	class ItemInfoPanel : public BaseUiElement
	{
	protected:
		bool ItemSwitched;
		const oCItem* DisplayItem;
		Array<ItemInfoLine> Content;
		zSTRING ContentWidthIndex;
		int ContentHeightIndex;

		inline void CreateContentLine(const zSTRING& txt, const zCOLOR& color = TextColor_Regular_Default, const UiContentAlignEnum alignMode = UiContentAlignEnum::Center);
		inline void CreateContentStatLine(const int statId, const int statValue, const int statDuration = Invalid, const zCOLOR& color = TextColor_Regular_Default);
		inline void CreateContentAbilityLine(const int abilityId, const int abilityValue, const int abilityDuration, const int abilityChance, const zCOLOR& color = TextColor_Regular_Default);
		inline void CreateContentSeparatorLine();

		inline int GetArtifactType();
		inline const zCOLOR& GetRankColor(const int rank);
		inline const zSTRING& GetItemRankString(const int rank);
		inline zSTRING GetItemFullClassString(const ItemExtension* itemExtension);
		inline zSTRING GetExtraDamageTypeString(const int damageTypes);
		inline zSTRING GetExtraDamageFlagString(const int damageFlags);

		inline void BuildItemNameDescription(const ItemExtension* itemExtension);
		inline void BuildItemPropertiesDescription(const ItemExtension* itemExtension);

		void BuildArtifactDescription(const int mode);
		void BuildSpellDescription(const ItemExtension* itemExtension);
		void BuildExtensionDescription(const ItemExtension* itemExtension);

		void BuildItemDescription();

	public:
		ItemInfoPanel();

		void SetDisplayItem(const oCItem* item);

		virtual void Init() override;
		virtual void Resize() override;
		virtual void Draw() override;
		virtual void Update() override;

		virtual ~ItemInfoPanel();
	};


	void StonedExtension_InitUi();
	void StonedExtension_InitUi_Menu();

	void StonedExtension_Loop_Ui();
	void StonedExtension_Loop_MenuController();
	void StonedExtension_Loop_MsgTray();

	void ConvertValueToYesNo(zSTRING& str, int val);
	void ConvertValueToSkill(zSTRING& str, int val);
	void ConvertValueToPermille(zSTRING& str, int val);
	void ConvertValueToMasteryRank(zSTRING& str, int val);

	void BoolValueColorFormatter(zCOLOR& color, int value);
	void SkillValueColorFormatter(zCOLOR& color, int value);

	MenuItem* BuildHSeparator(const float posY);
	MenuItem* BuildVSeparator(const float posX);

	void UpdateCheckButtonStyle(MenuItem* button, zSTRING& valueSym);

	void UpdateUiStatus();

	int HandleModKey(const int key);
	bool OpenMenuWindow(const zSTRING& name);
	void CloseMenuWindow();

	void DrawModInfo();


	void MsgTray_AddEntry(zSTRING text, zSTRING color);

	void MsgTray_Clear();

	extern void ParseHexColor(const zSTRING& stringColor, zCOLOR& color);

#include "ModUi.inl"
}
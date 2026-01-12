#include <UnionAfx.h>
#include <StExt_Helpers.h>

namespace Gothic_II_Addon
{
	//---------------------------------------------------------------------------
	//								CONSTANTS
	//---------------------------------------------------------------------------

	const int ReceiptInfo_Slots_Max = 8;
	const int ReceiptInfo_Vars_Max = 16;

	enum class SlotFlags : int
	{
		Empty = 0,
		PreserveItem = 1 << 0,			// Item always preserve and never be consumed
		PreserveItemOnFail = 1 << 1,	// When receipt fails (if can) item in slot will be preserved anyway
		AllowSelection = 1 << 2,		// Select matched item from list in menu
		TargetItem = 1 << 3,			// Modify item in this slot
	};
	inline SlotFlags operator|(SlotFlags a, SlotFlags b) noexcept { return static_cast<SlotFlags>(static_cast<int>(a) | static_cast<int>(b)); }
	inline SlotFlags operator&(SlotFlags a, SlotFlags b) noexcept { return static_cast<SlotFlags>(static_cast<int>(a) & static_cast<int>(b)); }
	inline SlotFlags operator~(SlotFlags a) noexcept { return static_cast<SlotFlags>(~static_cast<int>(a)); };
	inline SlotFlags& operator|=(SlotFlags& a, SlotFlags b) noexcept { a = a | b; return a; }
	inline SlotFlags& operator&=(SlotFlags& a, SlotFlags b) noexcept { a = a & b; return a; }

	enum class ReceiptType : int
	{
		Unknown = 0,
		
	};

	enum class ReceiptFlags : int
	{
		Empty = 0,
		CanFail = 1 << 0,				// Turn On chances
		IsBasic = 1 << 1,				// Learn from begining
		NoExp = 1 << 2,					// Do not give exp
		MultiCraft = 1 << 3,			// Allow set few receipts at once
		RequireTarget = 1 << 4,			// Mean that receipt is manipulate with selected item...
	};
	inline ReceiptFlags operator|(ReceiptFlags a, ReceiptFlags b) noexcept { return static_cast<ReceiptFlags>(static_cast<int>(a) | static_cast<int>(b)); }
	inline ReceiptFlags operator&(ReceiptFlags a, ReceiptFlags b) noexcept { return static_cast<ReceiptFlags>(static_cast<int>(a) & static_cast<int>(b)); }
	inline ReceiptFlags operator~(ReceiptFlags a) noexcept { return static_cast<ReceiptFlags>(~static_cast<int>(a)); };
	inline ReceiptFlags& operator|=(ReceiptFlags& a, ReceiptFlags b) noexcept { a = a | b; return a; }
	inline ReceiptFlags& operator&=(ReceiptFlags& a, ReceiptFlags b) noexcept { a = a & b; return a; }

	enum class FilterFlags : uflag64
	{
		Empty = 0ULL,
		ItemsCount = 1ULL << 0,
		ItemMainFlags = 1ULL << 1,
		ItemBaseFlags = 1ULL << 2,
		ItemAdditionalFlags = 1ULL << 3,
		ItemDamageType = 1ULL << 4,
		ItemLevel = 1ULL << 5,
		ItemRank = 1ULL << 6,
		ItemQuality = 1ULL << 7,
		ItemIncludeTags = 1ULL << 8,
		ItemExcludeTags = 1ULL << 9,
		ItemCraftData = 1ULL << 10,
		ItemCraftFlags = 1ULL << 11,
		ItemInstances = 1ULL << 12,
		BaseItemInstances = 1ULL << 13,
		ItemClasses = 1ULL << 14,
		All = ~0ULL
	};
	inline FilterFlags operator|(FilterFlags a, FilterFlags b) noexcept { return static_cast<FilterFlags>(static_cast<uflag64>(a) | static_cast<uflag64>(b)); }
	inline FilterFlags operator&(FilterFlags a, FilterFlags b) noexcept { return static_cast<FilterFlags>(static_cast<uflag64>(a) & static_cast<uflag64>(b)); }
	inline FilterFlags operator~(FilterFlags a) noexcept { return static_cast<FilterFlags>(~static_cast<uflag64>(a)); };
	inline FilterFlags& operator|=(FilterFlags& a, FilterFlags b) noexcept { a = a | b; return a; }
	inline FilterFlags& operator&=(FilterFlags& a, FilterFlags b) noexcept { a = a & b; return a; }


	//---------------------------------------------------------------------------
	//									CLASSES
	//---------------------------------------------------------------------------

	struct ReceiptSlotInfo
	{
		bool IsEmpty;
		SlotFlags Flags;
		FilterFlags EnabledFilters;
		zSTRING Pattern;

		Array<zSTRING> ItemInstances;
		Array<zSTRING> BaseItemInstances;
		Array<ItemClassKey> ItemClasses;

		int ItemsCount;
		int ItemMainFlags;
		int ItemBaseFlags;
		int ItemAdditionalFlags;
		unsigned long ItemDamageType;

		ValueRange<int> ItemLevel;
		ValueRange<int> ItemRank;
		ValueRange<int> ItemQuality;

		Array<int> ItemIncludeTags;
		Array<int> ItemExcludeTags;
		Map<int, ValueRange<int>> ItemCraftData;
		Map<int, ValueRange<byte>> ItemCraftFlags;
	};

    class ReceiptInfo
    {
	private:
		ReceiptSlotInfo SlotsData[ReceiptInfo_Slots_Max];

    public:
		const ReceiptSlotInfo* const Slots = SlotsData;

		int ConditionFunc;
		int OnCraftFunc;
		int OnFailFunc;

        struct
        {
			int Id;
			int Type;
			int Flags;
			int Vars[ReceiptInfo_Vars_Max];

			zSTRING Name;
			zSTRING Description;

			int SlotFlags[ReceiptInfo_Slots_Max];
			zSTRING SlotPattern[ReceiptInfo_Slots_Max];

			zSTRING Condition;
			zSTRING OnCraft;
			zSTRING OnFail;

			int BaseExp;
			int BaseChance;
			int ProfessionId;
			int ProfessionLevel;
			int MaxBatchAmount;

        } ScriptInstance;

		friend bool InitializeReceipt(const zSTRING& instanceName);
    };

	class CraftContext
	{
	public:

		bool IsValid;
		bool IsReady;

		oCItem* Items[ReceiptInfo_Slots_Max];
		const ReceiptInfo* Receipt;

		struct
		{
			int ReceiptId;
			int Type;
			int Flags;
			int IsReady;

			int Exp;
			int Chance;
			int Power;
			int Values[ReceiptInfo_Vars_Max];
			int BatchAmount;

			int SlotItemsPtrs[ReceiptInfo_Slots_Max];
			int SlotItemsCount[ReceiptInfo_Slots_Max];
			int SlotFlags[ReceiptInfo_Slots_Max];
		} ReceiptDescriptor;

		CraftContext(const ReceiptInfo* receipt);
	};


	//---------------------------------------------------------------------------
	//							GLOBAL VARS & API
	//---------------------------------------------------------------------------

	extern Map<int, ReceiptInfo*> ReceiptsData;

	bool CompileSlotPattern(zSTRING& pattern, const int slotFlags, ReceiptSlotInfo& slot);
	bool InitializeReceipt(const zSTRING& instanceName);

	void InitReceiptSlotParser();
	void InitCraftEngine();

	inline const ReceiptInfo* GetReceiptById(const int id);
	inline const ReceiptInfo* GetReceiptByName(const zSTRING& name);

	inline bool IsCraftContextExist();
	inline bool IsCraftContextValid();
	inline bool IsCraftContextReady();









	inline void IdentifyItem(const uint extensionId);

	//???
	inline void ChangeItemLevel(const uint extensionId, int delta);
	inline void ChangeItemRank(const uint extensionId, int delta);
	inline void ChangeItemQuality(const uint extensionId, int delta);

}
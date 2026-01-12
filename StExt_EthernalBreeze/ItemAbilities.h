#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    class oCItem;
    class ItemExtension;

    //---------------------------------------------------------------------------
    //								CONSTANTS
    //---------------------------------------------------------------------------

    const int ItemAbilityParamsMax = 16;
    const int ItemAbilityMasteriesMax = 3;

    //---------------------------------------------------------------------------
    //								  ENUMS
    //---------------------------------------------------------------------------

    enum class ItemAbilityTriggerIndex : int
    {
        Empty = 0,                  // None
        OnTick = 1,                 // on every second
        OnMidnight = 2,             // when time is 00:00
        OnDawn = 3,                 // when time is 04:00
        OnMorning = 4,              // when time is 08:00
        OnNoon = 5,                 // when time is 12:00
        OnAfternoon = 6,            // when time is 16:00
        OnDusk = 7,                 // when time is 18:00
        OnEvening = 8,              // when time is 20:00

        OnOffence = 9,              // when hero generaly do damage
        OnDefence = 10,             // when hero generaly get damage
        OnSweep = 11,               // when hero do just attack move with meele weapon
        OnShoot = 12,               // when hero shoot from bow/crossbow
        OnThrow = 13,               // when hero throw something
        OnCast = 14,                // when hero cast some spell
        OnBlock = 15,               // when hero do block
        OnKill = 16,                // when hero kill somebody

        OnLevelUp = 17,             // when hero get level
        OnCorruptionLevelUp = 18,   // when hero get level
        OnTheft = 19,               // when hero stealing pocket
        OnTheftSuccess = 20,        // when hero steal pocket
        OnTheftFail = 21,           // when hero fail to steal pocket
        OnLootChest = 22,           // when hero open new chest
        OnLootBody = 22,            // when hero loots enemy body
        OnPray = 23,                // when hero pray to any gods
        OnPrayBeliar = 24,          // when hero pray to any gods
        OnPrayAdanos = 25,          // when hero pray to any gods
        OnPrayInnos = 26,           // when hero pray to any gods

        OnSummonOffence = 25,       // when summon generaly do damage
        OnSummonDefence = 26,       // when summon generaly get damage
        OnSummonSweep = 27,         // when summon do just attack move with meele weapon
        OnSummonShoot = 28,         // when summon shoot from bow/crossbow
        OnSummonThrow = 29,         // when summon throw something
        OnSummonCast = 30,          // when summon cast some spell
        OnSummonBlock = 31,         // when summon do block
        OnSummonKill = 32,          // when summon kill somebody

        OnAllyOffence = 33,         // when ally generaly do damage
        OnAllyDefence = 34,         // when ally generaly get damage
        OnAllySweep = 35,           // when ally do just attack move with meele weapon
        OnAllyShoot = 36,           // when ally shoot from bow/crossbow
        OnAllyThrow = 37,           // when ally throw something
        OnAllyCast = 38,            // when ally cast some spell
        OnAllyBlock = 39,           // when ally do block
        OnAllyKill = 40,            // when ally kill somebody
    };

    enum class ItemAbilityFlags : int
    {
        Empty = 0,
        IsPassive = 1 << 0,
        HasValue = 1 << 1,
        HasRange = 1 << 2,
        HasChance = 1 << 3,
        HasDuration = 1 << 4,
        HasDescription = 1 << 5,
        ConsumeMp = 1 << 6,
        ConsumeHp = 1 << 7,
        ConsumeSt = 1 << 8,
        ConsumeEs = 1 << 9,
    };
    inline ItemAbilityFlags operator|(ItemAbilityFlags a, ItemAbilityFlags b) noexcept { return static_cast<ItemAbilityFlags>(static_cast<int>(a) | static_cast<int>(b)); }
    inline ItemAbilityFlags operator&(ItemAbilityFlags a, ItemAbilityFlags b) noexcept { return static_cast<ItemAbilityFlags>(static_cast<int>(a) & static_cast<int>(b)); }
    inline ItemAbilityFlags operator~(ItemAbilityFlags a) noexcept { return static_cast<ItemAbilityFlags>(~static_cast<int>(a)); };
    inline ItemAbilityFlags& operator|=(ItemAbilityFlags& a, ItemAbilityFlags b) noexcept { a = a | b; return a; }
    inline ItemAbilityFlags& operator&=(ItemAbilityFlags& a, ItemAbilityFlags b) noexcept { a = a & b; return a; }

    class ItemAbility
    {
    public:
        int UId;
        zSTRING InstanceName;
        ItemAbilityTriggerIndex TriggerType;
        ItemAbilityFlags AbilityFlags;

        int ConditionFunc;
        int OnAddFunc;
        int OnRemoveFunc;
        int OnTriggerFunc;

        struct
        {
            int Id;
            int Type;
            int Flags;

            int RollChance;

            zSTRING Name;
            zSTRING Description;
            zSTRING Effect;
            zSTRING Condition;
            zSTRING OnAdd;
            zSTRING OnRemove;
            zSTRING OnTrigger;

            int BaseValue;
            int BaseRange;
            int BaseChance;
            int BaseDuration;

            int ValueMax;
            int RangeMax;
            int ChanceMax;
            int DurationMax;

            int ManaCost;
            int StaminaCost;
            int HealthCost;
            int EnergyShieldCost;

            int Params[ItemAbilityParamsMax];
            int MasteryId[ItemAbilityMasteriesMax];
        } ScriptInstance;
    };

    class ItemAbilityDescriptor
    {
    public:
        const ItemAbility* Ability;
        const ItemExtension* Extension;

        struct
        {
            int AbilityId;
            int AbilityType;
            int AbilityFlags;
            int ExtensionId;

            int Value;
            int Range;
            int Chance;
            int Duration;
            int Params[ItemAbilityParamsMax];
        } ScriptInstance;

        ItemAbilityDescriptor(const ItemAbility* itemAbility, const ItemExtension* itemExtension);

        constexpr bool operator==(const ItemAbilityDescriptor& other) const noexcept;
        constexpr bool operator!=(const ItemAbilityDescriptor& other) const noexcept;
        constexpr bool operator<(const ItemAbilityDescriptor& other) const noexcept;
        constexpr bool operator>(const ItemAbilityDescriptor& other) const noexcept;
        constexpr bool operator<=(const ItemAbilityDescriptor& other) const noexcept;
        constexpr bool operator>=(const ItemAbilityDescriptor& other) const noexcept;
    };


    extern Map<int, ItemAbility> ItemAbilitiesData;
    extern Array<ItemAbilityDescriptor> ActiveItemAbilities;

    void InitItemAbilitiesData();
    int GetActiveItemAbilitiesCount();

    extern const ItemAbility* GetItemAbility(const int abilityId);
    extern void ClearActiveItemAbilities();
    void ApplyItemAbilities(const oCItem* item, const ItemExtension* extension);
    void RemoveItemAbilities(const oCItem* item, const ItemExtension* extension);
    bool IsItemAbilityActive(const int abilityId);

}
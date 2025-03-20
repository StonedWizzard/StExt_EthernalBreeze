#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    class ItemAbility;
    class oCItem;
    class C_EnchantmentData;

    const int StExt_ItemAbilityType_Passive = 0;				// Ability check and pulled by scripts in appropriated place. MANUALY!

    const int StExt_ItemAbilityType_Offensive = 1;				// Processed when npc attack other npc. Ability select from npc weapon + all other equipment (hero/summons only, except other weapon).
                                                                // Then scripts/engine AUTOMATICALLY build effect projectile. WORKS ONLY in damage handling.

    const int StExt_ItemAbilityType_Defensive = 2;				// Processed when hero get damage from other npc. Same as offencive, but for deffencive, lol)

    const int StExt_ItemAbilityType_Ñontinuous = 3;				// Constantly active ability. Try invoke 'OnApply()' every tick, if triggers not setted. 
                                                                // Otherwise handle all matching abilities to corresponding events in the loop.
                                                                // 'OnApply()' here is ESSENTIAL! Ability wouldn't do anything in this scheme if apply func not setted!

    const int StExt_ItemAbilityFlag_None = 0;
    const int StExt_ItemAbilityFlag_ForceApplyFunc = 1;				// Required to invoke 'OnApply()' instead default actions for offencive/deffencive abilities.
    const int StExt_ItemAbilityFlag_UseTimeBinding = 2;				// Ability applied only if time is appropriate. Otherwise, without this flags time of day doesn't matter.
    const int StExt_ItemAbilityFlag_SummonsOnly = 4;				// Ability works for summons only (kind of redirect abilities to them)
    const int StExt_ItemAbilityFlag_AlwaysSuccessChance = 8;		// Ignore chance variable on ability selection

    class ItemAbilityRecord
    {
    public:
        ItemAbility* Ability;
        oCItem* Item;
        C_EnchantmentData* Enchantment;

        struct
        {
            int Id;
            int Value;
            int Duration;
            int Chance;
            int IsFromWeapon;
        } C_ItemAbilityRecord;
    };

    class ItemAbility
    {
    public:
        int InstanceId;
        zSTRING InstanceName;

        int ConditionFunc;
        int OnApplyFunc;

        struct
        {
            int Id;
            zSTRING Name;

            int Type;
            int Flags;
            int TimeBindingFlags;
            int TriggerFlags;

            int AiVars[16];
            int DamageType;
            int DamageFlags;
            int MasteryExpPowerRatio;
            int MasteryId[3];

            zSTRING Effect;
            zSTRING Condition;
            zSTRING OnApply;
        } C_ItemAbility;
    };

    void InitializeItemAbility(zSTRING itemAbility);
    inline ItemAbility* GetItemAbility(const int abilityId);

    void ApplyItemAbilities(oCItem* item, C_EnchantmentData* enchantment);
    void RemoveItemAbilities(oCItem* item, C_EnchantmentData* enchantment);

    inline bool GetItemAbilityChance(int abilityId, int chance);
    inline bool GetItemAbilityChance(ItemAbility* ability, int chance);
    inline bool FilterItemAbility(ItemAbility* ability, int abilityType = Invalid, int includeFlags = 0, int excludeFlags = 0);


    int GetEquipedItemAbilitiesCount();
    bool IsItemAbilityEquiped(int abilityId);
    inline ItemAbilityRecord* BuildItemAbilityRecord(oCItem* item, C_EnchantmentData* enchantment, ItemAbility* itemAbility, bool isFromWeapon, int value = Invalid, int chance = Invalid, int duration = Invalid);
    ItemAbilityRecord* GetOffenciveAbilityFromItem(oCItem* item);
    ItemAbilityRecord* SelectItemAbility(int abilityType, int includeFlags = 0, int excludeFlags = 0);
}
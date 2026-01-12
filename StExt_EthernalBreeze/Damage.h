//#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    const int dam_invalid = 0;
    const int dam_barrier = 1;
    const int dam_blunt = 2;
    const int dam_edge = 4;
    const int dam_fire = 8;
    const int dam_fly = 16;
    const int dam_magic = 32;
    const int dam_point = 64;
    const int dam_fall = 128;
    const int dam_poison = 5;

    const int dam_index_barrier = 0;
    const int dam_index_blunt = 1;
    const int dam_index_edge = 2;
    const int dam_index_fire = 3;
    const int dam_index_fly = 4;
    const int dam_index_magic = 5;
    const int dam_index_point = 6;
    const int dam_index_fall = 7;
    const int dam_index_max = 8;

    const int StExt_DamageType_Unknown = 0;
    const int StExt_DamageType_Melee = 1;
    const int StExt_DamageType_Range = 2;
    const int StExt_DamageType_Spell = 4;
    const int StExt_DamageType_Ability = 8;
    const int StExt_DamageType_Fire = 16;
    const int StExt_DamageType_Ice = 32;
    const int StExt_DamageType_Electric = 64;
    const int StExt_DamageType_Air = 128;
    const int StExt_DamageType_Earth = 256;
    const int StExt_DamageType_Light = 512;
    const int StExt_DamageType_Dark = 1024;
    const int StExt_DamageType_Death = 2048;
    const int StExt_DamageType_Life = 4096;
    const int StExt_DamageType_Poision = 8192;
    const int StExt_DamageType_True = 16384;

    const int StExt_DamageFlag_None = 0;
    const int StExt_DamageFlag_Aura = 1;
    const int StExt_DamageFlag_Dot = 2;
    const int StExt_DamageFlag_Aoe = 4;
    const int StExt_DamageFlag_Heal = 8;
    const int StExt_DamageFlag_Summon = 16;
    const int StExt_DamageFlag_Support = 32;
    const int StExt_DamageFlag_Unlock = 64;
    const int StExt_DamageFlag_Blink = 128;
    const int StExt_DamageFlag_Buff = 256;
    const int StExt_DamageFlag_Debuff = 512;
    const int StExt_DamageFlag_Timed = 1024;
    const int StExt_DamageFlag_Transform = 2048;
    const int StExt_DamageFlag_Golem = 4096;
    const int StExt_DamageFlag_Demon = 8192;
    const int StExt_DamageFlag_Undead = 16384;
    const int StExt_DamageFlag_Reflect = 32768;
    const int StExt_DamageFlag_Human = 65536;
    const int StExt_DamageFlag_Potion = 131072;
    const int StExt_DamageFlag_Orc = 262144;
    const int StExt_DamageFlag_Animal = 524288;
    const int StExt_DamageFlag_Target = 1048576;
    const int StExt_DamageFlag_Totem = 2097152;
    const int StExt_DamageFlag_Single = 4194304;
    const int StExt_DamageFlag_Freeze = 8388608;
    const int StExt_DamageFlag_Stun = 16777216;
    const int StExt_DamageFlag_Roots = 33554432;

    const int DamageDescFlag_ExtraDamage = 65536;
    const int DamageDescFlag_DotDamage = 131072;
    const int DamageDescFlag_ReflectDamage = 262144;
    const int DamageDescFlag_IsAbilityDamage = 524288;

    const int ThrowableVelocity = 2000;

    const int StExt_DamageMessageType_Default = 0;
    const int StExt_DamageMessageType_Dot = 2;
    const int StExt_DamageMessageType_Reflect = 4;

    const int StExt_IncomingDamageFlag_Index_None = 0;
    const int StExt_IncomingDamageFlag_Index_Processed = 1;
    const int StExt_IncomingDamageFlag_Index_HasAttacker = 2;
    const int StExt_IncomingDamageFlag_Index_HasWeapon = 4;
    const int StExt_IncomingDamageFlag_Index_ExtraDamage = 8;
    const int StExt_IncomingDamageFlag_Index_DontKill = 16;
    const int StExt_IncomingDamageFlag_Index_DotDamage = 32;
    const int StExt_IncomingDamageFlag_Index_ReflectDamage = 64;

    struct DamageInfo
    {
        int SpellId;
        int Damage[8];
        int DamageEffective[8];
        int TotalDamage;
        int RealDamage;
        int DamageEnum;
        int WeaponEnum;
        int DamageType;
        int DamageFlags;
        int BlockDamage;
        int StopProcess;
        int IsInitial;
    };

    struct ExtraDamageInfo
    {
        int Damage[8];
        int DotDamage[8];
        int Ticks[8];
        int TotalDamage;
        int DamageType;
        int DamageFlags;
        int MaxTargets;
    };

    struct DotDamageInfo
    {
        int Damage[8];
        int Ticks[8];
        int TotalDamage;
        int DamageType;
        int DamageFlags;
    };

    struct IncomingDamageInfo
    {
        struct
        {
            int Flags;
            int Damage[8];
            int DamageTotal;
            int DamageType;
            int DamageFlags;
            int Processed;
            int SpellId;
        }ScriptInstance;

        oCNpc* Attacker;
        oCNpc* Target;
        oCItem* Weapon;
        oCNpc::oSDamageDescriptor* Desc;
    };
    
    struct ThrowItemDescriptor
    {
        oCNpc* attacker;
        oCNpc* target;
        int throwInstance;
        int damage;
        int damageType;
    };

    struct DamageMetaData
    {
        unsigned long DescriptorFlags;
        bool IsInitial;
        bool IsExtraDamage;
        bool IsDot;
        bool IsAbility;
        bool IsDamageInfo;
        bool IsConditionValid;
        int FightMode;
        int AbilityId;
        int SpellId;
        int DamageType;
        int DamageFlags;
    };

    extern ExtraDamageInfo ExtraDamage;
    extern DotDamageInfo DotDamage;
    extern ExtraDamageInfo ReflectDamage;
    extern IncomingDamageInfo IncomingDamage;

    extern Array<zSTRING> SpellFxNames;

    //-----------------------------------------------------------------
    //							DAMAGE
    //-----------------------------------------------------------------

    void ApplyDamages(ULONG type, ULONG* damage, int& total);
    void ApplyDamages(int type, int* damage, int& total);
    void AddDamages(int type, int* damage, int& total);

    void ApplyExtraDamage(Gothic_II_Addon::oCNpc* atk, Gothic_II_Addon::oCNpc* target);
    void ApplyDotDamage(Gothic_II_Addon::oCNpc* atk, Gothic_II_Addon::oCNpc* target);
    void ApplyReflectDamage(Gothic_II_Addon::oCNpc* atk, Gothic_II_Addon::oCNpc* target);

    void ClearDamageMeta();
    void ThrowItem(ThrowItemDescriptor& itemDescriptor);
}
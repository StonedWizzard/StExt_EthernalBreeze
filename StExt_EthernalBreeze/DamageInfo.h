#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	struct CDamageInfo
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

    struct CExtraDamage
    {
        int Damage[8];
        int DotDamage[8];
        int Ticks[8];
        int TotalDamage;
        int DamageType;
        int DamageFlags;
        int MaxTargets;
    };

    struct CDotDamage
    {
        int Damage[8];
        int Ticks[8];
        int TotalDamage;
        int DamageType;
        int DamageFlags;
    };

    struct CIncomingDamage
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

    struct CDamageMeta
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
}
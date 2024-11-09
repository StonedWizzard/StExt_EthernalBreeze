#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    class C_AuraData
    {
    public:
        int Id;                 // Id of aura
        int Flags;              // Flags
        int ReservedAtr;        // Index of reserved attribute
        int ReservedValue;      // Initial value for reservation
        int Duration;           // Initial duration
        int Chance;             // Base chance to apply effect
        int PowerRatio;         // Moddifier for applied aura power
        int CondAtr[3];         // Index of required stat to equipment
        int CondValue[3];       // Required stat amount
        int StatId[5];          // Index of applied effect. Used for stats system only
        int StatValue[5];       // Initial effect values
        int MasteryId[3];       // Bounded masteries
        zSTRING OnOffence;      // Invoke func on do damage
        zSTRING OnDeffence;     // Invoke func on get damage
        zSTRING OnTick;         // Invoke func on get damage
        zSTRING BoundedItem;    // Item instnce, bounded to this aura
        zSTRING Effect;			// SpellFx effect on activation

        void SetByScript(int index);
    };
}
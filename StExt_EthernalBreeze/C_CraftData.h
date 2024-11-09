#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    struct CraftData
    {
        int Flags;
        int Price;
        int ExpBonus;
        int CraftBonus;
        int Count;
        int IngredientId[16];
        int IngredientCount[16];
        zSTRING ConditionFunc;
        zSTRING ResultInstance;
    };
}

#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	const int EnchantAbilityMax = 4;
	const int EnchantStatsMax = 16;
	const int EnchantConditionsMax = 3;

	class  C_EnchantmentData
	{
	public:
		int UId;

		zSTRING Preffix;
		zSTRING Affix;
		zSTRING Suffix;

		int Type;
		int Flags;
		int Level;
		int Rank;
		int SocketsUsed;
		int SocketsMax;
		int Cost;		

		int AbilityId[EnchantAbilityMax];
		int AbilityChance[EnchantAbilityMax];
		int AbilityValue[EnchantAbilityMax];
		int AbilityDuration[EnchantAbilityMax];

		int StatId[EnchantStatsMax];
		int StatValue[EnchantStatsMax];
		int StatDuration[EnchantStatsMax];

		// Item modifiers
		int ExtraFlags;
		int AdditionalFlags;
		int ExtraMainflag;

		int CondAtr[EnchantConditionsMax];
		int CondValue[EnchantConditionsMax];

		int DamageTypes;
		int DamageTotal;	
		int Damage[oEDamageIndex_MAX];
		int Protection[oEDamageIndex_MAX];
		int Range;

		zSTRING VisualEffect;

		C_EnchantmentData();
		void Archive(zCArchiver& arc);
		void Unarchive(zCArchiver& arc);
	};
}
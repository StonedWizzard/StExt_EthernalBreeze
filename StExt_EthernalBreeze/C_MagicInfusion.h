#include <UnionAfx.h>

namespace Gothic_II_Addon
{
    class C_MagicInfusionData
    {
	public:
		// System values
		int Id;
		int Tier;
		int AffixId;
		int SuffixId;
		int PreffixId;
		// Npc mods
		int NpcFlags;
		float Size;
		int SpeedMod;
		int ExtraLevel;
		int ExtraLevelMult;
		zSTRING Effect;
		// Attribute mods
		int ExtraHp;
		int ExtraEs;
		int ExtraStr;
		int ExtraAgi;
		int ExtraHpMult;
		int ExtraEsMult;
		int ExtraStrMult;
		int ExtraAgiMult;
		// Damage mods
		int ExtraBluntDam;
		int ExtraEdgeDam;
		int ExtraFireDam;
		int ExtraFlyDam;
		int ExtraMagicDam;
		int ExtraPointDam;
		int ExtraPoisonDam;
		int ExtraHitchances;
		// Protection mods
		int ExtraDodgeChance;
		int ExtraBluntProt;
		int ExtraEdgeProt;
		int ExtraFireProt;
		int ExtraFlyProt;
		int ExtraMagicProt;
		int ExtraPointProt;
		int ExtraProtAllMult;
		int ResistEdge;
		int ResistBlunt;
		int ResistPoint;
		int ResistMagic;
		int ResistFire;
		int ResistFly;
		int ResistPois;		
		// Abilities
		int LowAbilityId;
		int MedAbilityId;
		int TopAbilityId;
		int SummonAbilityId;
		int PassiveAbilityId;
		int BuffAbilityId;
		int DebuffAbilityId;
		int Abilities[10];
		int LuckBonus;
		int GoldBonus;

		void SetByScript(int index);
    };
}
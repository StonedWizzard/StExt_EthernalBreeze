//#include <UnionAfx.h>

namespace Gothic_II_Addon
{
	//---------------------------------------------------------------------------
	//								CONSTANTS
	//---------------------------------------------------------------------------

	const int EmptyNpcUId = Invalid;
	//const zSTRING zString_Empty = "";
	const zSTRING zString_Unknown = "???";
	const zSTRING ModSymbolPrefix = "StExt_";
	const zSTRING ModSymbolNamePrefix = "StExt_Str_";

	const int StExt_InfusionType_Affix = 0;
	const int StExt_InfusionType_Preffix = 1;
	const int StExt_InfusionType_Suffix = 2;

	const int StExt_Corruption_Mage = 1;
	const int StExt_Corruption_Warrior = 2;
	const int StExt_Corruption_Ranger = 3;

	const int StExt_ArtifactIndex_None = 0;
	const int StExt_ArtifactIndex_Grimoir = 1;
	const int StExt_ArtifactIndex_Dagger = 2;
	const int StExt_ArtifactIndex_Max = 3;

	const int StatGroup_Skills = 0;
	const int StatGroup_Protection = 1;
	const int StatGroup_Attack = 2;
	const int StatGroup_Auras = 3;
	const int StatGroup_Summons = 4;
	const int StatGroup_Other = 5;
	const int StatGroup_Characteristics = 6;

	const int NpcExtension_StatsMax = 256;

	const int StExt_NpcAbility_Type_Any = Invalid;
	const int StExt_NpcAbility_Type_Passive = 0;
	const int StExt_NpcAbility_Type_Spell = 1;
	const int StExt_NpcAbility_Type_Throw = 2;
	const int StExt_NpcAbility_Type_Debuff = 4;
	const int StExt_NpcAbility_Type_Buff = 8;
	const int StExt_NpcAbility_Type_Summon = 16;
	const int StExt_NpcAbility_Type_Offence = 32;
	const int StExt_NpcAbility_Type_Deffence = 64;

	const int StExt_NpcAbility_Flag_None = 0;
	const int StExt_NpcAbility_Flag_OnTick = 1;
	const int StExt_NpcAbility_Flag_OnCast = 2;
	const int StExt_NpcAbility_Flag_OnOffense = 4;
	const int StExt_NpcAbility_Flag_OnDeffense = 8;
	const int StExt_NpcAbility_Flag_OnDeath = 16;
	const int StExt_NpcAbility_Flag_Target = 32;
	const int StExt_NpcAbility_Flag_Aoe = 64;
	const int StExt_NpcAbility_Flag_UseSummonPack = 128;
	const int StExt_NpcAbility_Flag_UseDistance = 256;
	const int StExt_NpcAbility_Flag_UseCooldown = 512;
	const int StExt_NpcAbility_Flag_UseSpell = 1024;

constexpr auto ConfigsExportTemplate = R"(META
{
    After = ConfigsPresets.d;
    Mod = EthernalBreeze.vdf;
};

instance [ConfigName](C_ConfigPreset)
{
    Name = "[ConfigName]";
    Text = "[ConfigText]";
    TextColor = StExt_Color_Header;
    OnApply = "[ConfigApplyFunc]";
};

func event StExt_Evt_OnModLoaded() { StExt_RegistrateConfigsPreset("[ConfigName]"); };	

func void [ConfigApplyFunc]() 
{
[ConfigsList]
};
)";

}

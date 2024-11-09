#include <UnionAfx.h>
#include <StonedExtension.h>
#include <algorithm>

namespace Gothic_II_Addon
{
	C_MagicInfusionData* CreateInfusion(int tier)
	{
        C_MagicInfusionData* prefix = GetRandomInfusion(StExt_InfusionType_Preffix, tier, false);
        C_MagicInfusionData* affix = GetRandomInfusion(StExt_InfusionType_Affix, tier, false);
        C_MagicInfusionData* suffix = GetRandomInfusion(StExt_InfusionType_Suffix, tier, false);

        if (!suffix && !affix && !prefix)
        {
            if((rand() % 100) >= 50) prefix = GetRandomInfusion(StExt_InfusionType_Preffix, tier, true);
            else suffix = GetRandomInfusion(StExt_InfusionType_Suffix, tier, true);
        }
        Array<C_MagicInfusionData*> infusionData = Array<C_MagicInfusionData*>();
        infusionData.Insert(prefix);
        infusionData.Insert(affix);
        infusionData.Insert(suffix);

        C_MagicInfusionData* result = new C_MagicInfusionData();
        result->Tier = tier;
        result->PreffixId = prefix ? prefix->Id : -1;
        result->AffixId = affix ? affix->Id : -1;
        result->SuffixId = suffix ? suffix->Id : -1;
        result->Effect = "";
        for (int i = 0; i < 10; i++)
            result->Abilities[i] = -1;

        Array<zSTRING> effectsData = Array<zSTRING>();
        Array<int> abilitiesData = Array<int>();
        for (unsigned int i = 0; i < infusionData.GetNum(); i++)
        {
            if (!infusionData[i]) continue;

            // Npc mods
            result->NpcFlags |= infusionData[i]->NpcFlags;
            result->Size += infusionData[i]->Size;
            result->SpeedMod += infusionData[i]->SpeedMod;
            result->ExtraLevel += infusionData[i]->ExtraLevel;
            result->ExtraLevelMult += infusionData[i]->ExtraLevelMult;

            // Attribute mods
            result->ExtraHp += infusionData[i]->ExtraHp;
            result->ExtraEs += infusionData[i]->ExtraEs;
            result->ExtraStr += infusionData[i]->ExtraStr;
            result->ExtraAgi += infusionData[i]->ExtraAgi;
            result->ExtraHpMult += infusionData[i]->ExtraHpMult;
            result->ExtraEsMult += infusionData[i]->ExtraEsMult;
            result->ExtraStrMult += infusionData[i]->ExtraStrMult;
            result->ExtraAgiMult += infusionData[i]->ExtraAgiMult;

            // Damage mods
            result->ExtraBluntDam += infusionData[i]->ExtraBluntDam;
            result->ExtraEdgeDam += infusionData[i]->ExtraEdgeDam;
            result->ExtraFireDam += infusionData[i]->ExtraFireDam;
            result->ExtraFlyDam += infusionData[i]->ExtraFlyDam;
            result->ExtraMagicDam += infusionData[i]->ExtraMagicDam;
            result->ExtraPointDam += infusionData[i]->ExtraPointDam;
            result->ExtraHitchances += infusionData[i]->ExtraHitchances;

            // Protection mods
            result->ExtraDodgeChance += infusionData[i]->ExtraDodgeChance;
            result->ExtraBluntProt += infusionData[i]->ExtraBluntProt;
            result->ExtraEdgeProt += infusionData[i]->ExtraEdgeProt;
            result->ExtraFireProt += infusionData[i]->ExtraFireProt;
            result->ExtraFlyProt += infusionData[i]->ExtraFlyProt;
            result->ExtraMagicProt += infusionData[i]->ExtraMagicProt;
            result->ExtraPointProt += infusionData[i]->ExtraPointProt;
            result->ExtraProtAllMult += infusionData[i]->ExtraProtAllMult;
            result->ResistEdge += infusionData[i]->ResistEdge;
            result->ResistBlunt += infusionData[i]->ResistBlunt;
            result->ResistPoint += infusionData[i]->ResistPoint;
            result->ResistMagic += infusionData[i]->ResistMagic;
            result->ResistFire += infusionData[i]->ResistFire;
            result->ResistFly += infusionData[i]->ResistFly;
            result->ResistPois += infusionData[i]->ResistPois;

            result->GoldBonus += infusionData[i]->GoldBonus;
            result->LuckBonus += infusionData[i]->LuckBonus;
            // Visual effects
            if(!infusionData[i]->Effect.IsEmpty() && (infusionData[i]->Effect != ""))
                effectsData.Insert(infusionData[i]->Effect);

            // Abilities
            if (!abilitiesData.HasEqual(infusionData[i]->PassiveAbilityId) && (infusionData[i]->PassiveAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->PassiveAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->LowAbilityId) && (infusionData[i]->LowAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->LowAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->MedAbilityId) && (tier >= 2) && (infusionData[i]->MedAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->MedAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->TopAbilityId) && (tier >= 3) && (infusionData[i]->TopAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->TopAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->SummonAbilityId) && (tier >= 2) && (infusionData[i]->SummonAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->SummonAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->BuffAbilityId) && (infusionData[i]->BuffAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->BuffAbilityId);
            if (!abilitiesData.HasEqual(infusionData[i]->DebuffAbilityId) && (infusionData[i]->DebuffAbilityId > 0))
                abilitiesData.Insert(infusionData[i]->DebuffAbilityId);
        }

        // Visual effects selection
        if (effectsData.GetNum() > 0)
            result->Effect = effectsData[rand() % effectsData.GetNum()];

        int abilityNum = abilitiesData.GetNum();
        if (abilityNum > 10)
            std::random_shuffle(abilitiesData.begin(), abilitiesData.end());   
        
        for (int i = 0; i < 10; i++)
        {
            if ((abilityNum == 0) || (i >= abilityNum))
                result->Abilities[i] = -1;
            else
                result->Abilities[i] = abilitiesData[i];
        }

        // Model scale
        result->Size += 1.0;
        if (result->Size < 0.75) result->Size = 0.75;
        if (result->Size > 2.00) result->Size = 2.00;
		return result;
	}
}
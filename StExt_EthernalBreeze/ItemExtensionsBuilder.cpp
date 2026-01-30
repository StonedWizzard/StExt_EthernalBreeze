#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    inline unsigned int GetNextItemUId()
    {
        if (ItemExtensionsState.NextItemExtensionUId == 0U || ItemExtensionsState.NextItemExtensionUId == Invalid) ItemExtensionsState.NextItemExtensionUId = 1U;
        uint attempts = 0U;
        while (GetItemExtension(ItemExtensionsState.NextItemExtensionUId)) 
        { 
            ++ItemExtensionsState.NextItemExtensionUId;
            ++attempts;
            if (attempts == 1024U)
                DEBUG_MSG("GetNextItemUId: search index...");
        }
        return ItemExtensionsState.NextItemExtensionUId;
    }

    inline const zSTRING CreateExtendedItemInstanceName(const int extensionId)
    {
        zSTRING result = GenerateItemPrefix + FormatNumberPad(extensionId, 10) + "_" + StExt_Rand::RandomString(5);
        return NormalizeInstanceName(result);
    }

    ItemClassKey ParseItemClassKey(const oCItem* item)
    {
        if (!item) return ItemClassKey_Empty;

        byte itemType = (byte)ItemType::Unknown;
        byte itemClass = (byte)ItemClass::Unknown;
        byte itemSubClass = (byte)ItemSubClass::Unknown;
        const int mainFlags = item->mainflag;
        const int extraFlags = item->flags;
        const int additionalFlags = item->hitp;
        const int wearFlags = item->wear;
        const int weight = item->weight;

        if (HasFlag(mainFlags, item_kat_armor))
        {
            itemType = (byte)ItemType::Armor;
            if (HasFlag(wearFlags, wear_torso))
            {
                itemClass = (byte)ItemClass::FullArmor;
                if (weight <= 0) itemSubClass = (byte)ItemSubClass::Clothing;
                else if (weight <= 3) itemSubClass = (byte)ItemSubClass::LightArmor;
                else if (weight <= 5) itemSubClass = (byte)ItemSubClass::MediumArmor;
                else if (weight > 5) itemSubClass = (byte)ItemSubClass::HeavyArmor;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }

            else if (HasFlag(wearFlags, wear_head))
            {
                itemClass = (byte)ItemClass::Helm;
                if (weight <= 0) itemSubClass = (byte)ItemSubClass::LightHelm;
                else if (weight <= 1) itemSubClass = (byte)ItemSubClass::MediumHelm;
                else if (weight > 1) itemSubClass = (byte)ItemSubClass::HeavyHelm;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            else if (HasFlag(wearFlags, wear_gloves)) itemClass = (byte)ItemClass::Gloves;
            else if (HasFlag(wearFlags, wear_boots)) itemClass = (byte)ItemClass::Boots;
            else if (HasFlag(wearFlags, wear_pants)) itemClass = (byte)ItemClass::Pants;
            else if (HasFlag(wearFlags, wear_breast)) itemClass = (byte)ItemClass::Torso;
            else
            { 
                DEBUG_MSG("ParseItemClassKey: fail to parse item '" + GetItemInstanceName(item) + "' as armor!"); 
            }

            if (weight <= 0) itemSubClass = (byte)ItemSubClass::LightArmor;
            else if (weight == 1) itemSubClass = (byte)ItemSubClass::MediumArmor;
            else if (weight > 1) itemSubClass = (byte)ItemSubClass::HeavyArmor;
            return ItemClassKey_Create(itemType, itemClass, itemSubClass);
        }

        else if (HasFlag(mainFlags, item_kat_magic))
        {
            itemType = (byte)ItemType::Jewelry;

            if (HasFlag(extraFlags, item_amulet))
            {
                itemClass = (byte)ItemClass::Amulet;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(extraFlags, item_ring))
            {
                itemClass = (byte)ItemClass::Ring;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(extraFlags, item_belt))
            {
                itemClass = (byte)ItemClass::Belt;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(extraFlags, item_trophy))
            {
                itemClass = (byte)ItemClass::Trophy;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            DEBUG_MSG("ParseItemClassKey: fail to parse item '" + GetItemInstanceName(item) + "' as jewelry!");
        }

        else if (HasFlag(mainFlags, item_kat_nf))
        {
            if (HasFlag(extraFlags, item_shield))
            {
                itemType = (byte)ItemType::Armor;
                itemClass = (byte)ItemClass::Shield;
                itemSubClass = (weight <= 0) ? (byte)ItemSubClass::LightShield : (byte)ItemSubClass::HeavyShield;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }

            itemType = (byte)ItemType::Weapon;
            itemClass = (byte)ItemClass::MeeleWeapon;

            if (HasFlag(additionalFlags, bit_item_mag_sword))
            {
                itemSubClass = (byte)ItemSubClass::MagicSword;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(additionalFlags, bit_item_dex_sword))
            {
                itemSubClass = (byte)ItemSubClass::DexSword;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(additionalFlags, bit_item_orc_weapon))
            {
                itemSubClass = (byte)ItemSubClass::OrcWeapon;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(additionalFlags, bit_item_hellebarde))
            {
                itemSubClass = (byte)ItemSubClass::Halleberd;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(additionalFlags, bit_item_speer))
            {
                itemSubClass = (byte)ItemSubClass::Spear;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(additionalFlags, bit_item_pierce_damage))
            {
                itemSubClass = (byte)ItemSubClass::Rapier;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }

            if (HasFlag(extraFlags, item_dag))
            {
                itemSubClass = (byte)ItemSubClass::DualR;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(extraFlags, item_throw))
            {
                itemSubClass = (byte)ItemSubClass::DualL;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            
            if (HasFlag(extraFlags, item_swd))
            {
                itemSubClass = (byte)ItemSubClass::Sword1H;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(extraFlags, item_axe))
            {
                itemSubClass = (byte)ItemSubClass::Axe1H;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            
            if (HasFlag(extraFlags, item_2hd_swd))
            {
                itemSubClass = (byte)ItemSubClass::Sword2H;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(extraFlags, item_2hd_axe))
            {
                itemSubClass = (byte)ItemSubClass::Axe2H;
                if (item->cond_atr[2] == 3) itemSubClass = (byte)ItemSubClass::Staff;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }

            DEBUG_MSG("ParseItemClassKey: fail to parse item '" + GetItemInstanceName(item) + "' as meele weapon!");
        }

        else if (HasFlag(mainFlags, item_kat_ff))
        {
            itemType = (byte)ItemType::Weapon;
            itemClass = (byte)ItemClass::RangeWeapon;

            if (HasFlag(extraFlags, item_bow))
            {
                itemSubClass = (item->damageTotal >= 100) ? (byte)ItemSubClass::HeavyBow : (byte)ItemSubClass::LightBow;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }
            if (HasFlag(extraFlags, item_crossbow))
            {
                itemSubClass = (item->damageTotal >= 100) ? (byte)ItemSubClass::HeavyCrossBow : (byte)ItemSubClass::LightCrossBow;
                return ItemClassKey_Create(itemType, itemClass, itemSubClass);
            }

            DEBUG_MSG("ParseItemClassKey: fail to parse item '" + GetItemInstanceName(item) + "' as range weapon!");
        }

        else if (HasFlag(mainFlags, item_kat_rune))
        {
            itemType = (byte)ItemType::Weapon;
            itemClass = (byte)ItemClass::CastWeapon;
            if (HasFlag(extraFlags, item_multi)) itemSubClass = (byte)ItemSubClass::Scroll;
            else if (false) itemSubClass = (byte)ItemSubClass::Wand;
            else itemSubClass = (byte)ItemSubClass::Rune;
            return ItemClassKey_Create(itemType, itemClass, itemSubClass);
        }

        else if (HasFlag(mainFlags, item_kat_mun))
        {
            itemType = (byte)ItemType::Munition;
            return ItemClassKey_Create(itemType, itemClass, itemSubClass);
        }

        else if (HasFlag(mainFlags, item_kat_potions))
        {
            itemType = (byte)ItemType::Consumable;
            itemClass = (byte)ItemClass::Potion;
            return ItemClassKey_Create(itemType, itemClass, itemSubClass);
        }

        else if (HasFlag(mainFlags, item_kat_food))
        {
            itemType = (byte)ItemType::Consumable;
            itemClass = (byte)ItemClass::Food;
            return ItemClassKey_Create(itemType, itemClass, itemSubClass);
        }

        else if (HasFlag(mainFlags, item_kat_docs))
        {
            //itemType = (byte)ItemType::Weapon;

        }

        DEBUG_MSG("ParseItemClassKey: fail to parse item '" + GetItemInstanceName(item) + "' class!");
        return ItemClassKey_Create(itemType, itemClass, itemSubClass);
    }

    ItemClassKey ParseItemClassKey(const zSTRING& instanceName) 
    { 
        ItemClassKey result = ItemClassKey_Empty;
        oCItem* item = new oCItem();
        if (item && parser->SetInstance(parser->GetIndex(instanceName), item))
            result = ParseItemClassKey(item);
        item->Release();
        return result;
    }

    zSTRING RollPrototypeInstanceName(const int tier, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return "";

        const Array<zSTRING>* arrayRef = Null;
        if (tier >= 3 && itemClassDescriptor->HasExtraTier) arrayRef = &itemClassDescriptor->ExtraTierPrototypes;
        else if (tier >= 2) arrayRef = &itemClassDescriptor->TopTierPrototypes;
        else if (tier >= 1) arrayRef = &itemClassDescriptor->MedTierPrototypes;
        else arrayRef = &itemClassDescriptor->LowTierPrototypes;

        if (!arrayRef || arrayRef->GetNum() == 0) return "";
        return (*arrayRef)[StExt_Rand::Index(arrayRef->GetNum())];
    }

    inline int CalcItemTier(const int power, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (power <= 0 || !itemClassDescriptor || itemClassDescriptor->TierStep <= 0) return 0;

        const int maxTier = itemClassDescriptor->HasExtraTier ? 3 : 2;
        int tier = static_cast<int>(power / itemClassDescriptor->TierStep);
        return ValidateValue(tier, 0, maxTier);
    }


    inline int RollItemLevel(const int power)
    {
        if (power <= 0) return 1;
        int level = static_cast<int>(power * ItemsGeneratorConfigs.ItemLevelPowerRatio);
        level += static_cast<int>(StExt_Rand::SymmetricRange(ItemsGeneratorConfigs.ItemLevelSpread) * level);
        return ValidateValue(level, 1, ItemsGeneratorConfigs.ItemLevelMax);
    }

    inline int RollItemRank(const int power)
    {
        if (power <= 0 || ItemsGeneratorConfigs.ItemRankMax <= 0) return 0;

        int minRank = 0;
        int maxRank = 0;
        for (int i = 0; i < ItemsGeneratorConfigs.ItemRankMax; ++i)
        {
            if (power >= ItemsGeneratorConfigs.ItemRankThresholds[i]) maxRank = i;
            else break;
        }
        minRank = ValidateValueMin(maxRank - 1, 0);
        return StExt_Rand::Percent(10 + (minRank * 2) + (maxRank * maxRank) * 2) ? minRank : maxRank;
    }

    inline int RollItemQuality(const int power, const int rank)
    {
        if (power <= 0) return 0;

        const int minCap = static_cast<int>(power * ItemsGeneratorConfigs.ItemQualityPowerRatio);
        const int maxCap = static_cast<int>(minCap * 2.0f);

        return ValidateValue(StExt_Rand::Range(minCap, maxCap), 0, rank >= ItemsGeneratorConfigs.ItemRankForExtraQualityThreshold ?
            ItemsGeneratorConfigs.ItemQualityHardCap : ItemsGeneratorConfigs.ItemQualitySoftCap);
    }

    inline int RollItemMaxSockets(const int level, const int rank, const int quality, const int extraSocket, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;

        const float classBonus = (level * itemClassDescriptor->SocketsLevelBonus) + (rank * itemClassDescriptor->SocketsRankBonus) + (quality * itemClassDescriptor->SocketsQualityBonus);
        const int socketsMax = static_cast<int>((itemClassDescriptor->SocketsMax + extraSocket + classBonus) * ItemsGeneratorConfigs.ItemSocketsCountMult);
        const int socketsMin = static_cast<int>((quality * itemClassDescriptor->SocketsQualityBonus) + (socketsMax * 0.1f) + (extraSocket * 0.5f));
        return ValidateValue(StExt_Rand::Range(socketsMin, socketsMax), 0, itemClassDescriptor->SocketsCap);
    }

    inline int RollItemRange(const int baseRange, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return baseRange;
        const float classBonus = quality * itemClassDescriptor->RangeQualityBonus;
        const float range = (baseRange + classBonus + (baseRange * StExt_Rand::SymmetricRange(itemClassDescriptor->RangeSpread)));

        const int valueMin = static_cast<int>(range * ItemsGeneratorConfigs.ItemRangeMinCap);
        const int valueMax = static_cast<int>(range * ItemsGeneratorConfigs.ItemRangeMaxCap);
        return ValidateValue(static_cast<int>(range), valueMin, valueMax);
    }

    
    inline int RollItemExtraConditionsCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;

        int conditionsCount = 0;
        const float classBonus = (level * itemClassDescriptor->ExtraConditionRollChanceLevelBonus) + (rank * itemClassDescriptor->ExtraConditionRollChanceRankBonus) + (quality * itemClassDescriptor->ExtraConditionRollChanceQualityBonus);
        const int rollChance = static_cast<int>((itemClassDescriptor->ExtraConditionRollChance + classBonus) * ItemsGeneratorConfigs.ItemExtraConditionRollChanceMult);
        
        conditionsCount += StExt_Rand::Permille(rollChance) ? 1 : 0;
        conditionsCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.5f)) ? 1 : 0;
        return conditionsCount;
    }

    inline int RollItemExtraConditionId(const Array<int>& baseConditions, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor || itemClassDescriptor->ConditionsList.IsEmpty()) return 0;

        Array<int> buffer = Array<int>();
        for (const auto condId : itemClassDescriptor->ConditionsList)
        {
            const ExtraStatData* condData = GetExtraConditionDataById(condId);
            if (!condData) continue;
            if (baseConditions.HasEqual(condId)) continue;

            if (StExt_Rand::Permille(condData->RollChance))
                buffer.InsertEnd(condId);
        }

        if (buffer.IsEmpty())
            return itemClassDescriptor->ConditionsList[StExt_Rand::Index(itemClassDescriptor->ConditionsList.GetNum())];

        return buffer[StExt_Rand::Index(buffer.GetNum())];
    }

    inline int RollItemExtraConditionValue(const ExtraStatData* condData, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!condData || !itemClassDescriptor) return 0;

        const float classBonus = power + ((level * itemClassDescriptor->ExtraConditionLevelBonus) + (rank * itemClassDescriptor->ExtraConditionRankBonus) + (quality * itemClassDescriptor->ExtraConditionQualityBonus));
        const int valueMin = static_cast<int>(((condData->RollMinPower * classBonus) * itemClassDescriptor->ExtraConditionMult) * ItemsGeneratorConfigs.ItemConditionValueMult);
        const int valueMax = static_cast<int>(((condData->RollMaxPower * classBonus) * itemClassDescriptor->ExtraConditionMult) * ItemsGeneratorConfigs.ItemConditionValueMult);
        return ValidateValue(StExt_Rand::Range(valueMin, valueMax), condData->MinCap, condData->MaxCap);
    }
    inline int RollItemExtraConditionValueById(const int condDataId, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (condDataId == 0) return 0;
        const ExtraStatData* condData = GetExtraConditionDataById(condDataId);
        return RollItemExtraConditionValue(condData, power, level, rank, quality, itemClassDescriptor);
    }


    inline int RollItemExtraProtectionsCount(const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        const int rollChance = static_cast<int>((itemClassDescriptor->ExtraProtectionRollChance + 
            (quality * itemClassDescriptor->ExtraProtectionRollChanceQualityBonus)) * ItemsGeneratorConfigs.ItemExtraProtectionRollChanceMult);
        
        int protCount = StExt_Rand::Permille(rollChance) ? 1 : 0;
        protCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.66f)) ? 1 : 0;
        protCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.33f)) ? 1 : 0;
        return protCount;
    }

    inline int RollItemExtraProtectionValue(const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;

        const int valueMin = static_cast<int>(((quality * itemClassDescriptor->ExtraProtectionMinQualityBonus) * itemClassDescriptor->ExtraProtectionQualityMult) * ItemsGeneratorConfigs.ItemExtraProtectionMult);
        const int valueMax = static_cast<int>(((quality * itemClassDescriptor->ExtraProtectionMaxQualityBonus) * itemClassDescriptor->ExtraProtectionQualityMult) * ItemsGeneratorConfigs.ItemExtraProtectionMult);
        return ValidateValue(StExt_Rand::Range(valueMin, valueMax), 0, valueMax);
    }

    inline int RollItemSpecialProtectionsCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        int protectionsCount = 0;

        const float classBonus = (level * itemClassDescriptor->SpecialProtectionRollChanceLevelBonus) + (rank * itemClassDescriptor->SpecialProtectionRollChanceRankBonus) + (quality * itemClassDescriptor->SpecialProtectionRollChanceQualityBonus);
        const int rollChance = static_cast<int>((itemClassDescriptor->SpecialProtectionRollChance + classBonus) * ItemsGeneratorConfigs.ItemSpecialProtectionRollChanceMult);

        protectionsCount += StExt_Rand::Permille(rollChance) ? 1 : 0;
        protectionsCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.75f)) ? 1 : 0;
        protectionsCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.50f)) ? 1 : 0;
        protectionsCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.25f)) ? 1 : 0;
        return protectionsCount;
    }

    inline int RollItemSpecialProtectionValue(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        const float classBonus = (level * itemClassDescriptor->SpecialProtectionLevelBonus) + (rank * itemClassDescriptor->SpecialProtectionRankBonus) + (quality * itemClassDescriptor->SpecialProtectionQualityBonus);
        const int valueMin = static_cast<int>(((classBonus * 0.25f) * itemClassDescriptor->SpecialProtectionMult) * ItemsGeneratorConfigs.ItemSpecialProtectionMult);
        const int valueMax = static_cast<int>((classBonus * itemClassDescriptor->SpecialProtectionMult) * ItemsGeneratorConfigs.ItemSpecialProtectionMult);
        return ValidateValue(StExt_Rand::Range(valueMin, valueMax), 0, valueMax);
    }


    inline int RollItemExtraDamagesCount(const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        const int rollChance = static_cast<int>((itemClassDescriptor->ExtraDamageRollChance +
            (quality * itemClassDescriptor->ExtraDamageRollChanceQualityBonus)) * ItemsGeneratorConfigs.ItemExtraDamageRollChanceMult);

        int damCount = StExt_Rand::Permille(rollChance) ? 1 : 0;
        damCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.66f)) ? 1 : 0;
        damCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.33f)) ? 1 : 0;
        return damCount;
    }

    inline int RollItemExtraDamageValue(const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        const int valueMin = static_cast<int>(((quality * itemClassDescriptor->ExtraDamageMinQualityBonus) * itemClassDescriptor->ExtraDamageQualityMult) * ItemsGeneratorConfigs.ItemExtraDamageMult);
        const int valueMax = static_cast<int>(((quality * itemClassDescriptor->ExtraDamageMaxQualityBonus) * itemClassDescriptor->ExtraDamageQualityMult) * ItemsGeneratorConfigs.ItemExtraDamageMult);
        return ValidateValue(StExt_Rand::Range(valueMin, valueMax), 0, valueMax);
    }

    inline int RollItemSpecialDamagesCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        int damCount = 0;

        const float classBonus = (level * itemClassDescriptor->SpecialDamageRollChanceLevelBonus) + (rank * itemClassDescriptor->SpecialDamageRollChanceRankBonus) + (quality * itemClassDescriptor->SpecialDamageRollChanceQualityBonus);
        const int rollChance = static_cast<int>((itemClassDescriptor->SpecialDamageRollChance + classBonus) * ItemsGeneratorConfigs.ItemSpecialDamageRollChanceMult);

        damCount += StExt_Rand::Permille(rollChance) ? 1 : 0;
        damCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.75f)) ? 1 : 0;
        damCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.50f)) ? 1 : 0;
        damCount += StExt_Rand::Permille(static_cast<int>(rollChance * 0.25f)) ? 1 : 0;
        return damCount;
    }

    inline int RollItemSpecialDamageValue(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        const float classBonus = (level * itemClassDescriptor->SpecialDamageLevelBonus) + (rank * itemClassDescriptor->SpecialDamageRankBonus) + (quality * itemClassDescriptor->SpecialDamageQualityBonus);
        const int valueMin = static_cast<int>(((classBonus * 0.25f) * itemClassDescriptor->SpecialDamageMult) * ItemsGeneratorConfigs.ItemSpecialDamageMult);
        const int valueMax = static_cast<int>((classBonus * itemClassDescriptor->SpecialDamageMult) * ItemsGeneratorConfigs.ItemSpecialDamageMult);
        return ValidateValue(StExt_Rand::Range(valueMin, valueMax), 0, valueMax);
    }

    inline int RollItemStatsCount(int& minStatsCount, int& maxStatsCount, const int extraStatsCount, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;

        const float classBonusMax = (level * itemClassDescriptor->StatsCountMaxLevelBonus) + (rank * itemClassDescriptor->StatsCountMaxRankBonus) + (quality * itemClassDescriptor->StatsCountMaxQualityBonus);
        const float classBonusMin = (level * itemClassDescriptor->StatsCountMinLevelBonus) + (rank * itemClassDescriptor->StatsCountMinRankBonus) + (quality * itemClassDescriptor->StatsCountMinQualityBonus);

        maxStatsCount = static_cast<int>(itemClassDescriptor->StatsCountMax + classBonusMax);
        minStatsCount = static_cast<int>(itemClassDescriptor->StatsCountMin + classBonusMin);
        minStatsCount = ValidateValue(minStatsCount, itemClassDescriptor->StatsCountMin, static_cast<int>(itemClassDescriptor->StatsCountCap * 0.5f));
        maxStatsCount = ValidateValue(maxStatsCount, minStatsCount, itemClassDescriptor->StatsCountCap);

        int result = static_cast<int>((StExt_Rand::Range(minStatsCount, maxStatsCount) + extraStatsCount) * ItemsGeneratorConfigs.ItemStatsCountMult);
        return ValidateValue(result, extraStatsCount, itemClassDescriptor->StatsCountCap);
    }

    inline void RollItemStatsChunk(Array<int>& statsBuffer, const Array<int>& currentStats, const int statsCount, const ItemClassDescriptor* itemClassDescriptor)
    {
        Map<int, int> statsPool;
        for (const auto& statKV : ExtraStatsData)
        {
            const auto& stat = statKV.GetValue();
            if (currentStats.HasEqualSorted(stat.Id)) continue;
            if (itemClassDescriptor->IncopatibleStatsList.HasEqualSorted(stat.Id)) continue;

            int chance = static_cast<int>(stat.RollChance * ItemsGeneratorConfigs.ItemStatRollChanceMult);
            if (itemClassDescriptor->PrimaryStatsList.HasEqualSorted(stat.Id))
                chance = static_cast<int>(chance * ItemsGeneratorConfigs.ItemPrimaryStatRollChanceMult);

            chance = ValidateValue(chance, 0, 999);
            if (chance <= 0) continue;
            statsPool.Insert(stat.Id, chance);
        }

        const uint statsInPool = statsPool.GetNum();
        if (statsInPool == 0U) return;

        Array<int> rollPool = Array<int>();
        int picked = 0, breaker = 0;
        do 
        {
            for (auto& statKV : statsPool)
            {
                if (statKV.GetValue() < 0) continue;
                if (StExt_Rand::Permille(statKV.GetValue()))
                {
                    rollPool.Insert(statKV.GetKey());
                    statKV.GetValue() = Invalid;
                    ++picked;
                }
            }
            ++breaker;
        } 
        while ((picked < statsCount) && (breaker < 4));
        if (rollPool.IsEmpty()) return;
       
        StExt_Rand::ShuffleArray(rollPool);
        uint max = ValidateValueMax(static_cast<uint>(statsCount), rollPool.GetNum());
        for (uint i = 0; i < max; ++i)
            statsBuffer.Insert(rollPool[i]);
        statsBuffer.QuickSort();
    }

    inline int RollItemStatValue(const ExtraStatData* statData, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor || !statData) return 0;
        const float classBonus = (level * itemClassDescriptor->StatsPowerLevelBonus) + (rank * itemClassDescriptor->StatsPowerRankBonus) + (quality * itemClassDescriptor->StatsPowerQualityBonus);
        const float classMult = (itemClassDescriptor->PrimaryStatsList.HasEqualSorted(statData->Id)) ?
            (itemClassDescriptor->PrimaryStatsPowerBonus + itemClassDescriptor->StatsPowerMult) * ItemsGeneratorConfigs.ItemPrimaryStatPowerMult :
            itemClassDescriptor->StatsPowerMult;

        const float valueMin = statData->RollMinPower * power;
        const float valueMax = statData->RollMaxPower * power;
        const int result = static_cast<int>(((StExt_Rand::Range(valueMin, valueMax) * (1.0f + classBonus)) * classMult) * ItemsGeneratorConfigs.ItemStatPowerMult);
        return ValidateValue(result, statData->RollMinCap, statData->RollMaxCap);
    }
    inline int RollItemStatValue(const int statDataId, const int power, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        const ExtraStatData* statData = GetExtraStatDataById(statDataId);
        return RollItemStatValue(statData, power, level, rank, quality, itemClassDescriptor);
    }

    inline int RollItemStatDuration(const int statDataId, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        const float classBonus = (level * itemClassDescriptor->StatsDurationLevelBonus) + (rank * itemClassDescriptor->StatsDurationRankBonus) + (quality * itemClassDescriptor->StatsDurationQualityBonus);
        const float classMult = (itemClassDescriptor->PrimaryStatsList.HasEqualSorted(statDataId)) ?
            (itemClassDescriptor->PrimaryStatsDurationBonus + itemClassDescriptor->StatsDurationMult) * ItemsGeneratorConfigs.ItemPrimaryStatDurationMult : itemClassDescriptor->StatsDurationMult;

        const int durationMin = static_cast<int>(((itemClassDescriptor->StatsDurationMin * classBonus) * classMult) * ItemsGeneratorConfigs.ItemStatDurationMult);
        const int durationMax = static_cast<int>(((itemClassDescriptor->StatsDurationMax * classBonus) * classMult) * ItemsGeneratorConfigs.ItemStatDurationMult);
        return ValidateValue(StExt_Rand::Range(durationMin, durationMax), ItemsGeneratorConfigs.ItemStatDurationMin, ItemsGeneratorConfigs.ItemStatDurationMax);
    }


    inline int RollItemAbilitiesCount(const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return 0;
        
        const float rollChanceClassBonus = (level * itemClassDescriptor->AbilitiesRollChanceLevelBonus) + (rank * itemClassDescriptor->AbilitiesRollChanceRankBonus) + (quality * itemClassDescriptor->AbilitiesRollChanceQualityBonus);
        if(!StExt_Rand::Permille(static_cast<int>(itemClassDescriptor->AbilitiesRollChance + rollChanceClassBonus))) return 0;

        const float classBonus = (level * itemClassDescriptor->AbilitiesCountLevelBonus) + (rank * itemClassDescriptor->AbilitiesCountRankBonus) + (quality * itemClassDescriptor->AbilitiesCountQualityBonus);
        const int maxAbilitiesCount = static_cast<int>((itemClassDescriptor->AbilitiesCountMax + classBonus) * ItemsGeneratorConfigs.ItemAbilityCountMult);
        return ValidateValue(StExt_Rand::Range(0, maxAbilitiesCount), 0, itemClassDescriptor->AbilitiesCountCap);
    }

    inline void RollItemAbilitiesChunk(Array<int>& abilitiesBuffer, const Array<int>& currentAbilities, const int abilitiesCount, const ItemClassDescriptor* itemClassDescriptor)
    {
        Map<int, int> abilitiesPool;
        for (const auto& abilityKV : ItemAbilitiesData)
        {
            const auto& ability = abilityKV.GetValue();
            if (currentAbilities.HasEqual(ability.UId)) continue;
            if (itemClassDescriptor->IncopatibleAbilitiesList.HasEqualSorted(ability.UId)) continue;

            int chance = static_cast<int>(ability.ScriptInstance.RollChance * ItemsGeneratorConfigs.ItemAbilityRollChanceMult);
            if (itemClassDescriptor->PrimaryAbilitiesList.HasEqualSorted(ability.UId))
                chance = static_cast<int>(chance * ItemsGeneratorConfigs.ItemPrimaryAbilityRollChanceMult);

            chance = ValidateValue(chance, 0, 999);
            if (chance <= 0) continue;

            abilitiesPool.Insert(ability.UId, chance);
        }

        const uint abilitiesInPool = abilitiesPool.GetNum();
        if (abilitiesInPool == 0U) return;

        Array<int> rollPool = Array<int>();
        int picked = 0, breaker = 0;
        do
        {
            for (auto& abilityKV : abilitiesPool)
            {
                if (abilityKV.GetValue() < 0) continue;
                if (StExt_Rand::Permille(abilityKV.GetValue()))
                {
                    rollPool.Insert(abilityKV.GetKey());
                    abilityKV.GetValue() = Invalid;
                    ++picked;
                }
            }
            ++breaker;
        } 
        while ((picked < abilitiesCount) && (breaker < 1));
        if (rollPool.IsEmpty()) return;

        StExt_Rand::ShuffleArray(rollPool);
        uint max = ValidateValueMax(static_cast<uint>(abilitiesCount), rollPool.GetNum());
        for (uint i = 0; i < max; ++i)
            abilitiesBuffer.Insert(rollPool[i]);
    }

    inline int RollItemAbilitiesValue(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor || !abilityData) return 0;
        const float classBonus = (level * itemClassDescriptor->AbilitiesPowerLevelBonus) + (rank * itemClassDescriptor->AbilitiesPowerRankBonus) + (quality * itemClassDescriptor->AbilitiesPowerQualityBonus);
        const float classMult = (itemClassDescriptor->PrimaryAbilitiesList.HasEqualSorted(abilityData->UId)) ?
            (itemClassDescriptor->PrimaryAbilitiesPowerBonus + itemClassDescriptor->AbilitiesPowerMult) * ItemsGeneratorConfigs.ItemPrimaryAbilityValueMult : itemClassDescriptor->AbilitiesPowerMult;

        const int value = static_cast<int>((abilityData->ScriptInstance.BaseValue + (abilityData->ScriptInstance.BaseValue * classBonus) * classMult) * ItemsGeneratorConfigs.ItemAbilityValueMult);
        return ValidateValue(StExt_Rand::Range(static_cast<int>(value * 0.5f), value), abilityData->ScriptInstance.BaseValue, abilityData->ScriptInstance.ValueMax);
    }

    inline int RollItemAbilitiesDuration(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor || !abilityData) return 0;
        const float classBonus = (level * itemClassDescriptor->AbilitiesDurationLevelBonus) + (rank * itemClassDescriptor->AbilitiesDurationRankBonus) + (quality * itemClassDescriptor->AbilitiesDurationQualityBonus);
        const float classMult = (itemClassDescriptor->PrimaryAbilitiesList.HasEqualSorted(abilityData->UId)) ?
            (itemClassDescriptor->PrimaryAbilitiesDurationBonus + itemClassDescriptor->AbilitiesDurationMult) * ItemsGeneratorConfigs.ItemPrimaryAbilityDurationMult : itemClassDescriptor->AbilitiesDurationMult;

        const int duration = static_cast<int>((abilityData->ScriptInstance.BaseDuration + (abilityData->ScriptInstance.BaseDuration * classBonus) * classMult) * ItemsGeneratorConfigs.ItemAbilityDurationMult);
        return ValidateValue(StExt_Rand::Range(static_cast<int>(duration * 0.5f), duration), abilityData->ScriptInstance.BaseDuration, abilityData->ScriptInstance.DurationMax);
    }

    inline int RollItemAbilitiesChance(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor || !abilityData) return 0;
        const float classBonus = (level * itemClassDescriptor->AbilitiesChanceLevelBonus) + (rank * itemClassDescriptor->AbilitiesChanceRankBonus) + (quality * itemClassDescriptor->AbilitiesChanceQualityBonus);
        const float classMult = (itemClassDescriptor->PrimaryAbilitiesList.HasEqualSorted(abilityData->UId)) ?
            (itemClassDescriptor->PrimaryAbilitiesChanceBonus + itemClassDescriptor->AbilitiesChanceMult) * ItemsGeneratorConfigs.ItemPrimaryAbilityChanceMult : itemClassDescriptor->AbilitiesChanceMult;

        const int chance = static_cast<int>((abilityData->ScriptInstance.BaseChance + (abilityData->ScriptInstance.BaseChance * classBonus) * classMult) * ItemsGeneratorConfigs.ItemAbilityChanceMult);
        return ValidateValue(StExt_Rand::Range(static_cast<int>(chance * 0.5f), chance), abilityData->ScriptInstance.BaseChance, abilityData->ScriptInstance.ChanceMax);
    }

    inline int RollItemAbilitiesRange(const ItemAbility* abilityData, const int level, const int rank, const int quality, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor || !abilityData) return 0;
        const float classBonus = (level * itemClassDescriptor->AbilitiesRangeLevelBonus) + (rank * itemClassDescriptor->AbilitiesRangeRankBonus) + (quality * itemClassDescriptor->AbilitiesRangeQualityBonus);
        const float classMult = (itemClassDescriptor->PrimaryAbilitiesList.HasEqualSorted(abilityData->UId)) ?
            (itemClassDescriptor->PrimaryAbilitiesRangeBonus + itemClassDescriptor->AbilitiesRangeMult) * ItemsGeneratorConfigs.ItemPrimaryAbilityRangeMult : itemClassDescriptor->AbilitiesRangeMult;

        const int chance = static_cast<int>((abilityData->ScriptInstance.BaseRange + (abilityData->ScriptInstance.BaseRange * classBonus) * classMult) * ItemsGeneratorConfigs.ItemAbilityRangeMult);
        return ValidateValue(StExt_Rand::Range(static_cast<int>(chance * 0.5f), chance), abilityData->ScriptInstance.BaseRange, abilityData->ScriptInstance.RangeMax);
    }


    inline bool RollItemAffix(const int rollChance, const int sourceId, zSTRING& affix)
    {
        Array<zSTRING>* sourceArray = Null;
        if (sourceId <= 0) sourceArray = &ItemPreffixesList;
        else if (sourceId == 1) sourceArray = &ItemAffixesList;
        else if (sourceId >= 2) sourceArray = &ItemSuffixesList;
        else sourceArray = &ItemPreffixesList;

        if (rollChance >= 1000 || StExt_Rand::Permille(rollChance))
        {
            affix = (*sourceArray)[StExt_Rand::Index(sourceArray->GetNum())];
            return true;
        }
        affix = "";
        return false;
    }

    inline bool RollItemName(const int rollChance, zSTRING& name)
    {
        if (StExt_Rand::Permille(rollChance)) {
            name = ItemNamesList[StExt_Rand::Index(ItemNamesList.GetNum())];
            return true;
        }
        return false;
    }

    inline int RollItemWeight(const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor->ModWeight) return Invalid;
        return ValidateValue(StExt_Rand::Range(itemClassDescriptor->WeightMin, itemClassDescriptor->WeightMax), 0, 10);
    }

    inline void CalcItemDamages(oCItem* item)
    {
        float dam = 0.0f;
        unsigned long mask = item->damageTypes;
        for (int i = 0; i < oEDamageIndex_MAX; ++i)
        {
            if (mask & 1) { dam += 1.0f; }
            mask >>= 1;
        }

        if (item->damageTotal < 5) item->damageTotal = 5;
        if (dam < 1.0f)
        {
            item->damage[dam_index_barrier] = item->damageTotal;
            return;
        }

        dam = item->damageTotal / dam + 0.5f;
        if (dam < 5.0f) dam = 5.0f;
        for (int i = 0; i < oEDamageIndex_MAX; i++)
            item->damage[i] = item->damageTypes & (1 << i) ? static_cast<int>(dam) : 0;
    }

    void SetItemExtensionInitialProps(ItemExtension* itemExtension, const oCItem* item)
    {
        if (!item || !itemExtension) return;
        
        itemExtension->ExtraFlags_Main = itemExtension->Properties[(int)ItemProperty::InitialMainExtraFlags] = item->mainflag;
        itemExtension->ExtraFlags_Base = itemExtension->Properties[(int)ItemProperty::InitialBaseFlags] = item->flags;
        itemExtension->ExtraFlags_Additional = itemExtension->Properties[(int)ItemProperty::InitialAdditionalFlags] = item->hitp;

        itemExtension->Properties[(int)ItemProperty::InitialSpell] = item->spell;
        itemExtension->Range = itemExtension->Properties[(int)ItemProperty::InitialRange] = item->range;
        itemExtension->Cost = itemExtension->Properties[(int)ItemProperty::InitialCost] = item->value;
        itemExtension->Weight = itemExtension->Properties[(int)ItemProperty::InitialWeight] = item->weight;
        
        for (int i = 0; i < ItemExtension_Conditions_Max; ++i)
        {
            itemExtension->CondAtr[i] = itemExtension->Properties[(int)ItemProperty::InitialConditionAttributeIndex + i] = item->cond_atr[i];
            itemExtension->CondValue[i] = itemExtension->Properties[(int)ItemProperty::InitialConditionAttributeValueIndex + i] = item->cond_value[i];
        }

        itemExtension->DamageTotal = itemExtension->Properties[(int)ItemProperty::InitialDamageTotal] = item->damageTotal;
        itemExtension->DamageTypes = itemExtension->Properties[(int)ItemProperty::InitialDamageTypes] = static_cast<int>(item->damageTypes);

        int damTotal = 0;
        for (int i = 0; i < oEDamageIndex_MAX; ++i)
        {
            damTotal += item->damage[i];
            itemExtension->Damage[i] = itemExtension->Properties[(int)ItemProperty::InitialDamageIndex + i] = item->damage[i];
            itemExtension->Protection[i] = itemExtension->Properties[(int)ItemProperty::InitialProtectionIndex + i] = item->protection[i];
        }

        if (damTotal == 0)
        {
            CalcItemDamages(const_cast<oCItem*>(item));
            for (int i = 0; i < oEDamageIndex_MAX; ++i)
                itemExtension->Damage[i] = itemExtension->Properties[(int)ItemProperty::InitialDamageIndex + i] = item->damage[i];            
        }
    }

    // -------------------------------------------------------------------------

    ItemExtension* CreateItemExtension(const int power, oCItem* item, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!item || !itemClassDescriptor) 
        {
            DEBUG_MSG("CreateItemExtension() - item or descriptor is null!");
            return Null;
        }

        ItemExtension* result = new ItemExtension();
        result->UId = GetNextItemUId();
        result->InstanceName = CreateExtendedItemInstanceName(result->UId);
        result->BaseInstanceName = NormalizeInstanceName(item->GetInstanceName());
        result->ItemClassID = itemClassDescriptor->ItemClassID;

        result->Type = ItemClassKey_GetType(result->ItemClassID);
        result->Class = ItemClassKey_GetClass(result->ItemClassID);
        result->SubClass = ItemClassKey_GetSubClass(result->ItemClassID);
        result->ItemClassData = itemClassDescriptor;
        
        result->Properties[(int)ItemProperty::InitialPower] = power;
        SetItemExtensionInitialProps(result, item);
        return result;
    }

    ItemExtension* CreateItemExtension(const int power, zSTRING& instanceName, const ItemClassDescriptor* itemClassDescriptor)
    {
        if (!itemClassDescriptor) return Null;

        NormalizeInstanceName(instanceName);
        int instanceId = parser->GetIndex(instanceName);
        if (instanceId == Invalid)
        {
            DEBUG_MSG("CreateItemExtension: symbol '" + instanceName + "' not found!");
            return Null;
        }

        oCItem* item = new oCItem();
        ItemExtension* result = Null;
        const bool itemCreated = parser->CreateInstance(instanceId, item);
        if (itemCreated)
            result = CreateItemExtension(power, item, itemClassDescriptor);

        DEBUG_MSG_IF(!itemCreated, "CreateItemExtension: fail to create original item instance from script! Instance name: '" + instanceName + "'");
        if (item) item->Release();
        return result;       
    }
}
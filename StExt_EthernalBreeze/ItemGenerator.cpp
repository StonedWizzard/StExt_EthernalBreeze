#include <StonedExtension.h>
#include <random>

namespace Gothic_II_Addon
{
    static std::map<int, C_EnchantmentData*> EnchantmentsData;
    static std::map<std::string, C_ItemData*> ItemsData;
    static int LastEnchntmentId;
    static int LastItemDataId;
    static int EnchntmentsCount;
    static int ItemDataCount;
    static int HideItemVisualEffect;

    inline int GetNextEnchntmentId()
    {
        int result = LastEnchntmentId + 1;
        while (EnchantmentsData.find(result) != EnchantmentsData.end()) result += 1;
        LastEnchntmentId = result;
        return result;
    }
    inline int GetNextItemId()
    {
        LastItemDataId += 1;
        return LastItemDataId;
    }
    inline ItemCondition* GetConditionData(int id)
    {
        for (uint i = 0U; i < GeneratorConfigs.ConditionsList.GetNum(); i++)
        {
            if (GeneratorConfigs.ConditionsList[i].Id == id)
                return &GeneratorConfigs.ConditionsList[i];
        }
        return Null;
    }
    inline ItemCondition* GetRandomConditionData(int itemType, int used1, int used2, int used3)
    {
        Array<ItemCondition*> allowedConditions = Array<ItemCondition*>();
        for (uint i = 0U; i < GeneratorConfigs.ConditionsList.GetNum(); i++)
        {
            ItemCondition* cond = &GeneratorConfigs.ConditionsList[i];
            if ((cond->AllowedItemTypes != 0) && !HasFlag(cond->AllowedItemTypes, itemType))
                continue;

            if ((used1 != cond->Id) && (used2 != cond->Id) && (used3 != cond->Id))
                allowedConditions.Insert(cond);
        }

        int allowedCount = allowedConditions.GetNum();
        if(allowedCount == 0) return Null;

        int rnd = rand() % allowedCount;
        return allowedConditions[rnd];
    }

    inline int CalcItemLevel(const int power)
    {
        if (power <= 0) return 1;
        int level = power * GeneratorConfigs.ItemLevelFromPowerRatio;
        return level <= 0 ? 1 : 
            level > GeneratorConfigs.MaxItemLevel ? GeneratorConfigs.MaxItemLevel : level;
    }
    inline int CalcItemRank(const int power)
    {
        if (power <= 0) return 0;
        int RankMax = parser->GetSymbol("StExt_ItemRankMax")->single_intdata - 1;
        int maxRank = power / GeneratorConfigs.NextRankOffset;        
        int minRank = power / GeneratorConfigs.NextRankOffsetMin;

        //normalize rank
        if (maxRank > RankMax) maxRank = RankMax;
        if (maxRank < 1) maxRank = 1;
        if (minRank >= maxRank) minRank = maxRank - 1;
        if (minRank < 0) minRank = 0;

        int rank = (rand() % (maxRank + 1));
        if (rank > maxRank) rank = maxRank;
        if (rank < minRank) rank = minRank;
        return rank;
    }
    inline int CalcDamageTypes(const int types)
    {
        int result = 0;
        for (int i = 0; i < oEDamageIndex_MAX; i++)
            result += HasFlag(types, 1 << i) ? 1 : 0;
        return result == 0 ? 1 : result;
    }

    inline float GetMultForItemType(int type)
    {
        if (HasFlag(type, ItemType_Helm)) { return GeneratorConfigs.HelmsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Armor)) { return GeneratorConfigs.ArmorsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Pants)) { return GeneratorConfigs.PantsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Boots)) { return GeneratorConfigs.BootsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Gloves)) { return GeneratorConfigs.GlovesGlobalPowerMult; }

        if (HasFlag(type, ItemType_Belt)) { return GeneratorConfigs.BeltsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Amulet)) { return GeneratorConfigs.AmuletsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Ring)) { return GeneratorConfigs.RingsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Shield)) { return GeneratorConfigs.ShieldsGlobalPowerMult; }

        if (HasFlag(type, ItemType_Sword1h)) { return GeneratorConfigs.Swords1hGlobalPowerMult; }
        if (HasFlag(type, ItemType_Sword2h)) { return GeneratorConfigs.Swords2hGlobalPowerMult; }
        if (HasFlag(type, ItemType_Axe1h)) { return GeneratorConfigs.Axes1hGlobalPowerMult; }
        if (HasFlag(type, ItemType_Axe2h)) { return GeneratorConfigs.Axes2hGlobalPowerMult; }
        if (HasFlag(type, ItemType_Mace1h)) { return GeneratorConfigs.Maces1hGlobalPowerMult; }
        if (HasFlag(type, ItemType_Mace2h)) { return GeneratorConfigs.Maces2hGlobalPowerMult; }
        if (HasFlag(type, ItemType_MagicSword)) { return GeneratorConfigs.MagicSwordsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Staff)) { return GeneratorConfigs.StaffsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Rapier)) { return GeneratorConfigs.RapiersGlobalPowerMult; }
        if (HasFlag(type, ItemType_DexSword)) { return GeneratorConfigs.DexSwordsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Halleberd)) { return GeneratorConfigs.HalleberdsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Spear)) { return GeneratorConfigs.SpearsGlobalPowerMult; }
        if (HasFlag(type, ItemType_DualL)) { return GeneratorConfigs.DualsLGlobalPowerMult; }
        if (HasFlag(type, ItemType_DualR)) { return GeneratorConfigs.DualsRGlobalPowerMult; }
        if (HasFlag(type, ItemType_Bow)) { return GeneratorConfigs.BowsGlobalPowerMult; }
        if (HasFlag(type, ItemType_CrossBow)) { return GeneratorConfigs.CrossBowsGlobalPowerMult; }
        if (HasFlag(type, ItemType_MagicStaff)) { return GeneratorConfigs.MagicStaffsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Potion)) { return GeneratorConfigs.PotionsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Scroll)) { return GeneratorConfigs.ScrollsGlobalPowerMult; }
        if (HasFlag(type, ItemType_Torso)) { return GeneratorConfigs.TorsoGlobalPowerMult; }
        return 1.0f;
    }

    inline int GetItemFlags(oCItem* item, const int itemType)
    {
        int flags = ItemFlag_None;
        if (item)
        {
            if (HasFlag(item->wear, wear_torso) || HasFlag(item->wear, wear_head)) flags |= ItemFlag_Armor;
            if (HasFlag(item->wear, wear_gloves) || HasFlag(item->wear, wear_boots) || HasFlag(item->wear, wear_pants) || HasFlag(item->wear, wear_pauldrons)) flags |= ItemFlag_Armor;
            if (HasFlag(item->flags, item_ring) || HasFlag(item->flags, item_amulet) || HasFlag(item->flags, item_belt)) flags |= ItemFlag_Jewelry;
            if (HasFlag(item->flags, item_shield)) flags |= ItemFlag_LeftHand | ItemFlag_Armor;
            if (HasFlag(item->flags, item_swd) || HasFlag(item->flags, item_axe) || HasFlag(item->flags, item_dag)) flags |= ItemFlag_RightHand | ItemFlag_Weapon | ItemFlag_Melee;
            if (HasFlag(item->flags, item_2hd_swd) || HasFlag(item->flags, item_2hd_axe)) flags |= ItemFlag_BothHands | ItemFlag_Weapon | ItemFlag_Melee;
            if (HasFlag(item->flags, item_bow) || HasFlag(item->flags, item_crossbow)) flags |= ItemFlag_BothHands | ItemFlag_Weapon | ItemFlag_Range;
            if (HasFlag(item->flags, item_throw)) flags |= ItemFlag_LeftHand | ItemFlag_Weapon | ItemFlag_Melee;
            if (HasFlag(item->hitp, bit_item_mag_sword) || HasFlag(itemType, ItemType_Staff)) flags |= ItemFlag_MagicWeapon;
            if (HasFlag(itemType, ItemType_MagicStaff)) flags |= ItemFlag_MagicWeapon;
        }
        return flags;
    }

    void ProcessItemEmbeddedStats(oCItem* item, C_EnchantmentData* enchantment)
    {
        DEBUG_MSG("ProcessItemEmbeddedStats...");
        if (!enchantment && !item)
        {
            DEBUG_MSG("ProcessItemEmbeddedStats - item or enchantment is null!");
            return;
        }

        int itemType = enchantment->Type;
        int itemFlags = enchantment->Flags;
        int itemLevel = enchantment->Level;
        int itemRank = enchantment->Rank;
        float itemTypeMult = GetMultForItemType(enchantment->Type);

        // damage change
        if (HasFlag(enchantment->Flags, ItemFlag_Weapon))
        {
            bool isTwoHanded = HasFlag(itemType, ItemType_Sword2h) || HasFlag(itemType, ItemType_Axe2h) || HasFlag(itemType, ItemType_Mace2h) ||
                HasFlag(itemType, ItemType_Staff) || HasFlag(itemType, ItemType_Halleberd) || HasFlag(itemType, ItemType_Spear) || HasFlag(itemType, ItemType_MagicStaff);

            bool isMagic = HasFlag(itemType, ItemType_MagicSword) || HasFlag(itemType, ItemType_Staff) || HasFlag(itemType, ItemType_MagicStaff);

            int extraDamagesCount = 0;
            int primaryDamageTypes = item->damageTypes;
            if (isMagic)
            {
                int rnd = rand() % 1000;
                if (rnd >= 666) primaryDamageTypes |= dam_fire;
                else if (rnd >= 333) primaryDamageTypes |= dam_magic;
                else primaryDamageTypes |= dam_fly;
                extraDamagesCount += 1;
            }
            float rankMult = (itemRank * GeneratorConfigs.DamagePerRankMult);
            int primaryDamageMin = itemLevel * GeneratorConfigs.DamagePerLevelMin;
            int primaryDamageMax = itemLevel * GeneratorConfigs.DamagePerLevelMax;

            primaryDamageMin += primaryDamageMin * rankMult;
            primaryDamageMax += primaryDamageMax * rankMult;
            primaryDamageMin *= itemTypeMult;
            primaryDamageMax *= itemTypeMult;
            if (isTwoHanded)
            {
                primaryDamageMin += primaryDamageMin * GeneratorConfigs.TwoHandedDamageMult;
                primaryDamageMax += primaryDamageMax * GeneratorConfigs.TwoHandedDamageMult;
            }

            int secondaryDamageMin = primaryDamageMin * GeneratorConfigs.ExtraDamageMinMult;
            int secondaryDamageMax = primaryDamageMax * GeneratorConfigs.ExtraDamageMaxMult;
            int extraDamageChance = 10 * (GeneratorConfigs.ExtraDamageChanceBase + (itemLevel * GeneratorConfigs.ExtraDamageChancePerLevelMult) + (itemRank * GeneratorConfigs.ExtraDamageChancePerRankMult));

            if (primaryDamageMin < 5) primaryDamageMin = 5;
            if (secondaryDamageMin < 5) secondaryDamageMin = 5;
            if (primaryDamageMax < 10) primaryDamageMax = 10;
            if (secondaryDamageMax < 10) secondaryDamageMax = 10;

            int damageTypes = primaryDamageTypes;
            int damageCount = CalcDamageTypes(damageTypes);
            if (damageCount <= 0)
            {
                DEBUG_MSG("ProcessItemEmbeddedStats - damages count is 0!");
                damageCount = 1;
            }

            for (int i = 0; i < oEDamageIndex_MAX; i++)
            {
                int damageType = 1 << i;                
                if (HasFlag(primaryDamageTypes, damageType))
                {
                    int origDamage = item->damage[i];
                    enchantment->Damage[i] = GetRandomRange(primaryDamageMin, primaryDamageMax);
                    if (origDamage == 0)
                        origDamage = item->damageTotal > 5 ? item->damageTotal / damageCount : 5;
                    enchantment->Damage[i] += origDamage * (GeneratorConfigs.OrigDamagePerLevelMult * itemLevel);
                }
                else if ((extraDamageChance >= (rand() % 1000)) && (extraDamagesCount < GeneratorConfigs.ExtraDamageTypesCap) && !HasFlag(primaryDamageTypes, damageType))
                {
                    if (i == 0) continue;
                    enchantment->Damage[i] = GetRandomRange(secondaryDamageMin, secondaryDamageMax);
                    extraDamageChance *= 0.5f;
                    extraDamagesCount += 1;
                    damageTypes |= damageType;
                }

                if (enchantment->Damage[i] > GeneratorConfigs.MaxItemDamage) enchantment->Damage[i] = GeneratorConfigs.MaxItemDamage;
                enchantment->DamageTotal += enchantment->Damage[i];
                enchantment->Cost += enchantment->Damage[i] * 0.25f;
            }

            enchantment->DamageTypes = damageTypes;
            enchantment->Range = item->range + (item->range * GetRandomFloatRange(GeneratorConfigs.RangeSpread));

            // handle weapons with prot mods
            for (int i = 0; i < oEDamageIndex_MAX; i++)
            {
                int prot = item->protection[i];
                if (prot > 0)
                {
                    prot *= (GeneratorConfigs.OrigProtectionPerLevelMult * itemLevel);
                    prot = prot <= 0 ? 1 : prot;
                    enchantment->Protection[i] = prot;
                }
            }
        }

        // protection change
        if (HasFlag(enchantment->Flags, ItemFlag_Armor))
        {
            float rankMult = itemRank * GeneratorConfigs.ProtectionPerRankMult;
            int protectionMin = itemLevel * GeneratorConfigs.ProtectionPerLevelMin;
            int protectionMax = itemLevel * GeneratorConfigs.ProtectionPerLevelMax;
            protectionMin += protectionMin * rankMult;
            protectionMax += protectionMax * rankMult;
            protectionMin *= itemTypeMult;
            protectionMax *= itemTypeMult;
            if (protectionMax < 5) protectionMax = 5;
            if (protectionMin <= 0) protectionMin = 1;
            int extraProtChance = 10 * (GeneratorConfigs.ExtraProtectionChanceBase + (itemLevel * GeneratorConfigs.ExtraProtectionChancePerLevelMult) + (itemRank * GeneratorConfigs.ExtraProtectionChancePerRankMult));
            for (int i = 0; i < oEDamageIndex_MAX; i++)
            {
                if (item->protection[i] > 0)
                    enchantment->Protection[i] = GetRandomRange(protectionMin, protectionMax);
                else if (extraProtChance >= (rand() % 1000))
                {
                    if (i == 0) continue;
                    enchantment->Protection[i] = GetRandomRange(protectionMin * GeneratorConfigs.ExtraProtectionMinMult, protectionMax * GeneratorConfigs.ExtraProtectionMaxMult);
                }
                                
                enchantment->Protection[i] += item->protection[i] * (GeneratorConfigs.OrigProtectionPerLevelMult * itemLevel);
                if (item->protection[i] > GeneratorConfigs.MaxItemProtection) enchantment->Protection[i] = GeneratorConfigs.MaxItemProtection;
                enchantment->Cost += enchantment->Protection[i] * 0.25f;
            }
        }

        // conditions change
        float conditionsMult = GeneratorConfigs.ConditionStaticMult + (itemLevel * GeneratorConfigs.ConditionPerLevelMult) + (itemRank * GeneratorConfigs.ConditionPerRankMult);
        float baseConditionMult = GeneratorConfigs.BaseConditionStaticMult + (itemLevel * GeneratorConfigs.BaseConditionPerLevelMult) + (itemRank * GeneratorConfigs.BaseConditionPerRankMult);
        if (conditionsMult < 0.1f) conditionsMult = 0.1f;
        if (baseConditionMult < 0.5f) baseConditionMult = 0.5f;
        int extraCondChance = 10 * (GeneratorConfigs.ExtraConditionChanceBase + (itemLevel * GeneratorConfigs.ExtraConditionChancePerLevelMult) + (itemRank * GeneratorConfigs.ExtraConditionChancePerRankMult));
        bool extraCondAdded = false;
        
        for (int i = 0; i < EnchantConditionsMax; i++)
        {
            enchantment->CondAtr[i] = item->cond_atr[i];
            enchantment->CondValue[i] = item->cond_value[i];

            // item have condition in this cell
            if (item->cond_atr[i] > 0)
            {
                ItemCondition* condData = GetConditionData(item->cond_atr[i]);
                if (condData)
                {
                    enchantment->CondValue[i] *= baseConditionMult;
                    enchantment->CondValue[i] = enchantment->CondValue[i] > condData->HardCap ? condData->HardCap : 
                        (enchantment->CondValue[i] <= 0 ? 1 : enchantment->CondValue[i]);
                }
            }
            else if ((item->cond_atr[i] == 0) && !extraCondAdded)
            {
                if (extraCondChance < (rand() % 1000)) continue;

                ItemCondition* condData = GetRandomConditionData(enchantment->Type, item->cond_atr[0], item->cond_atr[1], item->cond_atr[2]);
                if (condData)
                {
                    int tmpLvl = itemLevel + (itemRank * 10);
                    int condVal = GetRandomRange(tmpLvl * condData->PerLevelMin, tmpLvl * condData->PerLevelMax) * conditionsMult;
                    condVal = condVal > condData->HardCap ? condData->HardCap :
                        condVal <= 0 ? 1 : condVal;
                    enchantment->CondAtr[i] = condData->Id;
                    enchantment->CondValue[i] = condVal;
                    extraCondAdded = true;
                }
            }
        }

        // make sure to add requirement if no any present
        if ((enchantment->CondAtr[0] == 0) && (enchantment->CondAtr[1] == 0) && (enchantment->CondAtr[2] == 0))
        {
            ItemCondition* condData = GetRandomConditionData(enchantment->Type, enchantment->CondAtr[0], enchantment->CondAtr[1], enchantment->CondAtr[2]);
            int i = (enchantment->CondAtr[0] == 0) ? 0 :
                (enchantment->CondAtr[1] == 0) ? 1 :
                (enchantment->CondAtr[2] == 0) ? 2 : Invalid;

            if(i == Invalid)
                DEBUG_MSG("ProcessItemEmbeddedStats - condition index is Invalid!");

            if (condData && (i != Invalid))
            {
                int tmpLvl = itemLevel + (itemRank * 10);
                int condVal = GetRandomRange(tmpLvl * condData->PerLevelMin, tmpLvl * condData->PerLevelMax) * conditionsMult;
                condVal = condVal > condData->HardCap ? condData->HardCap :
                    condVal <= 0 ? 1 : condVal;
                enchantment->CondAtr[i] = condData->Id;
                enchantment->CondValue[i] = condVal;
            }
        }
        /*
        DEBUG_MSG("ProcessItemEmbeddedStats done");
        DEBUG_MSG("");
        DEBUG_MSG("DamageTypes = " + Z enchantment->DamageTypes);
        DEBUG_MSG("DamageTotal = " + Z enchantment->DamageTotal);
        DEBUG_MSG("Damage[0] = " + Z enchantment->Damage[0]);
        DEBUG_MSG("Damage[1] = " + Z enchantment->Damage[1]);
        DEBUG_MSG("Damage[2] = " + Z enchantment->Damage[2]);
        DEBUG_MSG("Damage[3] = " + Z enchantment->Damage[3]);
        DEBUG_MSG("Damage[4] = " + Z enchantment->Damage[4]);
        DEBUG_MSG("Damage[5] = " + Z enchantment->Damage[5]);
        DEBUG_MSG("Damage[6] = " + Z enchantment->Damage[6]);
        DEBUG_MSG("Damage[7] = " + Z enchantment->Damage[7]);
        DEBUG_MSG("Range = " + Z enchantment->Range);
        DEBUG_MSG("");
        DEBUG_MSG("Protection[0] = " + Z enchantment->Protection[0]);
        DEBUG_MSG("Protection[1] = " + Z enchantment->Protection[1]);
        DEBUG_MSG("Protection[2] = " + Z enchantment->Protection[2]);
        DEBUG_MSG("Protection[3] = " + Z enchantment->Protection[3]);
        DEBUG_MSG("Protection[4] = " + Z enchantment->Protection[4]);
        DEBUG_MSG("Protection[5] = " + Z enchantment->Protection[5]);
        DEBUG_MSG("Protection[6] = " + Z enchantment->Protection[6]);
        DEBUG_MSG("Protection[7] = " + Z enchantment->Protection[7]);
        DEBUG_MSG("");
        DEBUG_MSG("Requirement[0] = " + Z enchantment->CondAtr[0] + " / " + Z enchantment->CondValue[0]);
        DEBUG_MSG("Requirement[1] = " + Z enchantment->CondAtr[1] + " / " + Z enchantment->CondValue[1]);
        DEBUG_MSG("Requirement[2] = " + Z enchantment->CondAtr[2] + " / " + Z enchantment->CondValue[2]);
        DEBUG_MSG("");
        */
    }

    void ProcessItemStats(C_EnchantmentData* enchantment)
    {
        if (!enchantment)
        {
            DEBUG_MSG("ProcessItemStats - enchantment is null!");
            return;
        }

        int statsCount = static_cast<int>(GeneratorConfigs.StatsOptions.GetNum());
        if (statsCount == 0)
        {
            DEBUG_MSG("ProcessItemStats - stats not found in DB!");
            return;
        }

        // generate options slots
        int itemRank = enchantment->Rank <= 0 ? 1 : enchantment->Rank;
        int itemLevel = enchantment->Level;
        int optionsSlots = GetRandomRange(GeneratorConfigs.StatsMinPerRank * itemRank, GeneratorConfigs.StatsMaxPerRank * itemRank);
        optionsSlots += rand() % (int)((itemLevel * GeneratorConfigs.ExtraStatsPerLevel) + 1);
        if (optionsSlots <= 0)
        {
            DEBUG_MSG("ProcessItemStats - item options count is zero!");
            optionsSlots = 1;
        }
        if (optionsSlots > GeneratorConfigs.StatsMaxCap)
            optionsSlots = GeneratorConfigs.StatsMaxCap > EnchantStatsMax ? EnchantStatsMax : GeneratorConfigs.StatsMaxCap;
        if (optionsSlots > EnchantStatsMax) optionsSlots = EnchantStatsMax;

        std::vector<int> statsKeys;
        CreateUniqKeysArray(statsKeys, statsCount);
        float statValueMult = (itemLevel * GeneratorConfigs.StatPowerPerLevelMult) + (itemRank * GeneratorConfigs.StatPowerPerRankMult);

        if (HasFlag(enchantment->Type, ItemType_Potion) || HasFlag(enchantment->Type, ItemType_Scroll))
            statValueMult *= GeneratorConfigs.StatTimedEffectPowerMult;
        float statDurationMult = (itemLevel * GeneratorConfigs.StatDurationPerLevelMult) + (itemRank * GeneratorConfigs.StatDurationPerRankMult);

        statValueMult *= GetMultForItemType(enchantment->Type);
        statDurationMult *= GetMultForItemType(enchantment->Type);

        int totalOptions = 0;
        for (int i = 0, j = 0; i < optionsSlots;)
        {
            if (j >= statsCount) break;

            ItemStatOption* option = &GeneratorConfigs.StatsOptions[statsKeys[j]];
            j += 1;
            if (!option)
            {
                DEBUG_MSG("ProcessItemStats - option is null!");
                continue;
            }

            if (HasFlag(option->IncopatibleItemTypes, enchantment->Type)) continue;

            int rnd = rand() % 1000;
            rnd += (itemLevel * GeneratorConfigs.StatGetChanceFromLevelMult) + (itemRank * GeneratorConfigs.StatGetChanceFromRankMult);
            rnd *= GeneratorConfigs.StatGlobalGetChanceMult;
            if ((option->Rarity > rnd) && (totalOptions > 0)) continue;

            int optionValue = GetRandomRange(option->StatValueMin * statValueMult, option->StatValueMax * statValueMult) * GeneratorConfigs.StatGlobalPowerMult;
            float capBonusMult = (itemLevel * GeneratorConfigs.StatMaxCapBonusFromLevel) + (itemRank * GeneratorConfigs.StatMaxCapBonusFromRank);
            int optionMaxCap = option->StatMaxCap >= 10 ? option->StatMaxCap + (option->StatMaxCap * capBonusMult) : 
                capBonusMult > 0.5f ? option->StatMaxCap + (option->StatMaxCap * 0.5f) : option->StatMaxCap + (option->StatMaxCap * capBonusMult);

            if (optionValue > optionMaxCap) optionValue = optionMaxCap <= 0 ? 1 : optionMaxCap;
            if (optionValue < option->StatMinCap) optionValue = option->StatMinCap > 0 ? option->StatMinCap : 1;

            //DEBUG_MSG("option max (1): " + Z option->StatMaxCap);
            //DEBUG_MSG("optionValue (2): " + Z optionValue);

            enchantment->StatId[i] = option->StatId;
            enchantment->StatValue[i] = optionValue;
            int extraPrice = option->Price * optionValue;            

            if (HasFlag(enchantment->Type, ItemType_Potion) || HasFlag(enchantment->Type, ItemType_Scroll))
            {
                int optionDuration = GetRandomRange(GeneratorConfigs.StatTimedEffectDurationBaseMin * statDurationMult, GeneratorConfigs.StatTimedEffectDurationBaseMax * statDurationMult);
                optionDuration = optionDuration < 30 ? 30 : optionDuration;
                enchantment->StatDuration[i] = optionDuration;
                extraPrice *= GeneratorConfigs.StatTimedEffectPriceMult;
            }
            enchantment->Cost += extraPrice;

            ++totalOptions;
            ++i;
            //DEBUG_MSG("New stat added: " + Z enchantment->StatId[i] + " / " + Z enchantment->StatValue[i]);
        }
    }

    void ProcessItemAbilities(C_EnchantmentData* enchantment)
    {
        if (!enchantment)
        {
            DEBUG_MSG("ProcessItemAbilities - enchantment is null!");
            return;
        }

        uint abilitiesCount = GeneratorConfigs.AbilitiesOptions.GetNum();
        if (abilitiesCount == 0)
        {
            DEBUG_MSG("ProcessItemAbilities - abilities not found in DB!");
            return;
        }

        int itemRank = enchantment->Rank;
        int itemLevel = enchantment->Level;
        int abilityGetChance = 10 * (GeneratorConfigs.AbilitiesGetChanceBase + (itemLevel * GeneratorConfigs.AbilitiesGetChancePerLevelMult) + (itemRank * GeneratorConfigs.AbilitiesGetChancePerRankMult));

        if (abilityGetChance < (rand() % 1000))
            return;

        std::vector<int> abilitiesKyes;
        CreateUniqKeysArray(abilitiesKyes, GeneratorConfigs.StatsOptions.GetNum());
        float abilityValueMult = (itemLevel * GeneratorConfigs.AbilitiesPowerPerLevelMult) + (itemRank * GeneratorConfigs.AbilitiesPowerPerRankMult);
        float abilityDurationMult = (itemLevel * GeneratorConfigs.AbilitiesDurationPerLevelMult) + (itemRank * GeneratorConfigs.AbilitiesDurationPerRankMult);
        float abilityChanceMult = (itemLevel * GeneratorConfigs.AbilitiesChancePerLevelMult) + (itemRank * GeneratorConfigs.AbilitiesChancePerRankMult);
        abilityValueMult *= GetMultForItemType(enchantment->Type);
        abilityDurationMult *= GetMultForItemType(enchantment->Type);
        abilityChanceMult *= GetMultForItemType(enchantment->Type);

        for (int i = 0, j = 0; i < GeneratorConfigs.AbilitiesMaxCap; i++)
        {
            if (j >= abilitiesKyes.size())  break;

            ItemAbilityOption* option = &GeneratorConfigs.AbilitiesOptions[abilitiesKyes[j]];
            j += 1;
            if (!option)
            {
                DEBUG_MSG("ProcessItemAbilities - option is null!");
                continue;
            }

            int rnd = rand() % 1000;
            if (HasFlag(option->IncopatibleItemTypes, enchantment->Type) || (option->Rarity > rnd));
            continue;

            int optionValue = GetRandomRange(option->AbilityValueMin * abilityValueMult, option->AbilityValueMax * abilityValueMult);
            int optionDuration = GetRandomRange(option->AbilityDurationMin * abilityDurationMult, option->AbilityDurationMax * abilityDurationMult);
            int optionChance = GetRandomRange(option->AbilityChanceMin * abilityChanceMult, option->AbilityChanceMax * abilityChanceMult);
            int extraPrice = option->Price + optionValue + optionDuration + (optionChance * 5);

            enchantment->AbilityId[i] = option->AbilityId;
            enchantment->AbilityValue[i] = optionValue;
            enchantment->AbilityChance[i] = optionDuration;
            enchantment->AbilityDuration[i] = optionChance;
            enchantment->Cost += extraPrice;
        }
    }

    void ProcessItemVisualEffect(C_EnchantmentData* enchantment)
    {
        if (!enchantment)
        {
            DEBUG_MSG("ProcessItemVisualEffect - item or enchantment is null!");
            return;
        }

        int itemRank = enchantment->Rank;
        int itemLevel = enchantment->Level;
        int effectGetChance = 10 * (GeneratorConfigs.VisualEffectGetChanceBase + (itemLevel * GeneratorConfigs.VisualEffectGetChancePerLevelMult) + (itemRank * GeneratorConfigs.VisualEffectGetChancePerRankMult));

        if (effectGetChance < (rand() % 1000))
            return;

        int effectIndx = rand() % GeneratorConfigs.VisualEffects.GetNum();
        enchantment->VisualEffect = GeneratorConfigs.VisualEffects[effectIndx];            
    }

    void ProcessItemSockets(C_EnchantmentData* enchantment)
    {
        if (!enchantment)
        {
            DEBUG_MSG("ProcessItemVisualEffect - enchantment is null!");
            return;
        }

        int itemRank = enchantment->Rank;
        int itemLevel = enchantment->Level;
        int socketChance = 10 * (GeneratorConfigs.SocketBaseChance + (itemLevel * GeneratorConfigs.ExtraSocketChancePerLevelMult) + (itemRank * GeneratorConfigs.ExtraSocketChancePerRankMult));

        if (socketChance < (rand() % 1000))
            return;

        int sockets = 1 + (itemLevel * GeneratorConfigs.ExtraSocketsPerLevel) + (rand() % GeneratorConfigs.SocketsMaxCap);
        sockets = sockets > GeneratorConfigs.SocketsMaxCap ? GeneratorConfigs.SocketsMaxCap : sockets;
        enchantment->SocketsMax = sockets;
        enchantment->Flags |= ItemFlag_Socketed;
        enchantment->Cost += sockets * 100;
    }

    void ProcessItemCorruption(C_EnchantmentData* enchantment)
    {
        if (!enchantment)
        {
            DEBUG_MSG("ProcessItemCorruption - enchantment is null!");
            return;
        }

        int corruptionChance = 10 * GeneratorConfigs.CorruptionBaseChance;
        if (corruptionChance < (rand() % 1000))
            return;

        corruptionChance = corruptionChance > 300 ? 300 : corruptionChance;
        enchantment->Flags |= ItemFlag_Corrupted;
        if (HasFlag(enchantment->Flags, ItemFlag_Socketed))
        {
            enchantment->SocketsMax *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult);
            if (enchantment->SocketsMax <= 0)
            {
                enchantment->Flags = enchantment->Flags & ~ItemFlag_Socketed;
                enchantment->SocketsMax = 0;
            }
        }
        else if (100 > (rand() % 1000))
        {
            enchantment->SocketsMax = 1 + (rand() % GeneratorConfigs.SocketsMaxCap);
            enchantment->Flags |= ItemFlag_Socketed;
        }

        for (int i = 0; i < oEDamageIndex_MAX; i++)
        {
            if ((corruptionChance > (rand() % 1000)) && (enchantment->Damage[i] > 0))
            {
                enchantment->Damage[i] *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult) + 0.1f;
                enchantment->Damage[i] = enchantment->Damage[i] <= 0 ? 5 : enchantment->Damage[i];
            }
            if ((corruptionChance > (rand() % 1000)) && (enchantment->Protection[i] > 0))
            {
                enchantment->Protection[i] *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult) + 0.1f;
                enchantment->Protection[i] = enchantment->Protection[i] <= 0 ? 5 : enchantment->Protection[i];
            }
        }

        for (int i = 0; i < EnchantStatsMax; i++)
        {
            if (enchantment->StatId[i] > Invalid)
            {
                if (corruptionChance > (rand() % 1000))
                {
                    enchantment->StatValue[i] *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult) + 0.1f;
                    enchantment->StatValue[i] = enchantment->StatValue[i] <= 0 ? 1 : enchantment->StatValue[i];
                }
                if (corruptionChance > (rand() % 1000))
                {
                    enchantment->StatDuration[i] *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult) + 0.1f;
                    enchantment->StatDuration[i] = enchantment->StatDuration[i] <= 0 ? 15 : enchantment->StatDuration[i];
                }
            }
        }

        for (int i = 0; i < EnchantAbilityMax; i++)
        {
            if (enchantment->AbilityId[i] > Invalid)
            {
                if (corruptionChance > (rand() % 1000))
                    enchantment->AbilityValue[i] *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult);
                if (corruptionChance > (rand() % 1000))
                    enchantment->AbilityDuration[i] *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult);
                if (corruptionChance > (rand() % 1000))
                    enchantment->AbilityChance[i] *= GetRandomFloatRange(GeneratorConfigs.CorruptionSpreadMult);
            }
        }
    }

    C_ItemData* CreateItemData(zSTRING& instanceName, const zSTRING baseInstanceName, const int enchntmentId, const int itemType, const int flags, const int power)
    {
        DEBUG_MSG("Build item data...");
        std::string instName = instanceName.ToChar();
        if (ItemsData.find(instName) != ItemsData.end())
        {
            DEBUG_MSG("CreateItemData - instance with name '" + instanceName + "' present in collection. Override name...");
            int rnd = rand();
            std::string tmpInstanceName = instanceName.ToChar() + rnd;
            while (ItemsData.find(tmpInstanceName) != ItemsData.end())
            {
                rnd += 1;
                tmpInstanceName = instanceName.ToChar() + rnd;
            }
            instName = tmpInstanceName;
            instanceName = zSTRING(instName.c_str());
        }

        C_ItemData* result = new C_ItemData();
        result->BaseInstanceName = baseInstanceName;
        result->InstanceName = instanceName;
        result->EnchantmentId = enchntmentId;
        result->Type = itemType;
        result->Flags = flags;
        result->Power = power;

        ItemsData.insert({ instName, result });
        ItemDataCount += 1;
        DEBUG_MSG("Build item data with key: " + Z instanceName + " was done!");
        return result;
    }

    C_EnchantmentData* BuildEnchantmentData(oCItem* item, const int enchntmentId, const int itemType, const int power)
    {
        DEBUG_MSG("Build enchantment data...");
        if (!item)
        {
            DEBUG_MSG("BuildEnchantmentData - item is null!");
            return Null;
        }
        C_EnchantmentData* result = new C_EnchantmentData();
        result->UId = enchntmentId;
        SetItemAffixes(result->Preffix, result->Affix, result->Suffix);
        result->Type = itemType;
        result->Flags = GetItemFlags(item, itemType);
        result->Level = CalcItemLevel(power);
        result->Rank = CalcItemRank(result->Level);
        result->AdditionalFlags = item->hitp;

        DEBUG_MSG("Build enchantment data Item type: " + Z result->Type);
        DEBUG_MSG("Build enchantment data Item flags: " + Z result->Flags);
        DEBUG_MSG("Build enchantment data Level: " + Z result->Level);
        DEBUG_MSG("Build enchantment data Rank: " + Z result->Rank);

        if (HasFlag(result->Type, ItemType_Potion) || HasFlag(result->Type, ItemType_Scroll))
        {
            if (result->Rank <= 0) result->Rank = 1;
            ProcessItemStats(result);
            if (result->Rank >= GeneratorConfigs.VisualEffectBeginsOnRank) ProcessItemVisualEffect(result);
        }
        else
        {
            ProcessItemEmbeddedStats(item, result);
            if (result->Rank > 0) ProcessItemStats(result);
            else if (HasFlag(result->Type, ItemType_Belt) || HasFlag(result->Type, ItemType_Amulet) || HasFlag(result->Type, ItemType_Ring)) ProcessItemStats(result);

            //if (result->Rank >= GeneratorConfigs.AbilitiesBeginsOnRank) ProcessItemAbilities(result);
            if (GeneratorConfigs.SocketsMaxCap > 0) ProcessItemSockets(result);
            if (result->Rank >= GeneratorConfigs.VisualEffectBeginsOnRank) ProcessItemVisualEffect(result);
            if ((GeneratorConfigs.UndefinedBaseChance * 10) > (rand() % 1000)) result->Flags |= ItemFlag_Undefined;
        }
        ProcessItemCorruption(result);        

        float costMult = (result->Level * GeneratorConfigs.PriceMultPerLevel) + (result->Rank * GeneratorConfigs.PriceMultPerRank);
        result->Cost += result->Cost * costMult;
        result->Cost *= GeneratorConfigs.PriceMultGlobal;
        DEBUG_MSG("Build enchantment data Cost: " + Z result->Cost);

        EnchantmentsData.insert({ enchntmentId, result });
        EnchntmentsCount += 1;
        DEBUG_MSG("Build enchantment data with Id: " + Z enchntmentId + " was done!");

        return result;
    }

    C_ItemData* GetItemData(const zSTRING instanceName)
    {
        C_ItemData* result = Null;
        std::string instName = instanceName.ToChar();
        auto it = ItemsData.find(instName);
        if (it != ItemsData.end()) result = it->second;
        else { /*DEBUG_MSG("GetItemData - instance not found: " + instanceName + "!"); */ }
        return result;
    }

    inline C_EnchantmentData* GetEnchantmentData(const int enchantId)
    {
        C_EnchantmentData* result = Null;
        try { result = EnchantmentsData.at(enchantId); }
        catch (const std::out_of_range& e) { /*DEBUG_MSG("GetEnchantmentData - enchntment not found id: " + Z enchantId + "!");*/ }
        return result;
    }
    inline C_EnchantmentData* GetEnchantmentData(oCItem* item)
    {
        if (item)
        {
            if ((item->spell > 0) && !HasFlag(item->mainflag, item_kat_rune))
            {
                C_EnchantmentData* enchantment = GetEnchantmentData(item->spell);
                if (enchantment)
                    return enchantment;
            }

            zSTRING instName = item->GetInstanceName();
            C_ItemData* itemData = GetItemData(instName);
            if (!itemData)
            {
                //DEBUG_MSG("GetEnchantmentData - item data not found!");
                return Null;
            }
            return GetEnchantmentData(itemData->EnchantmentId);
        }
        DEBUG_MSG("GetEnchantmentData - item is null!");
        return Null;
    }
        
    void ApplyEnchntment(C_EnchantmentData* enchantment, oCItem* item)
    {
        DEBUG_MSG("Apply enchantment for '" + item->name + "'...");
        HideItemVisualEffect = parser->GetSymbol("StExt_Config_DisableEnchantedItemsEffects")->single_intdata;
        int RankMax = parser->GetSymbol("StExt_ItemRankMax")->single_intdata - 1;
        if (enchantment->Rank > RankMax) enchantment->Rank = RankMax;
        if (enchantment->Rank < 0) enchantment->Rank = 0;

        zSTRING affix = enchantment->Affix.IsEmpty() ? enchantment->Affix : " - " + enchantment->Affix;
        zSTRING preffix = enchantment->Preffix.IsEmpty() ? enchantment->Preffix : enchantment->Preffix + " ";
        zSTRING suffix = enchantment->Suffix.IsEmpty() ? enchantment->Suffix : " " + enchantment->Suffix;
        
        item->description = preffix + item->description + affix + suffix;
        item->name = item->description;

        item->flags |= enchantment->ExtraFlags;        
        item->hitp = enchantment->AdditionalFlags;
        item->value += enchantment->Cost;
        if (HasFlag(enchantment->Flags, ItemFlag_Undefined)) item->value = 100;

        for (int i = 0; i < oEDamageIndex_MAX; i++)
        {
            item->protection[i] = enchantment->Protection[i];
            item->damage[i] = enchantment->Damage[i];
        }
        item->damageTotal = enchantment->DamageTotal;
        item->damageTypes = enchantment->DamageTypes;
        item->range = enchantment->Range;
        item->effectName = HideItemVisualEffect ? zSTRING() : enchantment->VisualEffect;
        item->spell = enchantment->UId;

        for (int i = 0; i < EnchantConditionsMax; i++)
        {
            // clear change values
            item->change_atr[i] = 0;
            item->change_value[i] = 0;
            item->cond_atr[i] = enchantment->CondAtr[i];
            item->cond_value[i] = enchantment->CondValue[i];
        }
        BuildItemText(enchantment, item);
    }

    int CreateEnchantedItem(const int instance, const int itemType, int power)
    {
        DEBUG_MSG("Create enchanted item...");
        int resultInstance = Invalid;
        zCPar_Symbol* ps = parser->GetSymbol(instance);
        if (itemType == ItemType_None)
        {
            DEBUG_MSG("GenerateItem - item type is undefined!");
            return resultInstance;
        }
        if (!ps)
        {
            DEBUG_MSG("GenerateItem - base item instance not found!");
            return resultInstance;
        }
        
        oCItem* pItem = dynamic_cast<oCItem*>(ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, instance));
        if (!pItem)
        {
            DEBUG_MSG("GenerateItem - base item is null!");
            return resultInstance;
        }

        int itemId = GetNextItemId();
        int enchntmentId = GetNextEnchntmentId();
        zSTRING newInstName = BuildEnchantedItemInstanceName(itemId, enchntmentId);        

        C_EnchantmentData* enchantmentData = BuildEnchantmentData(pItem, enchntmentId, itemType, power);
        if (!enchantmentData)
        {
            DEBUG_MSG("GenerateItem - enchantmentData is null!");
            return resultInstance;
        }
        C_ItemData* itemData = CreateItemData(newInstName, ps->name, enchntmentId, itemType, enchantmentData->Flags, power);

        zCPar_Symbol* newItemSym = CopySymbol(ps->name, newInstName, resultInstance);
        pItem = dynamic_cast<oCItem*>(ogame->GetGameWorld()->CreateVob(zVOB_TYPE_ITEM, resultInstance));
        
        ApplyEnchntment(enchantmentData, pItem);
        parser->SetInstance(resultInstance, pItem);

        DEBUG_MSG("GenerateItem - Item '" + pItem->name + "' with instanceId: " + Z resultInstance + " and type: " + Z itemType + " was done!");
        pItem->Release();
        return resultInstance;
    }

    int GenerateNewItem(const int itemTypes, int power)
    {
        DEBUG_MSG("Start generate item...");
        // Get alread generated items from BD
        int rnd = rand() % 1000;
        if ((ItemDataCount > GeneratorConfigs.GetAlreadyGeneratedItemAfterCount) && 
            ((GeneratorConfigs.ChanceGetAlreadyGeneratedItem * 10) > rnd))
        {
            DEBUG_MSG("Select existing item...");
            Array<C_ItemData*> reservoir = Array<C_ItemData*>();
            int powerMin = power * 0.75f;
            int powerMax = power * 1.25f;
            for (const auto& pair : ItemsData)
            {
                if (HasFlag(pair.second->Type, itemTypes) && !HasFlag(pair.second->Flags, ItemFlag_Crafted) && 
                    ((pair.second->Power >= powerMin) && (pair.second->Power <= powerMax))) reservoir.Insert(pair.second);
            }

            int foundCount = reservoir.GetNum();
            if (foundCount > 10)
            {
                int rnd = rand() % foundCount;
                C_ItemData* data = reservoir[rnd];
                return parser->GetIndex(data->InstanceName);
            }
        }

        // Generate item
        DEBUG_MSG("Generate new item...");
        int itemType = itemTypes;
        int itemInstnce = Invalid;
        zSTRING instName;
        instName = GetRandomPrototype(itemType, power);
        itemInstnce = parser->GetIndex(instName);

        if (itemInstnce == Invalid)
        {
            DEBUG_MSG("Generate new item prototype not found. ProtoName: " + Z instName);
            return Invalid;
        }
        DEBUG_MSG("Generate new item prototype instance: " + Z instName);
        return CreateEnchantedItem(itemInstnce, itemType, power);
    }

    void IdentifyItem(int instanceId, oCItem* item)
    {
        zCPar_Symbol* ps = parser->GetSymbol(instanceId);
        if (!ps)
        {
            DEBUG_MSG("IdentifyItem - instance not found. Id: " + Z instanceId);
            return;
        }

        C_ItemData* itemData = GetItemData(ps->name);
        if (!itemData)
        {
            DEBUG_MSG("IdentifyItem - item data not found. Name: " + ps->name);
            return;
        }

        C_EnchantmentData* enchantment = GetEnchantmentData(itemData->EnchantmentId);
        if (!enchantment)
        {
            DEBUG_MSG("IdentifyItem - enchantment data not found. Id: " + Z itemData->EnchantmentId);
            return;
        }

        itemData->Flags = itemData->Flags & ~ItemFlag_Undefined;
        enchantment->Flags = enchantment->Flags & ~ItemFlag_Undefined;
        oCItem* parentItem = dynamic_cast<oCItem*>(ogame->GetGameWorld()->
            CreateVob(zVOB_TYPE_ITEM, parser->GetIndex(itemData->BaseInstanceName)));
        item->value = parentItem->value + enchantment->Cost;
        parentItem->Release();
    }

    Array<oCItem*> GetEnchantedItems(int flags)
    {
        Array<oCItem*> items = Array<oCItem*>();
        if (!player || !player->inventory2.GetContents())
        {
            DEBUG_MSG("GetEnchantedItems - player seems not initialized!");
            return items;
        }

        auto contents = player->inventory2.GetContents();
        int size = contents->GetNumInList();
        int i = 0;
        zSTRING instName;
        while (i < size)
        {
            oCItem* pItem = contents->Get(i);
            if (!pItem) { i++; continue; }
            instName = pItem->GetInstanceName();
            if (!instName.StartWith(GenerateItemPrefix)) { i++; continue; }
            if (pItem->HasFlag(ITM_FLAG_ACTIVE)) { i++; continue; }
            if (flags != 0)
            {
                C_ItemData* data = GetItemData(instName);
                if (!data) { i++; continue; }
                if (!HasFlag(data->Type, flags)) { i++; continue; }
            }
            
            items.Insert(pItem);
            i++;
        }
        DEBUG_MSG("GetEnchantedItems - items found: " + Z (int)items.GetNum());
        return items;
    }

    Array<oCItem*> GetUndefinedItems()
    {
        Array<oCItem*> items = Array<oCItem*>();
        if (!player || !player->inventory2.GetContents())
        {
            DEBUG_MSG("GetUndefinedItems - player seems not initialized!");
            return items;
        }

        auto contents = player->inventory2.GetContents();
        int size = contents->GetNumInList();
        int i = 0;
        zSTRING instName;
        while (i < size)
        {
            oCItem* pItem = contents->Get(i);
            if (!pItem) { i++; continue; }
            instName = pItem->GetInstanceName();
            if (!instName.StartWith(GenerateItemPrefix)) { i++; continue; }

            C_ItemData* data = GetItemData(instName);
            if (!data) { i++; continue; }
            if (!HasFlag(data->Flags, ItemFlag_Undefined)) { i++; continue; }

            items.Insert(pItem);
            i++;
        }
        DEBUG_MSG("GetUndefinedItems - items found: " + Z(int)items.GetNum());
        return items;
    }

    /* *** Serialization *** */
#pragma region Serialization

    void ClearGeneratedItemsData()
    {
        for (std::map<std::string, C_ItemData*>::iterator it = ItemsData.begin(); it != ItemsData.end(); ++it)
            delete it->second;

        for (std::map<int, C_EnchantmentData*>::iterator it = EnchantmentsData.begin(); it != EnchantmentsData.end(); ++it)
            delete it->second;

        ItemsData.clear();
        EnchantmentsData.clear();

        LastEnchntmentId = 1;
        LastItemDataId = 1;
        EnchntmentsCount = 0;
        ItemDataCount = 0;
    }

    string GetGeneratedItemsArchivePath()
    {
        int slotID = SaveLoadGameInfo.slotID;
        string savesDir = zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES);
        string slotDir = GetSlotNameByID(slotID);
        string archivePath = string::Combine("%s\\%s\\StExt_GeneratedItems.sav", savesDir, slotDir);
        return archivePath;
    }

    void SaveGeneratedItems(zCArchiver& arc)
    {
        DEBUG_MSG("Start to save generated items...");        

        arc.WriteInt("LastEnchntmentId", LastEnchntmentId);
        arc.WriteInt("LastItemDataId", LastItemDataId);
        arc.WriteInt("EnchntmentsCount", EnchntmentsCount);
        arc.WriteInt("ItemDataCount", ItemDataCount);

        for (std::map<std::string, C_ItemData*>::iterator it = ItemsData.begin(); it != ItemsData.end(); ++it)
            it->second->Archive(arc);

        for (std::map<int, C_EnchantmentData*>::iterator it = EnchantmentsData.begin(); it != EnchantmentsData.end(); ++it)        
            it->second->Archive(arc);
        
        DEBUG_MSG("Generated items saved!");
    }
    void SaveGeneratedItems()
    {
        DEBUG_MSG("Save generated items...");
        string archiveName = GetGeneratedItemsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverWrite(archiveName, zARC_MODE_ASCII, 0, 0);
        if (ar)
        {
            SaveGeneratedItems(*ar);
            ar->Close();
            ar->Release();
        }
        DEBUG_MSG("Save generated items done!");
    }

    void LoadGeneratedItems(zCArchiver& arc)
    {
        DEBUG_MSG("Start to load generated items...");

        arc.ReadInt("LastEnchntmentId", LastEnchntmentId);
        arc.ReadInt("LastItemDataId", LastItemDataId);
        arc.ReadInt("EnchntmentsCount", EnchntmentsCount);
        arc.ReadInt("ItemDataCount", ItemDataCount);

        for (int i = 0; i < ItemDataCount; i++)
        {
            C_ItemData* data = new C_ItemData();
            data->Unarchive(arc);
            std::string indx = data->InstanceName;
            ItemsData.insert({ indx , data });
            int index = Invalid;
            CopySymbol(data->BaseInstanceName, data->InstanceName, index);
            if (index == Invalid)
                DEBUG_MSG("Item data '" + data->InstanceName + "' seems not placed in symbols table!");
        }

        for (int i = 0; i < EnchntmentsCount; i++)
        {
            C_EnchantmentData* data = new C_EnchantmentData();
            data->Unarchive(arc);
            EnchantmentsData.insert({ data->UId, data });
        }

        DEBUG_MSG("Generated items loaded!");
    }
    void LoadGeneratedItems()
    {
        DEBUG_MSG("Load generated items...");

        HideItemVisualEffect = parser->GetSymbol("StExt_Config_DisableEnchantedItemsEffects")->single_intdata;
        ClearGeneratedItemsData();
        string archiveName = GetGeneratedItemsArchivePath();
        zCArchiver* ar = zarcFactory->CreateArchiverRead(archiveName, 0);
        if (ar)
        {
            LoadGeneratedItems(*ar);
            ar->Close();
            ar->Release();
        }
        int count = ItemsData.size();
        DEBUG_MSG("Load generated items done (" + Z count + ")!");
    }

#pragma endregion

    /* *** Hooks *** */

    HOOK Hook_oCItem_InitByScript PATCH(&oCItem::InitByScript, &oCItem::InitByScript_StExt);
    void oCItem::InitByScript_StExt(int instance, int bInSaveGame)
    {
        THISCALL(Hook_oCItem_InitByScript)(instance, bInSaveGame);
        
        if ((instance != Invalid) && this)
        {
            int price = this->value;
            if ((price > 0) && (ItemBasePriceMult >= 0.1f))
            {
                price *= ItemBasePriceMult;
                if (price <= 0) price = 1;
            }
            this->value = price;
            if (Gothic_II_Addon::HasFlag(this->mainflag, item_kat_rune)) return;

            C_EnchantmentData* enchantment = GetEnchantmentData(this);
            if (!enchantment)
            {
                //DEBUG_MSG("oCItem::InitByScript_StExt for " + this->GetInstanceName() + " - enchntment is null!");
                return;
            }
            ApplyEnchntment(enchantment, this);
        }
    }

    HOOK Hook_oCNpc_AddItemEffects PATCH(&oCNpc::AddItemEffects, &oCNpc::AddItemEffects_StExt);
    void oCNpc::AddItemEffects_StExt(oCItem* item)
    {
        THISCALL(Hook_oCNpc_AddItemEffects)(item);

        if (!item || !IsSelfPlayer()) return;
        if (Gothic_II_Addon::HasFlag(item->mainflag, item_kat_rune)) return;
        if (!item->GetInstanceName().StartWith(GenerateItemPrefix)) return;

        C_EnchantmentData* enchantment = GetEnchantmentData(item);
        if (!enchantment)
        {
            DEBUG_MSG("Enchantment for " + item->GetInstanceName() + " not found!");
            return;
        }
        if (HasFlag(enchantment->Type, ItemType_Potion) || HasFlag(enchantment->Type, ItemType_Scroll)) return;

        zCPar_Symbol* StExt_PcStats_ItemsArray = parser->GetSymbol("StExt_PcStats_Items");
        if (!StExt_PcStats_ItemsArray)
        {
            DEBUG_MSG("StExt_PcStats_Items symbol not found!");
            return;
        }

        DEBUG_MSG("Apply enchantment bonus for " + item->name);
        for (int i = 0; i < EnchantStatsMax; i++)
        {
            if (enchantment->StatId[i] > Invalid)
            {
                int tmp = StExt_PcStats_ItemsArray->intdata[enchantment->StatId[i]];
                tmp += enchantment->StatValue[i];
                StExt_PcStats_ItemsArray->SetValue(tmp, enchantment->StatId[i]);
                parser->CallFunc(HandlePcStatChangeFunc, enchantment->StatId[i], enchantment->StatValue[i]);
            }            
        }
        ApplyItemAbilities(item, enchantment);
    }

    HOOK Hook_oCNpc_RemoveItemEffects PATCH(&oCNpc::RemoveItemEffects, &oCNpc::RemoveItemEffects_StExt);
    void oCNpc::RemoveItemEffects_StExt(oCItem* item)
    {
        THISCALL(Hook_oCNpc_RemoveItemEffects)(item);

        if (!item || !IsSelfPlayer()) return;
        if (Gothic_II_Addon::HasFlag(item->mainflag, item_kat_rune)) return;
        if (!item->GetInstanceName().StartWith(GenerateItemPrefix)) return;

        C_EnchantmentData* enchantment = GetEnchantmentData(item);
        if (!enchantment)
        {
            DEBUG_MSG("Enchantment for " + item->name + " not found!");
            return;
        }
        if (HasFlag(enchantment->Type, ItemType_Potion) || HasFlag(enchantment->Type, ItemType_Scroll)) return;

        zCPar_Symbol* StExt_PcStats_ItemsArray = parser->GetSymbol("StExt_PcStats_Items");        
        if (!StExt_PcStats_ItemsArray)
        {
            DEBUG_MSG("StExt_PcStats_Items symbol not found!");
            return;
        }

        DEBUG_MSG("Remove enchantment bonus for " + item->name);
        for (int i = 0; i < EnchantStatsMax; i++)
        {
            if (enchantment->StatId[i] > Invalid)
            {
                int tmp = StExt_PcStats_ItemsArray->intdata[enchantment->StatId[i]];
                tmp -= enchantment->StatValue[i];
                if (tmp < 0) tmp = 0;
                StExt_PcStats_ItemsArray->SetValue(tmp, enchantment->StatId[i]);
                parser->CallFunc(HandlePcStatChangeFunc, enchantment->StatId[i], -enchantment->StatValue[i]);
            }
        }
        RemoveItemAbilities(item, enchantment);
    }

    HOOK Hook_oCNpc_CanUse PATCH(&oCNpc::CanUse, &oCNpc::CanUse_StExt);
    int oCNpc::CanUse_StExt(oCItem* item)
    {
        if (item)
        {
            bool hasSpecialStats = false;
            for (int i = 0; i < 3; i++) {
                if (item->cond_atr[i] > ItemCondSpecialSeparator)
                {
                    hasSpecialStats = true;
                    break;
                }
            }

            // handle stat check on my own
            if (hasSpecialStats)
            {
                if (!this->IsAPlayer() || (this->GetInstanceName().Upper() == "PC_HEROMUL") || 
                    (this->GetInstanceName().Upper() == "STEXT_HEROSHADOW")) return TRUE;

                int skill = GetTalentSkill(7);
                if (skill < item->mag_circle)
                {
                    parser->SetInstance("SELF", this);
                    parser->SetInstance("ITEM", item);
                    int index = parser->GetIndex("G_CANNOTCAST");
                    parser->CallFunc(index, IsAPlayer(), item->mag_circle, skill);
                    return FALSE;
                }

                bool isSuccess = true;
                for (int i = 0; i < 3; i++)
                {                    
                    int checkResult = *(int*)parser->CallFunc(StExt_CheckConditionStatFunc, item->cond_atr[i], item->cond_value[i]);
                    if (!checkResult)
                    {
                        isSuccess = false;
                        break;
                    }
                }
                return isSuccess ? TRUE : FALSE;
            }
        }

        int result = THISCALL(Hook_oCNpc_CanUse)(item);
        if (!result && item)
        {
            if ((this->GetInstanceName().Upper() == "STEXT_HEROSHADOW") || (this->GetInstanceName().Upper() == "PC_HEROMUL"))
                return TRUE;
            if (!this->IsAPlayer() && item->GetInstanceName().Upper().StartWith(GenerateItemPrefix))
                return TRUE;
        }
        return result;
    }
}

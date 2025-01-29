#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    static std::map<int, zSTRING> ConditionsText = std::map<int, zSTRING>
    {
        {1, zSTRING("StExt_Str_Item_Cond_Hp") },
        {2, zSTRING("StExt_Str_Item_Cond_Mp") },
        {3, zSTRING("StExt_Str_Item_Cond_Mp") },
        {4, zSTRING("StExt_Str_Item_Cond_Str") },
        {5, zSTRING("StExt_Str_Item_Cond_Agi") },
        {92, zSTRING("StExt_Str_Item_Cond_St") },
        {90, zSTRING("StExt_Str_Item_Cond_Shield") },
        {89, zSTRING("StExt_Str_Item_Cond_Bow") },
        {91, zSTRING("StExt_Str_Item_Cond_CBow") },
        {1001, zSTRING("StExt_Str_Item_Cond_OneHanded") },
        {1002, zSTRING("StExt_Str_Item_Cond_TwoHanded") },
        {1003, zSTRING("StExt_Str_Item_Cond_Int") },
    };

    void DebugSymInfo(zCPar_Symbol* ps)
    {
        if (ps)
        {
            DEBUG_MSG("name: " + ps->name);
            DEBUG_MSG("type: " + Z(int) ps->type);
            DEBUG_MSG("ele: " + Z(int) ps->ele);
            DEBUG_MSG("flags: " + Z(int) ps->flags);
            DEBUG_MSG("filenr: " + Z(int) ps->filenr);
            DEBUG_MSG("line: " + Z(int) ps->line);
            DEBUG_MSG("line_anz: " + Z(int) ps->line_anz);
            DEBUG_MSG("pos_beg: " + Z(int) ps->pos_beg);
            DEBUG_MSG("pos_anz: " + Z(int) ps->pos_anz);
            DEBUG_MSG("offset: " + Z(int) ps->offset);
            if (ps->intdata) DEBUG_MSG("IntData - Yes");
            if (ps->floatdata) DEBUG_MSG("FloatData - Yes");
            if (ps->stringdata) DEBUG_MSG("StringData - Yes");
        }
    }

    zCPar_Symbol* CopySymbol(const zSTRING origName, zSTRING name, int& index)
    {
        DEBUG_MSG("Copy symbol '" + origName + "' to '" + name + "'...");
        zCPar_Symbol* ps = parser->GetSymbol(origName);
        if (!ps)
        {
            DEBUG_MSG("Original symbol '" + origName + "' not found!");
            return Null;
        }

        name = name.Upper();
        index = parser->GetIndex(name);
        if (index != Invalid)
        {
            DEBUG_MSG("'" + name + "' already in symbols table! Just update it...");

            zCPar_Symbol* sym = parser->GetSymbol(index);
            sym->name = name;
            sym->adr = ps->adr;
            sym->intdata = ps->intdata;
            sym->floatdata = ps->floatdata;
            sym->stringdata = ps->stringdata;
            sym->single_intdata = ps->single_intdata;
            sym->single_floatdata = ps->single_floatdata;
            sym->offset = ps->offset;
            sym->type = ps->type;
            sym->flags = ps->flags;
            sym->space = ps->space;
            sym->filenr = ps->filenr;
            sym->line = ps->line;
            sym->line_anz = ps->line_anz;
            sym->pos_beg = ps->pos_beg;
            sym->pos_anz = ps->pos_anz;
            sym->parent = ps->parent;

            return sym;
        }

        zCPar_Symbol* newSymbol = new zCPar_Symbol();
        newSymbol->name = name;
        newSymbol->adr = ps->adr;
        newSymbol->intdata = ps->intdata;
        newSymbol->floatdata = ps->floatdata;
        newSymbol->stringdata = ps->stringdata;
        newSymbol->single_intdata = ps->single_intdata;
        newSymbol->single_floatdata = ps->single_floatdata;
        newSymbol->offset = ps->offset;
        newSymbol->type = ps->type;
        newSymbol->flags = ps->flags;
        newSymbol->space = ps->space;
        newSymbol->filenr = ps->filenr;
        newSymbol->line = ps->line;
        newSymbol->line_anz = ps->line_anz;
        newSymbol->pos_beg = ps->pos_beg;
        newSymbol->pos_anz = ps->pos_anz;
        newSymbol->parent = ps->parent;

        if (!parser->symtab.Insert(newSymbol))
            DEBUG_MSG("Symbol '" + name + "' seems not inserted in table!");

        index = parser->GetIndex(name);
        if (index == Invalid)
            DEBUG_MSG("Symbol '" + name + "' not created in table!");
        return newSymbol;
    }

    zSTRING BuildEnchantedItemInstanceName(int itemId, int enchntmentId)
    {
        zSTRING result = zSTRING();
        int rnd = rand();
        result = GenerateItemPrefix + Z rand() + Z itemId + Z rand() + Z enchntmentId + Z rand();
        return result.Upper();
    }

    bool IsGeneratedItem(oCItem* item)
    {
        if (!item) return false;
        zSTRING instanceId = item->GetInstanceName();
        return instanceId.StartWith(GenerateItemPrefix);
    }

    inline Array<zSTRING>* SelectPrototypeByTier(int power, Array<zSTRING>* low, Array<zSTRING>* med, Array<zSTRING>* top)
    {
        if (power <= 0) return low;
        int pwrRnd = GetRandomRange(power * 0.5f, power);
        if (pwrRnd >= GeneratorConfigs.TopTierSeparator) return top;
        else if (pwrRnd >= GeneratorConfigs.MedTierSeparator) return med;       
        return low;
    }

    std::map<int, Array<zSTRING>*> GetItemsPrototypesListByTypes(int itemType, int power)
    {
        std::map<int, Array<zSTRING>*> result = std::map<int, Array<zSTRING>*>();
        result.clear();
        if (HasFlag(itemType, ItemType_Helm))
            result.insert({ ItemType_Helm, SelectPrototypeByTier(power, &GeneratorConfigs.HelmsLowPrototypes, &GeneratorConfigs.HelmsMedPrototypes, &GeneratorConfigs.HelmsTopPrototypes) });
        
        if (HasFlag(itemType, ItemType_Armor))
            result.insert({ ItemType_Armor, SelectPrototypeByTier(power, &GeneratorConfigs.ArmorsLowPrototypes, &GeneratorConfigs.ArmorsMedPrototypes, &GeneratorConfigs.ArmorsTopPrototypes) });
        
        if (HasFlag(itemType, ItemType_Pants)) 
            result.insert({ ItemType_Pants, SelectPrototypeByTier(power, &GeneratorConfigs.PantsLowPrototypes, &GeneratorConfigs.PantsMedPrototypes, &GeneratorConfigs.PantsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Boots)) 
            result.insert({ ItemType_Boots, SelectPrototypeByTier(power, &GeneratorConfigs.BootsLowPrototypes, &GeneratorConfigs.BootsMedPrototypes, &GeneratorConfigs.BootsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Gloves)) 
            result.insert({ ItemType_Gloves, SelectPrototypeByTier(power, &GeneratorConfigs.GlovesLowPrototypes, &GeneratorConfigs.GlovesMedPrototypes, &GeneratorConfigs.GlovesTopPrototypes) });

        if (HasFlag(itemType, ItemType_Belt)) 
            result.insert({ ItemType_Belt, SelectPrototypeByTier(power, &GeneratorConfigs.BeltsLowPrototypes, &GeneratorConfigs.BeltsMedPrototypes, &GeneratorConfigs.BeltsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Amulet)) 
            result.insert({ ItemType_Amulet, SelectPrototypeByTier(power, &GeneratorConfigs.AmuletsLowPrototypes, &GeneratorConfigs.AmuletsMedPrototypes, &GeneratorConfigs.AmuletsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Ring)) 
            result.insert({ ItemType_Ring, SelectPrototypeByTier(power, &GeneratorConfigs.RingsLowPrototypes, &GeneratorConfigs.RingsMedPrototypes, &GeneratorConfigs.RingsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Shield)) 
            result.insert({ ItemType_Shield, SelectPrototypeByTier(power, &GeneratorConfigs.ShieldsLowPrototypes, &GeneratorConfigs.ShieldsMedPrototypes, &GeneratorConfigs.ShieldsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Sword1h)) 
            result.insert({ ItemType_Sword1h, SelectPrototypeByTier(power, &GeneratorConfigs.Swords1hLowPrototypes, &GeneratorConfigs.Swords1hMedPrototypes, &GeneratorConfigs.Swords1hTopPrototypes) });

        if (HasFlag(itemType, ItemType_Sword2h)) 
            result.insert({ ItemType_Sword2h, SelectPrototypeByTier(power, &GeneratorConfigs.Swords2hLowPrototypes, &GeneratorConfigs.Swords2hMedPrototypes, &GeneratorConfigs.Swords2hTopPrototypes) });

        if (HasFlag(itemType, ItemType_Axe1h)) 
            result.insert({ ItemType_Axe1h, SelectPrototypeByTier(power, &GeneratorConfigs.Axes1hLowPrototypes, &GeneratorConfigs.Axes1hMedPrototypes, &GeneratorConfigs.Axes1hTopPrototypes) });

        if (HasFlag(itemType, ItemType_Axe2h))
            result.insert({ ItemType_Axe2h, SelectPrototypeByTier(power, &GeneratorConfigs.Axes2hLowPrototypes, &GeneratorConfigs.Axes2hMedPrototypes, &GeneratorConfigs.Axes2hTopPrototypes) });

        if (HasFlag(itemType, ItemType_Mace1h)) 
            result.insert({ ItemType_Mace1h, SelectPrototypeByTier(power, &GeneratorConfigs.Maces1hLowPrototypes, &GeneratorConfigs.Maces1hMedPrototypes, &GeneratorConfigs.Maces1hTopPrototypes) });

        if (HasFlag(itemType, ItemType_Mace2h)) 
            result.insert({ ItemType_Mace2h, SelectPrototypeByTier(power, &GeneratorConfigs.Maces2hLowPrototypes, &GeneratorConfigs.Maces2hMedPrototypes, &GeneratorConfigs.Maces2hTopPrototypes) });

        if (HasFlag(itemType, ItemType_MagicSword)) 
            result.insert({ ItemType_MagicSword, SelectPrototypeByTier(power, &GeneratorConfigs.MagicSwordsLowPrototypes, &GeneratorConfigs.MagicSwordsMedPrototypes, &GeneratorConfigs.MagicSwordsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Staff)) 
            result.insert({ ItemType_Staff, SelectPrototypeByTier(power, &GeneratorConfigs.StaffsLowPrototypes, &GeneratorConfigs.StaffsMedPrototypes, &GeneratorConfigs.StaffsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Rapier))
            result.insert({ ItemType_Rapier, SelectPrototypeByTier(power, &GeneratorConfigs.RapiersLowPrototypes, &GeneratorConfigs.RapiersMedPrototypes, &GeneratorConfigs.RapiersTopPrototypes) });

        if (HasFlag(itemType, ItemType_DexSword)) 
            result.insert({ ItemType_DexSword, SelectPrototypeByTier(power, &GeneratorConfigs.DexSwordsLowPrototypes, &GeneratorConfigs.DexSwordsMedPrototypes, &GeneratorConfigs.DexSwordsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Halleberd)) 
            result.insert({ ItemType_Halleberd, SelectPrototypeByTier(power, &GeneratorConfigs.HalleberdsLowPrototypes, &GeneratorConfigs.HalleberdsMedPrototypes, &GeneratorConfigs.HalleberdsTopPrototypes) });

        if (HasFlag(itemType, ItemType_Spear)) 
            result.insert({ ItemType_Spear, SelectPrototypeByTier(power, &GeneratorConfigs.SpearsLowPrototypes, &GeneratorConfigs.SpearsMedPrototypes, &GeneratorConfigs.SpearsTopPrototypes) });

        if (HasFlag(itemType, ItemType_DualL))
            result.insert({ ItemType_DualL, SelectPrototypeByTier(power, &GeneratorConfigs.DualsLLowPrototypes, &GeneratorConfigs.DualsLMedPrototypes, &GeneratorConfigs.DualsLTopPrototypes) });

        if (HasFlag(itemType, ItemType_DualR)) 
            result.insert({ ItemType_DualR, SelectPrototypeByTier(power, &GeneratorConfigs.DualsRLowPrototypes, &GeneratorConfigs.DualsRMedPrototypes, &GeneratorConfigs.DualsRTopPrototypes) });

        if (HasFlag(itemType, ItemType_Bow)) 
            result.insert({ ItemType_Bow, SelectPrototypeByTier(power, &GeneratorConfigs.BowsLowPrototypes, &GeneratorConfigs.BowsMedPrototypes, &GeneratorConfigs.BowsTopPrototypes) });

        if (HasFlag(itemType, ItemType_CrossBow)) 
            result.insert({ ItemType_CrossBow, SelectPrototypeByTier(power, &GeneratorConfigs.CrossBowsLowPrototypes, &GeneratorConfigs.CrossBowsMedPrototypes, &GeneratorConfigs.CrossBowsTopPrototypes) });

        if (HasFlag(itemType, ItemType_MagicStaff)) 
            result.insert({ ItemType_MagicStaff, SelectPrototypeByTier(power, &GeneratorConfigs.MagicStaffsLowPrototypes, &GeneratorConfigs.MagicStaffsMedPrototypes, &GeneratorConfigs.MagicStaffsTopPrototypes) });
        
        if (HasFlag(itemType, ItemType_Torso))
            result.insert({ ItemType_Torso, SelectPrototypeByTier(power, &GeneratorConfigs.TorsoLowPrototypes, &GeneratorConfigs.TorsoMedPrototypes, &GeneratorConfigs.TorsoTopPrototypes) });

        if (HasFlag(itemType, ItemType_Potion))
        {
            if (power > 150) result.insert({ ItemType_Potion, &GeneratorConfigs.Potions4Prototypes });
            else if (power > 100) result.insert({ ItemType_Potion, &GeneratorConfigs.Potions3Prototypes });
            else if (power > 50) result.insert({ ItemType_Potion, &GeneratorConfigs.Potions2Prototypes });
            else result.insert({ ItemType_Potion, &GeneratorConfigs.Potions1Prototypes });
        }
        if (HasFlag(itemType, ItemType_Scroll)) result.insert({ ItemType_Scroll, &GeneratorConfigs.ScrollsPrototypes });
        
        DEBUG_MSG("GetItemsPrototypesListByTypes - found prototypes count: " + Z (int)result.size());
        return result;
    }

    zSTRING GetRandomPrototype(int& itemType, int power)
    {
        std::map<int, Array<zSTRING>*> prototypesList = GetItemsPrototypesListByTypes(itemType, power);
        zSTRING result = zSTRING();
        int prototypesSize = prototypesList.size();
        
        DEBUG_MSG("GetRandomPrototype - found prototypes: " + Z prototypesSize);
        if (prototypesSize == 0) return result;

        int chance = 1000 / prototypesSize;
        bool itemFound = false;
        for (int i = 0; i <= ItemTypesMax; i++)
        {
            int indx = 1 << i;
            bool chanceProc = (rand() % 1000) <= chance;
            if ((prototypesList.find(indx) != prototypesList.end()) && (chanceProc || !itemFound))
            {
                Array<zSTRING> elements = *prototypesList.at(indx);
                if (elements.GetNum() <= 0)
                {
                    DEBUG_MSG("GetRandomPrototype - prototypes list with type: " + Z indx + " is empty!");
                    continue;
                }
                int rnd = rand() % elements.GetNum();
                result = elements[rnd];
                itemType = indx;
                itemFound = true;
            }
            if (itemFound && ((rand() % 1000) <= 100)) break;
        }
        if (!itemFound)
        {
            DEBUG_MSG("GetRandomPrototype - prototype not found!");
            return result;
        }
        DEBUG_MSG("GetRandomPrototype - return prototype: " + Z result);
        return result;
    }

    zSTRING GetRandomElement(Array<zSTRING>* arr)
    {
        Array<zSTRING> elements = *arr;
        int rnd = rand() % elements.GetNum();
        return elements[rnd];
    }

    void SetItemAffixes(zSTRING& preffix, zSTRING& affix, zSTRING& suffix)
    {        
        preffix = zSTRING();
        affix = zSTRING();
        suffix = zSTRING();

        int rnd = rand() % 1000;
        if (rnd > 500) preffix = GetRandomElement(&GeneratorConfigs.Preffixes);
        rnd = rand() % 1000;
        if (rnd > 250) affix = GetRandomElement(&GeneratorConfigs.Affixes);
        rnd = rand() % 1000;
        if (rnd > 500) suffix = GetRandomElement(&GeneratorConfigs.Suffixes);

        if (preffix.IsEmpty() && affix.IsEmpty() && suffix.IsEmpty())
        {
            rnd = rand() % 1000;
            if(rnd > 800) affix = GetRandomElement(&GeneratorConfigs.Affixes);
            else if (rnd > 400) preffix = GetRandomElement(&GeneratorConfigs.Preffixes);
            else suffix = GetRandomElement(&GeneratorConfigs.Preffixes);
        }
    }

    inline void AppendTag(zSTRING& line, zSTRING tag)
    {
        if (!line.IsEmpty()) line += " | ";
        line += tag;
    }
    inline void ClearItemText(oCItem* item)
    {
        for (int i = 0; i < 6; i++)
        {
            item->text[i] = zSTRING();
            item->count[i] = 0;
        }
        item->count[5] = item->value;
    }

    inline bool IsWeapon(int flags) { return HasFlag(flags, ItemType_Sword1h) || HasFlag(flags, ItemType_Sword2h) || HasFlag(flags, ItemType_Axe1h) ||
        HasFlag(flags, ItemType_Axe2h) || HasFlag(flags, ItemType_Mace1h) || HasFlag(flags, ItemType_Mace2h) || HasFlag(flags, ItemType_MagicSword) || HasFlag(flags, ItemType_Staff) ||
        HasFlag(flags, ItemType_Rapier) || HasFlag(flags, ItemType_DexSword) || HasFlag(flags, ItemType_Halleberd) || HasFlag(flags, ItemType_Spear) || HasFlag(flags, ItemType_DualL) || 
        HasFlag(flags, ItemType_DualR) || HasFlag(flags, ItemType_Bow) || HasFlag(flags, ItemType_CrossBow) || HasFlag(flags, ItemType_MagicStaff); }
    inline bool IsJewelry(int flags) { return HasFlag(flags, ItemType_Belt) || HasFlag(flags, ItemType_Amulet) || HasFlag(flags, ItemType_Ring); }
    inline bool IsUsable(int flags) { return HasFlag(flags, ItemType_Potion) || HasFlag(flags, ItemType_Scroll); }
    inline bool IsArmor(int flags) { return HasFlag(flags, ItemType_Helm) || HasFlag(flags, ItemType_Armor) || HasFlag(flags, ItemType_Pants) || HasFlag(flags, ItemType_Boots) || HasFlag(flags, ItemType_Shield) || HasFlag(flags, ItemType_Gloves) || HasFlag(flags, ItemType_Torso); }

    inline zSTRING BuildItemRequirementsString(C_EnchantmentData* enchantment, oCItem* item)
    {
        zSTRING result = parser->GetSymbol("StExt_Str_Item_Cond")->stringdata;
        zSTRING cond = zSTRING();
        zSTRING tmp = zSTRING();
        for (int i = 0; i < EnchantConditionsMax; i++)
        {
            if (item->cond_value[i] > 0)
            {
                auto it = ConditionsText.find(item->cond_atr[i]);
                int val = item->cond_value[i];
                val = item->cond_atr[i] == 92 ? val * 10 : val;

                if (it != ConditionsText.end())
                {
                    if (item->cond_atr[i] == 90)
                    {
                        AppendTag(cond, parser->GetSymbol(it->second)->stringdata);
                        continue;
                    }

                    tmp = Z val + " " + parser->GetSymbol(it->second)->stringdata;
                    AppendTag(cond, tmp);
                }
                else
                {
                    tmp = Z item->cond_value[i] + " ??? (" + Z item->cond_atr[i] + ")";
                    AppendTag(cond, tmp);
                }
            }            
        }
        if (cond.IsEmpty())
            cond = parser->GetSymbol("StExt_Str_No")->stringdata;
        result += cond;
        return result;
    }

    void BuildItemText(C_EnchantmentData* enchantment, oCItem* item)
    {
        //item->text[0] = BuildItemFlagsString(enchantment, item);
        zCPar_Symbol* str_Item_DamageType = parser->GetSymbol("StExt_Str_Item_DamageType");
        
        if (IsJewelry(enchantment->Type))
        {
            //ClearItemText(item);
            item->text[0] = BuildItemRequirementsString(enchantment, item);
        }
        else if (IsWeapon(enchantment->Type))
        {
            ClearItemText(item);         

            // build damage line
            zSTRING damageLine = parser->GetSymbol("StExt_Str_Item_Damage")->stringdata + Z item->damageTotal;
            zSTRING damageTags = new zSTRING();
            for (int i = 0; i < oEDamageIndex_MAX; i++)
            {
                if (item->damage[i] > 0)
                {
                    zSTRING tmp = Z item->damage[i] + " " + str_Item_DamageType->stringdata[i];
                    AppendTag(damageTags, tmp);
                }
            }
            damageLine += " (" + damageTags + ")";

            // build range line
            zSTRING rangeLine = parser->GetSymbol("StExt_Str_Item_Range")->stringdata + Z item->range;
            zSTRING weapTags = zSTRING();
            for (int i = 0; i < 28; i++)
            {
                int indx = 1 << i;
                if (HasFlag(enchantment->Type, indx))
                    AppendTag(weapTags, parser->GetSymbol("StExt_Str_ItemType")->stringdata[i]);
            }
            if (HasFlag(enchantment->Type, ItemType_MagicSword))
            {
                if (HasFlag(enchantment->Flags, ItemFlag_BothHands))
                    AppendTag(weapTags, parser->GetSymbol("StExt_Str_TwoHandWeapon")->stringdata);
                else
                    AppendTag(weapTags, parser->GetSymbol("StExt_Str_OneHandWeapon")->stringdata);
            }
            if (HasFlag(item->hitp, bit_item_orc_weapon))
                AppendTag(weapTags, parser->GetSymbol("StExt_Str_OrcWeapon")->stringdata);
            AppendTag(rangeLine, weapTags);

            // build prot line
            zSTRING protLine = zSTRING();
            zSTRING protTags = zSTRING();
            for (int i = 0; i < oEDamageIndex_MAX; i++)
            {
                if (item->protection[i] > 0)
                {
                    zSTRING tmp = Z item->protection[i] + " " + str_Item_DamageType->stringdata[i];
                    AppendTag(protTags, tmp);
                }
            }
            if (!protTags.IsEmpty())
                protLine = parser->GetSymbol("StExt_Str_Item_Protection")->stringdata + protTags;

            item->text[1] = damageLine;
            item->text[2] = rangeLine;
            item->text[3] = protLine;
            item->text[4] = BuildItemRequirementsString(enchantment, item);
        }
        else if (IsArmor(enchantment->Type))
        {
            ClearItemText(item);
            zCPar_Symbol* str_Item_Protection = parser->GetSymbol("StExt_Str_Item_Protection");

            zSTRING protLine1 = zSTRING();
            zSTRING protLine2 = zSTRING();
            zSTRING protLine3 = zSTRING();
            zSTRING protTags = zSTRING();
            zSTRING tmp;

            protLine1 = str_Item_Protection->stringdata;
            tmp = Z item->protection[1] + " " + str_Item_DamageType->stringdata[1];
            AppendTag(protTags, tmp);
            tmp = Z item->protection[2] + " " + str_Item_DamageType->stringdata[2];
            AppendTag(protTags, tmp);
            tmp = Z item->protection[6] + " " + str_Item_DamageType->stringdata[6];
            AppendTag(protTags, tmp);
            protLine1 += protTags;
            protTags = zSTRING();

            protLine2 = str_Item_Protection->stringdata;
            tmp = Z item->protection[3] + " " + str_Item_DamageType->stringdata[3];
            AppendTag(protTags, tmp);
            tmp = Z item->protection[5] + " " + str_Item_DamageType->stringdata[5];
            AppendTag(protTags, tmp);
            protLine2 += protTags;
            protTags = zSTRING();

            protLine3 = str_Item_Protection->stringdata;
            tmp = Z item->protection[4] + " " + str_Item_DamageType->stringdata[4];
            AppendTag(protTags, tmp);
            tmp = Z item->protection[7] + " " + str_Item_DamageType->stringdata[7];
            AppendTag(protTags, tmp);
            protLine3 += protTags;
            protTags = zSTRING();

            item->text[1] = protLine1;
            item->text[2] = protLine2;
            item->text[3] = protLine3;
            item->text[4] = BuildItemRequirementsString(enchantment, item);
        }
        else if (IsUsable(enchantment->Type))
        {
            //ClearItemText(item);
        }

        item->text[5] = parser->GetSymbol("StExt_Str_ItemRank")->stringdata[enchantment->Rank] +
            " | " + parser->GetSymbol("StExt_Enchanted_Name_Value")->stringdata;
        /*
        DEBUG_MSG(item->name + " Text[0] = " + item->text[0]);
        DEBUG_MSG(item->name + " Text[1] = " + item->text[1]);
        DEBUG_MSG(item->name + " Text[2] = " + item->text[2]);
        DEBUG_MSG(item->name + " Text[3] = " + item->text[3]);
        DEBUG_MSG(item->name + " Text[4] = " + item->text[4]);
        DEBUG_MSG(item->name + " Text[5] = " + item->text[5]);
        */
    }
}
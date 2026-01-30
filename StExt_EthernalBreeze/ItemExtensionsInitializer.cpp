#include <JsonParser.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{	
	void InitItemGeneratorConfigs()
	{
		DEBUG_MSG("");
		DEBUG_MSG("InitItemGeneratorConfigs: start (re)initialization item generator configs...");
		ItemsGeneratorConfigs = ItemsGeneratorConfig();
		const zSTRING path = zoptions->GetDirString(zTOptionPaths::DIR_EXECUTABLE) + "Autorun\\EthernalBreezeConfigs\\ItemGeneratorConfigs.json";

		if (!ReadItemsGeneratorConfigs(path, ItemsGeneratorConfigs))
		{
			DEBUG_MSG("InitItemGeneratorConfigs: Fail to read basic ItemGeneratorConfigs! Path: " + path);
			Message::Error((string)("Fail to read basic ItemGeneratorConfigs!\nPath: " + path), "Critical error!");
			gameMan->ExitGame();
			return;
		}
		DEBUG_MSG("InitItemGeneratorConfigs: Ok!");
		DEBUG_MSG("");
	}

	bool SelectItemGeneratorConfigs(const zSTRING& fileName)
	{
		DEBUG_MSG("");
		DEBUG_MSG("SelectItemGeneratorConfigs: start change item generator configs. New configs name: '" + fileName + "'...");
		const zSTRING path = zoptions->GetDirString(zTOptionPaths::DIR_EXECUTABLE) + "Autorun\\EthernalBreezeConfigs\\" + fileName;
		ItemsGeneratorConfig newConfigs = ItemsGeneratorConfig();

		if (!ReadItemsGeneratorConfigs(path, newConfigs))
		{
			DEBUG_MSG("SelectItemGeneratorConfigs: Fail to read ItemGeneratorConfigs! Path: " + path);
			return false;
		}

		ItemsGeneratorConfigs = newConfigs;
		DEBUG_MSG("SelectItemGeneratorConfigs: Item generator configs updated!");
		DEBUG_MSG("");
		return true;
	}


	void InitItemsExtensionNameData(Array<zSTRING>& nameData, const zSTRING& initArray)
	{
		zCParser* par = zCParser::GetParser();
		zCPar_Symbol* nameArray = par->GetSymbol(initArray);
		if (!nameArray)
		{
			DEBUG_MSG("InitItemsExtensionNameData - '" + initArray + "' not found!");
			return;
		}
		DEBUG_MSG("InitItemsExtensionNameData - Init names from: " + initArray + ". size: " + Z((int)nameArray->ele) + " ...");

		nameData = Array<zSTRING>();
		for (uint i = 0; i < nameArray->ele; ++i) {
			nameData.InsertEnd(nameArray->stringdata[i]);
		}
		DEBUG_MSG("InitItemsExtensionNameData - '" + initArray + "' was initialized!");
	}

	void InitItemsExtensionNames()
	{
		InitItemsExtensionNameData(ItemPreffixesList, "StExt_Str_ItemPreffixName");
		InitItemsExtensionNameData(ItemAffixesList, "StExt_Str_ItemAffixName");
		InitItemsExtensionNameData(ItemSuffixesList, "StExt_Str_ItemSuffixName");
		InitItemsExtensionNameData(ItemNamesList, "StExt_Str_ItemSpecialName");
		InitItemsExtensionNameData(ItemRanksList, "StExt_Str_ItemRank");
		InitItemsExtensionNameData(ItemDamageTypesList, "StExt_Str_Item_DamageType");

		ItemNameValueString = parser->GetSymbol("StExt_Enchanted_Name_Value")->stringdata;
		ItemDamageString = parser->GetSymbol("StExt_Str_Item_Damage")->stringdata;
		ItemConditionString = parser->GetSymbol("StExt_Str_Item_Cond")->stringdata;
		ItemRangeString = parser->GetSymbol("StExt_Str_Item_Range")->stringdata;
		ItemOrcWeaponTagString = parser->GetSymbol("StExt_Str_OrcWeapon")->stringdata;
		ItemProtectionString = parser->GetSymbol("StExt_Str_Item_Protection")->stringdata;
		ItemWeightString = parser->GetSymbol("rx_inv_descarmor_weight")->stringdata;
	}

	void InitItemsConditionsNames()
	{
		DEBUG_MSG("InitItemsConditionsNames...");
		ItemConditionsText = Map<int, zSTRING>();
		ItemConditionsText.Insert(1, parser->GetSymbol("StExt_Str_Item_Cond_Hp")->stringdata);
		ItemConditionsText.Insert(2, parser->GetSymbol("StExt_Str_Item_Cond_Mp")->stringdata);
		ItemConditionsText.Insert(3, parser->GetSymbol("StExt_Str_Item_Cond_Mp")->stringdata);
		ItemConditionsText.Insert(4, parser->GetSymbol("StExt_Str_Item_Cond_Str")->stringdata);
		ItemConditionsText.Insert(5, parser->GetSymbol("StExt_Str_Item_Cond_Agi")->stringdata);
		ItemConditionsText.Insert(89, parser->GetSymbol("StExt_Str_Item_Cond_Bow")->stringdata);
		ItemConditionsText.Insert(90, parser->GetSymbol("StExt_Str_Item_Cond_Shield")->stringdata);
		ItemConditionsText.Insert(91, parser->GetSymbol("StExt_Str_Item_Cond_CBow")->stringdata);
		ItemConditionsText.Insert(92, parser->GetSymbol("StExt_Str_Item_Cond_St")->stringdata);
		ItemConditionsText.Insert(1001, parser->GetSymbol("StExt_Str_Item_Cond_OneHanded")->stringdata);
		ItemConditionsText.Insert(1002, parser->GetSymbol("StExt_Str_Item_Cond_TwoHanded")->stringdata);
		ItemConditionsText.Insert(1003, parser->GetSymbol("StExt_Str_Item_Cond_Int")->stringdata);
		ItemConditionsText.Insert(1004, parser->GetSymbol("StExt_Str_Item_Cond_Level")->stringdata);
		ItemConditionsText.Insert(1005, parser->GetSymbol("StExt_Str_Item_Cond_BeliarKarma")->stringdata);
		ItemConditionsText.Insert(1006, parser->GetSymbol("StExt_Str_Item_Cond_InnosKarma")->stringdata);
		ItemConditionsText.Insert(1007, parser->GetSymbol("StExt_Str_Item_Cond_AdanosKarma")->stringdata);
	}

	void InitItemsTypesNames()
	{
		DEBUG_MSG("InitItemsTypesNames...");
		ItemTypesText = Map<byte, zSTRING>();
		zCPar_Symbol* arraySym = parser->GetSymbol("StExt_Str_Item_Type");
		if (!arraySym)
		{
			DEBUG_MSG("InitItemsTypesNames - symbol 'StExt_Str_Item_Type' not found!");
			return;
		}

		ItemTypesText.Insert((byte)ItemType::Unknown, arraySym->stringdata[0]);
		ItemTypesText.Insert((byte)ItemType::Armor, arraySym->stringdata[1]);
		ItemTypesText.Insert((byte)ItemType::Weapon, arraySym->stringdata[2]);
		ItemTypesText.Insert((byte)ItemType::Jewelry, arraySym->stringdata[3]);
		ItemTypesText.Insert((byte)ItemType::Consumable, arraySym->stringdata[4]);
		ItemTypesText.Insert((byte)ItemType::Munition, arraySym->stringdata[5]);
		ItemTypesText.Insert((byte)ItemType::Inventory, arraySym->stringdata[6]);
		ItemTypesText.Insert((byte)ItemType::Craft, arraySym->stringdata[7]);
		ItemTypesText.Insert((byte)ItemType::Other, arraySym->stringdata[8]);
	}

	void InitItemsClassesNames()
	{
		DEBUG_MSG("InitItemsClassesNames...");
		ItemClassesText = Map<byte, zSTRING>();
		zCPar_Symbol* arraySym = parser->GetSymbol("StExt_Str_Item_Class");
		if (!arraySym)
		{
			DEBUG_MSG("InitItemsTypesNames - symbol 'StExt_Str_Item_Class' not found!");
			return;
		}

		ItemClassesText.Insert((byte)ItemClass::Unknown, arraySym->stringdata[0]);

		ItemClassesText.Insert((byte)ItemClass::FullArmor, arraySym->stringdata[1]);
		ItemClassesText.Insert((byte)ItemClass::Helm, arraySym->stringdata[2]);
		ItemClassesText.Insert((byte)ItemClass::Torso, arraySym->stringdata[3]);
		ItemClassesText.Insert((byte)ItemClass::Pants, arraySym->stringdata[4]);
		ItemClassesText.Insert((byte)ItemClass::Gloves, arraySym->stringdata[5]);
		ItemClassesText.Insert((byte)ItemClass::Boots, arraySym->stringdata[6]);
		ItemClassesText.Insert((byte)ItemClass::Pauldrons, arraySym->stringdata[7]);
		ItemClassesText.Insert((byte)ItemClass::Shield, arraySym->stringdata[8]);

		ItemClassesText.Insert((byte)ItemClass::MeeleWeapon, arraySym->stringdata[9]);
		ItemClassesText.Insert((byte)ItemClass::RangeWeapon, arraySym->stringdata[10]);
		ItemClassesText.Insert((byte)ItemClass::CastWeapon, arraySym->stringdata[11]);

		ItemClassesText.Insert((byte)ItemClass::Amulet, arraySym->stringdata[12]);
		ItemClassesText.Insert((byte)ItemClass::Ring, arraySym->stringdata[13]);
		ItemClassesText.Insert((byte)ItemClass::Earing, arraySym->stringdata[14]);
		ItemClassesText.Insert((byte)ItemClass::Braclets, arraySym->stringdata[15]);
		ItemClassesText.Insert((byte)ItemClass::Belt, arraySym->stringdata[16]);
		ItemClassesText.Insert((byte)ItemClass::Collar, arraySym->stringdata[17]);
		ItemClassesText.Insert((byte)ItemClass::Trophy, arraySym->stringdata[18]);

		ItemClassesText.Insert((byte)ItemClass::Potion, arraySym->stringdata[19]);
		ItemClassesText.Insert((byte)ItemClass::Scroll, arraySym->stringdata[20]);
		ItemClassesText.Insert((byte)ItemClass::Food, arraySym->stringdata[21]);

		ItemClassesText.Insert((byte)ItemClass::Charm, arraySym->stringdata[22]);

		ItemClassesText.Insert((byte)ItemClass::Jewel, arraySym->stringdata[23]);
		ItemClassesText.Insert((byte)ItemClass::Rune, arraySym->stringdata[24]);
		ItemClassesText.Insert((byte)ItemClass::SoulEssence, arraySym->stringdata[25]);

		ItemClassesText.Insert((byte)ItemClass::Misk, arraySym->stringdata[26]);
	}

	void InitItemsSubClassesNames()
	{
		DEBUG_MSG("InitItemsSubClassesNames...");
		ItemSubClassesText = Map<byte, zSTRING>();
		zCPar_Symbol* arraySym = parser->GetSymbol("StExt_Str_Item_SubClass");
		if (!arraySym)
		{
			DEBUG_MSG("InitItemsTypesNames - symbol 'StExt_Str_Item_SubClass' not found!");
			return;
		}

		ItemSubClassesText.Insert((byte)ItemSubClass::Unknown, arraySym->stringdata[0]);

		ItemSubClassesText.Insert((byte)ItemSubClass::Clothing, arraySym->stringdata[1]);
		ItemSubClassesText.Insert((byte)ItemSubClass::LightArmor, arraySym->stringdata[2]);
		ItemSubClassesText.Insert((byte)ItemSubClass::MediumArmor, arraySym->stringdata[3]);
		ItemSubClassesText.Insert((byte)ItemSubClass::HeavyArmor, arraySym->stringdata[4]);

		ItemSubClassesText.Insert((byte)ItemSubClass::Hood, arraySym->stringdata[5]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Circlet, arraySym->stringdata[6]);
		ItemSubClassesText.Insert((byte)ItemSubClass::LightHelm, arraySym->stringdata[7]);
		ItemSubClassesText.Insert((byte)ItemSubClass::MediumHelm, arraySym->stringdata[8]);
		ItemSubClassesText.Insert((byte)ItemSubClass::HeavyHelm, arraySym->stringdata[9]);

		ItemSubClassesText.Insert((byte)ItemSubClass::LightShield, arraySym->stringdata[10]);
		ItemSubClassesText.Insert((byte)ItemSubClass::HeavyShield, arraySym->stringdata[11]);

		ItemSubClassesText.Insert((byte)ItemSubClass::Sword1H, arraySym->stringdata[12]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Axe1H, arraySym->stringdata[13]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Mace1H, arraySym->stringdata[14]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Dagger, arraySym->stringdata[15]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Rapier, arraySym->stringdata[16]);
		ItemSubClassesText.Insert((byte)ItemSubClass::DexSword, arraySym->stringdata[17]);
		ItemSubClassesText.Insert((byte)ItemSubClass::MagicSword, arraySym->stringdata[18]);

		ItemSubClassesText.Insert((byte)ItemSubClass::Sword2H, arraySym->stringdata[19]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Axe2H, arraySym->stringdata[20]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Mace2H, arraySym->stringdata[21]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Staff, arraySym->stringdata[22]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Spear, arraySym->stringdata[23]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Halleberd, arraySym->stringdata[24]);

		ItemSubClassesText.Insert((byte)ItemSubClass::DualL, arraySym->stringdata[25]);
		ItemSubClassesText.Insert((byte)ItemSubClass::DualR, arraySym->stringdata[26]);
		ItemSubClassesText.Insert((byte)ItemSubClass::FistWeapon, arraySym->stringdata[27]);
		ItemSubClassesText.Insert((byte)ItemSubClass::ThrowableDagger, arraySym->stringdata[28]);
		ItemSubClassesText.Insert((byte)ItemSubClass::ThrowableAxe, arraySym->stringdata[29]);
		ItemSubClassesText.Insert((byte)ItemSubClass::ThrowableMace, arraySym->stringdata[30]);
		ItemSubClassesText.Insert((byte)ItemSubClass::ThrowableSpear, arraySym->stringdata[31]);

		ItemSubClassesText.Insert((byte)ItemSubClass::LightBow, arraySym->stringdata[32]);
		ItemSubClassesText.Insert((byte)ItemSubClass::HeavyBow, arraySym->stringdata[33]);
		ItemSubClassesText.Insert((byte)ItemSubClass::LightCrossBow, arraySym->stringdata[34]);
		ItemSubClassesText.Insert((byte)ItemSubClass::HeavyCrossBow, arraySym->stringdata[35]);

		ItemSubClassesText.Insert((byte)ItemSubClass::Scroll, arraySym->stringdata[36]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Rune, arraySym->stringdata[37]);
		ItemSubClassesText.Insert((byte)ItemSubClass::Wand, arraySym->stringdata[38]);
	}


	//-----------------------------------------------------------------------------
	//								Entry point
	//-----------------------------------------------------------------------------

	void InitItemsExtension()
	{
		InitItemGeneratorConfigs();
		InitItemsClassData();
		InitItemsExtensionNames();
		InitItemsConditionsNames();
		InitItemsTypesNames();
		InitItemsClassesNames();
		InitItemsSubClassesNames();

		ItemExtensionsState = ItemExtensionsCoreState();
		ItemsExtensionData = new ItemExtensionDataStorage();
	}
}
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	const zCOLOR ItemRankColor[] =
	{
		zCOLOR(235, 235, 235, 255),		// rank 0
		zCOLOR(0, 128, 255, 255),		// rank 1
		zCOLOR(128, 255, 0, 255),		// rank 2
		zCOLOR(255, 128, 64, 255),		// rank 3
		zCOLOR(255, 128, 255, 255),		// rank 4
		zCOLOR(128, 0, 255, 255),		// rank >=5
	};

	const zCOLOR ItemTextColor_Default = zCOLOR(235, 235, 235, 255);
	const zCOLOR ItemTextColor_Default_Faded = zCOLOR(200, 200, 200, 255);
	const zCOLOR ItemTextColor_Gold = zCOLOR(235, 235, 0, 255);
	const zCOLOR ItemTextColor_Orange = zCOLOR(235, 180, 235, 255);
	const zCOLOR ItemTextColor_Red = zCOLOR(235, 0, 0, 255);
	const zCOLOR ItemTextColor_Green = zCOLOR(0, 220, 0, 255);
	const zCOLOR ItemTextColor_Blue = zCOLOR(0, 125, 235, 255);
	const zCOLOR ItemTextColor_Violet = zCOLOR(235, 0, 235, 255);
	const zCOLOR ItemTextColor_Pink = zCOLOR(235, 120, 235, 255);


	ItemInfoLine::ItemInfoLine(const zSTRING& txt, const zCOLOR& color, const UiContentAlignEnum alignMode)
	{
		Text = txt;
		TextColor = color;
		TextAllignMode = alignMode;
	}

	ItemInfoPanel::ItemInfoPanel()
	{
		Parent = Null;
		DisplayItem = Null;
		Content = Array<ItemInfoLine>();
	}

	void ItemInfoPanel::Init()
	{
		Parent = Null;
		DisplayItem = Null;
		Content = Array<ItemInfoLine>();
		ContentWidthIndex = 0;
		ContentHeightIndex = 0;
		BaseUiElement::Init();
		IsHiden = true;
	}


	inline void ItemInfoPanel::CreateContentLine(const zSTRING& txt, const  zCOLOR& color, const UiContentAlignEnum alignMode)	{ Content.InsertEnd(ItemInfoLine(txt, color, alignMode)); }
	inline void ItemInfoPanel::CreateContentSeparatorLine() { Content.InsertEnd(ItemInfoLine("", TextColor_Regular_Default)); }
	inline void ItemInfoPanel::CreateContentStatLine(const int statId, const int statValue, const int statDuration, const zCOLOR& color)
	{
		const ExtraStatData* statData = GetExtraStatDataById(statId);
		if (!statData) return;

		zSTRING valueStr = "";
		const UiValueDisplayType displayType = (UiValueDisplayType)statData->ValueType;
		switch (displayType)
		{
			case UiValueDisplayType::Bool: ConvertValueToYesNo(valueStr, statValue); break;
			case UiValueDisplayType::Permille: ConvertValueToPermille(valueStr, statValue); break;
			case UiValueDisplayType::Percent: valueStr = zSTRING(statValue) + "%"; break;
			case UiValueDisplayType::DeciPercent: valueStr = zSTRING(statValue * 10) + "%"; break;
			case UiValueDisplayType::Default:
			default: valueStr = zSTRING(statValue); break;
		}

		zSTRING result = GetExtraStatNameById(statId) + " " + valueStr;
		if (statDuration > 0)
			result += " | " + zSTRING(statDuration) + SecondsSuffixString;
		CreateContentLine(result, color);
	}
	inline void ItemInfoPanel::CreateContentAbilityLine(const int abilityId, const int abilityValue, const int abilityDuration, const int abilityChance, const zCOLOR& color)
	{
		const ItemAbility* abilityData = GetItemAbility(abilityId);
		if (!abilityData) return;

		//ToDo: compleate ability string
		CreateContentLine(abilityData->ScriptInstance.Name, color);
	}


	inline int ItemInfoPanel::GetArtifactType()
	{
		if (!DisplayItem) return Invalid;
		zSTRING name = GetItemInstanceName(DisplayItem);
		if (name.CompareI("itut_stext_magicbook")) return 1;
		if (name.CompareI("itut_stext_magicdagger")) return 2;		
		return Invalid;
	}

	inline const zCOLOR& ItemInfoPanel::GetRankColor(const int rank)
	{
		if (rank < 0) return ItemRankColor[0];
		else if (rank >= 5) return ItemRankColor[5];
		else return ItemRankColor[rank];
	}

	inline const zSTRING& ItemInfoPanel::GetItemRankString(const int rank)
	{
		if (IsIndexInBounds(rank, ItemsGeneratorConfigs.ItemRankMax))
			return parser->GetSymbol("StExt_Str_ItemRank")->stringdata[rank];
		return zString_Unknown;
	}

	inline const zSTRING& ReadItemClassString(Map<byte, zSTRING>& nameMap, const byte index)
	{
		const auto pair = nameMap.GetSafePair(index);
		return pair ? pair->GetValue() : zString_Unknown;
	}

	inline zSTRING ItemInfoPanel::GetItemFullClassString(const ItemExtension* itemExtension)
	{
		byte itemType;
		byte itemClass;
		byte itemSubClass;

		if (itemExtension)
		{
			itemType = itemExtension->Type;
			itemClass = itemExtension->Class;
			itemSubClass = itemExtension->SubClass;
		}
		else
		{
			const ItemClassKey itemClassKey = GetItemClassKey(DisplayItem);
			ItemClassKey_Unpack(itemClassKey, itemType, itemClass, itemSubClass);
		}

		switch (static_cast<ItemType>(itemType))
		{
			case ItemType::Armor:
			case ItemType::Weapon: 
				return Z(ReadItemClassString(ItemClassesText, itemClass) + ", " + ReadItemClassString(ItemSubClassesText, itemSubClass));
				break;

			case ItemType::Jewelry:
			case ItemType::Consumable: 
				return Z(ReadItemClassString(ItemTypesText, itemType) + ", " + ReadItemClassString(ItemClassesText, itemClass));
				break;

			case ItemType::Munition:
			case ItemType::Inventory:
			case ItemType::Craft: 
			case ItemType::Other: 
				return Z(ReadItemClassString(ItemTypesText, itemType)); 
				break;

			case ItemType::Unknown:
			default: 
				return zString_Unknown; 
				break;
		}
		return zString_Unknown;
	}

	inline zSTRING ItemInfoPanel::GetExtraDamageTypeString(const int damageTypes)
	{
		zCPar_Symbol* namesArray = parser->GetSymbol("StExt_Str_ExtraDamageType");
		zSTRING result;
		for (uint i = 0; i < namesArray->ele; ++i)
		{
			const int flag = 1 << i;
			if (HasFlag(damageTypes, flag))
				AppendTag(result, namesArray->stringdata[i]);			
		}
		return result;
	}

	inline zSTRING ItemInfoPanel::GetExtraDamageFlagString(const int damageFlags)
	{
		zCPar_Symbol* namesArray = parser->GetSymbol("StExt_Str_ExtraDamageFlags");
		zSTRING result;
		for (uint i = 0; i < namesArray->ele; ++i)
		{
			const int flag = 1 << i;
			if (HasFlag(damageFlags, flag))
				AppendTag(result, namesArray->stringdata[i]);
		}
		return result;
	}


	inline void ItemInfoPanel::BuildItemNameDescription(const ItemExtension* itemExtension)
	{
		if (!DisplayItem) return;

		if (!itemExtension)
		{
			CreateContentLine(DisplayItem->name);
			return;
		}

		const zCOLOR& nameColor = GetRankColor(itemExtension->Rank);
		if (!itemExtension->OwnName.IsEmpty())
			CreateContentLine(itemExtension->OwnName, nameColor);
		CreateContentLine(DisplayItem->name, nameColor);
		CreateContentLine("[" + GetItemRankString(itemExtension->Rank) + "]", nameColor);
	}

	inline void ItemInfoPanel::BuildItemPropertiesDescription(const ItemExtension* itemExtension)
	{
		CreateContentLine(GetItemFullClassString(itemExtension), ItemTextColor_Default_Faded);
		CreateContentLine(parser->GetSymbol("StExt_Str_ItemLevel")->stringdata + Z(itemExtension->Level), ItemTextColor_Default_Faded);
		CreateContentLine(parser->GetSymbol("StExt_Str_ItemQuality")->stringdata + Z(itemExtension->Quality) + "%", ItemTextColor_Default_Faded);
		
		ItemExtension* itemExtensionUnsafe = const_cast<ItemExtension*>(itemExtension);
		const int maxSockets = itemExtensionUnsafe->GetProperty((int)ItemProperty::SocketsMax);
		const int usedSockets = itemExtensionUnsafe->GetProperty((int)ItemProperty::SocketsUsed);
		if (maxSockets > 0)
			CreateContentLine(parser->GetSymbol("StExt_Str_ItemSockets")->stringdata + Z(usedSockets) + "/" + Z(maxSockets), ItemTextColor_Default_Faded);

		// ToDo: organize other options...

		if ((itemExtension->SpecialDamageMin + itemExtension->SpecialDamageMax) > 0)
		{
			CreateContentSeparatorLine();
			CreateContentLine(parser->GetSymbol("StExt_Str_ItemSpecialDamage")->stringdata + 
				Z(itemExtension->SpecialDamageMin) + " - " + Z(itemExtension->SpecialDamageMax) + " | " + GetExtraDamageTypeString(itemExtension->SpecialDamageTypes), ItemTextColor_Green);
		}

		if (itemExtension->SpecialProtection > 0)
		{
			CreateContentSeparatorLine();
			CreateContentLine(parser->GetSymbol("StExt_Str_ItemSpecialProtection")->stringdata +
				Z(itemExtension->SpecialProtection) + " | " + GetExtraDamageTypeString(itemExtension->SpecialProtectionTypes), ItemTextColor_Green);
		}

		// ToDo: organize craft data info
		if (itemExtensionUnsafe->GetTag((int)ItemTags::Infused) > 0)
		{
			CreateContentSeparatorLine();

		}
	}


	void ItemInfoPanel::BuildArtifactDescription(const int mode)
	{
		BuildItemNameDescription(Null);
		CreateContentLine(GetItemFullClassString(Null), ItemTextColor_Default_Faded);
		CreateContentSeparatorLine();
		
		int expNow, expNext;
		int level;
		int lp;
		zCPar_Symbol* statsArray = Null;

		// Grimoir
		if (mode == 1) 
		{
			expNow = parser->GetSymbol("StExt_Grimoir_ExpNow")->single_intdata;
			expNext = parser->GetSymbol("StExt_Grimoir_ExpNext")->single_intdata;
			level = parser->GetSymbol("StExt_Grimoir_Level")->single_intdata;
			lp = parser->GetSymbol("StExt_Grimoir_Lp")->single_intdata;
			statsArray = parser->GetSymbol("StExt_PcStats_Grimoir");
		}
		// Dagger
		else if (mode == 2)         
		{
			expNow = parser->GetSymbol("StExt_Dagger_ExpNow")->single_intdata;
			expNext = parser->GetSymbol("StExt_Dagger_ExpNext")->single_intdata;
			level = parser->GetSymbol("StExt_Dagger_Level")->single_intdata;
			lp = parser->GetSymbol("StExt_Dagger_Lp")->single_intdata;
			statsArray = parser->GetSymbol("StExt_PcStats_Dagger");
		}
		else
		{
			DEBUG_MSG("BuildArtifactDescription - invalid mode: " + Z(mode) + "!");
			return;
		}

		CreateContentLine(parser->GetSymbol("StExt_Str_Level")->stringdata + Z(level), TextColor_Regular_Faded);
		CreateContentLine(parser->GetSymbol("StExt_Str_Exp")->stringdata + Z(expNow) + "/" + Z(expNext), TextColor_Regular_Faded);
		CreateContentLine(parser->GetSymbol("StExt_Str_Lp")->stringdata + Z(lp), TextColor_Regular_Faded);
		CreateContentSeparatorLine();

		for (int i = 0; i < MaxStatId; ++i)
		{
			if (statsArray->intdata[i] == 0) continue;
			CreateContentStatLine(i, statsArray->intdata[i], Invalid, ItemTextColor_Blue);
		}
	}

	void ItemInfoPanel::BuildSpellDescription(const ItemExtension* itemExtension)
	{
		BuildItemNameDescription(itemExtension);
		CreateContentLine(GetItemFullClassString(Null), ItemTextColor_Default_Faded);
		CreateContentSeparatorLine();

		const int damageTypes = *(int*)parser->CallFunc(GetSpellDamageFlagsFunc, DisplayItem->spell);
		const int damageFlags = *(int*)parser->CallFunc(GetSpellEffectFlagsFunc, DisplayItem->spell);
		zSTRING damageTypeStr = damageTypes != Invalid ? GetExtraDamageTypeString(damageTypes) : zString_Unknown;
		zSTRING damageFlagsStr = damageFlags != Invalid ? GetExtraDamageTypeString(damageFlags) : zString_Unknown;

		CreateContentLine(parser->GetSymbol("StExt_Str_ExtraDamageType_String")->stringdata);
		CreateContentLine(damageTypeStr);
		CreateContentSeparatorLine();
		CreateContentLine(parser->GetSymbol("StExt_Str_ExtraDamageFlags_String")->stringdata);
		CreateContentLine(damageFlagsStr);
	}

	void ItemInfoPanel::BuildExtensionDescription(const ItemExtension* itemExtension)
	{
		BuildItemNameDescription(itemExtension);
		CreateContentSeparatorLine();
		BuildItemPropertiesDescription(itemExtension);
		CreateContentSeparatorLine();

		ItemExtension* itemExtensionUnsafe = const_cast<ItemExtension*>(itemExtension);
		if (itemExtensionUnsafe->GetTag((int)ItemTags::Unidentified))
		{
			CreateContentLine(parser->GetSymbol("StExt_Str_Undefined")->stringdata, ItemTextColor_Red);
			return;
		}

		// ToDo: write infusions and attached souls
		bool hasOwnStatsPropsSeparator = false;
		if (!itemExtension->AttachedSoulName.IsEmpty())
		{

			hasOwnStatsPropsSeparator = true;
		}
		if (itemExtensionUnsafe->GetTag((int)ItemTags::Infused) > 0)
		{
			
			hasOwnStatsPropsSeparator = true;
		}		
		if (hasOwnStatsPropsSeparator) CreateContentSeparatorLine();		

		hasOwnStatsPropsSeparator = false;
		for (int i = 0; i < ItemExtension_OwnStats_Max; ++i)
		{
			if (!IsIndexInBounds(itemExtension->OwnStatId[i], MaxStatId)) continue;
			if (!hasOwnStatsPropsSeparator) hasOwnStatsPropsSeparator = true;
			CreateContentStatLine(itemExtension->OwnStatId[i], itemExtension->OwnStatValue[i], Invalid, ItemTextColor_Gold);
		}
		if (hasOwnStatsPropsSeparator) CreateContentSeparatorLine();

		for (int i = 0; i < ItemExtension_Stats_Max; ++i)
		{
			if (!IsIndexInBounds(itemExtension->StatId[i], MaxStatId)) continue;
			CreateContentStatLine(itemExtension->StatId[i], itemExtension->StatValue[i], itemExtension->StatDuration[i], ItemTextColor_Blue);
		}
	}


	void ItemInfoPanel::BuildItemDescription()
	{
		Content.Clear();
		ContentWidthIndex = 0;
		ContentHeightIndex = 0;

		if (!DisplayItem) return;		
		const ItemExtension* itemExtension = GetItemExtension(DisplayItem);
		const int artifactMode = GetArtifactType();

		if (artifactMode == 1 || artifactMode == 2)
			BuildArtifactDescription(artifactMode);
		else if (HasFlag(DisplayItem->mainflag, item_kat_rune))
			BuildSpellDescription(itemExtension);
		else if (itemExtension)
		{
			UpdateItemDescriptionText(const_cast<oCItem*>(DisplayItem), itemExtension);
			BuildExtensionDescription(itemExtension);
		}
		for (uint i = 0; i < Content.GetNum(); ++i)
		{
			const int width = screen->FontSize(Content[i].Text);
			if (width > ContentWidthIndex) ContentWidthIndex = width;
			++ContentHeightIndex;
		}
		Resize();
	}

	void ItemInfoPanel::SetDisplayItem(const oCItem* item)
	{
		ItemSwitched = DisplayItem != item || !item;
		DisplayItem = item;
	}


	void ItemInfoPanel::Resize()
	{
		PosX = ExtraItemInfoPanel_PosX;
		PosY = ExtraItemInfoPanel_PosY;
		SizeX = SizeY = 0.001f;

		LocalPosX = static_cast<int>(PosX * ScreenVBufferSize);
		LocalPosY = static_cast<int>(PosY * ScreenVBufferSize);
		LocalSizeX = static_cast<int>(SizeX * ScreenVBufferSize);
		LocalSizeY = static_cast<int>(SizeY * ScreenVBufferSize);

		if (View)
		{
			const int contentHeightFix = Content.IsEmpty() ? 1 : 2;
			LocalSizeY = screen->FontY() * (ContentHeightIndex + contentHeightFix);
			SizeY = static_cast<float>(LocalSizeY * ScreenToRelativePixDelta);

			const float contentWidthFix = Content.IsEmpty() ? 0.00f : 0.02f;
			LocalSizeX = ContentWidthIndex + static_cast<int>(contentWidthFix * ScreenVBufferSize);
			SizeX = static_cast<float>(LocalSizeX * ScreenToRelativePixDelta);

			View->SetSize(LocalSizeX, LocalSizeY);
			View->SetPos(LocalPosX, LocalPosY);
		}

		GlobalPosX = LocalPosX;
		GlobalPosY = LocalPosY;
		GlobalSizeX = LocalSizeX;
		GlobalSizeY = LocalSizeY;
	}

	void ItemInfoPanel::Draw()
	{
		if (!screen) return;
		if (!View)
		{
			View = zNEW(zCView)(0, 0, ScreenVBufferSize, ScreenVBufferSize);			
			View->InsertBack(BgTexture);
			View->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
			View->SetTransparency(240);
			Resize();
		}

		View->ClrPrintwin();
		if (!IsVisible || IsHiden)
		{
			if(screen->childs.IsIn(View))
				screen->RemoveItem(View);
			return;
		}

		const uint contentSize = Content.GetNum();
		const int yOffset = View->FontY();
		const int startX = static_cast<int>(ScreenVBufferSize * 0.01f);
		int y = yOffset, x = 0;
		for (uint i = 0; i < Content.GetNum(); ++i)
		{
			auto& contentLine = Content[i];
			if (contentLine.Text.IsEmpty())
			{
				y += yOffset;
				continue;
			}

			const int textWidth = View->FontSize(contentLine.Text);
			View->SetFontColor(contentLine.TextColor);

			if (contentLine.TextAllignMode == UiContentAlignEnum::Begin) x = startX;
			else if (contentLine.TextAllignMode == UiContentAlignEnum::End) x = ScreenVBufferSize - textWidth;
			else x = static_cast<int>(ScreenHalfVBufferSize - (textWidth * 0.5f));

			View->Print(x, y, contentLine.Text);
			y += yOffset;
		}

		if (!screen->childs.IsIn(View)) 
			screen->InsertItem(View);
		if (OnDraw) OnDraw(this);
	}

	void ItemInfoPanel::Update()
	{
		if (ItemSwitched)
		{
			BuildItemDescription();
			ItemSwitched = false;
		}
		BaseUiElement::Update();
		IsHiden = !DisplayItem || Content.IsEmpty();
	}

	ItemInfoPanel::~ItemInfoPanel() 
	{
		Content.Clear();
		DisplayItem = Null; 
		BaseUiElement::~BaseUiElement();
	}
}
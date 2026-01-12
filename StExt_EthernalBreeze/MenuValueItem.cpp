#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	MenuValueItem::MenuValueItem() 
	{ 
		PrimaryValueName = zSTRING();
		SecondaryValueName = zSTRING();
		RangeValueSeparator = zSTRING("/");
		SpecialColorFormatter = Null;
		PrimarySpecialValueFormatter = Null;
		SecondarySpecialValueFormatter = Null;
	}

	inline bool MenuValueItem::ReadValue(zSTRING& valueName, zSTRING& value, int index, bool isSecondary, int& iValRef)
	{
		iValRef = 0;
		zCPar_Symbol* sym = parser->GetSymbol(valueName);
		if (!sym) 
			return false;

		switch (sym->type)
		{
			case zPAR_TYPE_STRING:
			{
				value = sym->stringdata[index];
				return true;
			}
			case zPAR_TYPE_FLOAT:
			{
				float fVal = IsArray && index != Invalid ? sym->floatdata[index] : sym->single_floatdata;
				value = zSTRING(fVal);
				return true;
			}
			case zPAR_TYPE_INT:
			{
				int iVal = IsArray && index != Invalid ? sym->intdata[index] : sym->single_intdata;
				FormatValue(value, iVal, isSecondary);
				iValRef = iVal;
				return true;
			}
			default:
				return false;
		}		
		return false;
	}

	// Apply it only for int type
	inline void MenuValueItem::FormatValue(zSTRING& output, int value, bool isSecondary)
	{
		auto& formatter = (!isSecondary) ? PrimarySpecialValueFormatter : SecondarySpecialValueFormatter;
		if (formatter)
		{
			formatter(output, value);
			return;
		}

		UiValueDisplayType displayType = (!isSecondary) ? PrimaryValueDisplayType : SecondaryValueDisplayType;
		switch (displayType)
		{
			case UiValueDisplayType::Bool:
				ConvertValueToYesNo(output, value);
				break;

			case UiValueDisplayType::Permille:
				ConvertValueToPermille(output, value);
				break;

			case UiValueDisplayType::Percent:
				output = zSTRING(value) + "%";
				break;

			case UiValueDisplayType::DeciPercent:
				output = zSTRING(value * 10) + "%";
				break;

			case UiValueDisplayType::Default:
			default:
				output = zSTRING(value);
				break;
		}
	}

	void MenuValueItem::Resize() { MenuItem::Resize(); }

	void MenuValueItem::Init()
	{
		IsApproximal = false;
		IsRanged = false;
		IsArray = false;
		PrimaryValueArrayIndex = Invalid;
		SecondaryValueArrayIndex = Invalid;

		RangeValueSeparator = zSTRING("/");
		MenuItem::Init();
	}

	void MenuValueItem::Draw() { MenuItem::Draw(); }

	void MenuValueItem::Update() 
	{
		MenuItem::Update();
		Text.Clear();

		if (!IsVisible || IsHiden || !Parent) return;

		int primaryVal, secondaryVal;
		zSTRING primaryStr, secondaryStr;
		bool hasPrimary = ReadValue(PrimaryValueName, primaryStr, PrimaryValueArrayIndex, false, primaryVal);
		bool hasSecondary = IsRanged || !hasPrimary ? ReadValue(SecondaryValueName, secondaryStr, SecondaryValueArrayIndex, true, secondaryVal) : false;

		if (hasPrimary)
		{
			Text = primaryStr;
			if (hasSecondary)
				Text += RangeValueSeparator + secondaryStr;
		}
		else if (hasSecondary) Text = secondaryStr;
		else Text = zString_Unknown;

		if (IsApproximal)
			Text = MenuStr_ApproximalPrefix + Text;

		if (SpecialColorFormatter)
		{
			int colorVal = IsSecondaryColorFormat && hasSecondary ? secondaryVal :
				hasPrimary ? primaryVal : 0;
			SpecialColorFormatter(TextColor_Default, colorVal);
		}
	}

	bool MenuValueItem::HandleMouse(const UiMouseEventArgs& args) { return MenuItem::HandleMouse(args); }
	bool MenuValueItem::HandleKey(const UiKeyEventArgs& args) { return MenuItem::HandleKey(args); }

	MenuValueItem::~MenuValueItem() 
	{
		SpecialColorFormatter = Null;
		PrimarySpecialValueFormatter = Null;
		SecondarySpecialValueFormatter = Null;
	}
}
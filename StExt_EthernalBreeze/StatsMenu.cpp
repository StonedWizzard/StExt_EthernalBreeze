#include "StatsMenu.h"

namespace Gothic_II_Addon
{
	StatsMenu StExtStatMenu;
	static int StExtStatMenuInitialized = 0;

	void StonedExtension_InitUi_StatMenu()
	{
		StExtStatMenu.Init();
		DEBUG_MSG("StExt - Stat menu initialized!");
	}

	void StonedExtension_Loop_StatMenu() { StExtStatMenu.Loop(); }

	void StatsMenu::Init()
	{
		IsShow = false;
		CurrentTab = 0;
		MaxTab = 4;
		PrintOffset = 0;
		View = zNEW(zCView)(0, 0, 8192, 8192);

		Resize();
		View->InsertBack("STEXT_STATSMENU.TGA");
		StExtStatMenuInitialized = 1;
	}

	void StatsMenu::Resize()
	{
		int scaleX = parser->GetSymbol("StExt_Config_MenuScaleX")->single_intdata;
		int scaleY = parser->GetSymbol("StExt_Config_MenuScaleY")->single_intdata;

		if (scaleX < 35) scaleX = 35;
		if (scaleX > 100) scaleX = 100;

		if (scaleY < 35) scaleY = 35;
		if (scaleY > 100) scaleY = 100;

		ScaleX = scaleX * 0.01f;
		ScaleY = scaleY * 0.01f;

		SizeX = 8192 * ScaleX;
		SizeY = 8192 * ScaleY;
		PosX = (8192 - SizeX) * 0.5f;
		PosY = (8192 - SizeY) * 0.5f;

		int BorderOffsetX = 512;
		int BorderOffsetY = 512;

		PrintBorderLeft = BorderOffsetX;
		PrintBorderRight = 8192 - BorderOffsetX;
		PrintBorderTop = 128;
		PrintBorderBottom = 8192 - BorderOffsetY;

		View->SetSize(SizeX, SizeY);
		View->SetPos(PosX, PosY);
	}

	inline void StatsMenu::PrintText(int x, int y, zSTRING text)
	{
		if (y > PrintBorderTop && y < PrintBorderBottom)
			View->Print(x, y, text);
	}

	zSTRING GetSubString(zSTRING str, int start, int length)
	{
		std::string source = str.ToChar();
		std::string tmp;
		tmp = source.substr(start, length);
		zSTRING result = tmp.c_str();
		return result;
	}

	inline int StatsMenu::PrintTextMult(int x, int y, zSTRING text)
	{
		int extraLines = 1;		
		if (y > PrintBorderTop && y < PrintBorderBottom)
		{
			int maxLen = parser->GetSymbol("StExt_Config_MenuTextWrapIndex")->single_intdata;
			const int maxLength = maxLen < 32 ? 32 : maxLen;
			if (text.Length() <= maxLength)
			{
				View->Print(x, y, text);
				return 1;
			}
			else
			{
				extraLines = 0;
				int yy = (View->FontY() + 96) * 0.75f;
				int textLength = text.Length();
				for (int i = 0; i < textLength; i += maxLength)
				{
					int length = textLength - i;
					if (length > maxLength) length = maxLength;
					zSTRING subText = GetSubString(text, i, length);
					View->Print(x, y, subText);
					extraLines += 1;
					y += yy;
				}
			}	
		}
		return extraLines;
	}

	inline std::string trim(std::string input)
	{
		std::string result;
		const char* data = input.c_str();
		int size = input.size();
		for (int i = 0; i < size; i++)
		{
			if ((i == 0) && (data[i] == ' ')) continue;
			if ((i == size - 1) && (data[i] == ' ')) continue;
			result += data[i];
		}
		return result;
	}

	inline void StatsMenu::PrintTextColumn(int& y, zSTRING text, zCOLOR color)
	{
		std::vector<std::string> result;
		std::string token;
		std::string str = text.ToChar();
		char separator = '|';

		for (char ch : str) 
		{
			if (ch == separator) 
			{
				if (!token.empty()) 
				{
					token = trim(token);
					result.push_back(token);
					token.clear();
				}
			}
			else { token += ch; }
		}
		if (!token.empty()) 
		{ 
			token = trim(token);
			result.push_back(token); 
		}

		View->SetFontColor(color);
		for (unsigned int i = 0U; i < result.size(); i++)
		{
			zSTRING printText = result[i].c_str();
			int x = View->vposx + (View->vsizex * 0.5f) - (View->FontSize(printText) * 0.5f);
			View->Print(x, y, printText);
			y += View->FontY() + 96;
		}
		View->SetFontColor(zCOLOR(250, 250, 250));
	}

	void StatsMenu::PrevTab()
	{
		if (CurrentTab <= 0) CurrentTab = MaxTab;
		else CurrentTab -= 1;
		PrintOffset = 0;
	}

	void StatsMenu::NextTab()
	{
		if (CurrentTab >= MaxTab) CurrentTab = 0;
		else CurrentTab += 1;
		PrintOffset = 0;
	}

	void StatsMenu::PrintSectionHeader(int y, zSTRING sectText, zCOLOR color = zCOLOR(255, 255, 100))
	{
		View->SetFontColor(color);
		int x = View->vposx + (View->vsizex * 0.5f) - (View->FontSize(sectText) * 0.5f);
		PrintText(x, y, sectText);
		View->SetFontColor(zCOLOR(250, 250, 250));
	}

	zSTRING GetFloatStringFromInt(int val)
	{
		zSTRING valStr;
		if (val == 0) valStr = zSTRING("0.0%");
		else
		{
			float tmp = val * 0.1f;
			valStr = zSTRING(tmp) + "%";
		}
		return valStr;
	}

	zSTRING IntValueToYesNo(int val)
	{
		if (val == 0) return parser->GetSymbol("StExt_Str_No")->stringdata;
		return parser->GetSymbol("StExt_Str_Yes")->stringdata;
	}

	zSTRING StatsMenu::IntValueToSkill(int val)
	{
		if (val == 0) return parser->GetSymbol("StExt_Str_Learned_No")->stringdata;
		return parser->GetSymbol("StExt_Str_Learned_Yes")->stringdata;
	}

	zSTRING StatsMenu::GetTalentRank(int val)
	{
		if (val < 0) val = 0;
		if (val > 15) val = 15;
		return parser->GetSymbol("StExt_Str_MasteryLevels")->stringdata[val];
	}

	zCOLOR StatsMenu::GetTalentRankColor(int val)
	{
		if (val <= 0) return zCOLOR(250, 100, 100);
		else if (val == 1) return zCOLOR(250, 200, 0);
		else if (val == 2) return zCOLOR(200, 230, 0);
		else if (val == 3) return zCOLOR(170, 240, 0);
		else if (val == 4) return zCOLOR(130, 250, 0);
		else if (val >= 5) return zCOLOR(100, 250, 0);
		return zCOLOR(250, 250, 250);
	}

	inline void StatsMenu::PrintStatLine(int& y, zSTRING statDesc, int statVal, int valType, zCOLOR color = zCOLOR(250, 250, 250))
	{
		View->SetFontColor(color);
		zSTRING statValStr;

		if (valType == Value_Type_YesNo) statValStr = IntValueToYesNo(statVal);
		else if (valType == Value_Type_Percent) statValStr = GetFloatStringFromInt(statVal);
		else if (valType == Value_Type_DefaultPerc) statValStr = zSTRING(statVal) + "%";
		else if (valType == Value_Type_InvertPerc) statValStr = zSTRING(statVal * 10) + "%";
		else statValStr = zSTRING(statVal);

		PrintText(PrintBorderLeft, y, statDesc);
		if (valType == Value_Type_YesNo)
		{
			if (statVal) View->SetFontColor(zCOLOR(0, 250, 0));
			else View->SetFontColor(zCOLOR(250, 100, 100));
		}
		PrintText(PrintBorderRight - View->FontSize(statValStr) * 0.5f, y, statValStr);
		y += View->FontY() + 96;
		View->SetFontColor(zCOLOR(250, 250, 250));
	}

	void StatsMenu::PrintOptionLine(int y, zSTRING optionIndex, zSTRING valIndex, int valType, int optIsImportant)
	{
		zSTRING optionText = parser->GetSymbol(optionIndex)->stringdata;
		int valData = parser->GetSymbol(valIndex)->single_intdata;
		zSTRING valText;
		View->SetFontColor(zCOLOR(250, 250, 250));

		if (valType == Value_Type_YesNo) valText = IntValueToYesNo(valData);
		else if (valType == Value_Type_Percent) valText = GetFloatStringFromInt(valData);
		else if (valType == Value_Type_DefaultPerc) valText = zSTRING(valData) + "%";
		else if (valType == Value_Type_InvertPerc) valText = zSTRING(valData * 10) + "%";
		else valText = zSTRING(valData);

		if (optIsImportant) View->SetFontColor(zCOLOR(255, 255, 100));
		PrintText(PrintBorderLeft, y, optionText);
		View->SetFontColor(zCOLOR(250, 250, 250));
		if (valType == Value_Type_YesNo && optIsImportant)
		{
			if (valData) View->SetFontColor(zCOLOR(100, 250, 100));
			else View->SetFontColor(zCOLOR(250, 100, 100));
		}
		PrintText(PrintBorderRight - View->FontSize(valText) * 0.5f, y, valText);
		View->SetFontColor(zCOLOR(250, 250, 250));
	}

	void StatsMenu::ChangeOffset(int offset)
	{
		PrintOffset += offset;
		if (PrintOffset > PrintOffsetMax)
			PrintOffset = PrintOffsetMax;
		if (PrintOffset < 0) PrintOffset = 0;
	}

	void StatsMenu::PrintModPresetName(int y)
	{
		int presetId = parser->GetSymbol("StExt_SelectedConfigsIndex")->single_intdata;
		if (presetId == 0) View->SetFontColor(zCOLOR(200, 200, 200));
		else if (presetId == 1) View->SetFontColor(zCOLOR(0, 225, 0));
		else if (presetId == 2) View->SetFontColor(zCOLOR(225, 120, 0));
		else if (presetId == 3) View->SetFontColor(zCOLOR(225, 0, 100));
		else if (presetId == 4) View->SetFontColor(zCOLOR(225, 32, 225));
		else return;

		zSTRING presetName = parser->GetSymbol("StExt_Str_DiffLevel")->stringdata[presetId];
		int x = View->vposx + (View->vsizex * 0.5f) - (View->FontSize(presetName) * 0.5f);
		PrintText(x, y, presetName);
		View->SetFontColor(zCOLOR(250, 250, 250));
	}


	void StatsMenu::DrawTabTitle()
	{
		int x, y;
		zSTRING titleStr = parser->GetSymbol("StExt_StatsMenu_TabName")->stringdata[StExtStatMenu.CurrentTab];
		x = View->vposx + (View->vsizex * 0.5f) - (View->FontSize(titleStr) * 0.5f);
		//y = View->vposy - 128;
		y = 256;
		View->SetFontColor(zCOLOR(255, 255, 100));
		View->Print(x, y, titleStr);
		PrintBorderTop = y + View->FontY() + 128;
	}

	void StatsMenu::DrawAllStats()
	{
		int statsMax = parser->GetSymbol("StExt_PcStats_Index_Max")->single_intdata;
		int fontY = View->FontY() + 96;
		zCPar_Symbol* StExt_PcStatsArray = parser->GetSymbol("StExt_PcStats");
		zCPar_Symbol* StExt_PcStatsDescArray = parser->GetSymbol("StExt_PcStats_Desc");
		zCPar_Symbol* StExt_PcStats_SectionDescArray = parser->GetSymbol("StExt_PcStats_SectionDesc");		
		int y = PrintBorderTop + fontY - PrintOffset;
		PrintOffsetMax = (statsMax + 16) * fontY;
		View->SetFontColor(zCOLOR(250, 250, 250));

		// Stats
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[6]);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[0], StExt_PcStatsArray->intdata[0], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[1], StExt_PcStatsArray->intdata[1], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[2], StExt_PcStatsArray->intdata[2], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[3], StExt_PcStatsArray->intdata[3], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[4], StExt_PcStatsArray->intdata[4], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[5], StExt_PcStatsArray->intdata[5], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[6], StExt_PcStatsArray->intdata[6], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[7], StExt_PcStatsArray->intdata[7], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[8], StExt_PcStatsArray->intdata[8], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[14], StExt_PcStatsArray->intdata[14], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[15], StExt_PcStatsArray->intdata[15], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[16], StExt_PcStatsArray->intdata[16], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[17], StExt_PcStatsArray->intdata[17], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[18], StExt_PcStatsArray->intdata[18], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[19], StExt_PcStatsArray->intdata[19], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[20], StExt_PcStatsArray->intdata[20], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[21], StExt_PcStatsArray->intdata[21], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[22], StExt_PcStatsArray->intdata[22], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[168], StExt_PcStatsArray->intdata[168], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[169], StExt_PcStatsArray->intdata[169], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[170], StExt_PcStatsArray->intdata[170], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[171], StExt_PcStatsArray->intdata[171], Value_Type_DefaultPerc);

		//Skills
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[0]);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[9], StExt_PcStatsArray->intdata[9], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[10], StExt_PcStatsArray->intdata[10], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[11], StExt_PcStatsArray->intdata[11], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[12], StExt_PcStatsArray->intdata[12], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[13], StExt_PcStatsArray->intdata[13], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[146], StExt_PcStatsArray->intdata[146], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[147], StExt_PcStatsArray->intdata[147], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[148], StExt_PcStatsArray->intdata[148], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[149], StExt_PcStatsArray->intdata[149], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[150], StExt_PcStatsArray->intdata[150], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[151], StExt_PcStatsArray->intdata[151], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[152], StExt_PcStatsArray->intdata[152], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[153], StExt_PcStatsArray->intdata[153], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[154], StExt_PcStatsArray->intdata[154], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[155], StExt_PcStatsArray->intdata[155], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[156], StExt_PcStatsArray->intdata[156], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[157], StExt_PcStatsArray->intdata[157], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[249], StExt_PcStatsArray->intdata[249], 0);

		//Protection
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[1]);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[23], StExt_PcStatsArray->intdata[23], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[24], StExt_PcStatsArray->intdata[24], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[25], StExt_PcStatsArray->intdata[25], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[26], StExt_PcStatsArray->intdata[26], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[27], StExt_PcStatsArray->intdata[27], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[28], StExt_PcStatsArray->intdata[28], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[29], StExt_PcStatsArray->intdata[29], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[30], StExt_PcStatsArray->intdata[30], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[31], StExt_PcStatsArray->intdata[31], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[32], StExt_PcStatsArray->intdata[32], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[33], StExt_PcStatsArray->intdata[33], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[250], StExt_PcStatsArray->intdata[250], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[34], StExt_PcStatsArray->intdata[34], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[35], StExt_PcStatsArray->intdata[35], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[36], StExt_PcStatsArray->intdata[36], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[37], StExt_PcStatsArray->intdata[37], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[38], StExt_PcStatsArray->intdata[38], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[39], StExt_PcStatsArray->intdata[39], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[251], StExt_PcStatsArray->intdata[251], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[274], StExt_PcStatsArray->intdata[274], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[275], StExt_PcStatsArray->intdata[275], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[40], StExt_PcStatsArray->intdata[40], Value_Type_Percent);		
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[41], StExt_PcStatsArray->intdata[41], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[42], StExt_PcStatsArray->intdata[42], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[43], StExt_PcStatsArray->intdata[43], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[44], StExt_PcStatsArray->intdata[44], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[45], StExt_PcStatsArray->intdata[45], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[46], StExt_PcStatsArray->intdata[46], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[47], StExt_PcStatsArray->intdata[47], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[48], StExt_PcStatsArray->intdata[48], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[49], StExt_PcStatsArray->intdata[49], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[50], StExt_PcStatsArray->intdata[50], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[51], StExt_PcStatsArray->intdata[51], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[52], StExt_PcStatsArray->intdata[52], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[53], StExt_PcStatsArray->intdata[53], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[54], StExt_PcStatsArray->intdata[54], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[55], StExt_PcStatsArray->intdata[55], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[56], StExt_PcStatsArray->intdata[56], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[57], StExt_PcStatsArray->intdata[57], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[58], StExt_PcStatsArray->intdata[58], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[59], StExt_PcStatsArray->intdata[59], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[60], StExt_PcStatsArray->intdata[60], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[61], StExt_PcStatsArray->intdata[61], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[62], StExt_PcStatsArray->intdata[62], Value_Type_YesNo);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[63], StExt_PcStatsArray->intdata[63], Value_Type_YesNo);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[64], StExt_PcStatsArray->intdata[64], Value_Type_YesNo);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[65], StExt_PcStatsArray->intdata[65], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[66], StExt_PcStatsArray->intdata[66], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[67], StExt_PcStatsArray->intdata[67], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[68], StExt_PcStatsArray->intdata[68], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[253], StExt_PcStatsArray->intdata[253], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[254], StExt_PcStatsArray->intdata[254], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[255], StExt_PcStatsArray->intdata[255], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[256], StExt_PcStatsArray->intdata[256], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[257], StExt_PcStatsArray->intdata[257], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[258], StExt_PcStatsArray->intdata[258], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[259], StExt_PcStatsArray->intdata[259], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[260], StExt_PcStatsArray->intdata[260], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[261], StExt_PcStatsArray->intdata[261], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[262], StExt_PcStatsArray->intdata[262], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[263], StExt_PcStatsArray->intdata[263], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[264], StExt_PcStatsArray->intdata[264], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[265], StExt_PcStatsArray->intdata[265], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[266], StExt_PcStatsArray->intdata[266], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[267], StExt_PcStatsArray->intdata[267], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[268], StExt_PcStatsArray->intdata[268], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[269], StExt_PcStatsArray->intdata[269], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[270], StExt_PcStatsArray->intdata[270], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[271], StExt_PcStatsArray->intdata[271], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[272], StExt_PcStatsArray->intdata[272], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[273], StExt_PcStatsArray->intdata[273], 0);

		//Attack
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[2]);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[69], StExt_PcStatsArray->intdata[69], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[70], StExt_PcStatsArray->intdata[70], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[71], StExt_PcStatsArray->intdata[71], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[72], StExt_PcStatsArray->intdata[72], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[73], StExt_PcStatsArray->intdata[73], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[74], StExt_PcStatsArray->intdata[74], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[75], StExt_PcStatsArray->intdata[75], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[76], StExt_PcStatsArray->intdata[76], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[77], StExt_PcStatsArray->intdata[77], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[78], StExt_PcStatsArray->intdata[78], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[79], StExt_PcStatsArray->intdata[79], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[80], StExt_PcStatsArray->intdata[80], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[81], StExt_PcStatsArray->intdata[81], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[82], StExt_PcStatsArray->intdata[82], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[83], StExt_PcStatsArray->intdata[83], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[84], StExt_PcStatsArray->intdata[84], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[85], StExt_PcStatsArray->intdata[85], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[86], StExt_PcStatsArray->intdata[86], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[87], StExt_PcStatsArray->intdata[87], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[88], StExt_PcStatsArray->intdata[88], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[89], StExt_PcStatsArray->intdata[89], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[90], StExt_PcStatsArray->intdata[90], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[91], StExt_PcStatsArray->intdata[91], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[92], StExt_PcStatsArray->intdata[92], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[93], StExt_PcStatsArray->intdata[93], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[94], StExt_PcStatsArray->intdata[94], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[95], StExt_PcStatsArray->intdata[95], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[96], StExt_PcStatsArray->intdata[96], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[97], StExt_PcStatsArray->intdata[97], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[98], StExt_PcStatsArray->intdata[98], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[99], StExt_PcStatsArray->intdata[99], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[100], StExt_PcStatsArray->intdata[100], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[101], StExt_PcStatsArray->intdata[101], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[102], StExt_PcStatsArray->intdata[102], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[103], StExt_PcStatsArray->intdata[103], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[104], StExt_PcStatsArray->intdata[104], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[105], StExt_PcStatsArray->intdata[105], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[106], StExt_PcStatsArray->intdata[106], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[107], StExt_PcStatsArray->intdata[107], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[108], StExt_PcStatsArray->intdata[108], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[109], StExt_PcStatsArray->intdata[109], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[110], StExt_PcStatsArray->intdata[110], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[111], StExt_PcStatsArray->intdata[111], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[112], StExt_PcStatsArray->intdata[112], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[113], StExt_PcStatsArray->intdata[113], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[114], StExt_PcStatsArray->intdata[114], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[115], StExt_PcStatsArray->intdata[115], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[116], StExt_PcStatsArray->intdata[116], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[117], StExt_PcStatsArray->intdata[117], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[118], StExt_PcStatsArray->intdata[118], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[119], StExt_PcStatsArray->intdata[119], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[120], StExt_PcStatsArray->intdata[120], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[121], StExt_PcStatsArray->intdata[121], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[122], StExt_PcStatsArray->intdata[122], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[123], StExt_PcStatsArray->intdata[123], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[124], StExt_PcStatsArray->intdata[124], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[125], StExt_PcStatsArray->intdata[125], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[126], StExt_PcStatsArray->intdata[126], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[127], StExt_PcStatsArray->intdata[127], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[128], StExt_PcStatsArray->intdata[128], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[129], StExt_PcStatsArray->intdata[129], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[130], StExt_PcStatsArray->intdata[130], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[131], StExt_PcStatsArray->intdata[131], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[132], StExt_PcStatsArray->intdata[132], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[133], StExt_PcStatsArray->intdata[133], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[134], StExt_PcStatsArray->intdata[134], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[135], StExt_PcStatsArray->intdata[135], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[136], StExt_PcStatsArray->intdata[136], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[137], StExt_PcStatsArray->intdata[137], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[138], StExt_PcStatsArray->intdata[138], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[139], StExt_PcStatsArray->intdata[139], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[140], StExt_PcStatsArray->intdata[140], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[141], StExt_PcStatsArray->intdata[141], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[142], StExt_PcStatsArray->intdata[142], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[247], StExt_PcStatsArray->intdata[247], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[143], StExt_PcStatsArray->intdata[143], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[144], StExt_PcStatsArray->intdata[144], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[145], StExt_PcStatsArray->intdata[145], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[248], StExt_PcStatsArray->intdata[248], Value_Type_Percent);

		//Aura
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[3]);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[158], StExt_PcStatsArray->intdata[158], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[159], StExt_PcStatsArray->intdata[159], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[160], StExt_PcStatsArray->intdata[160], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[161], StExt_PcStatsArray->intdata[161], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[162], StExt_PcStatsArray->intdata[162], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[163], StExt_PcStatsArray->intdata[163], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[164], StExt_PcStatsArray->intdata[164], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[165], StExt_PcStatsArray->intdata[165], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[166], StExt_PcStatsArray->intdata[166], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[167], StExt_PcStatsArray->intdata[167], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[252], StExt_PcStatsArray->intdata[252], Value_Type_Percent);
		
		//Summons
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[4]);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[173], StExt_PcStatsArray->intdata[173], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[174], StExt_PcStatsArray->intdata[174], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[175], StExt_PcStatsArray->intdata[175], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[176], StExt_PcStatsArray->intdata[176], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[177], StExt_PcStatsArray->intdata[177], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[178], StExt_PcStatsArray->intdata[178], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[179], StExt_PcStatsArray->intdata[179], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[180], StExt_PcStatsArray->intdata[180], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[181], StExt_PcStatsArray->intdata[181], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[182], StExt_PcStatsArray->intdata[182], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[183], StExt_PcStatsArray->intdata[183], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[184], StExt_PcStatsArray->intdata[184], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[185], StExt_PcStatsArray->intdata[185], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[186], StExt_PcStatsArray->intdata[186], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[187], StExt_PcStatsArray->intdata[187], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[188], StExt_PcStatsArray->intdata[188], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[189], StExt_PcStatsArray->intdata[189], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[190], StExt_PcStatsArray->intdata[190], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[191], StExt_PcStatsArray->intdata[191], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[192], StExt_PcStatsArray->intdata[192], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[193], StExt_PcStatsArray->intdata[193], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[194], StExt_PcStatsArray->intdata[194], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[195], StExt_PcStatsArray->intdata[195], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[196], StExt_PcStatsArray->intdata[196], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[197], StExt_PcStatsArray->intdata[197], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[198], StExt_PcStatsArray->intdata[198], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[199], StExt_PcStatsArray->intdata[199], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[234], StExt_PcStatsArray->intdata[234], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[200], StExt_PcStatsArray->intdata[200], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[201], StExt_PcStatsArray->intdata[201], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[202], StExt_PcStatsArray->intdata[202], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[203], StExt_PcStatsArray->intdata[203], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[204], StExt_PcStatsArray->intdata[204], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[205], StExt_PcStatsArray->intdata[205], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[206], StExt_PcStatsArray->intdata[206], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[207], StExt_PcStatsArray->intdata[207], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[208], StExt_PcStatsArray->intdata[208], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[209], StExt_PcStatsArray->intdata[209], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[210], StExt_PcStatsArray->intdata[210], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[211], StExt_PcStatsArray->intdata[211], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[225], StExt_PcStatsArray->intdata[225], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[236], StExt_PcStatsArray->intdata[236], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[212], StExt_PcStatsArray->intdata[212], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[213], StExt_PcStatsArray->intdata[213], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[214], StExt_PcStatsArray->intdata[214], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[215], StExt_PcStatsArray->intdata[215], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[224], StExt_PcStatsArray->intdata[224], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[238], StExt_PcStatsArray->intdata[238], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[216], StExt_PcStatsArray->intdata[216], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[217], StExt_PcStatsArray->intdata[217], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[218], StExt_PcStatsArray->intdata[218], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[219], StExt_PcStatsArray->intdata[219], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[226], StExt_PcStatsArray->intdata[226], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[235], StExt_PcStatsArray->intdata[235], 0);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[220], StExt_PcStatsArray->intdata[220], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[221], StExt_PcStatsArray->intdata[221], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[222], StExt_PcStatsArray->intdata[222], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[223], StExt_PcStatsArray->intdata[223], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[227], StExt_PcStatsArray->intdata[227], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[237], StExt_PcStatsArray->intdata[237], 0);
		y += fontY;	
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[228], StExt_PcStatsArray->intdata[228], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[229], StExt_PcStatsArray->intdata[229], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[230], StExt_PcStatsArray->intdata[230], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[231], StExt_PcStatsArray->intdata[231], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[232], StExt_PcStatsArray->intdata[232], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[233], StExt_PcStatsArray->intdata[233], 0);

		//Other
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[5]);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[239], StExt_PcStatsArray->intdata[239], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[240], StExt_PcStatsArray->intdata[240], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[241], StExt_PcStatsArray->intdata[241], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[242], StExt_PcStatsArray->intdata[242], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[243], StExt_PcStatsArray->intdata[243], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[244], StExt_PcStatsArray->intdata[244], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[245], StExt_PcStatsArray->intdata[245], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[246], StExt_PcStatsArray->intdata[246], Value_Type_DefaultPerc);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[172], StExt_PcStatsArray->intdata[172], Value_Type_Percent);
	}

	void StatsMenu::DrawSkills() 
	{
		int fontY = View->FontY() + 96;
		int y = PrintBorderTop + fontY - PrintOffset;
		PrintOffsetMax = 800 * fontY;
		y += fontY;

		int corruptionType = parser->GetSymbol("StExt_CorruptionPath")->single_intdata;
		int corruptionPerksMax = parser->GetSymbol("StExt_CorruptionPerk_Max")->single_intdata;

		// Corruption skills
		if (corruptionType > 0)
		{
			zSTRING corruptionHeader = "--- * " + parser->GetSymbol("StExt_Str_CorruptionName")->stringdata[corruptionType] + " * ---";
			PrintSectionHeader(y, corruptionHeader);
			y += fontY;

			zCPar_Symbol* corrupPerkSym = Null;
			zCPar_Symbol* corrupPerkNameSym = Null;
			zCPar_Symbol* corrupPerkDescSym = Null;

			if (corruptionType == StExt_Corruption_Mage)
			{
				corrupPerkSym = parser->GetSymbol("StExt_Corruption_Perk_Mage");
				corrupPerkNameSym = parser->GetSymbol("StExt_Corruption_Perk_Name_Mage");
				corrupPerkDescSym = parser->GetSymbol("StExt_Corruption_Perk_Desc_Mage");
			}
			else if(corruptionType == StExt_Corruption_Warrior)
			{
				corrupPerkSym = parser->GetSymbol("StExt_Corruption_Perk_Warrior");
				corrupPerkNameSym = parser->GetSymbol("StExt_Corruption_Perk_Name_Warrior");
				corrupPerkDescSym = parser->GetSymbol("StExt_Corruption_Perk_Desc_Warrior");
			}
			else if (corruptionType == StExt_Corruption_Ranger)
			{
				corrupPerkSym = parser->GetSymbol("StExt_Corruption_Perk_Ranger");
				corrupPerkNameSym = parser->GetSymbol("StExt_Corruption_Perk_Name_Ranger");
				corrupPerkDescSym = parser->GetSymbol("StExt_Corruption_Perk_Desc_Ranger");
			}

			for (int j = 0; j < corruptionPerksMax; j++)
			{
				zSTRING perkName = "'" + zSTRING(corrupPerkNameSym->stringdata[j]) + "'";
				zSTRING perkLevel = zSTRING(IntValueToSkill(corrupPerkSym->intdata[j]));
				View->SetFontColor(zCOLOR(255, 255, 100));
				PrintText(PrintBorderLeft, y, perkName);
				if (corrupPerkSym->intdata[j]) View->SetFontColor(zCOLOR(100, 250, 100));
				else View->SetFontColor(zCOLOR(250, 100, 100));
				PrintText(PrintBorderRight - View->FontSize(perkLevel) * 0.5f, y, perkLevel);
				y += fontY * 0.75f;

				View->SetFontColor(zCOLOR(180, 180, 180));
				zSTRING perkDesc = "( " + zSTRING(corrupPerkDescSym->stringdata[j]) + " )";
				y += fontY * (PrintTextMult(PrintBorderLeft, y, perkDesc) + 0.25f);
			}			
			y += fontY * 2;
		}

		//		*** Talents strings ***
		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_Talents_Header")->stringdata);
		y += fontY * 2;

		zCPar_Symbol* talentNameSym = parser->GetSymbol("StExt_Str_MasteryNames");
		zCPar_Symbol* talentDescSym = parser->GetSymbol("StExt_Str_MasteryDescription");
		zCPar_Symbol* talentLevelSym = parser->GetSymbol("StExt_Talent_Level");
		zCPar_Symbol* talentProgressSym = parser->GetSymbol("StExt_Talent_Progression");
		zCPar_Symbol* talentExpNowSym = parser->GetSymbol("StExt_Talent_ExpNow");
		zCPar_Symbol* talentExpNextSym = parser->GetSymbol("StExt_Talent_ExpNext");
		zCPar_Symbol* talentLpSym = parser->GetSymbol("StExt_Talent_PerkPoints");

		zSTRING TalentsProgress = parser->GetSymbol("StExt_Str_Talents_Progress")->stringdata;
		zSTRING TalentsExp = parser->GetSymbol("StExt_Str_Talents_Exp")->stringdata;
		zSTRING TalentsLp = parser->GetSymbol("StExt_Str_Talents_Lp")->stringdata;

		int masteriesMax = parser->GetSymbol("StExt_MasteryIndex_Max")->single_intdata;
		int masteriesPerksMax = parser->GetSymbol("StExt_MasteryPerk_Max")->single_intdata;
		int generalPerksMax = parser->GetSymbol("StExt_Perk_Max")->single_intdata;
		for (int i = 0; i < masteriesMax; i++)
		{
			View->SetFontColor(zCOLOR(250, 250, 250));
			zSTRING name = talentNameSym->stringdata[i];
			zSTRING talentrank = GetTalentRank(talentLevelSym->intdata[i]);

			View->SetFontColor(zCOLOR(255, 255, 100));
			PrintText(PosX + (SizeX * 0.5f) - (View->FontSize(name) * 0.5f), y, name);
			View->SetFontColor(GetTalentRankColor(talentLevelSym->intdata[i]));
			PrintText(PrintBorderRight - View->FontSize(talentrank) * 0.5f, y, talentrank);
			View->SetFontColor(zCOLOR(250, 250, 250));
			y += fontY * 1.25f;

			zSTRING progress = TalentsProgress + zSTRING(talentProgressSym->intdata[i]);
			zSTRING exp = TalentsExp + zSTRING(talentExpNowSym->intdata[i]) + "/" + zSTRING(talentExpNextSym->intdata[i]);
			zSTRING lp = TalentsLp + zSTRING(talentLpSym->intdata[i]);
			PrintText(PrintBorderLeft, y, progress);
			PrintText(PosX + (SizeX * 0.5f) - (View->FontSize(exp) * 0.5f), y, exp);
			PrintText(PrintBorderRight - View->FontSize(lp) * 0.5f, y, lp);
			y += fontY;

			View->SetFontColor(zCOLOR(180, 180, 180));
			zSTRING desc = "(" + zSTRING(talentDescSym->stringdata[i]) + ")";
			PrintText(PosX + (SizeX * 0.5f) - (View->FontSize(desc) * 0.5f), y, desc);
			View->SetFontColor(zCOLOR(250, 250, 250));
			y += fontY * 1.5f;

			zSTRING perkDescArrId = zSTRING("StExt_Str_MasteryPerk_Desc_") + zSTRING(i);
			zSTRING perkNameArrId = zSTRING("StExt_Str_MasteryPerk_Name_") + zSTRING(i);
			zSTRING perkLevelArrId = zSTRING("StExt_Perk_") + zSTRING(i);
			zCPar_Symbol* perkNameSym = parser->GetSymbol(perkNameArrId);
			zCPar_Symbol* perkDeskSym = parser->GetSymbol(perkDescArrId);
			zCPar_Symbol* perkLevelSym = parser->GetSymbol(perkLevelArrId);

			for (int j = 0; j < masteriesPerksMax; j++)
			{
				zSTRING perkName = "'" + zSTRING(perkNameSym->stringdata[j]) + "'";
				zSTRING perkLevel = zSTRING(IntValueToSkill(perkLevelSym->intdata[j]));
				View->SetFontColor(zCOLOR(250, 250, 250));
				PrintText(PrintBorderLeft, y, perkName);
				if(perkLevelSym->intdata[j]) View->SetFontColor(zCOLOR(100, 250, 100));
				else View->SetFontColor(zCOLOR(250, 100, 100));
				PrintText(PrintBorderRight - View->FontSize(perkLevel) * 0.5f, y, perkLevel);
				y += fontY * 0.75f;

				View->SetFontColor(zCOLOR(180, 180, 180));
				zSTRING perkDesc = "( " + zSTRING(perkDeskSym->stringdata[j]) + " )";
				y += fontY * (PrintTextMult(PrintBorderLeft, y, perkDesc) + 0.25f);
			}
			y += fontY * 2.0f;
		}
		y += fontY * 2;

		//		*** Generic skills ***
		zCPar_Symbol* genPerkNameSym = parser->GetSymbol("StExt_Str_Perk_Name");
		zCPar_Symbol* genPerkDeskSym = parser->GetSymbol("StExt_Str_Perk_Desc");
		zCPar_Symbol* genPerkLevelSym = parser->GetSymbol("StExt_Perk");

		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_GeneralPerks_Header")->stringdata);
		y += fontY * 1.5f;

		for (int j = 0; j < generalPerksMax; j++)
		{
			zSTRING perkName = "'" + zSTRING(genPerkNameSym->stringdata[j]) + "'";
			zSTRING perkLevel = zSTRING(IntValueToSkill(genPerkLevelSym->intdata[j]));
			View->SetFontColor(zCOLOR(255, 255, 100));
			PrintText(PrintBorderLeft, y, perkName);
			if (genPerkLevelSym->intdata[j]) View->SetFontColor(zCOLOR(100, 250, 100));
			else View->SetFontColor(zCOLOR(250, 100, 100));
			PrintText(PrintBorderRight - View->FontSize(perkLevel) * 0.5f, y, perkLevel);
			y += fontY * 0.75f;

			View->SetFontColor(zCOLOR(180, 180, 180));
			zSTRING perkDesc = "( " + zSTRING(genPerkDeskSym->stringdata[j]) + " )";
			y += fontY * (PrintTextMult(PrintBorderLeft, y, perkDesc) + 0.25f);
		}
	}

	void StatsMenu::DrawGeneral() 
	{
		int statsMax = parser->GetSymbol("StExt_PcStats_Index_Max")->single_intdata;
		int fontY = View->FontY() + 96;
		zCPar_Symbol* StExt_PcStatsArray = parser->GetSymbol("StExt_PcStats");
		zCPar_Symbol* StExt_PcStatsDescArray = parser->GetSymbol("StExt_PcStats_Desc");
		zCPar_Symbol* StExt_PcStats_SectionDescArray = parser->GetSymbol("StExt_PcStats_SectionDesc");
		int y = PrintBorderTop + fontY - PrintOffset;
		PrintOffsetMax = 128 * fontY;
		View->SetFontColor(zCOLOR(250, 250, 250));

		int esCurFuncIndex = parser->GetIndex("StExt_Npc_GetBarCurEs");
		int esMaxFuncIndex = parser->GetIndex("StExt_Npc_GetBarMaxEs");
		zSTRING esStr = parser->GetSymbol("StExt_EsText")->stringdata;
		parser->SetInstance("StExt_FocusNpc", player);
		int esCur = *(int*)parser->CallFunc(NpcGetBarCurEsFunc);
		int esMax = *(int*)parser->CallFunc(NpcGetBarMaxEsFunc);

		zCPar_Symbol* talentNameSym = parser->GetSymbol("StExt_Str_MasteryNames");
		zCPar_Symbol* talentLevelSym = parser->GetSymbol("StExt_Talent_Level");
		zCPar_Symbol* talentProgressSym = parser->GetSymbol("StExt_Talent_Progression");
		zCPar_Symbol* talentExpNowSym = parser->GetSymbol("StExt_Talent_ExpNow");
		zCPar_Symbol* talentExpNextSym = parser->GetSymbol("StExt_Talent_ExpNext");
		zCPar_Symbol* talentLpSym = parser->GetSymbol("StExt_Talent_PerkPoints");

		zSTRING TalentsProgress = parser->GetSymbol("StExt_Str_Level")->stringdata;
		zSTRING TalentsExp = parser->GetSymbol("StExt_Str_Exp")->stringdata;
		zSTRING TalentsLp = parser->GetSymbol("StExt_Str_Lp")->stringdata;
		int masteriesMax = parser->GetSymbol("StExt_MasteryIndex_Max")->single_intdata;
		int corruptionType = parser->GetSymbol("StExt_CorruptionPath")->single_intdata;

		// Corruption stats
		if (corruptionType > 0)
		{
			zSTRING corruptionName = parser->GetSymbol("StExt_Str_CorruptionName")->stringdata[corruptionType];
			zSTRING corruptionLevelStr = TalentsProgress + Z parser->GetSymbol("StExt_CorruptionPath_Level")->single_intdata;
			zSTRING corruptionExpStr = TalentsExp + Z parser->GetSymbol("StExt_CorruptionPath_ExpNow")->single_intdata + "/" + Z parser->GetSymbol("StExt_CorruptionPath_ExpNext")->single_intdata;
			zSTRING corruptionLpStr = TalentsLp + Z parser->GetSymbol("StExt_CorruptionPath_SkillPoints")->single_intdata;

			zSTRING corruptionBonusHeader = parser->GetSymbol("StExt_Str_Corruption_BonusHeader")->stringdata;
			zCPar_Symbol* corruptionBonusNames = parser->GetSymbol("StExt_Str_Corruption_BonusStat");
			int corruptionBonusMax = parser->GetSymbol("StExt_CorruptionBonus_Max")->single_intdata;
			zCPar_Symbol* corruptionBonusCount = parser->GetSymbol("StExt_Corruption_BonusCount");
			zCPar_Symbol* corruptionBonusNext = parser->GetSymbol("StExt_Corruption_BonusNext");

			View->SetFontColor(zCOLOR(128, 64, 228));
			y += fontY;
			PrintText(PrintBorderLeft, y, corruptionName);
			View->SetFontColor(zCOLOR(250, 250, 250));
			y += fontY;
			PrintText(PrintBorderLeft, y, corruptionLevelStr);
			y += fontY;
			PrintText(PrintBorderLeft, y, corruptionExpStr);
			y += fontY;
			PrintText(PrintBorderLeft, y, corruptionLpStr);
			y += fontY;
			y += fontY;

			View->SetFontColor(zCOLOR(32, 250, 32));
			PrintText(PrintBorderLeft, y, corruptionBonusHeader);
			View->SetFontColor(zCOLOR(250, 250, 250));
			y += fontY;
			for (int i = 0; i < corruptionBonusMax; i++)
			{
				int count = corruptionBonusCount->intdata[i];
				int next = corruptionBonusNext->intdata[i];
				if (next || count)
				{
					zSTRING corruptionBonusLine;
					if (i == 5)
					{
						corruptionBonusLine = corruptionBonusNames->stringdata[i] + GetFloatStringFromInt(count) + " | " + GetFloatStringFromInt(next);
						PrintText(PrintBorderLeft, y, corruptionBonusLine);
						continue;
					}
					corruptionBonusLine = corruptionBonusNames->stringdata[i] + Z count + " | " + Z next;
					PrintText(PrintBorderLeft, y, corruptionBonusLine);
					y += fontY;
				}
			}
		}

		// Stats
		y += fontY;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[6]);
		y += fontY;
		zSTRING str;
		str = Z(int)esCur + "/" + Z(int)esMax;
		PrintText(PrintBorderLeft, y, esStr);
		PrintText(PrintBorderRight - View->FontSize(str) * 0.5f, y, str);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[27], StExt_PcStatsArray->intdata[27], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[246], StExt_PcStatsArray->intdata[246], Value_Type_DefaultPerc);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[274], StExt_PcStatsArray->intdata[274], 0);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[275], StExt_PcStatsArray->intdata[275], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[40], StExt_PcStatsArray->intdata[40], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[41], StExt_PcStatsArray->intdata[41], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[42], StExt_PcStatsArray->intdata[42], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[43], StExt_PcStatsArray->intdata[43], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[44], StExt_PcStatsArray->intdata[44], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[45], StExt_PcStatsArray->intdata[45], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[46], StExt_PcStatsArray->intdata[46], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[47], StExt_PcStatsArray->intdata[47], Value_Type_Percent);
		y += fontY;
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[239], StExt_PcStatsArray->intdata[239], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[240], StExt_PcStatsArray->intdata[240], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[241], StExt_PcStatsArray->intdata[241], Value_Type_Percent);
		PrintStatLine(y, StExt_PcStatsDescArray->stringdata[242], StExt_PcStatsArray->intdata[242], Value_Type_Percent);
		y += fontY * 2.0f;

		str = Z(parser->GetSymbol("StExt_HeroActiveAuras")->single_intdata) + "/" + Z(parser->GetSymbol("StExt_HeroActiveAurasMax")->single_intdata);
		PrintText(PrintBorderLeft, y, parser->GetSymbol("StExt_Str_Auras")->stringdata);
		PrintText(PrintBorderRight - View->FontSize(str) * 0.5f, y, str);
		y += fontY;
		PrintTextColumn(y, parser->GetSymbol("StExt_ActiveAurasNames")->stringdata, zCOLOR(0, 200, 0));
		y += fontY;

		str = Z(parser->GetSymbol("rx_summoncount")->single_intdata) + "/" + Z(parser->GetSymbol("rx_summoncountmax")->single_intdata);
		PrintText(PrintBorderLeft, y, parser->GetSymbol("StExt_Str_SummonsCountGlobalStat")->stringdata);
		PrintText(PrintBorderRight - View->FontSize(str) * 0.5f, y, str);
		y += fontY;

		str = Z(parser->GetSymbol("StExt_HeroMasteriesLearned")->single_intdata) + "/" + Z(parser->GetSymbol("StExt_HeroMasteriesMax")->single_intdata);
		PrintText(PrintBorderLeft, y, parser->GetSymbol("StExt_Str_Masteries")->stringdata);
		PrintText(PrintBorderRight - View->FontSize(str) * 0.5f, y, str);
		y += fontY;

		y += fontY * 2.0f;
		PrintSectionHeader(y, StExt_PcStats_SectionDescArray->stringdata[0]);
		y += fontY * 1.5f;
		for (int i = 0; i < masteriesMax; i++)
		{
			zSTRING progress = TalentsProgress + zSTRING(talentProgressSym->intdata[i]);
			zSTRING exp = TalentsExp + zSTRING(talentExpNowSym->intdata[i]) + "/" + zSTRING(talentExpNextSym->intdata[i]);
			zSTRING lp = TalentsLp + zSTRING(talentLpSym->intdata[i]);

			zSTRING talentinfo = "(" + progress + "; " + exp + "; " + lp + ")";
			zSTRING talentname = talentNameSym->stringdata[i];
			zSTRING talentrank = GetTalentRank(talentLevelSym->intdata[i]);

			View->SetFontColor(zCOLOR(255, 255, 200));
			PrintText(PrintBorderLeft, y, talentname);
			View->SetFontColor(GetTalentRankColor(talentLevelSym->intdata[i]));
			PrintText(PrintBorderRight - View->FontSize(talentrank) * 0.5f, y, talentrank);
			View->SetFontColor(zCOLOR(180, 180, 180));
			PrintText(PosX + (SizeX * 0.5f) - (View->FontSize(talentinfo) * 0.5f), y, talentinfo);
			View->SetFontColor(zCOLOR(250, 250, 250));
			y += fontY;
		}

		y += fontY * 2.0f;
		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_Professions_Header")->stringdata);
		y += fontY * 1.5f;

		zCPar_Symbol* professionName = parser->GetSymbol("StExt_Str_Profession_Name");
		zCPar_Symbol* professionLevel = parser->GetSymbol("StExt_Profession_Level");
		zCPar_Symbol* professionSkillsName = parser->GetSymbol("StExt_Str_Profession_Skill_Scrollmaking_Name");
		zCPar_Symbol* professionSkills = parser->GetSymbol("StExt_Profession_Skill_Scrollmaking");
		int professionSkillsCount = parser->GetSymbol("StExt_ScrollmakingSkillIndex_Max")->single_intdata;

		// Scrollmaking
		PrintStatLine(y, professionName->stringdata[0], professionLevel->intdata[0], Value_Type_YesNo, zCOLOR(0, 250, 0));
		y += fontY * 0.5f;
		for (int i = 0; i < professionSkillsCount; i++)
			PrintStatLine(y, professionSkillsName->stringdata[i], professionSkills->intdata[i], Value_Type_YesNo);		
	}

	void StatsMenu::DrawBonusStats()
	{
		zCPar_Symbol* StExt_PcStats_ItemsArray = parser->GetSymbol("StExt_PcStats_Items");
		zCPar_Symbol* StExt_PcStats_TalismansArray = parser->GetSymbol("StExt_PcStats_Talismans");

		zCPar_Symbol* StExt_PcStats_AlchemyArray = parser->GetSymbol("StExt_PcStats_Alchemy");
		zCPar_Symbol* StExt_PcStats_AlchemyDurArray = parser->GetSymbol("StExt_PcStats_Alchemy_Cooldown");
		zCPar_Symbol* StExt_PcStats_BuffsArray = parser->GetSymbol("StExt_PcStats_Buffs");	
		zCPar_Symbol* StExt_PcStats_BuffsDurArray = parser->GetSymbol("StExt_PcStats_Buffs_Cooldown");

		zCPar_Symbol* StExt_ArtifactEquipped = parser->GetSymbol("StExt_ArtifactEquipped");
		zCPar_Symbol* StExt_PcStats_DaggerArray = parser->GetSymbol("StExt_PcStats_Dagger");
		zCPar_Symbol* StExt_PcStats_GrimoirArray = parser->GetSymbol("StExt_PcStats_Grimoir");

		zCPar_Symbol* StExt_PcStats_AurasArray = parser->GetSymbol("StExt_PcStats_Auras");		

		zCPar_Symbol* StExt_PcStatsDescArray = parser->GetSymbol("StExt_PcStats_Desc");
		zCPar_Symbol* StExt_PcStatsBonusDescArray = parser->GetSymbol("StExt_BonusStats_Desc");

		zCPar_Symbol* StExt_PcBuffs_Timer = parser->GetSymbol("StExt_PcBuffs_Timer");
		int StExt_PcBuff_Index_Max = parser->GetSymbol("StExt_PcBuff_Index_Max")->single_intdata;
		zCPar_Symbol* StExt_Str_PcBuffs_Names = parser->GetSymbol("StExt_Str_PcBuffs_Names");

		zSTRING NoStr = parser->GetSymbol("StExt_Str_No")->stringdata;
		zSTRING SecStr = parser->GetSymbol("StExt_Str_Seconds")->stringdata;

		int size = parser->GetSymbol("StExt_PcStats_Index_Max")->single_intdata;
		int bonusCount_Items = 0, bonusCount_Artifact = 0, bonusCount_Potions = 0, bonusCount_Effects = 0, bonusCount_Auras = 0;
		for (int i = 0; i < size; i++)
		{
			if (StExt_PcStats_ItemsArray->intdata[i] > 0) bonusCount_Items += 1;
			if (StExt_PcStats_TalismansArray->intdata[i] > 0) bonusCount_Items += 1;

			if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Grimoir)
			{
				if (StExt_PcStats_GrimoirArray->intdata[i] > 0) bonusCount_Artifact += 1;
			}
			else if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Dagger)
			{
				if (StExt_PcStats_DaggerArray->intdata[i] > 0) bonusCount_Artifact += 1;
			}

			if (StExt_PcStats_AlchemyArray->intdata[i] > 0) bonusCount_Potions += 1;
			if (StExt_PcStats_BuffsArray->intdata[i] > 0) bonusCount_Effects += 1;
			if (StExt_PcStats_AurasArray->intdata[i] > 0) bonusCount_Auras += 1;
		}
		int fontY = View->FontY() + 96;
		int y = PrintBorderTop + fontY - PrintOffset;
		PrintOffsetMax = (bonusCount_Items + bonusCount_Artifact + bonusCount_Potions + bonusCount_Effects + bonusCount_Auras + 20) * fontY;

		// items bonuses
		y += fontY * 2;
		PrintSectionHeader(y, StExt_PcStatsBonusDescArray->stringdata[0]);
		y += fontY;
		if(bonusCount_Items == 0)
			PrintSectionHeader(y, NoStr);
		else
			for (int i = 0; i < size; i++)
			{
				int val = StExt_PcStats_ItemsArray->intdata[i] + StExt_PcStats_TalismansArray->intdata[i];
				if (val == 0) continue;
				PrintStatLine(y, StExt_PcStatsDescArray->stringdata[i], val, StatsTypeMap[i]);
			}

		// artifact bonuses
		y += fontY * 2;
		PrintSectionHeader(y, StExt_PcStatsBonusDescArray->stringdata[1]);
		y += fontY;
		if (bonusCount_Artifact == 0)
			PrintSectionHeader(y, NoStr);
		else
			for (int i = 0; i < size; i++)
			{
				int val = 0;
				if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Grimoir)
					val = StExt_PcStats_GrimoirArray->intdata[i];
				else if (StExt_ArtifactEquipped->single_intdata == StExt_ArtifactIndex_Dagger)
					val = StExt_PcStats_DaggerArray->intdata[i];

				if (val == 0) continue;
				PrintStatLine(y, StExt_PcStatsDescArray->stringdata[i], val, StatsTypeMap[i]);
			}

		// potions bonuses
		y += fontY * 2;
		PrintSectionHeader(y, StExt_PcStatsBonusDescArray->stringdata[2]);
		y += fontY;
		if (bonusCount_Potions == 0)
			PrintSectionHeader(y, NoStr);
		else
			for (int i = 0; i < size; i++)
			{
				int val = StExt_PcStats_AlchemyArray->intdata[i];
				if (val == 0) continue;
				zSTRING statLine = StExt_PcStatsDescArray->stringdata[i] + " (" + Z StExt_PcStats_AlchemyDurArray->intdata[i] + SecStr + ")";
				PrintStatLine(y, statLine, val, StatsTypeMap[i]);
			}

		// timed bonuses
		y += fontY * 2;
		PrintSectionHeader(y, StExt_PcStatsBonusDescArray->stringdata[3]);
		y += fontY;
		for (int i = 0; i < StExt_PcBuff_Index_Max; i++)
		{
			if (StExt_PcBuffs_Timer->intdata[i] > 0)
			{
				zSTRING effectStr = StExt_Str_PcBuffs_Names->stringdata[i] + " (" + Z StExt_PcBuffs_Timer->intdata[i] + SecStr + ")";
				PrintSectionHeader(y, effectStr);
				y += fontY;
				bonusCount_Effects += 1;
			}
		}
		if (bonusCount_Effects == 0)
			PrintSectionHeader(y, NoStr);
		else
			for (int i = 0; i < size; i++)
			{
				int val = StExt_PcStats_BuffsArray->intdata[i];
				if (val == 0) continue;
				zSTRING statLine = StExt_PcStatsDescArray->stringdata[i] + " (" + Z StExt_PcStats_BuffsDurArray->intdata[i] + SecStr + ")";
				PrintStatLine(y, statLine, val, StatsTypeMap[i]);
			}

		// auras bonuses
		y += fontY * 2;
		PrintSectionHeader(y, StExt_PcStatsBonusDescArray->stringdata[4]);
		y += fontY;
		if (bonusCount_Auras == 0)
			PrintSectionHeader(y, NoStr);
		else
			for (int i = 0; i < size; i++)
			{
				int val = StExt_PcStats_AurasArray->intdata[i];
				if (val == 0) continue;
				PrintStatLine(y, StExt_PcStatsDescArray->stringdata[i], val, StatsTypeMap[i]);
			}
	}

	void StatsMenu::DrawModConfigs() 
	{
		int fontY = View->FontY() + 96;
		int y = PrintBorderTop + fontY - PrintOffset;
		PrintOffsetMax = 228 * fontY;

		PrintModPresetName(y);
		y += fontY * 1.5f;
		
		//		*** Difficulty configs strings ***
		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_Config_Diff_Header")->stringdata);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableStatic", "StExt_Config_Diff_EnableStatic", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Static_Power", "StExt_Config_Diff_Static_Power", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Static_Hp", "StExt_Config_Diff_Static_Hp", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Static_Prot", "StExt_Config_Diff_Static_Prot", Value_Type_DefaultPerc, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableDaily", "StExt_Config_Diff_EnableDaily", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Daily_Power", "StExt_Config_Diff_Daily_Power", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Daily_Hp", "StExt_Config_Diff_Daily_Hp", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Daily_Prot", "StExt_Config_Diff_Daily_Prot", Value_Type_DefaultPerc, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableKapitel", "StExt_Config_Diff_EnableKapitel", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Power", "StExt_Config_Diff_Kapitel_Power", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Hp", "StExt_Config_Diff_Kapitel_Hp", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Prot", "StExt_Config_Diff_Kapitel_Prot", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Speed", "StExt_Config_Diff_Kapitel_Speed", Value_Type_DefaultPerc, false);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableLevel", "StExt_Config_Diff_EnableLevel", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_Power", "StExt_Config_Diff_Level_Power", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_Hp", "StExt_Config_Diff_Level_Hp", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_Prot", "StExt_Config_Diff_Level_Prot", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_Speed", "StExt_Config_Diff_Level_Speed", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_DuplicationChance", "StExt_Config_Diff_Level_DuplicationChance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_MagicInfusionChance", "StExt_Config_Diff_Level_MagicInfusionChance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_MagicInfusionPower", "StExt_Config_Diff_Level_MagicInfusionPower", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Level_CorruptionChance", "StExt_Config_Diff_Level_CorruptionChance", Value_Type_Percent, false);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_Duplication_Chance", "StExt_Config_Diff_Kapitel_Npc_Duplication_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_Duplication_Count", "StExt_Config_Diff_Kapitel_Npc_Duplication_Count", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_MagicInfusion_Chance", "StExt_Config_Diff_Kapitel_Npc_MagicInfusion_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_MagicInfusion_Power", "StExt_Config_Diff_Kapitel_Npc_MagicInfusion_Power", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_MagicInfusion_CorruptionChance", "StExt_Config_Diff_Kapitel_Npc_MagicInfusion_CorruptionChance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_RandomMeleeWeapon_Chance", "StExt_Config_Diff_Kapitel_Npc_RandomMeleeWeapon_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_RandomRangeWeapon_Chance", "StExt_Config_Diff_Kapitel_Npc_RandomRangeWeapon_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_RandomShield_Chance", "StExt_Config_Diff_Kapitel_Npc_RandomShield_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_RandomHelm_Chance", "StExt_Config_Diff_Kapitel_Npc_RandomHelm_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_RandomScroll_Chance", "StExt_Config_Diff_Kapitel_Npc_RandomScroll_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_Npc_HealingScroll_Chance", "StExt_Config_Diff_Kapitel_Npc_HealingScroll_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_DailyHpMod", "StExt_Config_Diff_Kapitel_DailyHpMod", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_DailyPowerMod", "StExt_Config_Diff_Kapitel_DailyPowerMod", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_Kapitel_DailyProtMod", "StExt_Config_Diff_Kapitel_DailyProtMod", Value_Type_DefaultPerc, false);
		y += fontY * 3;

		//		*** Npc configs strings ***
		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_Config_Npc_Header")->stringdata);
		y += fontY;
		y += fontY;		
		PrintOptionLine(y, "StExt_Str_Config_Npc_EnableStatsRandomization", "StExt_Config_Npc_EnableStatsRandomization", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_StatsRandomization_Hp", "StExt_Config_Npc_StatsRandomization_Hp", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_StatsRandomization_Stats", "StExt_Config_Npc_StatsRandomization_Stats", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_StatsRandomization_Protection", "StExt_Config_Npc_StatsRandomization_Protection", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_StatsRandomization_Damage", "StExt_Config_Npc_StatsRandomization_Damage", Value_Type_DefaultPerc, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_EnableLevelBonus", "StExt_Config_Npc_EnableLevelBonus", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_LevelBonus_Hp", "StExt_Config_Npc_LevelBonus_Hp", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_LevelBonus_Stats", "StExt_Config_Npc_LevelBonus_Stats", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_LevelBonus_Protection", "StExt_Config_Npc_LevelBonus_Protection", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_LevelBonus_Damage", "StExt_Config_Npc_LevelBonus_Damage", Value_Type_Default, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_EnableDuplication", "StExt_Config_Npc_EnableDuplication", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_Duplication_Chance", "StExt_Config_Npc_Duplication_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_Duplication_CountMin", "StExt_Config_Npc_Duplication_CountMin", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_Duplication_CountMax", "StExt_Config_Npc_Duplication_CountMax", Value_Type_Default, false);
		y += fontY;		
		PrintOptionLine(y, "StExt_Str_Config_Npc_Duplication_AllowBosses", "StExt_Config_Npc_Duplication_AllowBosses", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_Duplication_AllowHumansBandits", "StExt_Config_Npc_Duplication_AllowHumansBandits", Value_Type_YesNo, false);		
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_Duplication_AllowHumans", "StExt_Config_Npc_Duplication_AllowHumans", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_Duplication_LevelThreshold", "StExt_Config_Npc_Duplication_LevelThreshold", Value_Type_Default, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_EnableMagicInfusion", "StExt_Config_Npc_EnableMagicInfusion", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_MagicInfusion_Chance", "StExt_Config_Npc_MagicInfusion_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_MagicInfusion_LevelThreshold", "StExt_Config_Npc_MagicInfusion_LevelThreshold", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_MagicInfusion_PowerMod", "StExt_Config_Npc_MagicInfusion_PowerMod", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_MagicInfusion_AllowHumans", "StExt_Config_Npc_MagicInfusion_AllowHumans", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_MagicInfusion_AllowBosses", "StExt_Config_Npc_MagicInfusion_AllowBosses", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_MagicInfusion_CorruptionChance", "StExt_Config_Npc_MagicInfusion_CorruptionChance", Value_Type_Percent, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_EnableRandomEquipment", "StExt_Config_Npc_EnableRandomEquipment", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_RandomMeleeWeapon_Chance", "StExt_Config_Npc_RandomMeleeWeapon_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_RandomRangeWeapon_Chance", "StExt_Config_Npc_RandomRangeWeapon_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_RandomShield_Chance", "StExt_Config_Npc_RandomShield_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_RandomHelm_Chance", "StExt_Config_Npc_RandomHelm_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_RandomScroll_Chance", "StExt_Config_Npc_RandomScroll_Chance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_HealingScroll_Chance", "StExt_Config_Npc_HealingScroll_Chance", Value_Type_Percent, false);
		y += fontY;	
		y += fontY;
		
		
		PrintOptionLine(y, "StExt_Str_Config_Npc_SummonsCanHaveOwnSummons", "StExt_Config_Npc_SummonsCanHaveOwnSummons", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_CantUseSpellAbilities", "StExt_Config_Npc_CantUseSpellAbilities", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_CantUseBuffAbilities", "StExt_Config_Npc_CantUseBuffAbilities", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_CantUseSummonAbilities", "StExt_Config_Npc_CantUseSummonAbilities", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_MagicInfusedAlwaysHasRandomEquipment", "StExt_Config_Npc_MagicInfusedAlwaysHasRandomEquipment", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_LuckyFlagChance", "StExt_Config_Npc_LuckyFlagChance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_RichFlagChance", "StExt_Config_Npc_RichFlagChance", Value_Type_Percent, false);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_Npc_GlobalHpMult", "StExt_Config_Npc_GlobalHpMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_GlobalStatMult", "StExt_Config_Npc_GlobalStatMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_GlobalProtMult", "StExt_Config_Npc_GlobalProtMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_GlobalDamMult", "StExt_Config_Npc_GlobalDamMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Npc_GlobalEsMult", "StExt_Config_Npc_GlobalEsMult", Value_Type_DefaultPerc, false);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_NpcSum_CanBeDuplicated", "StExt_Config_NpcSum_CanBeDuplicated", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NpcSum_CanBeInfused", "StExt_Config_NpcSum_CanBeInfused", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NpcSum_GlobalHpMult", "StExt_Config_NpcSum_GlobalHpMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NpcSum_GlobalStatMult", "StExt_Config_NpcSum_GlobalStatMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NpcSum_GlobalProtMult", "StExt_Config_NpcSum_GlobalProtMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NpcSum_GlobalDamMult", "StExt_Config_NpcSum_GlobalDamMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NpcSum_GlobalEsMult", "StExt_Config_NpcSum_GlobalEsMult", Value_Type_DefaultPerc, false);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_Sum_GlobalHpMult", "StExt_Config_Sum_GlobalHpMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Sum_GlobalStatMult", "StExt_Config_Sum_GlobalStatMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Sum_GlobalProtMult", "StExt_Config_Sum_GlobalProtMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Sum_GlobalDamMult", "StExt_Config_Sum_GlobalDamMult", Value_Type_DefaultPerc, false);
		y += fontY * 3;

		//		*** Luck configs strings ***
		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_Config_Luck_Header")->stringdata);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_EnableRandomLoot_Bodies", "StExt_Config_Luck_EnableRandomLoot_Bodies", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_EnableRandomLoot_Chests", "StExt_Config_Luck_EnableRandomLoot_Chests", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLoot_EnableUnHumanDrop", "StExt_Config_Luck_RandomLoot_EnableUnHumanDrop", Value_Type_YesNo, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_Difficulty", "StExt_Config_Luck_Difficulty", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_Power", "StExt_Config_Luck_Power", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_Quantity", "StExt_Config_Luck_Quantity", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_Money", "StExt_Config_Luck_Money", Value_Type_Percent, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootPowerMod_Food", "StExt_Config_Luck_RandomLootPowerMod_Food", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootPowerMod_Alchemy", "StExt_Config_Luck_RandomLootPowerMod_Alchemy", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootPowerMod_Magic", "StExt_Config_Luck_RandomLootPowerMod_Magic", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootPowerMod_Misk", "StExt_Config_Luck_RandomLootPowerMod_Misk", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootPowerMod_Armor", "StExt_Config_Luck_RandomLootPowerMod_Armor", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootPowerMod_Weapon", "StExt_Config_Luck_RandomLootPowerMod_Weapon", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootPowerMod_Jewelry", "StExt_Config_Luck_RandomLootPowerMod_Jewelry", Value_Type_Percent, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootQuantityMod_Food", "StExt_Config_Luck_RandomLootQuantityMod_Food", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootQuantityMod_Alchemy", "StExt_Config_Luck_RandomLootQuantityMod_Alchemy", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootQuantityMod_Magic", "StExt_Config_Luck_RandomLootQuantityMod_Magic", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootQuantityMod_Munition", "StExt_Config_Luck_RandomLootQuantityMod_Munition", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RandomLootQuantityMod_Misk", "StExt_Config_Luck_RandomLootQuantityMod_Misk", Value_Type_Percent, false);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_Luck_ChanceForEnchantedDrop", "StExt_Config_Luck_ChanceForEnchantedDrop", Value_Type_Percent, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_Food", "StExt_Config_Luck_RelativeDropChance_Food", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_Alchemy", "StExt_Config_Luck_RelativeDropChance_Alchemy", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_Magic", "StExt_Config_Luck_RelativeDropChance_Magic", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_Misk", "StExt_Config_Luck_RelativeDropChance_Misk", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_Munition", "StExt_Config_Luck_RelativeDropChance_Munition", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_Armors", "StExt_Config_Luck_RelativeDropChance_Armors", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_Jewelry", "StExt_Config_Luck_RelativeDropChance_Jewelry", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_MagicWeapons", "StExt_Config_Luck_RelativeDropChance_MagicWeapons", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_MeeleWeapons", "StExt_Config_Luck_RelativeDropChance_MeeleWeapons", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Luck_RelativeDropChance_RangeWeapons", "StExt_Config_Luck_RelativeDropChance_RangeWeapons", Value_Type_Percent, false);
		y += fontY * 3;

		//		*** World randomization configs strings ***
		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_Config_WorldRandomization_Header")->stringdata);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableRot", "StExt_Config_Diff_EnableRot", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableRot_NightEncounters", "StExt_Config_Diff_EnableRot_NightEncounters", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableRot_Avengers", "StExt_Config_Diff_EnableRot_Avengers", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_Diff_EnableRot_Bandits", "StExt_Config_Diff_EnableRot_Bandits", Value_Type_YesNo, false);

		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EnableWorldRandomization_Npc", "StExt_Config_EnableWorldRandomization_Npc", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EnableWorldRandomization_Items", "StExt_Config_EnableWorldRandomization_Items", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_MinCooldown", "StExt_Config_WorldRandomization_MinCooldown", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_MaxCooldown", "StExt_Config_WorldRandomization_MaxCooldown", Value_Type_Default, false);
		y += fontY;		
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_Npc_SpawnChance", "StExt_Config_WorldRandomization_Npc_SpawnChance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_Npc_SpawnPowerMod", "StExt_Config_WorldRandomization_Npc_SpawnPowerMod", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_Npc_SpawnCountMod", "StExt_Config_WorldRandomization_Npc_SpawnCountMod", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_Item_SpawnChance", "StExt_Config_WorldRandomization_Item_SpawnChance", Value_Type_Percent, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_Item_SpawnPowerMod", "StExt_Config_WorldRandomization_Item_SpawnPowerMod", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_Npc_SpawnCountMod", "StExt_Config_WorldRandomization_Item_SpawnCountMod", Value_Type_DefaultPerc, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_WorldRandomization_TradersExtraItemsAddRate", "StExt_Config_WorldRandomization_TradersExtraItemsAddRate", Value_Type_Default, false);
		y += fontY * 3;
		
		//		*** Other configs strings ***
		PrintSectionHeader(y, parser->GetSymbol("StExt_Str_Config_Misk_Header")->stringdata);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ShowNpcLevel", "StExt_Config_ShowNpcLevel", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ShowNpcExtraInfo", "StExt_Config_ShowNpcExtraInfo", Value_Type_YesNo, false);
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_DisplayLuckMessage", "StExt_Config_DisplayLuckMessage", Value_Type_YesNo, false);
		y += fontY;
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EnableSkeletonSkin", "StExt_Config_EnableSkeletonSkin", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EnableSkeletonSkin_OnlyAtNight", "StExt_Config_EnableSkeletonSkin_OnlyAtNight", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EnableFemaleLook", "StExt_Config_EnableFemaleSkin", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EnableFemaleLook_AlwaysWig", "StExt_Config_EnableFemaleSkin_AlwaysWig", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EnableFemaleLook_DemonLook", "StExt_Config_EnableFemaleSkin_DemonLook", Value_Type_YesNo, false);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_GainExpMod", "StExt_Config_GainExpMod", Value_Type_DefaultPerc, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExpMod", "StExt_Config_ExpMod", Value_Type_InvertPerc, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NpcExp", "StExt_Config_NpcExp", Value_Type_DefaultPerc, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_PartyExp", "StExt_Config_PartyExp", Value_Type_DefaultPerc, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_SncExp", "StExt_Config_SncExp", Value_Type_DefaultPerc, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_BelliarRageExp", "StExt_Config_BelliarRageExp", Value_Type_DefaultPerc, true);
		y += fontY;
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_BelliarRage_Active", "StExt_Config_BelliarRage_Active", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_BelliarRage_SpawnMonsters", "StExt_Config_BelliarRage_SpawnMonsters", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_BelliarRage_StaminaReduce", "StExt_Config_BelliarRage_StaminaReduce", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_BelliarRage_AddDamageReduce", "StExt_Config_BelliarRage_AddDamageReduce", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_BelliarRage_RegenEnemies", "StExt_Config_BelliarRage_RegenEnemies", Value_Type_YesNo, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_BelliarRage_SoundEnable", "StExt_Config_BelliarRage_SoundEnable", Value_Type_YesNo, false);
		y += fontY;		
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_ExtraLp", "StExt_Config_ExtraLp", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExtraHpPerLevel", "StExt_Config_ExtraHpPerLevel", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExtraMpPerLevel", "StExt_Config_ExtraMpPerLevel", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExtraEsPerLevel", "StExt_Config_ExtraEsPerLevel", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExtraStrPerLevel", "StExt_Config_ExtraStrPerLevel", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExtraAgiPerLevel", "StExt_Config_ExtraAgiPerLevel", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExtraIntPerLevel", "StExt_Config_ExtraIntPerLevel", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ExtraLuckPerLevel", "StExt_Config_ExtraLuckPerLevel", Value_Type_Percent, false);
		y += fontY;
		y += fontY;
		
		PrintOptionLine(y, "StExt_Str_Config_MasteriesExpMult", "StExt_Config_MasteriesExpMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_CorruptionExpMult", "StExt_Config_CorruptionExpMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ArtifactsExpMult", "StExt_Config_ArtifactsExpMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_ArtifactsUpgradeCostMult", "StExt_Config_ArtifactsUpgradeCostMult", Value_Type_DefaultPerc, false);
		y += fontY;		
		PrintOptionLine(y, "StExt_Str_Config_EducationMoneyCostMult", "StExt_Config_EducationMoneyCostMult", Value_Type_DefaultPerc, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_EducationRequirementsMult", "StExt_Config_EducationRequirementsMult", Value_Type_DefaultPerc, false);
		y += fontY;		
		y += fontY;

		PrintOptionLine(y, "StExt_Str_Config_FoodBonusResetDay", "StExt_Config_FoodBonusResetDay", Value_Type_Default, false);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_NoChapterForMagicCircle", "StExt_Config_NoChapterForMagicCircle", Value_Type_YesNo, true);
		y += fontY;
		PrintOptionLine(y, "StExt_Str_Config_DisplayBuffEffects", "StExt_Config_DisplayBuffEffects", Value_Type_YesNo, true);		
		y += fontY * 4;

		zSTRING modVersionString = ModVersionString;
		zSTRING modAuthor = "Created by StonedWizzard.";
		PrintSectionHeader(y, modVersionString);
		y += fontY;
		PrintSectionHeader(y, modAuthor, zCOLOR(250, 250, 250));
		y += fontY * 2;
		PrintSectionHeader(y, Z "Special thanks to:");
		y += fontY;
		PrintSectionHeader(y, Z "Liker, Haart (New Balance mod Team)", zCOLOR(250, 250, 250));
		y += fontY;
		PrintSectionHeader(y, Z "Gratt (zParserExtender), Piranha Bytes", zCOLOR(250, 250, 250));
		y += fontY;
		PrintSectionHeader(y, Z "And");
		y += fontY;
		PrintSectionHeader(y, Z "ToXaL1, Icefist, Junes, Shiva", zCOLOR(250, 250, 250));
		y += fontY;
		PrintSectionHeader(y, Z "Gothicmap, Dezomorphin, lev4enko and others...", zCOLOR(250, 250, 250));
	}

	void StatsMenu::Loop()
	{
		if (!ogame || !player || !screen)
			return;
		if (!StExtStatMenuInitialized) return;

		refreshRate += 1;
		if (refreshRate > 30)
		{
			refreshRate = 0;			
			Resize();			
		}

		if (ogame->IsOnPause() || !ShowModMenu)
			IsShow = false;

		screen->RemoveItem(View);
		if (IsShow)
		{
			BlockMovement = true;
			player->SetMovLock(BlockMovement);
			View->ClrPrintwin();
			DrawTabTitle();
			if (CurrentTab == 0 ) { DrawGeneral(); }			// General statistic			
			else if(CurrentTab == 1) { DrawSkills(); }			// Talents			
			else if (CurrentTab == 2) { DrawAllStats(); }		// All stats
			else if (CurrentTab == 3) { DrawBonusStats(); }		// All bonus stats		
			else if (CurrentTab == 4) { DrawModConfigs(); }		// Mod configs			
			screen->InsertItem(View);
		}
		else if(BlockMovement)
			player->SetMovLock(true);
		else
			player->SetMovLock(false);
	}
	
	HOOK ivk_oCGame_HandleEvent PATCH(&oCGame::HandleEvent, &oCGame::HandleEvent_StExt);
	int oCGame::HandleEvent_StExt(int key) 
	{
		if (IsLoading || IsLevelChanging) return true;

		if (ogame && player && !GetWorld()->csPlayer->GetPlayingGlobalCutscene())
		{
			int isHandled = *(int*)parser->CallFunc(HandleKeyEventFunc, key);
			if (isHandled) return true;

			int modMenuKey = parser->GetSymbol("StExt_Config_ModMenuKey")->single_intdata;
			int isModDialog = parser->GetSymbol("StExt_DisplayModMenu")->single_intdata;
			float scrollMult = 1.5f;
			if (StExtStatMenuInitialized && !isModDialog && !isHandled)
			{
				if ((zKeyPressed(modMenuKey) || key == modMenuKey) && zKeyPressed(KEY_LSHIFT) && ShowModMenu)
				{
					if (StExtStatMenu.IsShow) StExtStatMenu.IsShow = false;
					else StExtStatMenu.IsShow = true;
					isHandled = true; 
				}
				else if (StExtStatMenu.IsShow)
				{
					if (zKeyPressed(KEY_LSHIFT)) scrollMult = 15.0f;

					if (key == KEY_A || key == KEY_LEFTARROW) { StExtStatMenu.PrevTab(); isHandled = true; }
					if (key == KEY_D || key == KEY_RIGHTARROW) { StExtStatMenu.NextTab(); isHandled = true; }
					if (key == KEY_W || key == KEY_UPARROW) { StExtStatMenu.ChangeOffset(-2048 * scrollMult); isHandled = true; }
					if (key == KEY_S || key == KEY_DOWNARROW) { StExtStatMenu.ChangeOffset(2048 * scrollMult); isHandled = true; }
					if (key == KEY_ESCAPE || key == KEY_TAB) { StExtStatMenu.IsShow = false; isHandled = true; }
				}
			}
			if (isHandled) return true;
		}
		return THISCALL(ivk_oCGame_HandleEvent) (key);
	}
	
	HOOK ivk_zCViewDialogChoice_HandleEvent PATCH(&zCViewDialogChoice::HandleEvent, &zCViewDialogChoice::HandleEvent_StExt);
	int zCViewDialogChoice::HandleEvent_StExt(int key)
	{
		int isModMenu = parser->GetSymbol("StExt_DisplayModMenu")->single_intdata;
		if (isModMenu)
		{
			int uiMoving = parser->GetSymbol("StExt_UiMovingEnabled")->single_intdata;
			int configValueEdit = parser->GetSymbol("StExt_ConfigValueEditEnabled")->single_intdata;
			bool isDirKeys = (key == KEY_LEFTARROW || key == KEY_RIGHTARROW || key == KEY_UPARROW || key == KEY_DOWNARROW);
			if (isDirKeys && (uiMoving || configValueEdit))
			{
				int funcIndex = parser->GetIndex("StExt_HandleUiButtom");
				parser->CallFunc(funcIndex, key);
				return true;
			}
		}
		return THISCALL(ivk_zCViewDialogChoice_HandleEvent) (key);
	}
}
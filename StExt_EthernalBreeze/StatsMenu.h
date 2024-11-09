#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	class StatsMenu
	{
	public:
		zCView* View;
		int IsShow;
		int CurrentTab;
		int MaxTab;
		
		float ScaleX, ScaleY;
		int SizeX;
		int SizeY;
		int PosX;
		int PosY;
		uint refreshRate;

		int PrintBorderLeft;
		int PrintBorderRight;
		int PrintBorderTop;
		int PrintBorderBottom;
		int PrintOffset;
		int PrintOffsetMax;

		void Loop();
		void Init();
		void Resize();
		void DrawTabTitle();
		void DrawAllStats();
		void DrawBonusStats();
		void DrawSkills();
		void DrawGeneral();
		void DrawModConfigs();
		void PrintText(int x, int y, zSTRING text);
		int PrintTextMult(int x, int y, zSTRING text);
		void PrintSectionHeader(int y, zSTRING sectText, zCOLOR color);
		void PrintModPresetName(int);
		void NextTab();
		void PrevTab();
		void ChangeOffset(int offset);
		int IsPercentStat(int index);		
		zSTRING IntValueToSkill(int val);
		zCOLOR GetTalentRankColor(int val);
		zSTRING GetTalentRank(int val);
		void PrintOptionLine(int y, zSTRING optionIndex, zSTRING valIndex, int valType, int optIsImportant);
		void PrintStatLine(int& y, zSTRING statDesc, int statVal, int valType, zCOLOR color);
		void StatsMenu::PrintTextColumn(int& y, zSTRING text, zCOLOR color);
	};
}

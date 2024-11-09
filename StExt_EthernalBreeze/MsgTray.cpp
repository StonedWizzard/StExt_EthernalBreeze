#include <UnionAfx.h>
#include <string> 
#include <regex>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	struct MsgTrayEntry
	{
		zSTRING Text;
		zCOLOR Color;
		int LifeTime;
	};

	zCView* MsgTrayView;
	Array<MsgTrayEntry> MsgTrayBuffer = Array<MsgTrayEntry>();
	const int UpdateInterval = 30;
	const int LifeTimeMax = 24;
	int UpdateCounter;

	zCOLOR ParseHexColor(std::string inputColor)
	{
		std::regex pattern("#([0-9a-fA-F]{6})");
		std::smatch match;
		if (std::regex_match(inputColor, match, pattern))
		{
			int r, g, b;
			sscanf(match.str(1).c_str(), "%2x%2x%2x", &r, &g, &b);
			return zCOLOR(r, g, b);
		}
		return zCOLOR(220, 220, 220);
	}

	void StonedExtension_MsgTray_Loop()
	{
		if (ogame && ogame->IsOnPause()) return;

		UpdateCounter += 1;
		if (ogame && UpdateCounter < UpdateInterval) return;

		if (!MsgTrayView)
		{
			MsgTrayView = zNEW(zCView)(0, 0, 8192, 8192);
			MsgTrayView->SetSize(8192, 8192);
			MsgTrayView->SetPos(0, 0);
			screen->InsertItem(MsgTrayView);
		}
		MsgTrayView->ClrPrintwin();

		int y, x;
		y = parser->GetSymbol("StExt_Config_MsgTray_PosY")->single_intdata * 81.919998;
		x = parser->GetSymbol("StExt_Config_MsgTray_PosX")->single_intdata * 81.919998;
		int fontY = MsgTrayView->FontY() + 96;

		int printLine = y;
		for (int i = 0; i < MsgTrayBuffer.GetNum(); i++)
		{
			int printLineTmp = y - (MsgTrayBuffer[i].LifeTime * fontY);
			while (printLineTmp >= printLine) printLineTmp = printLine - fontY;
			printLine = printLineTmp;

			MsgTrayView->SetFontColor(MsgTrayBuffer[i].Color);
			MsgTrayView->Print(x - (MsgTrayView->FontSize(MsgTrayBuffer[i].Text) * 0.5f), printLine, MsgTrayBuffer[i].Text);
			MsgTrayView->SetFontColor(zCOLOR(250, 250, 250));

			MsgTrayBuffer[i].LifeTime += 1;
			if (MsgTrayBuffer[i].LifeTime >= LifeTimeMax)
			{
				MsgTrayBuffer.RemoveAt(i);
				i -= 1;
			}
		}
		UpdateCounter = 0;
	}

	void MsgTray_AddEntry(zSTRING text, zSTRING color)
	{
		MsgTrayEntry entry = MsgTrayEntry();
		entry.Text = zSTRING(text);
		entry.Color = ParseHexColor(color.ToChar());
		MsgTrayBuffer.InsertFront(entry);
		UpdateCounter += UpdateInterval;
	}
}
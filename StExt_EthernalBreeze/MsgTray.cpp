#include <UnionAfx.h>
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
	int MsgTrayPosX_SymIndex = Invalid, MsgTrayPosY_SymIndex = Invalid;

	void StonedExtension_Loop_MsgTray()
	{
		UpdateCounter += 1;
		if (IsLoading || IsLevelChanging || (UpdateCounter < UpdateInterval) || !ogame || (ogame && ogame->IsOnPause())) return;

		if (!MsgTrayView)
		{
			MsgTrayView = zNEW(zCView)(0, 0, ScreenVBufferSize, ScreenVBufferSize);
			MsgTrayView->SetSize(ScreenVBufferSize, ScreenVBufferSize);
			MsgTrayView->SetPos(0, 0);
			screen->InsertItem(MsgTrayView);
		}
		MsgTrayView->ClrPrintwin();

		int x, y;
		x = static_cast<int>(parser->GetSymbol(MsgTrayPosX_SymIndex)->single_intdata * 81.919998f);
		y = static_cast<int>(parser->GetSymbol(MsgTrayPosY_SymIndex)->single_intdata * 81.919998f);
		const int fontY = MsgTrayView->FontY() + 96;

		int printLine = y;
		for (uint i = 0; i < MsgTrayBuffer.GetNum(); ++i)
		{
			int printLineTmp = y - (MsgTrayBuffer[i].LifeTime * fontY);
			while (printLineTmp >= printLine) printLineTmp = printLine - fontY;
			printLine = printLineTmp;

			MsgTrayView->SetFontColor(MsgTrayBuffer[i].Color);
			MsgTrayView->Print(static_cast<int>(x - (MsgTrayView->FontSize(MsgTrayBuffer[i].Text) * 0.5f)), printLine, MsgTrayBuffer[i].Text);
			MsgTrayView->SetFontColor(TextColor_Default);

			++(MsgTrayBuffer[i].LifeTime);
			if (MsgTrayBuffer[i].LifeTime >= LifeTimeMax)
			{
				MsgTrayBuffer.RemoveAt(i);
				--i;
			}
		}
		UpdateCounter = 0;
	}

	void MsgTray_AddEntry(zSTRING text, zSTRING color)
	{
		MsgTrayEntry entry = MsgTrayEntry();
		entry.Text = zSTRING(text);
		ParseHexColor(color, entry.Color);
		MsgTrayBuffer.InsertFront(entry);
		UpdateCounter += UpdateInterval;
	}

	void MsgTray_Clear() { MsgTrayBuffer.Clear(); }
}
#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	const int StExt_UncaperStatIndex_Max = 7;
	const int StExt_UncaperStatIndex_StamMax = 0;
	const int StExt_UncaperStatIndex_StamCur = 1;
	const int StExt_UncaperStatIndex_LearnSpells = 2;
	const int StExt_UncaperStatIndex_BowTalent = 3;
	const int StExt_UncaperStatIndex_CBowTalent = 4;
	const int StExt_UncaperStatIndex_1hTalent = 5;
	const int StExt_UncaperStatIndex_2hTalent = 6;

	Array<UncapedStatData*> UnacapedStatsData;
	bool UncaperStarted = false;

	int UncaperSetFunc = Invalid;
	int UncaperGetFunc = Invalid;
	int UncaperUpdateFunc = Invalid;

	zCPar_Symbol* UncaperStatNowArray = Null;
	zCPar_Symbol* UncaperStatWasArray = Null;
	zCPar_Symbol* UncaperStatBonusNowArray = Null;
	zCPar_Symbol* UncaperStatBonusWasArray = Null;

	void InitStatsUncaper()
	{
		zCParser* par = zCParser::GetParser();
		zCPar_Symbol* uncaperIndxArray = par->GetSymbol("StExt_UncaperStatsIndexArray");
		if (!uncaperIndxArray)
		{
			DEBUG_MSG("'StExt_UncaperStatsIndexArray' not found!");
			return;
		}

		UnacapedStatsData = Array<UncapedStatData*>();
		DEBUG_MSG("Initialize " + Z(int)uncaperIndxArray->ele + "/" + Z StExt_UncaperStatIndex_Max + " uncapers...");
		for (unsigned int i = 0; i < uncaperIndxArray->ele; i++)
		{
			int index = par->GetIndex(uncaperIndxArray->stringdata[i]);
			UncapedStatData* statData = new UncapedStatData();
			parser->CreateInstance(index, statData);
			UnacapedStatsData.InsertEnd(statData);
		}

		UncaperSetFunc = par->GetIndex("StExt_StatUncaper_SetValue");
		UncaperGetFunc = par->GetIndex("StExt_StatUncaper_GetValue");
		UncaperUpdateFunc = par->GetIndex("");

		UncaperStatNowArray = par->GetSymbol("StExt_PcStatsUncaper_Now");
		UncaperStatWasArray = par->GetSymbol("StExt_PcStatsUncaper_Was");
		UncaperStatBonusNowArray = par->GetSymbol("StExt_PcStatsUncaper_BonusNow");
		UncaperStatBonusWasArray = par->GetSymbol("StExt_PcStatsUncaper_BonusWas");

		DEBUG_MSG("StatsUncaper initialized!");
	}

	void inline UpdateUncapedStat(int indx)
	{
		if (indx < 0 || indx > StExt_UncaperStatIndex_Max)
		{
			DEBUG_MSG("UpdateUncapedStat - invalid index: " + Z indx);
			return;
		}
		if (!UncaperStarted) return;

		UncapedStatData* statData = UnacapedStatsData[indx];
		if (!statData)
		{
			DEBUG_MSG("UpdateUncapedStat - statData is invalid. index: " + Z indx);
			return;
		}
		int statNow = *(int*)parser->CallFunc(UncaperGetFunc, indx);

		// value didn't change
		if (statData->Now == statNow) return;

		int bonusNow = statNow > statData->Max ? statNow - statData->Max : 0;
		int statWas = statData->Now;
		int bonusWas = statData->BonusNow;
		int statDelta = statNow - statWas;
		//int bonusDelta = bonusNow - bonusWas;
		//int offsetDelta = statNow - statData->Max;
		int statChange = 0;

		bool isStatUncaped = statNow > statData->Max;
		bool isStatWasUncaped = statWas > statData->Max;
		bool isStatDecreased = statDelta < 0;

		// state cases:
		// value didn't change
		// value was normal but increased to normal
		// value was normal but increased to overcap
		// value was overcap but decreased to normal exact
		// value was overcap but decreased to normal or less
		// value was overcap but decreased to overcap
		// value was overcap and increased to overcap

		// value was overcap but decreased to...
		if (isStatWasUncaped && isStatDecreased)
		{
			// ... to normal exact
			if (statNow == statData->Max) statChange = bonusWas;
		}

		statNow += statChange;		
		statData->Was = statData->Now;
		statData->Now = statNow;
		bonusNow = statData->Now > statData->Max ? statData->Now - statData->Max : 0;
		statData->BonusWas = statData->BonusNow;
		statData->BonusNow = bonusNow;

		/*
		if (indx == 1)
		{
			DEBUG_MSG(" ");
			DEBUG_MSG("UpdateUncapedStat - Stamina Exit: ");
			DEBUG_MSG("UpdateUncapedStat - CurrentValue: " + Z statNow);
			DEBUG_MSG("UpdateUncapedStat - Now: " + Z statData->Now);
			DEBUG_MSG("UpdateUncapedStat - Was: " + Z statData->Was);
			DEBUG_MSG("UpdateUncapedStat - BonusNow: " + Z statData->BonusNow);
			DEBUG_MSG("UpdateUncapedStat - BonusWas: " + Z statData->BonusWas);
			DEBUG_MSG("UpdateUncapedStat - bonusNow: " + Z bonusNow);
			DEBUG_MSG("UpdateUncapedStat - statDelta: " + Z statDelta);
			DEBUG_MSG("UpdateUncapedStat - bonusDelta: " + Z bonusDelta);
			DEBUG_MSG("UpdateUncapedStat - offsetDelta: " + Z offsetDelta);
			DEBUG_MSG("UpdateUncapedStat - CHANGE: " + Z statChange);
			DEBUG_MSG(" ");
		}*/		

		parser->CallFunc(UncaperSetFunc, indx, statData->Now);
		UncaperStatNowArray->SetValue(statData->Now, indx);
		UncaperStatWasArray->SetValue(statData->Was, indx);
		UncaperStatBonusNowArray->SetValue(statData->BonusNow, indx);
		UncaperStatBonusWasArray->SetValue(statData->BonusWas, indx);
	}

	void StatsUncaperLoop()
	{
		if (!UncaperStarted || !ogame || !player || ogame->IsOnPause())
			return;

		for (int i = 0; i < StExt_UncaperStatIndex_Max; i++)
			UpdateUncapedStat(i);	
	}

	void StartUncaper()
	{
		for (int i = 0; i < StExt_UncaperStatIndex_Max; i++)
		{
			UncapedStatData* statData = UnacapedStatsData[i];
			if (!statData)
			{
				DEBUG_MSG("UpdateUncapedStat - statData is invalid. index: " + Z i);
				continue;
			}

			int statNow = *(int*)parser->CallFunc(UncaperGetFunc, i);
			int bonusNow = statNow > statData->Max ? statNow - statData->Max : 0;
			statData->Now = statNow;
			statData->Was = UncaperStatNowArray->intdata[i];
			statData->BonusNow = bonusNow;
			statData->BonusWas = UncaperStatBonusWasArray->intdata[i];
			UpdateUncapedStat(i);
		}
		UncaperStarted = true;
	}

	void StopUncaper() { UncaperStarted = false; }
}
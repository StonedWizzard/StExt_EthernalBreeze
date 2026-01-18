#include <UnionAfx.h>
#include <StonedExtension.h>

namespace Gothic_II_Addon
{
	Map<zSTRING, zSTRING> FemVoiceOverrides = {};

	bool VoiceControllerInitialized = false;
	int VoiceControllerShouldRead = false;

	int VoiceControllerEnabled = 1;
	int VoiceRate = 11;
	int VoiceVolume = -1;
	int Language = 1;
	uint32 CodePage;
	bool IsFemale = false;	
	wstring LanguageID;	
	wstring Gender = L"Female";

	ISpVoice* Voice;
	oCNpc* CurrentSpeaker = Null;
	int CurrentHandle = 0;

	inline void InitVoiceControllerConfigs()
	{
		DEBUG_MSG("InitVoiceControllerConfigs - init configs...");

		VoiceControllerEnabled = parser->GetSymbol("StExt_Config_FemaleVoice_Enable")->single_intdata;
		VoiceRate = parser->GetSymbol("StExt_Config_FemaleVoice_Rate")->single_intdata;
		VoiceVolume = parser->GetSymbol("StExt_Config_FemaleVoice_Volume")->single_intdata;
		Language = parser->GetSymbol("StExt_Config_FemaleVoice_Language")->single_intdata;
		CodePage = parser->GetSymbol("StExt_Config_FemaleVoice_CodePage")->single_intdata;
		IsFemale = parser->GetSymbol("StExt_Config_EnableFemaleSkin")->single_intdata;

		VoiceControllerShouldRead = IsFemale && VoiceControllerEnabled;
		zSTRING langId = parser->GetSymbol("StExt_Config_FemaleVoice_LanguageId")->stringdata;

		if (VoiceVolume < 0)
			VoiceVolume = static_cast<int>(zoptions->ReadReal("SOUND", "soundVolume", 1.0f) * 200.0f);
		
		switch (Language)
		{
			case Lang_Rus:			
				LanguageID = L"419";
				CodePage = ANSI_CODEPAGE_CYRILLIC;
				break;
			case Lang_Ger:			
				LanguageID = L"407";
				CodePage = ANSI_COPEDAGE_NORTHORWESTERN_EUROPEAN;
				break;
			case Lang_Pol:
				LanguageID = L"415";
				CodePage = ANSI_COPEDAGE_CENTRALOREASTERN_EUROPEAN;
				break;
			case Lang_Rou:
				LanguageID = L"418";
				CodePage = ANSI_COPEDAGE_CENTRALOREASTERN_EUROPEAN;
				break;
			case Lang_Ita:
				LanguageID = L"410";
				CodePage = ANSI_COPEDAGE_NORTHORWESTERN_EUROPEAN;
				break;
			case Lang_Cze:
				LanguageID = L"405";
				CodePage = ANSI_COPEDAGE_CENTRALOREASTERN_EUROPEAN;
				break;
			case Lang_Esp:
				LanguageID = L"C0A";
				CodePage = ANSI_COPEDAGE_NORTHORWESTERN_EUROPEAN;
				break;
			case Lang_Other:
				LanguageID = string(langId.Length() > 0 ? langId : "409").AToW();
				break;
			case Lang_Eng:
			default:
				LanguageID = L"409";
				CodePage = ANSI_COPEDAGE_NORTHORWESTERN_EUROPEAN;
				break;
		}

		DEBUG_MSG("InitVoiceControllerConfigs - init configs DONE!");
	}

	inline void ReleaseVoice() 
	{
		if (Voice)
		{
			Voice->Release();
			Voice = NULL;
		}
	}

	inline bool InitVoice()
	{
		HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&Voice);
		if (!SUCCEEDED(hr))
		{
			DEBUG_MSG("InitVoice - ISp voice initialization failed!");
			return false;
		}
		return true;
	}
	
	void VoiceControllerReloadVoiceSettings()
	{
		if (!VoiceControllerInitialized) return;
		if (!Voice)
		{
			if (!InitVoice()) return;
		}

		Voice->SetVolume(VoiceVolume);
		Voice->SetRate(VoiceRate);

		CComPtr<IEnumSpObjectTokens> cpEnum;
		CComPtr<ISpObjectToken> cpToken;

		SpEnumTokens(L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech_OneCore\\Voices", L"Gender = " + Gender + L"; Language = " + LanguageID, NULL, &cpEnum);
		cpEnum->Next(1, &cpToken, NULL);
		Voice->SetVoice(cpToken);

		cpEnum.Release();
		cpToken.Release();
	}

	void InitializeVoiceOverrides()
	{
		ExtraStatsNameData = Map<int, zSTRING>();
		zCPar_Symbol* soundOverrideArray = parser->GetSymbol("StExt_FemaleVoiceReplacementIndexArray");
		if (!soundOverrideArray)
		{
			DEBUG_MSG("InitializeVoiceOverrides - 'StExt_FemaleVoiceReplacementIndexArray' not found!");
			return;
		}

		FemVoiceOverrides = Map<zSTRING, zSTRING>();
		for (unsigned int i = 0; i < soundOverrideArray->ele; ++i)
		{
			zSTRING strIndx = soundOverrideArray->stringdata[i];
			const int index = parser->GetIndex(strIndx);
			if (index == Invalid)
			{
				DEBUG_MSG("InitializeVoiceOverrides - Can't load infusion instance '" + strIndx + "'!");
				continue;
			}
			SoundOverrideData overrideData = SoundOverrideData();
			parser->CreateInstance(index, &overrideData);
			FemVoiceOverrides.Insert(overrideData.OrigSound, overrideData.NewSound);
		}
	}

	bool InitVoiceController() 
	{
		InitializeVoiceOverrides();
		if (!InitVoice())
		{
			DEBUG_MSG("InitVoiceController - ISp voice initialization failed!");
			return false;
		}

		InitVoiceControllerConfigs();
		VoiceControllerInitialized = true;
		return true;
	}


	bool VoiceControllerIsDoneReading() 
	{
		if (!Voice)
			return true;

		SPVOICESTATUS status;
		Voice->GetStatus(&status, NULL);
		if (status.dwRunningState == SPRS_DONE)
			return true;

		return false;
	}

	oCNpc* VoiceControllerGetCurrentSpeaker() { return CurrentSpeaker; }
	int VoiceControllerGetCurrentHandle() { return CurrentHandle; }

	void VoiceControllerRead(const string& str, oCNpc* speaker, int handle) 
	{
		VoiceControllerReloadVoiceSettings();

		CurrentHandle = handle;
		CurrentSpeaker = speaker;
		DWORD flags = SPF_ASYNC | SPF_IS_NOT_XML | SPF_PURGEBEFORESPEAK;
		Voice->Speak(str.AToW(CodePage), flags, NULL);
	}

	void VoiceControllerStopReading() 
	{
		ReleaseVoice();
		CurrentSpeaker = Null;
		CurrentHandle = 0;
	}

	inline bool RequireHookFemSound(const zSTRING& nameSFX, zSTRING& newNameSFX)
	{
		if (!parser || !StExt_ModReady) return false;

		const int isFem = parser->GetSymbol("StExt_Config_EnableFemaleSkin")->single_intdata;
		if (isFem)
		{
			const auto& pair = FemVoiceOverrides.GetSafePair(nameSFX);
			if (!pair) return false;			
			newNameSFX = pair->GetValue();
			return true;			
		}
		return false;
	}

	//-------------------------------------------------------------------
	//						        Hooks
	//-------------------------------------------------------------------

	HOOK Ivk_zCSndSys_MSS_LoadSoundFXScript PATCH(&zCSndSys_MSS::LoadSoundFXScript, &zCSndSys_MSS::LoadSoundFXScript_StExt);
	zCSoundFX* zCSndSys_MSS::LoadSoundFXScript_StExt(zSTRING const& nameSFX)
	{
		zSTRING newNameSFX = zString_Empty;
		if (RequireHookFemSound(nameSFX, newNameSFX))
			return THISCALL(Ivk_zCSndSys_MSS_LoadSoundFXScript)(newNameSFX);
		return THISCALL(Ivk_zCSndSys_MSS_LoadSoundFXScript)(nameSFX);
	}

	HOOK Ivk_zCSndSys_MSS_LoadSoundFX PATCH(&zCSndSys_MSS::LoadSoundFX, &zCSndSys_MSS::LoadSoundFX_StExt);
	zCSoundFX* zCSndSys_MSS::LoadSoundFX_StExt(zSTRING const& nameSFX)
	{
		zSTRING newNameSFX = zString_Empty;
		if (RequireHookFemSound(nameSFX, newNameSFX))
			return THISCALL(Ivk_zCSndSys_MSS_LoadSoundFX)(newNameSFX);
		return THISCALL(Ivk_zCSndSys_MSS_LoadSoundFX)(nameSFX);
	}

	HOOK Ivk_zCSoundSystem_LoadSoundFXScript PATCH(&zCSoundSystem::LoadSoundFXScript, &zCSoundSystem::LoadSoundFXScript_StExt);
	zCSoundFX* zCSoundSystem::LoadSoundFXScript_StExt(zSTRING const& nameSFX)
	{
		zSTRING newNameSFX = zString_Empty;
		if(RequireHookFemSound(nameSFX, newNameSFX))
			return THISCALL(Ivk_zCSoundSystem_LoadSoundFXScript)(newNameSFX);
		return THISCALL(Ivk_zCSoundSystem_LoadSoundFXScript)(nameSFX);
	}

	HOOK Ivk_zCSoundSystem_LoadSoundFX PATCH(&zCSoundSystem::LoadSoundFX, &zCSoundSystem::LoadSoundFX_StExt);
	zCSoundFX* zCSoundSystem::LoadSoundFX_StExt(zSTRING const& nameSFX)
	{
		zSTRING newNameSFX = zString_Empty;
		if (RequireHookFemSound(nameSFX, newNameSFX))
			return THISCALL(Ivk_zCSoundSystem_LoadSoundFX)(newNameSFX);
		return THISCALL(Ivk_zCSoundSystem_LoadSoundFX)(nameSFX);
	}

	HOOK Ivk_zCView_DialogMessageCXY PATCH(&zCView::DialogMessageCXY, &zCView::DialogMessageCXY_StExt);
	void zCView::DialogMessageCXY_StExt(zSTRING const& name, zSTRING const& text, float time, zCOLOR& color)
	{
		if (VoiceControllerShouldRead) time = text.Length() * 125.0f;
		THISCALL(Ivk_zCView_DialogMessageCXY)(name, text, time, color);
	}

	HOOK Ivk_oCNpc_StopAllVoices AS(&oCNpc::StopAllVoices, &oCNpc::StopAllVoices_StExt);
	void oCNpc::StopAllVoices_StExt()
	{
		THISCALL(Ivk_oCNpc_StopAllVoices)();
		if (this == VoiceControllerGetCurrentSpeaker()) 
			VoiceControllerStopReading();
	}

	HOOK Ivk_oCNpc_UpdateNextVoice PATCH(&oCNpc::UpdateNextVoice, &oCNpc::UpdateNextVoice_StExt);
	int oCNpc::UpdateNextVoice_StExt()
	{
		if (voiceIndex >= listOfVoiceHandles.GetNum())
			return THISCALL(Ivk_oCNpc_UpdateNextVoice)();

		const auto currentHandle = listOfVoiceHandles.GetSafe(voiceIndex);
		if (currentHandle == VoiceControllerGetCurrentHandle())
		{
			if (VoiceControllerIsDoneReading()) listOfVoiceHandles.RemoveIndex(voiceIndex);
			else voiceIndex += 1;

			if (voiceIndex >= listOfVoiceHandles.GetNum())
			{
				voiceIndex = 0;
				return 1;
			}
		}
		return THISCALL(Ivk_oCNpc_UpdateNextVoice)();
	}

	HOOK Ivk_oCNpc_EV_PlaySound PATCH(&oCNpc::EV_PlaySound, &oCNpc::EV_PlaySound_StExt);
	int oCNpc::EV_PlaySound_StExt(oCMsgConversation* msg)
	{
		if ((this != player) && (this != oCInformationManager::GetInformationManager().Npc))
			return THISCALL(Ivk_oCNpc_EV_PlaySound)(msg);

		if (msg->handle == 0)
		{
			VoiceControllerEnabled = parser->GetSymbol("StExt_Config_FemaleVoice_Enable")->single_intdata;
			IsFemale = parser->GetSymbol("StExt_Config_EnableFemaleSkin")->single_intdata;
			VoiceControllerShouldRead = IsFemale && VoiceControllerEnabled && (this == player);
		}

		int result = THISCALL(Ivk_oCNpc_EV_PlaySound)(msg);
		if (zsound->IsSoundActive(msg->handle) && VoiceControllerShouldRead)
			zsound->StopSound(msg->handle);

		if (VoiceControllerShouldRead) 
		{
			msg->f_no = msg->text.Length() * 125;
			VoiceControllerRead(string(msg->text), this, msg->handle);
			VoiceControllerShouldRead = false;
		}

		if (VoiceControllerGetCurrentHandle() == msg->handle && VoiceControllerIsDoneReading())
		{
			ogame->GetWorld()->csPlayer->StopAllOutputUnits(this);
			VoiceControllerStopReading();
		}
		return result;
	}
}
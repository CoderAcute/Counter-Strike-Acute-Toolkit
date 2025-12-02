#include"MenuManager.hpp"
#include"../../../Include/Charsets/Charsets.hpp"

void MenuManager::Init(HMENU* hMenuMain, WindowCommand* WndCmd) {
	this->WinCommand = WndCmd;
	*hMenuMain = CreateMenu();

	HMENU hM0_Ctrl = CreatePopupMenu();
	AppendMenuW(*hMenuMain, MF_STRING | MF_POPUP, (UINT_PTR)hM0_Ctrl, U8ToW(this->MenuStrings.Ctrl).c_str()); {
		AppendMenuW(hM0_Ctrl, MF_STRING, this->WinCommand->OpenCS2, U8ToW(this->MenuStrings.OpenCS2).c_str());
		//AppendMenuW(hM0_Ctrl, MF_STRING, this->WinCommand->Init, U8ToW(this->MenuStrings.Init).c_str());
		//AppendMenuW(hM0_Ctrl, MF_SEPARATOR, 0, NULL);
		//AppendMenuW(hM0_Ctrl, MF_STRING, this->WinCommand->Inject, U8ToW(this->MenuStrings.Inject).c_str());
		AppendMenuW(hM0_Ctrl, MF_SEPARATOR, 0, NULL);
		AppendMenuW(hM0_Ctrl, MF_STRING, this->WinCommand->Exit, U8ToW(this->MenuStrings.Exit).c_str());
	}

	/*HMENU hM0_Monitor = CreatePopupMenu();
	AppendMenuW(*hMenuMain, MF_STRING | MF_POPUP, (UINT_PTR)hM0_Monitor, U8ToW(this->MenuStrings.Monitor).c_str()); {
		HMENU hM1_Debug = CreatePopupMenu();
		AppendMenuW(hM0_Monitor, MF_STRING | MF_POPUP, (UINT_PTR)hM1_Debug, U8ToW(this->MenuStrings.Debug).c_str()); {
			AppendMenuW(hM1_Debug, MF_STRING, this->WinCommand->OpenConsole, U8ToW(this->MenuStrings.OpenConsole).c_str());
			AppendMenuW(hM1_Debug, MF_STRING, this->WinCommand->CloseConsole, U8ToW(this->MenuStrings.CloseConsole).c_str());
			AppendMenuW(hM1_Debug, MF_STRING, this->WinCommand->ClearConsole, U8ToW(this->MenuStrings.ClearConsole).c_str());
		}
		AppendMenuW(hM0_Monitor, MF_STRING, this->WinCommand->OpenSharedMemoryMonitor, U8ToW(this->MenuStrings.OpenSharedMemoryMonitor).c_str());
	}

	HMENU hM0_Language = CreatePopupMenu();
	AppendMenuW(*hMenuMain, MF_STRING | MF_POPUP, (UINT_PTR)hM0_Language, U8ToW(this->MenuStrings.Language).c_str()); {
		HMENU hM1_ChangeLanguage = CreatePopupMenu();
		AppendMenuW(hM0_Language, MF_STRING | MF_POPUP, (UINT_PTR)hM1_ChangeLanguage, U8ToW(this->MenuStrings.SetLanguage).c_str()); {
			AppendMenuW(hM1_ChangeLanguage, MF_STRING, this->WinCommand->LSetSpCn, U8ToW(this->MenuStrings.LSetSpCn).c_str());
			AppendMenuW(hM1_ChangeLanguage, MF_STRING, this->WinCommand->LSetEn, U8ToW(this->MenuStrings.LSetEn).c_str());
		}
	}*/


}
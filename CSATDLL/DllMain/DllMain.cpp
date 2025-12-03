#include"../CSATiManager/CSATiManager.hpp"
#include"../CSATiManager/HookManager/HookManager.hpp"

CSATiManager CSATi;//唯一全局核心实例

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		CSATi.HookManager().NeedReHook = true;
		HANDLE hThread = CreateThread(NULL, 0, CSATi.HookManager().CreateHookEntry, NULL, 0, NULL);
		break;
	}
	case DLL_THREAD_ATTACH: {
		break;
	}
	case DLL_THREAD_DETACH: {
		break;
	}
	case DLL_PROCESS_DETACH: {
		break;
	}
	default: {
		break;
	}
	}
	return TRUE;
}
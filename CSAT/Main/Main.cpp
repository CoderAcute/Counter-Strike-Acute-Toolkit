#include"Main.h"

#include"../CSATeCore/CSATeMainWindowManager/CSATeMainWindowManager.hpp"
#include"../CSATeCore/IPCer/IPCer.hpp"

#include <ShObjIdl_core.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	CSATeCore* Core = new CSATeCore;
	Core->Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);


	while (Core->IsRunning) {
		if (PeekMessageW(&Core->WindowManager().Msg, NULL, 0, 0, PM_NOREMOVE)) {
			GetMessageW(&Core->WindowManager().Msg, NULL, 0, 0);
			if (Core->WindowManager().Msg.message == WM_QUIT) {
				Core->IsRunning = false;
			}
			TranslateMessage(&Core->WindowManager().Msg);
			DispatchMessageW(&Core->WindowManager().Msg);
			
		}
		Core->VirtualMain();
	}

	return 0;
}

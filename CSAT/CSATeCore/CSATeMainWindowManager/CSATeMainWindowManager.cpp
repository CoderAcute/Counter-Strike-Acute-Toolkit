#include"CSATeMainWindowManager.hpp"
#include"../CSATeCore.hpp"
#include"../IPCer/IPCer.hpp"

CSATeMainWindowManager* pThis = nullptr;

LRESULT CALLBACK GlobalWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (Msg == WM_NCCREATE) {
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast<CSATeMainWindowManager*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else {
		pThis = reinterpret_cast<CSATeMainWindowManager*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis) {
		return pThis->CSATWndProc(hWnd, Msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK CSATeMainWindowManager::CSATWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	switch (Msg) {
	case WM_COMMAND: {
		//自定义ID
		this->WMCOMMANDProc(wParam);
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE: {
		int result = MessageBoxW(hWnd,
			L"确定要退出程序吗？",
			L"提示",
			MB_YESNO | MB_ICONQUESTION);
		if (result == IDYES) {
			PostQuitMessage(0);
		}
		break;
	}
	default:
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}
}

void CSATeMainWindowManager::WMCOMMANDProc(WPARAM wParam) {
	UINT ID = LOWORD(wParam);
	if (ID == this->WindowCommand.OpenCS2) {
		this->Core->IPCer().OpenCS2();
	}
	if (ID == this->WindowCommand.Init) {
		this->Core->IPCer().TryCatchCS2();
	}
	if (ID == this->WindowCommand.Inject) {
		this->Core->IPCer().Inject();
	}
	if (ID == this->WindowCommand.Exit) {
		PostMessageW(this->hWnd, WM_CLOSE, 0, 0);
	}
}



void CSATeMainWindowManager::InitWnd(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	this->hInstance = hInstance;
	this->hPrevInstance = hPrevInstance;
	this->lpCmdLine = lpCmdLine;
	this->nCmdShow = nCmdShow;
}


void CSATeMainWindowManager::InitWndCls() {
	this->CSATWndCls.lpfnWndProc = GlobalWndProc;
	this->CSATWndCls.hInstance = this->hInstance;
	this->CSATWndCls.lpszClassName = L"Counter-Strike:Acute's Toolkit";
}

void CSATeMainWindowManager::RegisterWndCls() {
	if (!RegisterClass(&this->CSATWndCls)) {
		MessageBox(NULL, L"注册窗口失败！", L"错误！", MB_ICONERROR | MB_OK);
		return;
	}
}

void CSATeMainWindowManager::CreateWnd() {
	this->hWnd = CreateWindowExW(
		0L,
		this->CSATWndCls.lpszClassName,//窗口类名
		L"Counter-Strike:Acute's Toolkit",//窗口标题
		WS_OVERLAPPEDWINDOW,//窗口风格
		100, 100,//窗口位置
		500, 500,//窗口大小
		NULL, //父窗口句柄
		NULL, //菜单句柄
		this->hInstance, //应用程序实例句柄
		this//附加参数
	);
	if (!this->hWnd) {
		MessageBoxW(NULL, L"创建窗口失败！", L"错误！", MB_ICONERROR | MB_OK);
		return;
	}
}

void CSATeMainWindowManager::ShowWnd() {
	ShowWindow(this->hWnd, this->nCmdShow);
	UpdateWindow(this->hWnd);
}


void CSATeMainWindowManager::InitMenu() {
	this->MenuManager.Init(&this->hMenuMain, &this->WindowCommand);
}
void CSATeMainWindowManager::SetMenu() {
	::SetMenu(this->hWnd, this->hMenuMain);
}





void CSATeMainWindowManager::Init(CSATeCore* Core, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	this->Core = Core;
	this->InitWnd(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	this->InitWndCls();
	this->RegisterWndCls();
	this->CreateWnd();
	//this->ShowWnd();
	this->InitMenu();
	this->SetMenu();
}
#pragma once

#include"MenuManager/MenuManager.hpp"
#include"WindowCommandIDs.hpp"
#include"ConsoleManager/ConsoleManager.hpp"

class CSATeCore;

class CSATeMainWindowManager {
private:
	CSATeCore* Core;
public:
	WindowCommand WindowCommand{};
	MenuManager MenuManager{};
	ConsoleManager ConsoleManager{};

	HWND hWnd{};
	HMENU hMenuMain;

	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpCmdLine;
	int nCmdShow;

	WNDCLASS CSATWndCls = { 0 };

	MSG Msg = { 0 };

	void Init(CSATeCore* Core, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	void InitWnd(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	void InitWndCls();
	void RegisterWndCls();

	void CreateWnd();
	void ShowWnd();

	void InitMenu();
	void SetMenu();

	LRESULT CALLBACK CSATWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	void WMCOMMANDProc(WPARAM wParam);
};
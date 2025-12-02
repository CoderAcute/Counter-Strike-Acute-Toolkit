#pragma once

#include <Windows.h>

#include"MenuStrings.hpp"
#include"../WindowCommandIDs.hpp"


class MenuManager {
public:
	WindowCommand* WinCommand = nullptr;
	MenuStrings MenuStrings{};


	void Init(HMENU* hMenuMain, WindowCommand* WndCmd);
};
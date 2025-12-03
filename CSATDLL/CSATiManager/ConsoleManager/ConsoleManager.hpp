#pragma once

#include"../ModuleBase/ModuleBase.hpp"

class ConsoleManager final :public ModuleBase {
private:
	int TargetTick = 0;
public:
	ConsoleManager(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::ConsoleManager;
	}

	bool Init()override;
	void VirtualMain()override;

	void Menu()override;
	
	void ShowFilePath();
};
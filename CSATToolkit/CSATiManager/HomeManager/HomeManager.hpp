#pragma once

#include"../ModuleBase/ModuleBase.hpp"

class HomeManager final :public ModuleBase {
public:
	HomeManager(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::HomeManager;
	}

	bool Init()override;
	void VirtualMain()override;

	void Menu()override;
};
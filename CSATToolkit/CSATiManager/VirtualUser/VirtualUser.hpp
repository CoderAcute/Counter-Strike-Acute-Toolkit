#pragma once

#include"../ModuleBase/ModuleBase.hpp"

#include<atomic>

class ICameraSystem;
class IAbstractLayer3D;

class VirtualUser final :public ModuleBase {
private:
	ICameraSystem* CameraSystem = nullptr;
	IAbstractLayer3D* AL3D = nullptr;
public:
	VirtualUser(CSATiManager* CSATi) : ModuleBase(CSATi) {
		this->Type = ModuleType::VirtualUser;
	}

	std::atomic<bool>Running = false;
	std::atomic<bool>MyFlag = false;

	bool Init()override final;

	void VirtualMain()override final;
	void ProcessMsg()override final;

	void Menu()override final;
};
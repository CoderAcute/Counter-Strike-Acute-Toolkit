//ICameraSystem.hpp
//任何使用摄像机系统的组件应当包含该头文件

#pragma once

#include"../ModuleBase/ModuleBase.hpp"

#include"../../ThirdParty/imgui_d11/imgui.h"
#include<DirectXMath.h>
#include<string>

class ICameraSystem :public ModuleBase {
public:
	ICameraSystem(CSATiManager* CSATi) : ModuleBase(CSATi) {
		this->Type = ModuleType::CameraSystem;
	}

	virtual void test() = 0;

	virtual void ResetCameraModule(float CameraHigh, float CameraX, float CameraY, float AxisLenth, ImU32 Colour) = 0;
	virtual void DrawCameraByPAR(DirectX::XMFLOAT3& Position, DirectX::XMFLOAT3& Rotation, const char* label) = 0;

	virtual bool CallProject(const std::string& ProjectName) = 0;

	virtual bool CallSolution(const std::string& SolutionName) = 0;
	virtual bool CallSolution(const CSATMessage& Msg) = 0;

	virtual bool ShutDown() = 0;

	virtual bool Save() = 0;
};
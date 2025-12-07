#pragma once

#include"IAbstractLayer3D.hpp"

#include<atomic>

class CSController;

class AbstractLayer3D final : public IAbstractLayer3D {
private:
	CSController* CS = nullptr;

	std::atomic<float>CurrentTime = 0.0f;
public:
	AbstractLayer3D(CSATiManager* CSATi) :IAbstractLayer3D(CSATi) {
		this->Type = ModuleType::AbstractLayer3D;
	}
	bool Init()override;
	void VirtualMain()override;

	//接口实现
	bool SetTime(float time)override;
	bool PushTime(float Time)override;
	float GetTime()const override;
	CSATMath::SpatialState GetSpatialState()const override;
	float* GetViewMatrix()const override;
	float GetWinWidth()const override;
	float GetWinHeight()const override;
	bool SetLocalFrame(const CSATMath::Frame& frame)override;
	bool ExecuteCommand(const char* command)override;
	bool ExecuteCommand(const std::string& command)override;
	bool UpdatePlayerMsg(D_Player&& PlayMsg)override;
	bool SpecPlayer(int IndexInMap)override;
	D_Player& GetPlayerMsg(int Index)override;
};
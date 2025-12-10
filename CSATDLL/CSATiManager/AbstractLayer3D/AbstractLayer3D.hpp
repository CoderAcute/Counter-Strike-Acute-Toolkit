#pragma once

#include"IAbstractLayer3D.hpp"

#include<atomic>

class CSController;

class AbstractLayer3D final : public IAbstractLayer3D {
private:
	CSController* CS = nullptr;

	std::atomic<float>CurrentTime = 0.0f;

	//阶段时间基准
	float PhaseStartTime = 0.0f;
	//阶段时长
	float PhaseDuration = 0.0f;
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

	//获取阶段已用时间（正计时）
	float GetPhaseElapsedTime()const override;
	//获取阶段剩余时间（倒计时）
	float GetPhaseRemainingTime()const override;
	//获取阶段总时长
	float GetPhaseDuration()const override;
	//设置阶段开始时间基准
	void SetPhaseStartTime(float startTime)override;
	//设置阶段总时长
	void SetPhaseDuration(float duration)override;

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
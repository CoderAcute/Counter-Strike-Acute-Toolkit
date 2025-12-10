#pragma once

#include"../../CSATMath/CSATMath.hpp"	
#include"../ModuleBase/ModuleBase.hpp"

#include<string>
#include<shared_mutex>

//1到10为玩家，0为本地
class D_Player {
public:
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Rotation;
	int HP;
	int Team;
	bool Alive;
	int IndexInEntityList;
	int IndexInMap;
};

class D_GameData {
public:
	D_Player Players[11];

};

class IAbstractLayer3D :public ModuleBase {
protected:
	std::shared_mutex Mtx;
	D_GameData GameData{};
public:
	std::shared_mutex& GetMtx() {
		return this->Mtx;
	}

	IAbstractLayer3D(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::AbstractLayer3D;
	}
	virtual ~IAbstractLayer3D() = default;

	virtual bool SetTime(float time) = 0;
	virtual bool PushTime(float Time) = 0;
	virtual float GetTime()const = 0;

	//获取阶段已用时间（正计时）
	virtual float GetPhaseElapsedTime()const = 0;
	//获取阶段剩余时间（倒计时）
	virtual float GetPhaseRemainingTime()const = 0;
	//获取阶段总时长
	virtual float GetPhaseDuration()const = 0;
	//设置阶段开始时间基准
	virtual void SetPhaseStartTime(float startTime) = 0;
	//设置阶段总时长
	virtual void SetPhaseDuration(float duration) = 0;

	virtual CSATMath::SpatialState GetSpatialState()const = 0;

	virtual float* GetViewMatrix()const = 0;
	virtual float GetWinWidth()const = 0;
	virtual float GetWinHeight()const = 0;


	virtual bool SetLocalFrame(const CSATMath::Frame& frame) = 0;
	//执行命令
	virtual bool ExecuteCommand(const char* command) = 0;
	virtual bool ExecuteCommand(const std::string& command) = 0;


	//信息
	virtual bool UpdatePlayerMsg(D_Player&& PlayMsg) = 0;
	virtual D_Player& GetPlayerMsg(int Index) = 0;

	virtual bool SpecPlayer(int Index) = 0;
};
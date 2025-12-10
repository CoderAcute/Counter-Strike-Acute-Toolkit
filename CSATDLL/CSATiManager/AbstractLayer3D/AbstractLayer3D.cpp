#include"AbstractLayer3D.hpp"

#include"../CSATiManager.hpp"
#include"../CSController/CSController.hpp"
#include"../GlobalVars/GlobalVars.hpp"

bool AbstractLayer3D::Init() {
	this->CS = &this->CSATi->CS();
	return true;
}
void AbstractLayer3D::VirtualMain() {
	if (!this->CSATi->GlobalVars().CampathPlaying) {
		*this->CS->Local.pGlobalFOV = 0.0f;
	}

	return;
}

//接口实现

bool AbstractLayer3D::SetTime(float time) {
	this->CurrentTime.store(time);
	return true;
}
bool AbstractLayer3D::PushTime(float Time) {
	if (Time > this->CurrentTime) {
		this->CurrentTime = Time;
	}
	else if (this->CurrentTime - Time > 0.1) {
		this->CurrentTime = Time;
	}
	return true;
}
float AbstractLayer3D::GetTime()const {
	return this->CurrentTime.load();
}

//获取阶段已用时间（正计时）
float AbstractLayer3D::GetPhaseElapsedTime()const {
	return this->CurrentTime - this->PhaseStartTime;
}
//获取阶段剩余时间（倒计时）
float AbstractLayer3D::GetPhaseRemainingTime()const {
	return this->PhaseDuration - (this->CurrentTime - this->PhaseStartTime);
}
//获取阶段总时长
float AbstractLayer3D::GetPhaseDuration()const {
	return this->PhaseDuration;
}
//设置阶段开始时间基准
void AbstractLayer3D::SetPhaseStartTime(float startTime) {
	this->PhaseStartTime = startTime;
	return;
}
//设置阶段总时长
void AbstractLayer3D::SetPhaseDuration(float duration) {
	this->PhaseDuration = duration;
	return;
}






CSATMath::SpatialState AbstractLayer3D::GetSpatialState()const {
	return this->CS->CSGetLocalSpatialState();
}
float* AbstractLayer3D::GetViewMatrix()const {
	return this->CS->CSGetMatrix();
}
float AbstractLayer3D::GetWinWidth()const {
	return 1920.0f;
}
float AbstractLayer3D::GetWinHeight()const {
	return 1080.0f;
}
bool AbstractLayer3D::SetLocalFrame(const CSATMath::Frame& frame) {
	return this->CS->SetLocalFrame(frame);
}
bool AbstractLayer3D::ExecuteCommand(const char* command) {
	this->CS->Execute(command);
	return true;
}
bool AbstractLayer3D::ExecuteCommand(const std::string& command) {
	this->ExecuteCommand(command.c_str());
	return true;
}

bool AbstractLayer3D::UpdatePlayerMsg(D_Player&& PlayMsg) {
	std::unique_lock<std::shared_mutex> lock(this->GetMtx());
	this->GameData.Players[PlayMsg.IndexInMap] = std::move(PlayMsg);
	return true;
}
bool AbstractLayer3D::SpecPlayer(int IndexInMap) {
	this->ExecuteCommand("spec_mode 2;spec_player " + std::to_string(this->GameData.Players[IndexInMap].IndexInMap));
	return true;
}
D_Player& AbstractLayer3D::GetPlayerMsg(int Index) {
	//std::shared_lock lock(this->GetMtx());
	return this->GameData.Players[Index];
}
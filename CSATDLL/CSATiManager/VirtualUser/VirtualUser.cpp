#include"VirtualUser.hpp"

#include"../CSATiManager.hpp"

#include"../CameraSystem/ICameraSystem.hpp"
#include"../Debugger/IDebugger.hpp"

#include"../../ThirdParty/All_ImGui.hpp"

#include"../MessageManager/IMessageManager.hpp"

#include"../KeyTracker/KeyTracker.hpp"
#include"../AbstractLayer3D/IAbstractLayer3D.hpp"
bool VirtualUser::Init() {
	this->CameraSystem = &this->CSATi->ICameraSystem();
	this->Running = true;
	this->AL3D = &this->CSATi->IAbstractLayer3D();

	this->CSATi->IMessageManager().Registe(this->CurrentMsg, RegistePack{ &this->MyFlag, "VirtualUser" });
	this->CSATi->IMessageManager().Subscribe(MsgType::Game_NewRound, &this->MyFlag);
	this->CSATi->IMessageManager().Subscribe(MsgType::CameraSystem_PlayingShutdown, &this->MyFlag);
	//this->CSATi->IMessageManager().Subscribe(MsgType::Core_Tick1, &this->MyFlag);
	this->CSATi->IMessageManager().Subscribe(MsgType::Core_Tick60, &this->MyFlag);
	this->CSATi->IMessageManager().Subscribe(MsgType::Command_SpecPlayer, &this->MyFlag);

	return true;
}
void VirtualUser::Menu() {
	if (ImGui::Button("接口访问测试")) {
		this->CameraSystem->test();
	}
	bool temp = this->Running.load();
	if (ImGui::Checkbox("启动自动化控制", &temp)) {
		this->Running.store(temp);
	}
	static std::string TargetSolutionName{};
	ImGui::Text("想调用的解决方案：");
	ImGui::SameLine();
	ImGui::InputText("##TargetWorkspaceName", &TargetSolutionName);
	if (ImGui::Button("调用")) {
		this->CameraSystem->CallSolution(TargetSolutionName);
	}
	
	if (ImGui::Button("停止播放")) {
		this->CameraSystem->ShutDown();
	}


	return;
}
void VirtualUser::VirtualMain() {
	if (this->MyFlag.load()) {
		this->ProcessMsg();
		this->MyFlag.store(false);
		this->CSATi->IMessageManager().Release();
	}
	if (this->CSATi->KT().CheckWithPack(KeyCheckPack{ true,false,false,true,'P',1 })) {
		this->CameraSystem->ShutDown();
	}
	if (this->CSATi->KT().CheckWithPack(KeyCheckPack{ true,false,false,true,'T',1 })) {
		bool AutoRunning = this->Running.load();
		if (AutoRunning) {
			this->Running.store(false);
			this->CSATi->IDebugger().AddWarning("自动化功能已关闭");
		}
		else {
			this->Running.store(true);
			this->CSATi->IDebugger().AddWarning("自动化功能已开启");
		}
	}
	return;
}

void VirtualUser::ProcessMsg() {
	if (!this->Running)return;
	switch ((*this->CurrentMsg)->Type) {
	case MsgType::Game_NewRound: {
		this->CSATi->IDebugger().AddInfo("接收到新回合信息");
		this->CameraSystem->CallSolution(**this->CurrentMsg);
		break;
	}
	case MsgType::CameraSystem_PlayingShutdown: {
		//处理播放停止消息
		this->CSATi->IDebugger().AddInfo("接收到摄像机系统播放停止信息");
		this->CameraSystem->ShutDown();
		break;
	}
	case MsgType::Core_Tick1: {
		//this->CSATi->IDebugger().AddInfo("一秒");
		break;
	}
	case MsgType::Core_Tick60: {
		this->CameraSystem->Save();
		this->CSATi->IDebugger().AddInfo("已触发自动保存（频率：每分钟）");
		break;
	case MsgType::Command_SpecPlayer: {
		this->CameraSystem->ShutDown();
		this->AL3D->SpecPlayer((*this->CurrentMsg)->Param1);
	}
	}


	}

	return;
}
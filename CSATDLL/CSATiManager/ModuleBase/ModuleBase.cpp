#include"ModuleBase.hpp"

#include"../CSATiManager.hpp"

ModuleBase::ModuleBase(CSATiManager* CSATi) {
	this->CSATi = CSATi;
}
ModuleType ModuleBase::GetType()const {
	return this->Type;
}
std::string ModuleBase::GetName()const {
    switch (this->Type) {
    case ModuleType::ModuleBase:
        return "ModuleBase";
    case ModuleType::AbstractLayer3D:
        return "AbstractLayer3D";
    case ModuleType::CameraSystem:
        return "CameraSystem";
    case ModuleType::ConsoleManager:
        return "ConsoleManager";
    case ModuleType::CSController:
        return "CSController";
    case ModuleType::Debugger:
        return "Debugger";
    case ModuleType::GameSettingsManager:
        return "GameSettingsManager";
    case ModuleType::GlobalVars:
        return "GlobalVars";
    case ModuleType::HomeManager:
        return "HomeManager";
    case ModuleType::HookManager:
        return "HookManager";
    case ModuleType::IPCer:
        return "IPCer";
    case ModuleType::KeyTracker:
        return "KeyTracker";
    case ModuleType::MessageManager:
        return "MessageManager";
    case ModuleType::MiniMap:
        return "MiniMap";
    case ModuleType::VirtualUser:
        return "VirtualUser";
    default:
        return "UnknownModule";
    }
}

void ModuleBase::VirtualMain() {
	return;
}
void ModuleBase::ProcessMsg() {
	return;
}

void ModuleBase::Menu() {
	return;
}
void ModuleBase::Windows() {
	return;
}


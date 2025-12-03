#pragma once

#include"../ModuleBase/ModuleBase.hpp"

class IDebugger :public ModuleBase {
public:
	IDebugger(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::Debugger;
	}

	virtual void AddInfo(const std::string& NewMsg) = 0;
	virtual void AddSucc(const std::string& NewMsg) = 0;
	virtual void AddWarning(const std::string& NewMsg) = 0;
	virtual void AddError(const std::string& NewMsg) = 0;

	virtual void ShowWindow() = 0;
	virtual void HideWindow() = 0;

	bool ShowWhenError = true;
	bool AutoScroll = true;
};
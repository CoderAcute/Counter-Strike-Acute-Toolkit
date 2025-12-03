#pragma once

#include"../ModuleBase/ModuleBase.hpp"

#include<atomic>

//GlobalVarsOnlyRead类，只读变量存储类
//用于存储系统信息等，可以安全跨线程读取的变量
//不允许写操作
class GlobalVarsOnlyRead {
public:
	static constexpr const char CSATiVersion[] = "V1.0.3";
	static constexpr const char TimeStamp[] = "Built: " __DATE__ " " __TIME__;
	static constexpr const char FullName[] = "Counter-Strike: Acute Toolkit";
	static constexpr const char DeveloperName[] = "Acute";
	static constexpr bool IsDebugVersion = true;
};

class IMessageManager;
enum class MsgType :uint32_t;
//GlobalVars类，存储全局变量
//这些全局变量为线程安全的，可以在多线程环境下使用
//只存储系统关键变量，避免滥用全局变量
//其它模块间变量互相访问，需要自行保证线程安全
class GlobalVars final :public ModuleBase {
private:
	IMessageManager* MsgManager = nullptr;

	void PublishTick(MsgType type);
	void PublishTickAll();
	void Tick();
public:
	GlobalVars(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::GlobalVars;
	}

	std::atomic<bool> DebugMode = false;
	std::atomic<bool> InGamePlaying = false;
	std::atomic<uint32_t> CoreTick = 0;

	bool Init()override;
	void VirtualMain()override;
};
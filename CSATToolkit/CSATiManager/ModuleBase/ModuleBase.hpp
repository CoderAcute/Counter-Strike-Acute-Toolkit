#pragma once

#include<string>

//核心管理器
class CSATiManager;
//CSAT消息
class CSATMessage;

//模块类型枚举
enum class ModuleType :unsigned int {
	ModuleBase,
	AbstractLayer3D,
	CameraSystem,
	ConsoleManager,
	CSController,
	Debugger,
	GameSettingsManager,
	GlobalVars,
	HomeManager,
	HookManager,
	IPCer,
	KeyTracker,
	MessageManager,
	MiniMap,
	VirtualUser
};

//模块基类
class ModuleBase {
protected:
	CSATiManager* CSATi;
	ModuleType Type = ModuleType::ModuleBase;
	CSATMessage** CurrentMsg = nullptr;
public:
	//删除不需要的构造函数
	ModuleBase() = delete;
	ModuleBase(const ModuleBase&) = delete;
	ModuleBase(ModuleBase&&) = delete;
	ModuleBase& operator=(const ModuleBase&) = delete;
	ModuleBase& operator=(ModuleBase&&) = delete;
	//提供输入核心管理器指针的构造函数，进行绑定
	ModuleBase(CSATiManager* CSATi);
	//虚析构函数确保正确调用析构函数
	virtual ~ModuleBase() = default;

	//通用函数：

	//获取模块类型
	ModuleType GetType()const;
	std::string GetName()const;

	//接口：

	//初始化，拉取各种依赖
	virtual bool Init() = 0;

	//虚拟主循环，执行组件逻辑
	virtual void VirtualMain();
	//消息处理函数
	virtual void ProcessMsg();

	//菜单绘制
	virtual void Menu();
	//窗口绘制
	virtual void Windows();
};
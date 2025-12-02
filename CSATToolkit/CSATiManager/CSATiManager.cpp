#include"../ThirdParty/All_ImGui.hpp"

#include"CSATiManager.hpp"

#include"HomeManager/HomeManager.hpp"
#include"ConsoleManager/ConsoleManager.hpp"
#include"GameSettingsManager/GameSettingsManager.hpp"
#include"EffectorManager/EffectorManager.hpp"
#include"Debugger/Debugger.hpp"
#include"CameraSystem/CameraSystem.hpp"

#include"CSController/CSController.hpp"
#include"AbstractLayer3D/AbstractLayer3D.hpp"
#include"IPCer/IPCer.hpp"
#include"KeyTracker/KeyTracker.hpp"

#include"VirtualUser/VirtualUser.hpp"

#include"MessageManager/MessageManager.hpp"


#include"HookManager/HookManager.hpp"

#include"GlobalVars/GlobalVars.hpp"
#include"MiniMap/MiniMap.hpp"
enum class Page :int {
	Home = 1,
	Control = 2,
	GameSettings = 3,
	Features = 4,
	Debug = 5,
	CameraSystem = 6
};

//实现类，掌握所有的模块实例
class CSATiManagerImpl {	
public:
	CSATiManager* CSATi;
	
	Page Page = Page::Home;

	//地基模块
	KeyTracker KT;
	//基础服务模块
	CSController CS{ this->CSATi };
	AbstractLayer3D AL3D{ this->CSATi };
	IPCer IPCer{ this->CSATi };
	C_HookManager HookManager;
	MessageManager MessageManager{ this->CSATi };
	GlobalVars GlobalVars{ this->CSATi };
	//上层建筑
	HomeManager HomeManager{ this->CSATi };
	ConsoleManager ConsoleMaganger{ this->CSATi };
	GameSettingsManager GameSettingsManager{ this->CSATi };
	Debugger Debugger{ this->CSATi };
	CameraSystem CameraSystem{ this->CSATi };
	VirtualUser VirtualUser{ this->CSATi };
	MiniMap MiniMap{ this->CSATi };
};

//构造函数和析构函数需要管理pImpl的生命周期
CSATiManager::CSATiManager() {
	this->pImpl = std::make_unique<CSATiManagerImpl>(this);
	this->pImpl->HookManager.CSATi = this;
}

CSATiManager::~CSATiManager() {
	return;
}

//获取子模块的接口实现
CSController& CSATiManager::CS() { return this->pImpl->CS; }
IAbstractLayer3D& CSATiManager::IAbstractLayer3D() { return this->pImpl->AL3D; }
IPCer& CSATiManager::IPCer() { return this->pImpl->IPCer; }
KeyTracker& CSATiManager::KT() { return this->pImpl->KT; }
HomeManager& CSATiManager::HomeManager() { return this->pImpl->HomeManager; }
ConsoleManager& CSATiManager::ConsoleManager() { return this->pImpl->ConsoleMaganger; }
GameSettingsManager& CSATiManager::GameSettingsManager() { return this->pImpl->GameSettingsManager; }
IDebugger& CSATiManager::IDebugger() { return this->pImpl->Debugger; }
ICameraSystem& CSATiManager::ICameraSystem() { return this->pImpl->CameraSystem; }
VirtualUser& CSATiManager::VirtualUser() { return this->pImpl->VirtualUser; }
IMessageManager& CSATiManager::IMessageManager() { return this->pImpl->MessageManager; }
C_HookManager& CSATiManager::HookManager() { return this->pImpl->HookManager; }
GlobalVars& CSATiManager::GlobalVars() { return this->pImpl->GlobalVars; }
MiniMap& CSATiManager::MiniMap() { return this->pImpl->MiniMap; }

//专用初始化函数
void CSATiManager::Init() {
	//Hook初始化
	//this->pImpl->HookManager.Init(this);
	//逆向层初始化
	this->pImpl->CS.InitInterface();
	this->pImpl->CS.Init();
	this->pImpl->AL3D.Init();
	this->pImpl->CS.TryCreateUpdateThread();//包含线程创建
	//基础模块初始化
	this->pImpl->MessageManager.Init();
	this->pImpl->MessageManager.TryCreateUpdateThread();//包含线程创建
	
	this->pImpl->IPCer.Init();
	this->pImpl->KT.TryCreateUpdateThread();//包含线程创建
	this->pImpl->Debugger.Init();
	this->pImpl->Debugger.AddSucc("注入成功！");

	this->pImpl->GlobalVars.Init();
	//功能模块初始化
	this->pImpl->HomeManager.Init();
	this->pImpl->ConsoleMaganger.Init();
	this->pImpl->GameSettingsManager.Init();
	this->pImpl->CameraSystem.Init();
	//虚拟用户层初始化（用于自动化任务）
	this->pImpl->VirtualUser.Init();
	//小地图模块初始化
	this->pImpl->MiniMap.Init();

	//初始化额外任务（对系统无影响）
	this->pImpl->Debugger.AddSucc("各模块初始化完成！");
	this->pImpl->Debugger.AddWarning("您正在使用测试版本！！");
	//this->pImpl->CS.Execute("playdemo zhiqiu");
	//打开主窗口
	this->OpenMainWindow = true;
	std::thread([]() {
		MessageBoxW(NULL, L"Acute's DLL 注入成功！", L"CSAT", MB_OK | MB_ICONINFORMATION);
		}).detach(); // 分离线程，使其独立运行
	return;
}

//主窗口
void CSATiManager::CSATiMainWindow() {
	if (!this->OpenMainWindow)return;
	ImGui::Begin(GlobalVarsOnlyRead::FullName, &this->OpenMainWindow);//窗口标题


	if (ImGui::BeginTabBar("MainTabBar")) {
		if (ImGui::BeginTabItem("首页")) {
			this->pImpl->Page = Page::Home;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("总控台")) {
			this->pImpl->Page = Page::Control;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("游戏设置")) {
			this->pImpl->Page = Page::GameSettings;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("功能")) {
			this->pImpl->Page = Page::Features;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("调试")) {
			this->pImpl->Page = Page::Debug;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("摄像机系统")) {
			this->pImpl->Page = Page::CameraSystem;
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	static std::string StrText{};

	
	switch (this->pImpl->Page) {
	case Page::Home:
		ImGui::BeginChild("Menus", ImVec2(0, 0), true);
		this->pImpl->HomeManager.Menu();
		break;
	case Page::Control:
		ImGui::BeginChild("Menus", ImVec2(0, 0), true);
		this->pImpl->ConsoleMaganger.Menu();
		break;
	case Page::GameSettings:
		ImGui::BeginChild("Menus", ImVec2(0, 0), true);
		this->pImpl->GameSettingsManager.Menu();
		break;
	case Page::Features:
		ImGui::BeginChild("Menus", ImVec2(0, 0), true);
		this->pImpl->VirtualUser.Menu();
		ImGui::Text("命令执行：");
		ImGui::SameLine();
		ImGui::InputText("##String", &StrText);
		if (ImGui::Button("执行")) {
			if (!StrText.empty()) {
				this->pImpl->CS.Execute(StrText.c_str());
				StrText.clear();
			}
		}
		break;
	case Page::Debug:
		ImGui::BeginChild("Menus", ImVec2(0, 0), true);
		this->pImpl->Debugger.Menu();
		break;
	case Page::CameraSystem:
		ImGui::BeginChild("Menus");
		this->pImpl->CameraSystem.Menu();
		break;
	}
	
	ImGui::EndChild();
	
	ImGui::End();
}

//主逻辑，在DllMain中被调用
void CSATiManager::VirtualMain() {
	if (this->pImpl->KT.CheckComboClick(VK_INSERT, 2)) {
		this->OpenMainWindow = !this->OpenMainWindow;
	}

	//先让VirtualUser处理消息，避免状态撕裂
	this->pImpl->VirtualUser.VirtualMain();

	if (this->OpenMainWindow) {
		this->CSATiMainWindow();
		this->pImpl->Debugger.Windows();
		this->pImpl->CameraSystem.Windows();
	}
	this->pImpl->MiniMap.Windows();

	this->pImpl->HomeManager.VirtualMain();
	this->pImpl->ConsoleMaganger.VirtualMain();
	this->pImpl->GameSettingsManager.VirtualMain();
	this->pImpl->CameraSystem.VirtualMain();
	
	this->pImpl->AL3D.VirtualMain();
	this->pImpl->MiniMap.VirtualMain();
	this->pImpl->GlobalVars.VirtualMain();

	return;
}
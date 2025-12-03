#pragma once

//#include"HookManager/HookManager.hpp"

#include<memory>

//前向声明实现类
class CSATiManagerImpl;

//前向声明子模块类
class CSController;
class IPCer;
class KeyTracker;
class HomeManager;
class ConsoleManager;
class GameSettingsManager;
class IDebugger;
class ICameraSystem;
class VirtualUser;
class IMessageManager;
class C_HookManager;
class GlobalVars;
class IAbstractLayer3D;
class MiniMap;

class CSATiManager {
private:
    //数据存储：

    //Impl的指针
    std::unique_ptr<CSATiManagerImpl> pImpl;
public:
    
    //构造函数
    CSATiManager();
    //析构函数
    ~CSATiManager();

    //删除拷贝构造
    CSATiManager(const CSATiManager&) = delete;
    //删除拷贝赋值
    CSATiManager& operator=(const CSATiManager&) = delete;

    //初始化
    void Init();
    //主循环
    void VirtualMain();
    //窗口逻辑
    bool OpenMainWindow = false;
    void CSATiMainWindow();
    
    //获取子模块的接口
    CSController& CS();
    IPCer& IPCer();
    KeyTracker& KT();
    HomeManager& HomeManager();
    ConsoleManager& ConsoleManager();
    GameSettingsManager& GameSettingsManager();
    IDebugger& IDebugger();
    ICameraSystem& ICameraSystem();
    VirtualUser& VirtualUser();
    IMessageManager& IMessageManager();

    C_HookManager& HookManager();
	GlobalVars& GlobalVars();
    IAbstractLayer3D& IAbstractLayer3D();
    MiniMap& MiniMap();
};
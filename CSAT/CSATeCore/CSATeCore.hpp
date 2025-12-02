#pragma once

#include<Windows.h>
#include<filesystem>

//前向声明实现类
class CSATeCoreImpl;

class IPCer;
class CSATeMainWindowManager;
class ConsoleManager;
class ConfigManager;

class CSATeCore {
private:
	CSATeCoreImpl* pImpl;
public:
	//是否运行中
	bool IsRunning;
	//构造和析构函数
	CSATeCore();
	~CSATeCore();
	//初始化
	bool Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	
	void VirtualMain();
	//模块接口
	IPCer& IPCer();
	CSATeMainWindowManager& WindowManager();
	ConsoleManager& Console();
	ConfigManager& ConfigManager();
};
#include"CSATeCore.hpp"

#include"IPCer/IPCer.hpp"
#include"CSATeMainWindowManager/CSATeMainWindowManager.hpp"
#include"ConfigManager/ConfigManager.hpp"

//Impl
class CSATeCoreImpl {
	friend CSATeCore;
	IPCer IPCer;
	CSATeMainWindowManager WindowManager;
	ConfigManager ConfigManager;
};

//各模块接口
CSATeCore::CSATeCore() {
	this->pImpl = new CSATeCoreImpl();
	this->IsRunning = false;
}
CSATeCore::~CSATeCore() {
	delete this->pImpl;
}
IPCer& CSATeCore::IPCer() {
	return this->pImpl->IPCer;
}
CSATeMainWindowManager& CSATeCore::WindowManager() {
	return this->pImpl->WindowManager;
}
ConsoleManager& CSATeCore::Console() {
	return this->pImpl->WindowManager.ConsoleManager;
}
ConfigManager& CSATeCore::ConfigManager() {
	return this->pImpl->ConfigManager;
}

//系统初始化
bool CSATeCore::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//窗口及控制台初始化
	this->WindowManager().Init(this, hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	this->Console().Init();
	this->Console().PrintInfor("CSAT窗口初始化完成！\n");
	this->Console().PrintInfor("CSAT控制台初始化完成！\n");

	//IPCer初始化
	this->IPCer().Init(this);
	this->Console().PrintInfor("CSAT.exe路径:  " + this->IPCer().PathGet_CSAT_exe().string() + "\n");
	this->Console().PrintInfor("CSATToolkit.dll路径:  " + this->IPCer().PathGet_CSATToolKit_dll().string() + "\n");
	this->Console().PrintInfor("Configs文件夹路径：" + this->IPCer().PathGet_Configs().string() + "\n");

	//ConfigManager初始化
	this->ConfigManager().Init(this);
	this->Console().PrintInfor("配置控制器初始化成功！\n");

	//加载配置文件
	this->ConfigManager().Config_Load(this->IPCer().PathGet_Configs());
	this->Console().PrintInfor("读取到的路径：" + this->ConfigManager().Config_GetCS2Path().string() + "\n");
	this->IPCer().SetCS2Path(this->ConfigManager().Config_GetCS2Path());


	//输出总成功信息
	this->Console().PrintInfor("CSAT外部核心准备就绪！\n");

	this->IsRunning = true;

	this->WindowManager().ShowWnd();

	return true;
}

void CSATeCore::VirtualMain() {
	this->IPCer().VirtualMain();
}
#pragma once

#include<string>

class MenuStrings {
public:
	std::string Ctrl = "控制";//控制
	std::string Init = "初始化";//控制-初始化
	std::string OpenCS2 = "打开CS2";//控制-打开CS2
	std::string Inject = "注入";//控制-注入
	std::string Exit = "退出";//控制-退出

	std::string Monitor = "监视";//监视
	std::string Debug = "调试";//监视-调试
	std::string OpenConsole = "打开控制台";//监视-调试-打开控制台
	std::string CloseConsole = "关闭控制台";//监视-调试-关闭控制台
	std::string ClearConsole = "清空控制台";//监视-调试-清空控制台
	std::string OpenSharedMemoryMonitor = "共享内存监控";//监视-共享内存监控

	std::string Language = "语言";//语言
	std::string SetLanguage = "设定为";//语言-设定为
	std::string LSetSpCn = "简体中文";//语言-设定为-简体中文
	std::string LSetEn = "English";//语言-设定为-English
};
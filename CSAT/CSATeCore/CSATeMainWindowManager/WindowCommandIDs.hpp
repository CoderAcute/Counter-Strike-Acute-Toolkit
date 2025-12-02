#pragma once

#include<Windows.h>

class WindowCommand {
public:
	// 控制
	UINT Init=1001;//控制-初始化
	UINT OpenCS2=1002;//控制-打开CS2
	UINT Inject=1003;//控制-注入
	UINT Exit=1004;//控制-退出

	//监控
	UINT OpenConsole=1005;//调试-打开控制台
	UINT CloseConsole=1006;//调试-关闭控制台
	UINT ClearConsole=1007;//调试-清空控制台
	UINT OpenSharedMemoryMonitor=1008;//监视-共享内存监控

	//语言
	UINT LSetSpCn=1009;//语言-设定为-简体中文
	UINT LSetEn=1010;//语言-设定为-English
};
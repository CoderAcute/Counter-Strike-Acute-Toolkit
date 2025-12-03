#pragma once

#include"../../Elements.hpp"

class ElementManager;
class CSATiManager;
class IAbstractLayer3D;
class CameraDrawer;

class ElementDebugger {
	friend ElementManager;
private:
	//CSATi指针，用于其它服务
	CSATiManager* CSATi = nullptr;

	IAbstractLayer3D* AL3D = nullptr;
	CameraDrawer* CamDrawer = nullptr;

	ElementManager* EManager = nullptr;

	//初始化
	void Init(CSATiManager* CSATi, CameraDrawer* CamDrawer, ElementManager* EManager);

	//调试菜单入口点
	void DebugMenus(ElementBase* const pElement);



	//自由摄像机轨道调试菜单
	void DebugMenu_FreeCameraPath(FreeCameraPath* const FreeCamPath);
	//第一人称视角轨道调试菜单
	void DebugMenu_FirstPersonCameraPath(FirstPersonCameraPath* const FirstPersonCameraPath);
};
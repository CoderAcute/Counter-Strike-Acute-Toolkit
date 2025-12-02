#pragma once

#include"../Workspace/Workspace.hpp"

#include<memory>
#include<filesystem>

class CSATiManager;

class ElementManager;
class SolutionManager;
class ProjectManager;

class WorkspaceManager {
private:
	CSATiManager* CSATi{};

	ElementManager* EManager = nullptr;
	SolutionManager* SManager = nullptr;
	ProjectManager* PManager = nullptr;
public:
	bool InWorkspace = false;
	//数据存储
	std::unique_ptr<Workspace> CurrentWorkspace = nullptr;

	//工作区管理器基本函数
	
	//初始化
	void Init(CSATiManager* CSATi, ElementManager* EManager, SolutionManager* SManager, ProjectManager* PManager);
	//逻辑主函数
	void VirtualMain();


	//工作区

	//创建工作区，创建文件路径
	bool Workspace_Create(const std::string& Name);
	//保存工作区所有内容到磁盘中
	bool Workspace_Save();
	//加载工作区并重置文件路径（会验证目标的存在性，如果目标文件夹不存在会返回false，此时应当不影响当前工作区）
	bool Workspace_TrySetPath(const std::string& Name);
	//真正切换到目标工作区并清空当前数据（在开始会先调用Workspace_TrySetPath进行检测，通过后才会从当前工作区退出）
	bool Workspace_Set(const std::string& Name);
	
	



	//工作区配置相关

	//生成配置到内存中
	bool Workspace_ConfigGenerate();
	//将内存中的配置保存到磁盘中
	bool Workspace_ConfigSave();
	//加载配置到内存中
	bool Workspace_ConfigLoad(const std::filesystem::path& WorkspacePath);
	//应用内存中的配置产生实际效果
	bool Workspace_ConfigApply();
};
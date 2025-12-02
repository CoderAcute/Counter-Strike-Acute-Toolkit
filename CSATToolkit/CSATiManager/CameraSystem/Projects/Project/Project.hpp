#pragma once

#include"../../../KeyTracker/KeyTracker.hpp"

#include<filesystem>

class Project {
public:
	//项目名称
	std::string Name{};
	//项目描述
	std::string Description{};
	//快捷键
	KeyCheckPack KCPack{};

	bool FullLoad = true;
	//解决方案名称储存，为切换做准备

	std::vector<std::string> OnNewRound{};
	std::vector<std::string> OnRoundStart{};
	std::vector<std::string> OnRoundEnd{};
	
	//构造函数
	explicit Project(const std::string& name) :
		Name(name) {}
	//析构函数
	~Project() = default;

	//修改名称
	void ResetName(const std::string& NewName);
	//刷新项目信息
	void Refresh();

	//保存项目到XML文件（加载在ProjectManager）
	bool SaveToXML(const std::filesystem::path& FolderPath, std::string& strRuselt);

	//获取项目描述
	std::string GetDescription()const {
		return this->Description;
	}
	//获取项目信息
	std::string GetMsg()const;
};
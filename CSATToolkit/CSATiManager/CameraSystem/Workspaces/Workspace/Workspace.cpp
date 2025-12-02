#include"Workspace.hpp"

#include"../../../../ThirdParty/All_pugixml.hpp"
#include<filesystem>

//辅助函数：添加布尔配置节点
void AddBoolConfigNode(pugi::xml_node& Parent, const char* NodeName, bool Value) {
	pugi::xml_node Child = Parent.append_child(NodeName);
	Child.append_attribute("Enable").set_value(Value ? 1 : 0);
}

bool Workspace::SaveConfigToXML(const std::filesystem::path& FolderPath, std::string& strRuselt) {
	//检查文件路径和名称存在性
	if (FolderPath.empty()) {
		strRuselt = "文件夹路径为空，无法保存工作区配置文件！";
		return false;
	}
	//拼接完整路径
	std::filesystem::path FullPath = FolderPath / ("WorkspaceConfig.xml");
	//创建临时XML文件
	pugi::xml_document NewXML;
	//初始化
	pugi::xml_node declarationNode = NewXML.prepend_child(pugi::node_declaration);
	declarationNode.append_attribute("version") = "1.0";
	declarationNode.append_attribute("encoding") = "utf-8";
	//开始保存信息
	//保存WorkspaceConfig节点
	pugi::xml_node node_WorkspaceConfig = NewXML.append_child("WorkspaceConfig");

	//保存ElementConfig信息
	pugi::xml_node node_ElementConfig = node_WorkspaceConfig.append_child("ElementConfig");
	AddBoolConfigNode(node_ElementConfig, "PreviewDraw", this->Config.ElementCfg.PreviewDraw);
	AddBoolConfigNode(node_ElementConfig, "PreviewOverride", this->Config.ElementCfg.PreviewOverride);

	//保存SolutionConfig信息
	pugi::xml_node node_SolutionConfig = node_WorkspaceConfig.append_child("SolutionConfig");
	AddBoolConfigNode(node_SolutionConfig, "SolutionShortcutEnable", this->Config.SolutionCfg.SolutionShortcutEnable);
	AddBoolConfigNode(node_SolutionConfig, "PlayingDraw", this->Config.SolutionCfg.PlayingDraw);
	AddBoolConfigNode(node_SolutionConfig, "PlayingOverride", this->Config.SolutionCfg.PlayingOverride);
	
	//保存ProjectConfig信息
	pugi::xml_node node_ProjectConfig = node_WorkspaceConfig.append_child("ProjectConfig");
	AddBoolConfigNode(node_ProjectConfig, "ProjectShortcutEnable", this->Config.ProjectCfg.ProjectShortcutEnable);
	
	//保存XML文件
	if (NewXML.save_file(FullPath.c_str())) {
		strRuselt = "成功保存工作区配置文件到XML文件！ 文件路径：" + FullPath.string();
		return true;
	}
	else {
		strRuselt = "无法保存工作区配置文件到XML文件！ 文件路径：" + FullPath.string();
		return false;
	}
}
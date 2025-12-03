#include"Project.hpp"

#include"../../../../ThirdParty/All_pugixml.hpp"

#include<strstream>

void Project::ResetName(const std::string& NewName) {
    this->Name = NewName;
    return;
}

void Project::Refresh() {
    //刷新元素和解决方案名称个数
	//this->Size_Elements = this->ElementNames.size();
	//this->Size_Solutions = this->SolutionNames.size();
    return;
}

bool Project::SaveToXML(const std::filesystem::path& FolderPath, std::string& strRuselt) {
    //检查文件路径和名称存在性
    if (FolderPath.empty()) {
        strRuselt = "文件夹路径为空，无法保存解决方案！";
        return false;
    }

    //拼接完整路径
    std::filesystem::path FullPath = FolderPath / (this->Name + ".xml");

    //创建临时XML文件
    pugi::xml_document NewXML;
    //初始化
    pugi::xml_node declarationNode = NewXML.prepend_child(pugi::node_declaration);
    declarationNode.append_attribute("version") = "1.0";
    declarationNode.append_attribute("encoding") = "utf-8";

    //开始保存信息

    //保存Project节点
    pugi::xml_node node_Project = NewXML.append_child("Project");

    //保存Project名称
    node_Project.append_attribute("Name").set_value(this->Name);

    //保存KeyCheckPack信息
    pugi::xml_node node_KeyCheckPack = node_Project.append_child("KeyCheckPack");
    this->KCPack.WriteXMLNode(node_KeyCheckPack);

    //保存Config节点
    pugi::xml_node node_Config = node_Project.append_child("Config");

    //保存AutoPlay节点
    pugi::xml_node node_AutoPlay = node_Config.append_child("AutoPlay");

    //保存OnNewRound节点
    pugi::xml_node node_OnNewRound = node_AutoPlay.append_child("OnNewRound");
    if (!this->OnNewRound.empty()) {
        for (size_t i = 0; i < this->OnNewRound.size(); ++i) {
            pugi::xml_node node_Solution = node_OnNewRound.append_child("Solution");
            node_Solution.append_attribute("Name").set_value(this->OnNewRound[i]);
        }
    }
    //保存OnRoundStart节点
    pugi::xml_node node_OnRoundStart = node_AutoPlay.append_child("OnRoundStart");
    if (!this->OnRoundStart.empty()) {
        for (size_t i = 0; i < this->OnRoundStart.size(); ++i) {
            pugi::xml_node node_Solution = node_OnRoundStart.append_child("Solution");
            node_Solution.append_attribute("Name").set_value(this->OnRoundStart[i]);
        }
    }
    //保存OnRoundEnd节点
    pugi::xml_node node_OnRoundEnd = node_AutoPlay.append_child("OnRoundEnd");
	if (!this->OnRoundEnd.empty()) {
        for (size_t i = 0; i < this->OnRoundEnd.size(); ++i) {
            pugi::xml_node node_Solution = node_OnRoundEnd.append_child("Solution");
            node_Solution.append_attribute("Name").set_value(this->OnRoundEnd[i]);
        }
    }

    //保存XML文件到磁盘
    if (!NewXML.save_file(FullPath.c_str())) {
        strRuselt = "保存文件失败！路径：" + FullPath.string();
        return false;
    }

    strRuselt = "保存成功  项目名：" + this->Name;

    return true;
}

std::string Project::GetMsg()const {
    std::ostringstream oss;
    oss << "项目名称：" << this->Name << "   项目描述：" << this->GetDescription();

    return oss.str();
}
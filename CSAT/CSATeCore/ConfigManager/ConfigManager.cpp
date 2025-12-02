#include"ConfigManager.hpp"

#include"../CSATeCore.hpp"
#include"../CSATeMainWindowManager/ConsoleManager/ConsoleManager.hpp"

#include"../../ThirdParty/All_pugixml.hpp"

#include <windows.h>
#include <ShObjIdl.h>


bool ConfigManager::Init(CSATeCore* Core) {
	this->Core = Core;
	return true;
}



bool ConfigManager::Config_SetCS2Path(const std::filesystem::path& CS2Path) {
    if (!CS2Path.empty() && std::filesystem::exists(CS2Path)) {
        this->Config.CS2Path = CS2Path;
        return true;
    }
    return false;
}


bool ConfigManager::Config_Save(const std::filesystem::path& ConfigPath)const {
    //检查文件路径
    if (ConfigPath.empty()) {
        this->Core->Console().PrintError("尝试保存配置文件时出现问题，空路径：" + ConfigPath.string() + "\n");
        return false;
    }
    if (!std::filesystem::exists(ConfigPath)) {
        this->Core->Console().PrintError("尝试保存配置文件时出现问题，路径不存在：" + ConfigPath.string() + "\n");
        return false;
    }

    //拼接完整路径
    std::filesystem::path FullPath = ConfigPath / "Paths.xml";

    //创建临时XML文件
    pugi::xml_document XML;
    //初始化
    pugi::xml_node node_Declaration = XML.prepend_child(pugi::node_declaration);
    node_Declaration.append_attribute("version") = "1.0";
    node_Declaration.append_attribute("encoding") = "utf-8";

    //开始保存信息

    //保存Paths节点
    pugi::xml_node node_Paths = XML.append_child("Paths");

    //保存CS2路径
    pugi::xml_node node_CS2 = node_Paths.append_child("CS2");
    node_CS2.append_attribute("Path").set_value(this->Config.CS2Path.string());
    
    //保存XML文件到磁盘
    if (!XML.save_file(FullPath.c_str())) {
        this->Core->Console().PrintError("尝试保存配置文件时出现问题，保存失败：" + FullPath.string() + "\n");
        return false;
    }
    this->Core->Console().PrintInfor("配置文件保存成功：" + FullPath.string() + "\n");
    return true;
}

bool ConfigManager::Config_Load(const std::filesystem::path& ConfigPath) {
    //检查文件路径
    if (ConfigPath.empty() || !std::filesystem::exists(ConfigPath)) {
        return false;
    }

    //拼接完整路径
    std::filesystem::path FullPath = ConfigPath / "Paths.xml";

    //创建临时XML文件
    pugi::xml_document XML;
    pugi::xml_parse_result Result = XML.load_file(FullPath.c_str());

    pugi::xml_node node_Paths = XML.child("Paths");

    pugi::xml_node node_CS2 = node_Paths.child("CS2");
    
    this->Config.CS2Path = node_CS2.attribute("Path").as_string();

    return true;
}

std::filesystem::path ConfigManager::Config_GetCS2Path()const {
	return this->Config.CS2Path;
}
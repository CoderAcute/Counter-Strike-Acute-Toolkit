#include"FirstPersonCameraPath.hpp"

#include<sstream>

bool FirstPersonCameraPath::Call(CSATMath::Frame& Frame, float Time, const PlaybackMode Mode)const {
    //如果不在理论影响范围内，应当直接返回而不做任何修改
    //是与被遍历的其它call一起工作
    if (!this->TargetPlayerIndexInMap)return false;

	if (!this->TimeReform(Time, Mode))return false;

    Frame.m_ui8TargetOBMode = 2;

    Frame.m_ui8TargetPlayerIndexInMap = this->TargetPlayerIndexInMap;

    return true;
}
void FirstPersonCameraPath::Refresh() {
    if (this->TargetPlayerIndexInMap == 0) {
        this->StartTime = 0;
        this->EndTime = 0;
        this->DurationTime = 0;
        return;
    }
    else {
        this->DurationTime = this->EndTime - this->StartTime;
        return;
    }
    return;
}

std::string FirstPersonCameraPath::GetMsg()const {
    std::ostringstream oss;
    oss << this->GetBaseMsg()
        << "\n目标玩家索引：" << static_cast<int>(this->TargetPlayerIndexInMap);

    return oss.str();
}


bool FirstPersonCameraPath::ReadElementMain(const pugi::xml_node& node_ElementMain, std::string& strRuselt) {
    pugi::xml_node node_Target = node_ElementMain.child("Target");
    if (!node_Target) {
        strRuselt = "找不到Target节点！ 元素名：" + this->Name;
        return false;
	}
	this->TargetPlayerIndexInMap = static_cast<uint8_t>(node_Target.attribute("PlayerIndex").as_int());
	this->StartTime = node_Target.attribute("StartTime").as_float();
	this->EndTime = node_Target.attribute("EndTime").as_float();

	return true;
}
bool FirstPersonCameraPath::SaveToXML(const std::filesystem::path& FolderPath, std::string& strRuselt)const {
    if (FolderPath.empty()) {
        strRuselt = "文件夹路径为空，无法保存元素到XML文件！";
        return false;
    }
    pugi::xml_document XML;
    pugi::xml_node node_ElementMain;
    if (!this->SaveBase(XML, node_ElementMain, strRuselt)) {
        return false;
    }
    std::filesystem::path FullPath = FolderPath / (this->Name + ".xml");
    
    pugi::xml_node node_Target = node_ElementMain.append_child("Target");

	node_Target.append_attribute("PlayerIndex") = static_cast<int>(this->TargetPlayerIndexInMap);
	node_Target.append_attribute("StartTime") = this->StartTime;
	node_Target.append_attribute("EndTime") = this->EndTime;

    // 保存XML到文件
    if (!XML.save_file(FullPath.c_str())) {
        strRuselt = "尝试保存到XML文件失败，无法保存XML文件！ 文件路径：" + FullPath.string();
        return false;
    }
    strRuselt = "成功保存到XML文件！ 文件路径：" + FullPath.string();
    return true;
}
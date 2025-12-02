#include"ElementBase.hpp"

std::string ElementType_EnumToString(const ElementType Type) {
    switch (Type) {
    case ElementType::FreeCameraPath:return "FreeCameraPath";
    case ElementType::FirstPersonCameraPath:return "FirstPersonCameraPath";
    case ElementType::LockedCameraPath:return "LockedCameraPath";
    case ElementType::ElementBase:return "ElementBase";
    default:return "None";
    }
}

ElementType ElementType_StringToEnum(const std::string& typeStr) {
    if (typeStr == "FreeCameraPath")  return ElementType::FreeCameraPath;
    else if (typeStr == "FirstPersonCameraPath") return ElementType::FirstPersonCameraPath;
    else if (typeStr == "LockedCameraPath") return ElementType::LockedCameraPath;
    else if (typeStr == "ElementBase") return ElementType::ElementBase;
    else return ElementType::None;
}

ElementType ElementBase::TypeGet_Enum()const {
    return this->Type;
}

std::string ElementBase::TypeGet_String()const {
    return ElementType_EnumToString(this->Type);
}

std::string ElementBase::GetBaseMsg()const {
    return "元素名称：" + this->Name +
        "  元素类型：" + this->TypeGet_String() +
        "  持续时长：" + std::to_string(this->DurationTime);
}
std::string ElementBase::GetName()const {
    return this->Name;
}
void ElementBase::ResetName(const std::string& NewName) {
    this->Name = NewName;
}

bool ElementBase::TimeReform(float& Time, const PlaybackMode Mode)const {
    switch (Mode) {
    case PlaybackMode::Composition:
        break;
    case PlaybackMode::Reuse:
        Time += this->StartTime;
        break;
    default:
		return false;
        break;
    }
    //处理边界情况
    if (Time <= this->StartTime) return false;
    if (Time >= this->EndTime) return false;
    return true;
}

bool ElementBase::SaveBase(pugi::xml_document& doc, pugi::xml_node& node_ElementMain, std::string& strRuselt)const {
    if (this->Name.empty()) {
        strRuselt = "元素名为空，无法保存元素到XML文件！";
        return false;
    }
    //初始化
    pugi::xml_node declarationNode = doc.prepend_child(pugi::node_declaration);
    declarationNode.append_attribute("version") = "1.0";
    declarationNode.append_attribute("encoding") = "utf-8";

    //创建根节点
    pugi::xml_node node_Element = doc.append_child("Element");
	node_Element.append_attribute("Type") = this->TypeGet_String().c_str();
    node_Element.append_attribute("Name") = this->Name.c_str();

    //创建ElementMain节点并设置属性
    node_ElementMain = node_Element.append_child("ElementMain");
    node_ElementMain.append_attribute("DurationTime") = this->DurationTime;

    return true;
}

float ElementBase::GetStartTime()const {
    return this->StartTime;
}
float ElementBase::GetEndTime()const {
    return this->EndTime;
}
float ElementBase::GetDurationTime()const {
    return this->DurationTime;
}
#pragma once

#include"../../ElementBase/ElementBase.hpp"

class FirstPersonCameraPath final :public ElementBase {
public:
	static constexpr ElementType TemplateType = ElementType::FirstPersonCameraPath;

    uint8_t TargetPlayerIndex = 0;

    //构造函数
    explicit FirstPersonCameraPath(const std::string& name) :
        ElementBase(name) {
        this->Type = ElementType::FirstPersonCameraPath;
    }

    //虚函数：

    //析构函数（虚）
    ~FirstPersonCameraPath()override = default;
    //调用函数（虚）（Mode:0为默认，1自动减去头时间）
    bool Call(CSATMath::Frame& Frame, float Time, const PlaybackMode Mode)const override final;
    //刷新状态（虚）
    void Refresh()override final;
    //信息获取函数（虚）
    std::string GetMsg()const override final;
    //加载信息函数（虚），注意这不是创建而是加载
    bool ReadElementMain(const pugi::xml_node& node_ElementMain, std::string& strRuselt)override final;
    //保存函数（虚）
    bool SaveToXML(const std::filesystem::path& FolderPath, std::string& strRuselt)const override final;
};
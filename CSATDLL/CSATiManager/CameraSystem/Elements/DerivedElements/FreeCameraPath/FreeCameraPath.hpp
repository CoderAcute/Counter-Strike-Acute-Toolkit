#pragma once

#include"../../ElementBase/ElementBase.hpp"

class ElementManager;
class ElementDebugger;

class CameraDrawer;

//自由摄像机轨道，继承自Element
class FreeCameraPath final :public ElementBase {
    //友元声明
    friend ElementManager;
    friend ElementDebugger;
public:
    //模板类型
    static constexpr ElementType TemplateType = ElementType::FreeCameraPath;
    //额外数据存储
    std::vector<CSATMath::CameraKeyFrame> CameraKeyFrames{};
    size_t Size_Frames = 0;
    //构造函数
    explicit FreeCameraPath(const std::string& name) : 
        ElementBase(name) {
		this->Type = ElementType::FreeCameraPath;
        this->Drawable = true;
    }
public:
    //虚函数类
    //刷新状态（虚）
    void Refresh()override final;
    //调用函数（虚）（Mode:0为默认，1自动减去头时间）
    bool Call(CSATMath::Frame& Frame, float Time, const PlaybackMode Mode)const override final;
    //绘制函数（虚），各个元素按需实现
    bool Draw(CameraDrawer* CamDrawer, const float* Matrix, const float WinWidth, const float WinHeight)const override;
    //获取详细信息（虚）
    std::string GetMsg()const override final;
    //加载信息函数（虚），注意这不是创建而是加载
    bool ReadElementMain(const pugi::xml_node& node_ElementMain, std::string& strRuselt)override final;
    //保存到XML文件（虚）
    bool SaveToXML(const std::filesystem::path& FolderPath, std::string& strRuselt)const override final;
    
    //非虚函数类
    //增加关键帧，拷贝语义
	void AddKeyframe(const CSATMath::CameraKeyFrame& KeyFrame);
    //增加关键帧，移动语义
	void AddKeyframe(CSATMath::CameraKeyFrame&& KeyFrame);
    //归一化关键帧时间
    void TimeNormalize();
    //清空所有关键帧
    void Clear();
    
    size_t GetKeyFrameCount()const;//获取关键帧数量
    const CSATMath::CameraKeyFrame& GetKeyFrame(const size_t& index)const;//获取特定关键帧    
    const std::vector<CSATMath::CameraKeyFrame>& GetAllKeyFrames()const;//获取所有关键帧

};
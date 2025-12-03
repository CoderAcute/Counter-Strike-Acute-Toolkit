#pragma once

#include"../../../../CSATMath/CSATMath.hpp"
#include"../../../../ThirdParty/All_pugixml.hpp"

#include<string>
#include<filesystem>
//定义元素的类型
enum class ElementType :int {
    None = -1,//错误类型
    ElementBase = 0,//元素基类
    FreeCameraPath = 1,//自由摄像机轨道
    FirstPersonCameraPath = 2,//第一人称摄像机轨道
    LockedCameraPath = 3//锁定摄像机轨道
};

//播放模式
enum class PlaybackMode {
    Composition,     //组合模式（保留元素头，相当于元素同时激活）
    Reuse            //复用模式（不保留元素头，相当于元素被化为复用资产）
};

std::string ElementType_EnumToString(const ElementType Type);
ElementType ElementType_StringToEnum(const std::string& typeStr);

//元素基类，内含call方法修改target
class ElementBase {
    //对于元素基类，持续时长和名称是所有元素都需要的属性，我们还需要一个纯虚函数Call用于多态调用，每个元素可以有自己的数据和实现，尤其针对Call函数
    //元素的创建、获取、删除由ElementManager统一管理
    //Call函数的通用调用是CameraSystem的实现的核心，因为CameraSystem只需要调用Call函数而不需要知道具体元素的类型，各种Call会在各种不同的地方调用
    //元素的具体功能由各个元素类自己实现

    //友元声明
    friend class ElementManager;
    friend class Solution;
    friend class SolutionManager;
    friend class ElementDebugger;
protected:

    //基本数据存储

    //元素类型
    //模板类型
    static constexpr ElementType TemplateType = ElementType::ElementBase;
	//实际类型
	ElementType Type;
    //元素名称
    std::string Name{};

    //开始时间（绝对）
    float StartTime{};
    //结束时间（绝对）
    float EndTime{};
    //持续时长
    float DurationTime{};
    
    //需要被删除
    bool NeedBeDelete = false;
    
    //能否绘制
    bool Drawable = false;
    //是否绘制
    bool IfDraw = false;
public:
    //构造函数
    ElementBase() = default;
    explicit ElementBase(const std::string& name) :
        Name(name) {
        this->Type = ElementType::ElementBase;
    }

    
public:

    //ID相关

    //获取枚举类型
    ElementType TypeGet_Enum()const;
    //获取字符串类型
    std::string TypeGet_String()const; 
    //获取基本信息
    std::string GetBaseMsg()const;
    //获取名字
	std::string GetName()const;
    //重设名字
    void ResetName(const std::string& NewName);

	//时间变换辅助函数
    bool TimeReform(float& Time, const PlaybackMode Mode)const;

    float GetStartTime()const;
	float GetEndTime()const;
	float GetDurationTime()const;

    //析构函数（虚）
    virtual ~ElementBase() = default;
    //调用函数（虚）（Mode:0为默认，1自动减去头时间）
    virtual bool Call(CSATMath::Frame& Frame, float Time, const PlaybackMode Mode)const = 0;
    //刷新状态（虚）
    virtual void Refresh() = 0;
    //信息获取函数（虚）
    virtual std::string GetMsg()const = 0;
    //加载信息函数（虚），注意这不是创建而是加载
    virtual bool ReadElementMain(const pugi::xml_node& node_ElementMain, std::string& strRuselt) = 0;

	//保存基础函数（元素类型，元素名称，持续时间（附加声明））
    bool SaveBase(pugi::xml_document& doc, pugi::xml_node& node_ElementMain, std::string& strRuselt)const;
    //保存函数（虚）
    virtual bool SaveToXML(const std::filesystem::path& FolderPath, std::string& strRuselt)const = 0;
};

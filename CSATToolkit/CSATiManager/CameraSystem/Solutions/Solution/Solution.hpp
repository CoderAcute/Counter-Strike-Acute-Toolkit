#pragma once

#include"../../Elements/Elements.hpp"
#include"../../../KeyTracker/KeyTracker.hpp"


class ElementWithOffset {
public:
    //元素
    std::shared_ptr<ElementBase> Element;

    //这个Offset决定了元素的播放时间头
    float Offset = 0;
};

//解决方案，包含ElementWithOffset，用于调用call方法
class Solution final{
    //友元声明
    friend class SolutionManager;
private:
    //数据存储

    //解决方案名称
    std::string Name{};

    //存储元素的指针，允许调用不同元素的同一个Call函数（多态，内部实现不同）
    std::vector<ElementWithOffset> Elements;
    //元素个数
    size_t Size_Elements{};

    
    //开始时间
    float StartTime{};
    //结束时间
    float EndTime{};
    //总持续时间
	float TotalDurationTime{};
    //解决方案时间偏移
    float SolutionOffset = 0;
	//播放模式（0：默认游戏时间轴播放，1：偏移时间轴播放）
    PlaybackMode Playmode = PlaybackMode::Reuse;

    //按键检测包
    KeyCheckPack KCPack{};
public:
    //构造函数
    Solution(const std::string& Name) :
        Name(Name) {
        this->Refresh();
    }

    //解决方案能否安全使用
    bool SafeUse = false;
    //析构函数
    ~Solution() = default;

    //添加元素
    //常量指针常量说明没有修改权，时间偏移默认是0.0f
    bool AddElement(const std::shared_ptr<ElementBase> const element, const float Offset = 0.0f);
    //移除指定位置的元素
    bool RemoveElementAt(const size_t Index);
    
    //将解决方案保存到XML文件(序列化，反序列化在SolutionManager)
    bool SaveToXML(const std::filesystem::path& FolderPath, std::string& stRuselt);

    //清空数据
    void Clear();
    //重设名字
    void ResetName(std::string_view NewName);
	//获取名字
	std::string GetName()const;

    //解决方案插值调用
    bool Call(CSATMath::Frame& frame, const float time, const float PlayBackRate, bool& isPlaying);
    //设置解决方案偏移
    void SetSolutionOffset(const float Offset);
    //刷新（会移除已经删除的元素）
    void Refresh();
    //展示信息
    std::string GetMsg();

    //设置按键检测包
    void SetKeyCheckPack(KeyCheckPack& KCPack);
};
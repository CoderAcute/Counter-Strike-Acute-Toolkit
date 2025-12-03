#pragma once

#include"../Solution/Solution.hpp"
#include"SolutionConfig.hpp"
#include"../../../KeyTracker/KeyTracker.hpp"

#include<filesystem>

class CSATiManager;

class KeyTracker;
class KeyCheckPack;
class IAbstractLayer3D;

class CameraDrawer;

class ElementManager;
class ProjectManager;

class CSATMessage;

//解决方案管理器，用于管理解决方案
class SolutionManager {
private:
    CSATiManager* CSATi = nullptr;
	IAbstractLayer3D* AL3D = nullptr;

    CameraDrawer* CamDrawer = nullptr;
    ElementManager* EManager = nullptr;
    ProjectManager* PManager = nullptr;

    KeyTracker* KeyTracker = nullptr;
public:
    bool NeedRefresh = false;
    SolutionConfig Config{};
private:
    ////////////////////////////////////////
    //当前操作的解决方案指针
	Solution* CurrentSolution = nullptr;
    ////////////////////////////////////////
    //操作调试窗口
public:
    //是否打开解决方案调试窗口
    bool OpenSolutionDebugWindow = false;
private:
    //是否打开解决方案按键绑定调试窗口
    bool OpenSolutionKCPackDebugWindow = false;
    //是否打开解决方案名称修改调试窗口
    bool OpenSolutionNameDebugWindow = false;
    ////////////////////////////////////////
public:

    //数据存储
    std::vector<std::unique_ptr<Solution>> Solutions{};

    ////////////////////////////////////////
    //播放

    //当前播放的解决方案
    Solution* Playing_pSolution = nullptr;
    //是否处于播放状态
    //播放完成之后需要变为false，切换解决方案要变成true
    bool Playing = false; 
    //播放模式（0：默认游戏时间轴播放，1：偏移时间轴播放）
	PlaybackMode Playmode = PlaybackMode::Reuse;
    //播放比率，用于调控解决方案播放速度和游戏速度，用于制作子弹时间等
    float PlaybackRate = 1.0f;

    ////////////////////////////////////////

    //解决方案管理器基本函数
    //初始化函数
    void Init(CSATiManager* CSATi, CameraDrawer* CamDrawer, ElementManager* EManager, ProjectManager* PManager);
    //逻辑主函数
    void VirtualMain();
    //遍历，用于迭代处理每个解决方案
    void Traversal();
    //刷新，更深层次的遍历，主要用于清理失效元素
    void Refresh();



    //创建解决方案
    bool Solution_Create(const std::string& Name);
	//保存所有解决方案到XML文件
	bool Solution_SaveAll();
    //从XML文件加载解决方案（反序列化，序列化在Solution）
    bool Solution_LoadFromXML(const std::string& XMLName, const std::filesystem::path& FolderPath);
    //从XML文件加载解决方案（反序列化，序列化在Solution）
    bool Solution_LoadFromXML(const std::filesystem::path& FullPath);
    //获取解决方案对应的迭代器
    std::vector<std::unique_ptr<Solution>>::iterator Solution_GetIterator(const std::string& Name);
    //获取解决方案指针
    Solution* Solution_Get(const std::string& Name);
    //删除解决方案
    bool Solution_Delete(Solution* Solution);
    bool Solution_Delete(const std::string& Name);
	//删除所有解决方案
	bool Solution_ClearAll();
    //展示某解决方案信息
    void Solution_ShowMsg(const std::string& Name);
    //展示所有解决方案信息
    void Solution_ShowAll();
    //获取所有解决方案名称容器（危险函数，只有摄像机系统内部可用）
    const std::vector<std::string>& Solution_GetNames()const;
    //展示单个解决方案信息在一行上
	void Solution_ShowInLine(Solution* solution);
    //按行展示所有解决方案
    void Solution_ShowAllInLines();


	//解决方案调试相关所有窗口
	void Windows();
private:
    //解决方案调试窗口及菜单
	void Solution_DebugWindow();
    //按键调试缓存
    KeyCheckPack Buffer_KCPack{};
    //按键绑定菜单
    void Solution_KCPack_DebugWindow();
    //名称修改缓存
    std::string Buffer_Name{};
    //名称修改窗口
    void Solution_Name_DebugWindow();




    //预览功能相关
public:
    //开启播放
    void Playing_Enable();
    //关闭播放
    void Playing_Disable();
    //通过指针设置当前播放的解决方案（0：默认游戏时间轴播放，1：偏移时间轴播放）
    bool Playing_SetSolution(Solution* const solution, const PlaybackMode Playmode);
    //通过名称设置当前播放的解决方案（0：默认游戏时间轴播放，1：偏移时间轴播放）
    bool Playing_SetSolution(const std::string& SolutionName, const PlaybackMode Playmode);
    
    
    //设置播放时间偏移
    void Playing_SetTimeSchema(const float Time);
    //调用播放
    bool Playing_Call();
};
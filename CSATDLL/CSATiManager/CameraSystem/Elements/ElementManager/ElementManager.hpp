#pragma once

#include"../Elements.hpp"
#include"ElementConfig.hpp"

#include"../../../CSATiManager.hpp"
#include"../../../Debugger/IDebugger.hpp"


#include<filesystem>

class CSATiManager;

class IAbstractLayer3D;
class CameraDrawer;

class SolutionManager;
class ProjectManager;

class ElementDebugger;

//元素管理器，用于管理元素
class ElementManager {
	//对于元素，我们只给出三个通用接口：创建、获取、删除，具体的各个元素的功能由各个元素类自己实现
private:
	//CSATi指针，用于其它服务
	CSATiManager* CSATi = nullptr;

	IAbstractLayer3D* AL3D = nullptr;

	CameraDrawer* CamDrawer = nullptr;

	SolutionManager* SManager = nullptr;
	ProjectManager* PManager = nullptr;

	ElementDebugger* ElementDebugger = nullptr;

	//是否需要更新当前操作的元素
	bool NeedUpdateCurrentElement = false;
	//更新当前操作的元素的函数
	void UpdateCurrentElement();
	//当前操作的元素名称
	std::string CurrentElementName = "";
	//当前操作的元素指针
	std::shared_ptr<ElementBase> CurrentElement = nullptr;

public:
	ElementManager();
	~ElementManager();

	ElementConfig Config{};
	//使用智能指针存储多态对象，以存储不同类型的元素
	std::vector<std::shared_ptr<ElementBase>> Elements;

	//预览相关

	//预览时间偏移
	float Preview_TimeSchema{};
	//预览结束时间点
	float Preview_EndTime{};
	//当前预览元素指针
	std::shared_ptr<ElementBase> Preview_CurrentElement;
	//是否处于预览状态
	bool OnPreview = false;



	//元素管理器基本函数
	
	//初始化
	void Init(CSATiManager* CSATi, CameraDrawer* CamDrawer, SolutionManager* SManager, ProjectManager* PManager);
	//逻辑主函数
	void VirtualMain();


	//通用基础函数：

	//获取元素对应的迭代器（Element基类）
	std::vector<std::shared_ptr<ElementBase>>::iterator Element_GetIterator(const std::string_view Name);

	//获取元素指针
	template<Element T>
	std::shared_ptr<T> Element_Get(const std::string_view Name) {
		//编译期获取目标类型
		constexpr ElementType TargetElementType = T::TemplateType;
		//获取迭代器
		std::vector<std::shared_ptr<ElementBase>>::iterator it = this->Element_GetIterator(Name);
		//验证是否找到
		if (it == this->Elements.end()) {
			return nullptr;
		}
		//得到指针
		std::shared_ptr<ElementBase> Ptr = *it;
		//ElementBase直接返回有效指针
		if constexpr (TargetElementType == ElementType::ElementBase) {
			return std::static_pointer_cast<T>(Ptr);
		}
		//检验类型匹配
		if (Ptr->Type == TargetElementType) {
			//静态转换类型（高性能）
			return std::static_pointer_cast<T>(Ptr);
		}
		//失败则返回空指针
		return nullptr;
	}
	

	//创建元素函数，支持传递任意参数给元素构造函数
	template<Element T>
	bool Element_Create(const std::string& Name) {
		// 检查是否已存在同名元素
		if (this->Element_Get<ElementBase>(Name)) {
			this->CSATi->IDebugger().AddError("元素名已占用！ 元素名：" + Name);
			return false;
		}
		//输出成功信息
		this->CSATi->IDebugger().AddSucc("成功创建元素！  元素名：" + Name);
		//创建指针
		std::shared_ptr<T> Elem = std::make_shared<T>(std::move(Name));
		//添加进Elements
		this->Elements.push_back(std::move(Elem));
		return true;
	}

	//保存所有元素到XML文件
	bool Element_SaveAll();
	//从XML文件加载元素的预处理函数，内部会创建对应类型的元素，并调用具体加载函数加载信息
	bool Element_LoadFromXML_Pre(const std::string& XMLName, const std::filesystem::path& FolderPath);
	//从XML文件加载元素的预处理函数，内部会创建对应类型的元素，并调用具体加载函数加载信息
	bool Element_LoadFromXML_Pre(const std::filesystem::path& FullPath);
	//删除函数元素，返回true表示名称现在可用
	bool Element_Delete(const std::string& Name);
	//清空所有元素
	bool Element_ClearAll();
	//展示单个元素信息在一行上
	void Element_ShowInLine(const std::shared_ptr<ElementBase> const element);
	//展示所有元素信息（编号、名称）到调试窗口
	void Element_ShowAll();
	//展示某个元素的详细信息到调试窗口
	void Element_ShowMsgToDebugMenu(const std::shared_ptr<ElementBase> const element)const;
	//获取元素名称容器
	std::vector<std::string> Element_GetNames()const;



	//预览功能相关：
	//启用预览
	void Preview_Enable();
	//禁用预览
	void Preview_Disable();
	//切换预览元素
	void Preview_SetElement(const std::string& Name);
	//设置预览偏移
	void Preview_SetPreviewSchema(const float Time);
	//预览Call
	bool Preview_Call(CSATMath::Frame& Frame, const float Time);


	//元素调试相关所有窗口
	void Windows();
	//是否打开元素调试窗口
	bool OpenElementDebugWindow = false;
	//元素调试窗口
	void ElementDebugWindow();
	//自由摄像机轨道特化函数：
	//绘制自由摄像机轨道

	
};
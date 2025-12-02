#include"CameraSystem.hpp"

#include"../CSATiManager.hpp"
#include"../AbstractLayer3D/IAbstractLayer3D.hpp"
#include"../IPCer/IPCer.hpp"

#include"../MessageManager/IMessageManager.hpp"

#include"../GlobalVars/GlobalVars.hpp"
//摄像机系统

bool CameraSystem::Init() {
    //传递指针，注入依赖，提升性能，直接调用
    //注意，本模块所有级别的管理器相互显示注入，其它服务借助CSATi隐式注入
    //高频服务隐式注入，指针直调提升性能
	this->AL3D = &this->CSATi->IAbstractLayer3D();
    this->CamDrawer.Init(20.0, 30.0, 15.0, 10.0, IM_COL32(255, 0, 255, 255));
    this->EManager.Init(CSATi, &this->CamDrawer, &this->SManager, &this->PManager);
    this->SManager.Init(CSATi, &this->CamDrawer, &this->EManager, &this->PManager);
    this->PManager.Init(CSATi, &this->EManager, &this->SManager);
    this->WManager.Init(CSATi, &this->EManager, &this->SManager, &this->PManager);

    return true;
}
void CameraSystem::Menu() {
    //顶部：工作区信息（始终显示）
    ImGui::BeginChild("工作区面板", ImVec2(0, 150), true);{
        //工作区状态
        ImGui::Text("工作区状态: %s", this->WManager.InWorkspace ? "已进入" : "未进入");
        //未进入工作区允许打开默认工作区
        if (!this->WManager.InWorkspace) {
            ImGui::SameLine();
            if (ImGui::Button("打开默认工作区")) {
                this->WManager.Workspace_Set("DefaultWorkspace");
            }
        }
        //打开工作区后允许保存工作区
        if (this->WManager.InWorkspace) {
            ImGui::Text("当前工作区: %s", this->WManager.CurrentWorkspace->Name.c_str());
            ImGui::SameLine();
            if (ImGui::Button("保存工作区")) {
                this->WManager.Workspace_Save();
            }
        }
        //详情信息
        ImGui::Separator();
        ImGui::BeginChild("工作区详情菜单");
        if (ImGui::CollapsingHeader("详情信息")) {
            this->MenuWorkspace();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();


    if(!this->WManager.InWorkspace) {
        //如果不在工作区，显示提示信息
        ImGui::BeginChild("提示", ImVec2(0, 0), true);
        ImGui::Text("请先进入工作区");
        ImGui::Text("在上方的工作区面板中加载（或创建并加载）一个工作区");
        ImGui::Text("工作区是管理项目、解决方案和元素的基础");
        ImGui::EndChild();
        return;
    }

    //进入工作区，显示工作区内容
    static int SelectedTab = 0;
    const char* tabNames[] = { "项目", "解决方案", "元素" };

    //左侧导航栏
    ImGui::BeginChild("导航", ImVec2(150, 0), true);
    for (int Index = 0; Index < IM_ARRAYSIZE(tabNames); ++Index) {
        if (ImGui::Selectable(tabNames[Index], SelectedTab == Index)) {
            SelectedTab = Index;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    //右侧三类控制区
    ImGui::BeginChild("内容", ImVec2(0, 0), true);
    bool InProject = false;
    if (this->PManager.ActiveProject) {
        InProject = true;
        ImGui::Text(("当前项目：" + this->PManager.ActiveProject->Name).c_str());
    }
    else {
		ImGui::Text("当前未进入任何项目");
    }
    
    ImGui::Separator();
    switch (SelectedTab) {
    case 0://项目菜单
        this->MenuProject();
        break;
    case 1://解决方案菜单
        if (!InProject) {
            ImGui::Text("请先进入项目");
            break;
        }
        this->MenuSolution();
        break;
    case 2://元素菜单
        if (!InProject) {
            ImGui::Text("请先进入项目");
            break;
        }
        this->MenuElement();
        break;
    }
    ImGui::EndChild();
    return;
}

//各个小菜单

void CameraSystem::MenuElement() {
    //展示预览功能相关状态
    ImGui::Text(("预览状态： " + std::string(this->EManager.OnPreview ? "开启" : "关闭") +
        "   预览元素名： " + (this->EManager.Preview_CurrentElement ? this->EManager.Preview_CurrentElement->GetName() : "无") +
        "   预览时间偏移： " + std::to_string(this->EManager.Preview_TimeSchema)).c_str());
    ImGui::Separator();
    //元素总设置
    if (ImGui::CollapsingHeader("元素设置")) {
        ImGui::Checkbox("预览插值摄像机绘制", &this->EManager.Config.PreviewDraw);
        ImGui::Checkbox("预览插值摄像机覆盖", &this->EManager.Config.PreviewOverride);
    }
    //创建元素
    if (ImGui::CollapsingHeader("元素创建")) {
		ImGui::Text("新元素名：");
        ImGui::SameLine();
        static std::string CreateElementName = "";
        ImGui::InputText("##新元素名", &CreateElementName);
		//创建成功则清空输入框
		//创建自由摄像机轨道
        if (ImGui::Button("新的自由摄像机轨道")) {
            if (this->EManager.Element_Create<FreeCameraPath>(CreateElementName)) {
                CreateElementName.clear();
            }
        }
        //创建第一人称摄像机轨道
        if (ImGui::Button("新的第一人称摄像机轨道")) {
            if (this->EManager.Element_Create<FirstPersonCameraPath>(CreateElementName)) {
                CreateElementName.clear();
            }
        }
		////调试模式下解锁更多元素类型的创建
  //      if (this->CSATi->GlobalVars().DebugMode) {
  //          
  //      }
  //      else {
		//	ImGui::Text("打开调试模式以解锁更多元素类型的创建功能");
  //      }
    }
    //载入元素
    if (ImGui::CollapsingHeader("元素载入")) {
		ImGui::Text("载入元素名：");
		ImGui::SameLine();
		static std::string LoadElementName = "";
		ImGui::InputText("##载入元素名", &LoadElementName);
        ImGui::SameLine();
        //载入成功则清空输入框
        if (ImGui::Button("载入##元素")) {
            if (this->EManager.Element_LoadFromXML_Pre(LoadElementName, this->CSATi->IPCer().PathGet_CurrentElements())) {
                LoadElementName.clear();
            }
        }
    }
    //展示修改元素
    if (ImGui::CollapsingHeader("元素列表")) {
        //输出是否打开了元素调试窗口
        ImGui::Text(("元素调试窗口状态：" + std::string(this->EManager.OpenElementDebugWindow ? "打开" : "关闭")).c_str());
        //使用迭代器遍历所有元素
        for (const auto& element : this->EManager.Elements) {
            this->EManager.Element_ShowInLine(element);
        }
    }

    return;
}
void CameraSystem::MenuSolution() {
    ImGui::Text(("播放状态：" + std::string(this->SManager.Playing ? "播放中" : "关闭") +
        "  活跃解决方案名：" + (this->SManager.Playing_pSolution ? this->SManager.Playing_pSolution->GetName() : "无")).c_str());
	ImGui::Separator();
    //解决方案总设置
    if (ImGui::CollapsingHeader("解决方案设置")) {
        ImGui::Checkbox("解决方案快捷键检测系统", &this->SManager.Config.SolutionShortcutEnable);
        ImGui::Checkbox("解决方案插值摄像机绘制", &this->SManager.Config.PlayingDraw);
        ImGui::Checkbox("解决方案插值覆盖", &this->SManager.Config.PlayingOverride);

        ImGui::Text("播放倍率：%f", 1 / this->SManager.PlaybackRate);
        static float temp = 1.0f;
        ImGui::Text("目标倍率:");
        ImGui::SameLine();
        ImGui::InputFloat("##目标倍率", &temp);
        ImGui::SameLine();
        if (ImGui::Button("切换##解决方案播放倍率")) {
            this->AL3D->ExecuteCommand("host_timescale " + std::to_string(temp));
            this->SManager.PlaybackRate = 1 / temp;
        }
        
    }
    //创建解决方案
    if (ImGui::CollapsingHeader("解决方案创建")) {
		ImGui::Text("新解决方案名：");
		ImGui::SameLine();
        static std::string CreateSolutionName = "";
        ImGui::InputText("##新解决方案名", &CreateSolutionName);
        ImGui::SameLine();
        //创建成功则清空输入框
        if (ImGui::Button("创建解决方案")) {
            if (this->SManager.Solution_Create(CreateSolutionName)) {
                CreateSolutionName.clear();
            }
        }
    }
    //载入解决方案
    if (ImGui::CollapsingHeader("解决方案载入")) {
        static std::string LoadSolutionName = "";
        ImGui::Text("载入解决方案名：");
        ImGui::SameLine();
        ImGui::InputText("##载入解决方案名", &LoadSolutionName);
        ImGui::SameLine();
		//载入成功则清空输入框
        if (ImGui::Button("载入解决方案")) {
            if (this->SManager.Solution_LoadFromXML(LoadSolutionName, this->CSATi->IPCer().PathGet_CurrentSolutions())) {
                this->CSATi->IDebugger().AddSucc(("加载解决方案成功：" + LoadSolutionName).c_str());
                LoadSolutionName.clear();
            }
            else {
                this->CSATi->IDebugger().AddError(("加载解决方案失败：" + LoadSolutionName).c_str());
            }
        }
    }
    //展示修改解决方案
    if (ImGui::CollapsingHeader("解决方案列表")) {
        //输出是否打开了解决方案调试窗口
        ImGui::Text(("解决方案调试窗口状态：" + std::string(this->SManager.OpenSolutionDebugWindow ? "打开" : "关闭")).c_str());
        this->SManager.Solution_ShowAllInLines();
    }

    return;
}
void CameraSystem::MenuProject() {
    //项目总设置
    if (ImGui::CollapsingHeader("项目设置")) {
        ImGui::Checkbox("项目快捷键检测系统", &this->PManager.Config.ProjectShortcutEnable);
    }
    //创建项目
    if (ImGui::CollapsingHeader("项目创建")) {
        static std::string CreateProjectName = "";
        ImGui::Text("新项目名：");
        ImGui::SameLine();
        ImGui::InputText("##CreateProject", &CreateProjectName);
        ImGui::SameLine();
        if (ImGui::Button("创建")) {
            if (this->PManager.Project_Create(CreateProjectName)) {
                CreateProjectName.clear();
            }
        }

    }
    //展示修改项目
    if (ImGui::CollapsingHeader("项目列表")) {
        //输出是否打开了项目调试窗口
        ImGui::Text(("项目调试窗口状态：" + std::string(this->PManager.OpenProjectDebugWindows ? "打开" : "关闭")).c_str());
        this->PManager.Project_ShowAllInLines();
    }

    return;
}
void CameraSystem::MenuWorkspace() {
    static std::string TargetWorkspaceName{};
    ImGui::Text("工作区名：");
    ImGui::SameLine();
    ImGui::InputText("##TargetWorkspaceName", &TargetWorkspaceName);
    if (ImGui::Button("新建")) {
        this->WManager.Workspace_Create(TargetWorkspaceName);
    }
    ImGui::SameLine();
    if (ImGui::Button("切换")) {
        if (!this->WManager.Workspace_Set(TargetWorkspaceName))return;
        
    }

    if (!this->WManager.CurrentWorkspace) {//无工作区
        ImGui::Text("当前未打开任何工作区");
        return;
    }
    this->WManager.InWorkspace = true;

    return;
}



//其它功能

void CameraSystem::VirtualMain() {
	//四大管理器逻辑主函数
	//摄像机绘制器更新 
	//注意调用顺序，元素管理器在前，项目管理器在后
	//每次循环，前管理器执行操作，后管理器可以安全地读取前管理器的状态
    this->CamDrawer.Update(this->AL3D->GetViewMatrix(), this->AL3D->GetWinWidth(), this->AL3D->GetWinHeight());
    this->EManager.VirtualMain();
    this->SManager.VirtualMain();
	this->PManager.VirtualMain();
	this->WManager.VirtualMain();
    return;
}
void CameraSystem::Windows() {
    this->EManager.Windows();
	this->SManager.Windows();
	this->PManager.Windows();
}
void CameraSystem::MemoryClear() {
    this->EManager.Element_ClearAll();
    this->SManager.Solution_ClearAll();
    this->PManager.Project_ClearAll();
    return;
}

void CameraSystem::test() {
    this->CSATi->IDebugger().AddSucc("摄像机系统接口测试成功！");
    return;
}




//接口实现：

void CameraSystem::ResetCameraModule(float CameraHigh, float CameraX, float CameraY, float AxisLenth, ImU32 Colour) {
    this->CamDrawer.Init(CameraHigh, CameraX, CameraY, AxisLenth, Colour);
    return;
}
void CameraSystem::DrawCameraByPAR(DirectX::XMFLOAT3& Position, DirectX::XMFLOAT3& Rotation, const char* label) {
    this->CamDrawer.DrawCamera(Position, Rotation, label);
    return;
}
bool CameraSystem::CallProject(const std::string& ProjectName) {
    return true;
}
bool CameraSystem::CallSolution(const std::string& SolutionName) {
    if (this->SManager.Playing_SetSolution(SolutionName, PlaybackMode::Reuse)) {
        this->SManager.Playing_Enable();
        return true;
    }
    else {
        return false;
    }
}
bool CameraSystem::CallSolution(const CSATMessage& Msg) {
    if (this->PManager.Playing_AutoCall(Msg)) {
        this->SManager.Playing_Enable();
        return true;
    }
    else {
        return false;
    }
}
bool CameraSystem::ShutDown() {
    this->CSATi->IDebugger().AddWarning("ShutDown被调用！");
    this->SManager.Playing_Disable();
    return true;
}
bool CameraSystem::Save() {
    return this->WManager.Workspace_Save();
}
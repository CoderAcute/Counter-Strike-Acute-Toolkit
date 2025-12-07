#include"ElementManager.hpp"

#include"../../../CSATiManager.hpp"

#include"../../../AbstractLayer3D/IAbstractLayer3D.hpp"

#include"../../CameraDrawer/CameraDrawer.hpp"
#include"../../Solutions/SolutionManager/SolutionManager.hpp"
#include"../../Projects/ProjectManager/ProjectManager.hpp"

#include"../../../Debugger/IDebugger.hpp"
#include"../../../KeyTracker/KeyTracker.hpp"
#include"../../../IPCer/IPCer.hpp"


#include"ElementDebugger/ElementDebugger.hpp"

ElementManager::ElementManager() 
    : ElementDebugger(new class ElementDebugger){

}
ElementManager::~ElementManager() {
    delete this->ElementDebugger;
}

//元素管理器基本函数
void ElementManager::Init(CSATiManager* CSATi, CameraDrawer* CamDrawer, SolutionManager* SManager, ProjectManager* PManager) {
    //核心服务
    this->CSATi = CSATi;
    //获取3D抽象服务
    this->AL3D = &this->CSATi->IAbstractLayer3D();
    //摄像机系统服务
    this->CamDrawer = CamDrawer;
    this->SManager = SManager;
    this->PManager = PManager;

    this->ElementDebugger->Init(CSATi, CamDrawer, this);
}
void ElementManager::UpdateCurrentElement() {
    if (this->NeedUpdateCurrentElement) {
        this->CurrentElement = this->Element_Get<ElementBase>(this->CurrentElementName);
        this->NeedUpdateCurrentElement = false;
    }
}
void ElementManager::VirtualMain() {
    this->UpdateCurrentElement();

	size_t Size = this->Elements.size();
    if (Size) {
        //通过迭代器绘制所有可以绘制的元素
        for (auto& elem : this->Elements) {
            elem->DrawBase(this->CamDrawer, this->AL3D->GetViewMatrix(), this->AL3D->GetWinWidth(), this->AL3D->GetWinHeight());
        }
    }

    if(this->OnPreview) {
        CSATMath::Frame frame;
        if (this->Preview_Call(frame, this->AL3D->GetTime())) {
            //自由摄像机轨道预览
            if (this->Preview_CurrentElement->Type == ElementType::FreeCameraPath) {
                if (this->Config.PreviewOverride) {
                    this->AL3D->SetLocalFrame(frame);
                }
                else if (this->Config.PreviewDraw) {
                    this->CamDrawer->DrawFrameCamera(frame, "预览摄像机");
                }
            }
        }
		//其它类型预览
	}
}
void ElementManager::Windows() {
    if (this->OpenElementDebugWindow) {
		this->ElementDebugWindow();
    }
    return;
}
void ElementManager::ElementDebugWindow() {
    //打开窗口
	ImGui::Begin("元素调试", &this->OpenElementDebugWindow);

    //检查当前是否有操作元素
    if (this->CurrentElement) {
        ImGui::Text(("当前操作元素名称：" + this->CurrentElement->Name + "   元素类型：" + this->CurrentElement->TypeGet_String() + "   持续时长：" + std::to_string(this->CurrentElement->DurationTime)).c_str());
        
        if(ImGui::Button("打印元素信息到调试窗口")) {
            this->Element_ShowMsgToDebugMenu(this->CurrentElement);
		}
        
        if (this->CurrentElement->Drawable) {
			ImGui::Checkbox("绘制", &this->CurrentElement->IfDraw);
        }
        if (ImGui::Button("保存到XML文件")) {
            std::string Ruselt;
            if (this->CurrentElement->SaveToXML(this->CSATi->IPCer().PathGet_CurrentElements(), Ruselt)) {
                this->CSATi->IDebugger().AddSucc(std::move(Ruselt));
            }
            else {
                this->CSATi->IDebugger().AddError(std::move(Ruselt));
            }
        }
        if (ImGui::Button("删除当前元素")) {
            this->Element_Delete(this->CurrentElement->Name);
			this->NeedUpdateCurrentElement = true;
            this->UpdateCurrentElement();
            ImGui::End();
            return;
        }

		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();

		//根据元素类型调用不同的调试菜单
        this->ElementDebugger->DebugMenus(this->CurrentElement.get());

    }
    //如果没有操作元素
    else {
        ImGui::Text("当前未选择任何元素");
    }

    ImGui::Separator();
    ImGui::Separator();
    ImGui::Separator();

    if (ImGui::Button("关闭调试页面")) {
        this->OpenElementDebugWindow = false;
    }

    //关闭窗口
    ImGui::End();
    return;
}

//ElementBase，Create和Get已在头文件中实现

std::vector<std::shared_ptr<ElementBase>>::iterator ElementManager::Element_GetIterator(const std::string_view Name) {
    return std::find_if(Elements.begin(), Elements.end(),
        [&Name](const std::shared_ptr<ElementBase>& elem) {
            return elem->Name == Name;
        });
}
bool ElementManager::Element_SaveAll() {
    //检查是否有元素
    if (this->Elements.empty()) {
        this->CSATi->IDebugger().AddWarning("当前没有任何元素，跳过保存操作！");
        return true;
    }
    std::filesystem::path ElementFolderPath = this->CSATi->IPCer().PathGet_CurrentElements();
    //遍历所有元素并保存
    for (const auto& elem : this->Elements) {
        std::string Ruselt;
        if (elem->SaveToXML(ElementFolderPath, Ruselt)) {
            this->CSATi->IDebugger().AddSucc(Ruselt);
        }
        else {
            this->CSATi->IDebugger().AddError(Ruselt);
            return false;
        }
    }
    this->CSATi->IDebugger().AddSucc("成功保存所有元素到XML文件！");
    return true;
}
bool ElementManager::Element_LoadFromXML_Pre(const std::string& XMLName, const std::filesystem::path& FolderPath) {
    //检查文件路径和名称存在性
    if (FolderPath.empty() || XMLName.empty()) {
        this->CSATi->IDebugger().AddError("文件夹路径或XML文件名为空，无法从XML文件加载元素！");
        return false;
    }
    //拼接完整路径
    std::filesystem::path FullPath = FolderPath / (XMLName + ".xml");
    
    return this->Element_LoadFromXML_Pre(FullPath);
}
bool ElementManager::Element_LoadFromXML_Pre(const std::filesystem::path& FullPath) {
    this->CSATi->IDebugger().AddInfo("尝试从XML文件加载元素，文件路径：" + FullPath.string());
    //检查文件本身存在性
    if (!std::filesystem::exists(FullPath)) {
        this->CSATi->IDebugger().AddError("XML文件不存在！文件路径：" + FullPath.string());
        return false;
    }

    //创建临时XML文件
    pugi::xml_document NewXML;
    pugi::xml_parse_result result = NewXML.load_file(FullPath.c_str());

    //检验XML文件加载结果
    if (!result) {
        this->CSATi->IDebugger().AddError("尝试从XML文件加载元素失败，无法加载XML文件！ 文件路径：" + FullPath.string() +
            "\n     错误描述：" + result.description());
        return false;
    }

    //获取Element节点
    pugi::xml_node node_Element = NewXML.child("Element");
    if (!node_Element) {
        this->CSATi->IDebugger().AddError("尝试从XML文件加载元素失败，XML文件格式错误，找不到根节点！ 文件路径：" + FullPath.string());
        return false;
    }

    //获取元素类型
    std::string NewElementTypeString = node_Element.attribute("Type").as_string();
    ElementType NewElementType = ElementType_StringToEnum(NewElementTypeString);
    if (static_cast<int>(NewElementType) <= 0) {
        this->CSATi->IDebugger().AddError("尝试从XML文件加载元素失败，不可加载的元素类型！");
        return false;
    }

    //获取元素名称
    std::string NewElementName = node_Element.attribute("Name").as_string();
    //检查元素名是否为空
    if (NewElementName.empty()) {
        this->CSATi->IDebugger().AddError("尝试从XML文件加载元素失败，元素名称为空！");
        return false;
    }
    //检查是否存在同名元素
    if (this->Element_Get<ElementBase>(NewElementName)) {
        this->CSATi->IDebugger().AddError("元素名已占用，无法从XML文件加载元素！ 元素名：" + NewElementName);
        return false;
    }
    //检查ElementMain节点是否存在，确保加载函数可以直接获取
    pugi::xml_node node_ElementMain = node_Element.child("ElementMain");
    if (!node_ElementMain) {
        this->CSATi->IDebugger().AddError("尝试从XML文件加载元素失败，找不到ElementMain节点！ 文件路径：" + FullPath.string());
        return false;
    }



    //创建基类指针
    std::shared_ptr<ElementBase> pElement = nullptr;
    this->CSATi->IDebugger().AddInfo("尝试进行分发，元素类型为 " + NewElementName + " ，文件路径：" + FullPath.string());
    //分发到具体类型的加载函数
    switch (NewElementType) {
    case ElementType::FreeCameraPath:
        pElement = std::make_shared<FreeCameraPath>(std::move(NewElementName));
        break;
    case ElementType::FirstPersonCameraPath:
        pElement = std::make_shared<FirstPersonCameraPath>(std::move(NewElementName));
        break;
    case ElementType::LockedCameraPath:
        break;
    case ElementType::ElementBase:
        break;
    case ElementType::None:
        break;
    }

	//判空
    if (!pElement) {
        this->CSATi->IDebugger().AddError("尝试从XML文件加载元素失败，无法创建指定类型的元素实例！ 元素类型：" + NewElementTypeString);
        return false;
	}
	//设置元素类型
	pElement->Type = NewElementType;
	//重设名称
	pElement->ResetName(NewElementName);
    //统一加载信息
    std::string strRuselt;
    if (pElement->ReadElementMain(node_ElementMain, strRuselt)) {
        pElement->Refresh();
        this->CSATi->IDebugger().AddSucc(std::move(strRuselt));
        this->Elements.push_back(std::move(pElement));
        return true;
    }
    else {
        this->CSATi->IDebugger().AddError(std::move(strRuselt));
        return false;
    }
}
bool ElementManager::Element_Delete(const std::string& Name) {
    // 安全检查
    if (Name.empty()) {
        this->CSATi->IDebugger().AddError("尝试删除空名称的元素！");
        return false;
    }

    //获取迭代器
    auto it = this->Element_GetIterator(Name);
    //判空
    if (it == this->Elements.end()) {
        this->CSATi->IDebugger().AddError("未找到指定名称的元素：" + Name);
        return false;
    }

    //检查是否正在预览此元素
    if (this->Preview_CurrentElement && this->Preview_CurrentElement->Name == Name) {
        this->Preview_Disable(); // 禁用预览
    }

    //检查是否当前正在操作此元素
    if (this->CurrentElement && this->CurrentElement->Name == Name) {
        this->CurrentElement = nullptr;
        this->CurrentElementName = "";
        this->NeedUpdateCurrentElement = true;
    }

    //先标记为需要清理
    it->get()->NeedBeDelete = true;
    //通过迭代器删除元素
    Elements.erase(it);
    //添加刷新信息
    this->SManager->NeedRefresh = true;

    this->CSATi->IDebugger().AddSucc("成功删除元素：" + Name);
    return true;
}
bool ElementManager::Element_ClearAll() {
    //检查是否有元素
    if (this->Elements.empty()) {
        this->CSATi->IDebugger().AddWarning("当前没有任何元素，跳过清空操作！");
        return true;
    }
    //禁用预览
    this->Preview_Disable();
    this->Preview_CurrentElement = nullptr;
    //清空当前操作元素
    this->CurrentElement = nullptr;
    this->CurrentElementName = "";
    this->NeedUpdateCurrentElement = true;
	//把所有元素标记为需要清理并从Elements中释放
    for (std::shared_ptr<ElementBase> elem : this->Elements) {
		elem->NeedBeDelete = true;
    }
    this->Elements.clear();
    //添加刷新信息
    this->SManager->NeedRefresh = true;
    this->CSATi->IDebugger().AddSucc("成功清空所有元素！");
    return true;
}
void ElementManager::Element_ShowInLine(const std::shared_ptr<ElementBase> const element) {
    if (!element) {
        this->CSATi->IDebugger().AddError("元素指针为空，无法展示信息！");
        return;
    }
    ImGui::Text("|元素名称：");
	ImGui::SameLine();
    if (ImGui::Button(element->Name.data())) {
		this->CurrentElementName = element->Name;
        this->NeedUpdateCurrentElement = true;
        this->OpenElementDebugWindow = true;
    }
    ImGui::SameLine();
    ImGui::Text(("   元素类型：" + element->TypeGet_String() + "   持续时长：" + std::to_string(element->DurationTime)).c_str());
}
void ElementManager::Element_ShowAll() {
    size_t Size = this->Elements.size();
    if (Size == 0) {
        this->CSATi->IDebugger().AddError("没有找到任何元素正存储在内存中！");
        return;
    }
    this->CSATi->IDebugger().AddInfo(Line_);
    for (size_t i = 0; i < Size; ++i) {
        this->CSATi->IDebugger().AddInfo(" |元素编号：" + std::to_string(i) + "   元素名称：" + this->Elements.at(i).get()->Name);
    }
    this->CSATi->IDebugger().AddInfo(Line_);
}
void ElementManager::Element_ShowMsgToDebugMenu(const std::shared_ptr<ElementBase> const element)const {
    this->CSATi->IDebugger().AddInfo(Line_);
    this->CSATi->IDebugger().AddInfo("元素名称：" + element->Name);
    this->CSATi->IDebugger().AddInfo("元素类型：" + element->TypeGet_String());
    this->CSATi->IDebugger().AddInfo("持续时长：" + std::to_string(element->DurationTime));
    this->CSATi->IDebugger().AddInfo("详细信息：");

    this->CSATi->IDebugger().AddInfo(element->GetMsg());
    this->CSATi->IDebugger().AddInfo(Line_);
}
std::vector<std::string> ElementManager::Element_GetNames()const {
    std::vector<std::string> ElementsNames;
    if (this->Elements.empty())return ElementsNames;
    ElementsNames.reserve(this->Elements.size());
    for (size_t i = 0; i < this->Elements.size(); ++i) {
        ElementsNames.push_back(this->Elements[i]->Name);
    }
    return ElementsNames;
}



//预览相关
void ElementManager::Preview_Enable() {
    if (!this->Preview_CurrentElement) {
        this->CSATi->IDebugger().AddError("无法开启预览：未设置预览元素！");
        return;
    }
    this->OnPreview = true;
    this->CSATi->IDebugger().AddInfo("已开启预览");
}
void ElementManager::Preview_Disable() {
    this->OnPreview = false;
    this->CSATi->IDebugger().AddInfo("已关闭预览");
}
void ElementManager::Preview_SetElement(const std::string& Name) {
    std::shared_ptr<ElementBase>element = this->Element_Get<ElementBase>(Name);
    if (!element) {
        this->CSATi->IDebugger().AddError("找不到目标元素   元素名：" + Name);
        return;
    }
    this->Preview_CurrentElement = element;
    this->CSATi->IDebugger().AddInfo("准备预览该元素   元素名：" + Name);
}
void ElementManager::Preview_SetPreviewSchema(const float Time) {
    this->Preview_TimeSchema = Time;
    this->CSATi->IDebugger().AddInfo("元素预览时间偏移设置为：" + std::to_string(this->Preview_TimeSchema));
    this->Preview_EndTime = this->Preview_CurrentElement->StartTime + this->Preview_CurrentElement->DurationTime;
}
bool ElementManager::Preview_Call(CSATMath::Frame& Frame, const float Time) {
	if (!this->OnPreview)return false;
    if (this->Preview_CurrentElement) {
        float SchemaedTime = Time - this->Preview_TimeSchema;
        if (SchemaedTime < 0.0f || SchemaedTime > this->Preview_CurrentElement->DurationTime) {
            this->Preview_Disable();
            return false;
        }
        return this->Preview_CurrentElement->Call(Frame, SchemaedTime, PlaybackMode::Reuse);
    }
    else {
        this->Preview_Disable();
        return false;
    }
}
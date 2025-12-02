#include"ProjectManager.hpp"

#include"../../Elements/ElementManager/ElementManager.hpp"
#include"../../Solutions/SolutionManager/SolutionManager.hpp"

#include"../../../IPCer/IPCer.hpp"
#include"../../../Debugger/IDebugger.hpp"
#include"../../../KeyTracker/KeyTracker.hpp"
#include"../../../MessageManager/IMessageManager.hpp"

#include"../../../../ThirdParty/All_pugixml.hpp"
#include"../../../../ThirdParty/All_ImGui.hpp"



//构造与析构函数

ProjectManager::ProjectManager() {
	this->Buffer_KCPack = new KeyCheckPack();
}
ProjectManager::~ProjectManager() {
	delete this->Buffer_KCPack;
}

//项目管理器基本函数

void ProjectManager::Init(CSATiManager* CSATi, ElementManager* EManager, SolutionManager* SManager) {
	//核心服务
	this->CSATi = CSATi;
	//系统服务
	this->EManager = EManager;
	this->SManager = SManager;
	//其它服务
	this->KeyTracker = &this->CSATi->KT();
	return;
}
void ProjectManager::VirtualMain() {
	this->Traversal();
	return;
}
void ProjectManager::Traversal() {
	if (!this->Config.ProjectShortcutEnable)return;
	for (const auto& Project : this->Projects) {
		if (this->KeyTracker->CheckWithPack(Project->KCPack)) {
			this->Project_Apply(Project);
		}
	}
}


std::vector<std::shared_ptr<Project>>::iterator ProjectManager::Project_GetIterator(const std::string& Name) {
	return std::find_if(this->Projects.begin(), this->Projects.end(),
		[&Name](const std::shared_ptr<Project>& project) {
			return project->Name == Name;
		});
}
std::shared_ptr<Project> ProjectManager::Project_Get(const std::string& Name) {
	auto it = this->Project_GetIterator(Name);
	if (it == this->Projects.end()) {
		return nullptr;
	}
	return (*it);
}
bool ProjectManager::Project_Delete(const std::string& Name) {
	std::vector<std::shared_ptr<Project>>::iterator it = this->Project_GetIterator(Name);
	if (it == this->Projects.end()) {
		return true;
	}
	this->Projects.erase(it);
	return true;
}
bool ProjectManager::Project_ClearAll() {
	this->Projects.clear();
	this->ActiveProject = nullptr;
	return true;
}



bool ProjectManager::Project_Create(const std::string& Name) {
	//检查是否已存在同名项目
	if (this->Project_Get(Name)) {
		this->CSATi->IDebugger().AddError("项目名已占用！ 项目名：" + Name);
		return false;
	}
	//创建项目指针
	std::shared_ptr<Project> CreateProject = std::make_shared<Project>(Name);
	CreateProject->Refresh();
	//执行路径操作
	if (!this->CSATi->IPCer().PathCreate_Project(Name))return false;
	//添加进项目组
	this->Projects.push_back(std::move(CreateProject));
	this->CSATi->IDebugger().AddSucc("成功创建项目：" + Name);
	return true;
}
bool ProjectManager::Project_Refresh() {
	//先验证有没有活跃项目
	if (!this->ActiveProject) {
		return false;
	}
	////获取元素名称列表
	//this->ActiveProject->ElementNames = std::move(this->EManager->Element_GetNames());
	////获取解决方案名称列表
	//this->ActiveProject->SolutionNames = std::move(this->SManager->Solution_GetNames());
	this->ActiveProject->Refresh();
	//刷新完毕
	return true;
}
bool ProjectManager::Project_Save() {
	if (!this->Project_Refresh()) {
		return false;
	}
	//保存所有元素和解决方案到磁盘
	this->EManager->Element_SaveAll();
	this->SManager->Solution_SaveAll();
	//保存项目到磁盘
	std::filesystem::path Path = this->CSATi->IPCer().PathGet_CurrentWorkspace() / this->ActiveProject->Name;
	std::string strRuselt;
	if (this->ActiveProject->SaveToXML(Path, strRuselt)) {
		this->CSATi->IDebugger().AddSucc(std::move(strRuselt));
		return true;
	}
	else {
		this->CSATi->IDebugger().AddError(std::move(strRuselt));
		return false;
	}
}
//bool ProjectManager::Project_SaveAll() {
//	if (this->Projects.empty()) {
//		this->CSATi->IDebugger().AddWarning("尝试在没有任何项目的情况下保存");
//		return true;
//	}
//	
//	//遍历所有项目保存
//	for (const auto& Project : this->Projects) {
//		std::filesystem::path Path = this->CSATi->IPCer().PathGet_CurrentWorkspace() / Project->Name;
//		std::string Ruselt;
//		if (Project->SaveToXML(Path, Ruselt)) {
//			this->CSATi->IDebugger().AddSucc(Ruselt);
//		}
//		else {
//			this->CSATi->IDebugger().AddError(Ruselt);
//			return false;
//		}
//	}
//	this->CSATi->IDebugger().AddSucc("成功保存所有项目到XML文件！");
//	return true;
//}
bool ProjectManager::Project_Apply(const std::shared_ptr<Project> Project) {
	//先尝试保存当前活跃项目
	this->Project_Save();
	//切换项目
	this->ActiveProject = Project;
	//清空旧解决方案，防止冲突
	this->SManager->Solution_ClearAll();
	//清空旧元素，防止冲突
	this->EManager->Element_ClearAll();
	if (!this->CSATi->IPCer().PathSet_Project(Project->Name)) {
		this->CSATi->IDebugger().AddError("尝试切换到项目时出现问题，设置项目文件夹路径失败！");
		return false;
	}
	//获取元素文件夹路径
	std::filesystem::path ElementsPath = this->CSATi->IPCer().PathGet_CurrentElements();
	std::vector<std::string>ElementXMLs = this->CSATi->IPCer().GetFileNamesByPath(ElementsPath);
	//遍历加载元素
	for (const std::string& ElementXML : ElementXMLs) {
		this->EManager->Element_LoadFromXML_Pre(ElementsPath / ElementXML);
	}
	//获取解决方案文件夹路径
	std::filesystem::path SolutionsPath = this->CSATi->IPCer().PathGet_CurrentSolutions();
	std::vector<std::string>SolutionXMLs = this->CSATi->IPCer().GetFileNamesByPath(SolutionsPath);
	//遍历加载解决方案
	for (const std::string& SolutionXML : SolutionXMLs) {
		this->SManager->Solution_LoadFromXML(SolutionsPath / SolutionXML);
	}
	this->ActiveProject = Project;
	this->CSATi->IDebugger().AddSucc("已切换至项目" + Project->Name);
	this->CSATi->IDebugger().AddSucc("尝试加载元素总数：" + std::to_string(ElementXMLs.size()));
	this->CSATi->IDebugger().AddSucc("尝试加载解决方案总数：" + std::to_string(SolutionXMLs.size()));
	this->CSATi->IDebugger().AddSucc("成功加载元素总数：" + std::to_string(this->EManager->Elements.size()));
	this->CSATi->IDebugger().AddSucc("成功加载解决方案总数：" + std::to_string(this->SManager->Solutions.size()));
	return true;
}
bool ProjectManager::Project_LoadFromXML(const std::filesystem::path& ProjectPath, const std::string& XMLName) {
	//检查文件路径和名称存在性
	if (ProjectPath.empty() || XMLName.empty()) {
		this->CSATi->IDebugger().AddError("文件夹路径或XML文件名为空，无法从XML文件加载项目！");
		return false;
	}

	//拼接完整路径
	std::filesystem::path FullPath = ProjectPath / (XMLName + ".xml");

	//输出调试信息
	this->CSATi->IDebugger().AddInfo("尝试从XML文件加载项目，文件路径：" + FullPath.string());

	//检查文件本身存在性
	if (!std::filesystem::exists(FullPath)) {
		this->CSATi->IDebugger().AddError("XML文件不存在！文件路径：" + FullPath.string());
		return false;
	}

	//创建临时XML文件
	pugi::xml_document LoadXML;

	//打开XML文件并检验结果
	pugi::xml_parse_result result = LoadXML.load_file(FullPath.c_str());
	if (!result) {
		this->CSATi->IDebugger().AddError("尝试从XML文件加载项目失败，无法加载XML文件！ 文件路径：" + FullPath.string()
			+ "\n错误描述：" + result.description());
		return false;
	}
	 
	//开始获取信息
	//获取Project节点
	pugi::xml_node node_Project = LoadXML.child("Project");
	if (!node_Project) {
		this->CSATi->IDebugger().AddError("尝试从XML文件加载项目失败，XML文件格式错误，找不到根节点！ 文件路径：" + FullPath.string());
		return false;
	}
	//获取项目名称
	std::string LoadProjectName = node_Project.attribute("Name").as_string();
	if (LoadProjectName.empty()) {
		this->CSATi->IDebugger().AddError("尝试从XML文件加载项目失败，项目名称为空！");
		return false;
	}
	//检查是否存在同名项目
	if (this->Project_Get(LoadProjectName)) {
		this->CSATi->IDebugger().AddError("项目名已占用，无法从XML文件加载项目！ 项目名：" + std::move(LoadProjectName));
		return false;
	}
	//获取KeyCheckPack节点
	pugi::xml_node node_KeyCheckPack = node_Project.child("KeyCheckPack");
	if (!node_KeyCheckPack) {
		this->CSATi->IDebugger().AddError("尝试从XML文件加载项目失败，找不到KeyCheckPack节点！ 文件路径：" + FullPath.string());
		return false;
	}
	//制作项目
	std::shared_ptr<Project>LoadProject = std::make_shared<Project>(LoadProjectName);

	//加载KeyCheckPack
	if (!LoadProject->KCPack.ReadXMLNode(node_KeyCheckPack)) {
		this->CSATi->IDebugger().AddError("尝试从XML文件加载项目失败，在解析KeyCheckPack节点时发生了错误！ 文件路径：" + FullPath.string());
		return false;
	}

	//获取Config节点
	pugi::xml_node node_Config = node_Project.child("Config");
	if (!node_Config) {
		this->CSATi->IDebugger().AddError("找不到Config节点！ 项目名：" + std::move(LoadProjectName));
		return false;
	}
	//获取AutoPlay节点
	pugi::xml_node node_AutoPlay = node_Config.child("AutoPlay");
	if (!node_AutoPlay) {
		this->CSATi->IDebugger().AddError("找不到AutoPlay节点！ 项目名：" + std::move(LoadProjectName));
		return false;
	}
	//获取OnNewRound节点
	pugi::xml_node node_OnNewRound = node_AutoPlay.child("OnNewRound");
	if (!node_OnNewRound) {
		this->CSATi->IDebugger().AddError("找不到OnNewRound节点！ 项目名：" + std::move(LoadProjectName));
		return false;
	}
	//获取OnNewRound信息
	for (const auto& node_Solution : node_OnNewRound.children("Solution")) {
		//获取解决方案名称
		std::string LoadSolutionName = node_Solution.attribute("Name").as_string();
		LoadProject->OnNewRound.push_back(std::move(LoadSolutionName));
	}
	//获取OnRoundStart节点
	pugi::xml_node node_OnRoundStart = node_AutoPlay.child("OnRoundStart");
	if (!node_OnRoundStart) {
		this->CSATi->IDebugger().AddError("找不到OnRoundStart节点！ 项目名：" + std::move(LoadProjectName));
		return false;
	}
	//获取OnRoundStart信息
	for (const auto& node_Solution : node_OnRoundStart.children("Solution")) {
		//获取解决方案名称
		std::string LoadSolutionName = node_Solution.attribute("Name").as_string();
		LoadProject->OnRoundStart.push_back(std::move(LoadSolutionName));
	}
	//获取OnRoundEnd节点
	pugi::xml_node node_OnRoundEnd = node_AutoPlay.child("OnRoundEnd");
	if (!node_OnRoundEnd) {
		this->CSATi->IDebugger().AddError("找不到OnRoundEnd节点！ 项目名：" + std::move(LoadProjectName));
		return false;
	}
	//获取OnRoundEnd信息
	for (const auto& node_Solution : node_OnRoundEnd.children("Solution")) {
		//获取解决方案名称
		std::string LoadSolutionName = node_Solution.attribute("Name").as_string();
		LoadProject->OnRoundEnd.push_back(std::move(LoadSolutionName));
	}
	
	LoadProject->Refresh();
	this->CSATi->IDebugger().AddSucc("成功从XML文件加载项目：" + LoadProjectName);

	//添加进项目组
	this->Projects.push_back(std::move(LoadProject));
	return true;
}



void ProjectManager::Project_ShowMsg(const std::string& Name) {
	std::shared_ptr<Project> Project = this->Project_Get(Name);
	if (!Project) {
		return;
	}
	//判断是否需要更新
	if (Project == this->ActiveProject) {
		//如果操作项目和活跃项目是同一个项目，则需要刷新
		this->Project_Refresh();
	}
	this->CSATi->IDebugger().AddInfo(Line_);
	this->CSATi->IDebugger().AddInfo(this->ControllingProject->GetMsg());
	this->CSATi->IDebugger().AddInfo(Line_);
	return;
}
void ProjectManager::Project_ShowAll() {
	//判断是否有项目存在
	if (!this->Projects.empty())return;
	//先尝试更新活跃项目
	this->Project_Refresh();
	//隔离线
	this->CSATi->IDebugger().AddInfo(Line_);
	this->CSATi->IDebugger().AddInfo(Line_);
	this->CSATi->IDebugger().AddInfo(Line_);
	//依次展示所有项目信息
	for (const auto& Project : this->Projects) {
		this->CSATi->IDebugger().AddInfo(Project->GetMsg());
		this->CSATi->IDebugger().AddInfo(Line_);//隔离线
	}
	//隔离线
	this->CSATi->IDebugger().AddInfo(Line_);
	this->CSATi->IDebugger().AddInfo(Line_);

	return;
}
void ProjectManager::Project_ShowInLine(std::shared_ptr<Project> Project) {
	if (!Project) {
		this->CSATi->IDebugger().AddError("项目指针为空，无法展示信息！");
		return;
	}
	ImGui::Text("|项目名称：");
	ImGui::SameLine();
	if (ImGui::Button(Project->Name.c_str())) {
		this->ControllingProject = Project;
		this->OpenProjectDebugWindows = true;
	}
	ImGui::SameLine();
}
void ProjectManager::Project_ShowAllInLines() {
	//使用迭代器遍历所有项目
	for (const auto& Project : this->Projects) {
		this->Project_ShowInLine(Project);
	}
}



void ProjectManager::Windows() {
	if (this->OpenProjectDebugWindows) {
		//项目调试窗口
		this->Project_DebugWindowWithMenu();
		if (this->OpenProjectKCPackDebugWindow) {
			//项目按键绑定调试窗口
			this->Project_KCPack_DebugWindowWithMenu();
		}
		if (this->OpenProjectNameDebugWindow) {
			//项目名称调试窗口
			this->Project_Name_DebugWindow();
		}
	}
	return;
}

void ProjectManager::Project_DebugWindowWithMenu() {
	//打开窗口
	ImGui::Begin("项目调试", &this->OpenProjectDebugWindows);
	//检查当前是否操作解决方案
	if (this->ControllingProject) {
		ImGui::Text("当前操作项目：");
		ImGui::SameLine();
		ImGui::Text(this->ControllingProject->Name.c_str());
		if (ImGui::Button("切换到当前项目")) {
			this->Project_Apply(this->ControllingProject);
		}
		if (ImGui::Button("打印项目信息到调试窗口")) {
			this->Project_ShowMsg(this->ControllingProject->Name);
		}
		if (ImGui::Button("卸载当前项目")) {
			this->Project_Delete(this->ControllingProject->Name);
			this->OpenProjectDebugWindows = false;
			ImGui::End();
			return;
		}
		if (ImGui::Button("修改按键绑定")) {
			*this->Buffer_KCPack = this->ControllingProject->KCPack;//缓存
			this->OpenProjectKCPackDebugWindow = true;//打开窗口
		}
		if (ImGui::Button("修改项目名称")) {
			this->Buffer_Name = this->ControllingProject->Name;//缓存
			this->OpenProjectNameDebugWindow = true;//打开窗口
		}
		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::TreeNode("进入新回合")) {
			if (!this->ControllingProject->OnNewRound.empty()) {
				for (const std::string SolutionName : this->ControllingProject->OnNewRound) {
					ImGui::Text(SolutionName.c_str());
					ImGui::SameLine();
					if(ImGui::Button((std::string("删除##OnNewRound") + SolutionName).c_str())){
						auto it = std::find(this->ControllingProject->OnNewRound.begin(), this->ControllingProject->OnNewRound.end(), SolutionName);
						if (it != this->ControllingProject->OnNewRound.end()) {
							this->ControllingProject->OnNewRound.erase(it);
						}
					}
				}
			}
			else {
				ImGui::Text("空");
			}
			


			ImGui::TreePop();
		}
		if (ImGui::TreeNode("回合结束")) {
			if (!this->ControllingProject->OnRoundEnd.empty()) {
				for (const std::string SolutionName : this->ControllingProject->OnRoundEnd) {
					ImGui::Text(SolutionName.c_str());
				}
			}
			else {
				ImGui::Text("空");
			}


			ImGui::TreePop();
		}
		
	}
	else {
		ImGui::Text("当前未选择任何项目");
	}
	if (ImGui::Button("关闭项目调试页面")) {
		this->OpenProjectDebugWindows = false;
	}
	ImGui::End();
}
void ProjectManager::Project_KCPack_DebugWindowWithMenu() {
	ImGui::Begin("按键绑定##Project", &this->OpenProjectKCPackDebugWindow);

	if (this->ControllingProject) {
		ImGui::Text(("当前绑键：" + this->ControllingProject->KCPack.GetMsg()).c_str());
		ImGui::Separator();

		// 修饰键复选框
		ImGui::Checkbox("Ctrl", &this->Buffer_KCPack->Ctrl);
		ImGui::SameLine();
		ImGui::Checkbox("Shift", &this->Buffer_KCPack->Shift);
		ImGui::SameLine();
		ImGui::Checkbox("Alt", &this->Buffer_KCPack->Alt);

		// 按键选择下拉菜单
		static const char* keyItems[] = {
			"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
			"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
			"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12"
		};

		// 查找当前键码对应的索引
		int currentKeyIndex = 0;
		for (int i = 0; i < IM_ARRAYSIZE(keyItems); i++) {
			if (i < 26) { // A-Z
				if (this->Buffer_KCPack->vkCode == 0x41 + i) {
					currentKeyIndex = i;
					break;
				}
			}
			else { // F1-F12
				if (this->Buffer_KCPack->vkCode == 0x70 + (i - 26)) {
					currentKeyIndex = i;
					break;
				}
			}
		}

		ImGui::Text("按键:");
		ImGui::SameLine();
		if (ImGui::Combo("##ProjectKey", &currentKeyIndex, keyItems, IM_ARRAYSIZE(keyItems))) {
			// 更新键码
			if (currentKeyIndex < 26) {
				this->Buffer_KCPack->vkCode = 0x41 + currentKeyIndex; // A-Z
			}
			else {
				this->Buffer_KCPack->vkCode = 0x70 + (currentKeyIndex - 26); // F1-F12
			}
		}

		// 连击数输入
		int comboClick = static_cast<int>(this->Buffer_KCPack->ComboClick);
		ImGui::Text("连击数:");
		ImGui::SameLine();
		if (ImGui::InputInt("##ProjectComboClick", &comboClick, 1, 5)) {
			// 限制在 1-255 范围内
			comboClick = std::clamp(comboClick, 1, 255);
			this->Buffer_KCPack->ComboClick = static_cast<unsigned char>(comboClick);
		}

		if (ImGui::Button("修改项目绑键")) {
			this->ControllingProject->KCPack = *this->Buffer_KCPack;
			this->ControllingProject->KCPack.Refresh();
			this->OpenProjectKCPackDebugWindow = false;
			ImGui::End();
			return;
		}
	}
	else {
		ImGui::Text("当前没有要调试的项目，无法修改按键绑定");
	}

	ImGui::Separator();

	if (ImGui::Button("关闭项目按键绑定调试页面")) {
		this->OpenProjectKCPackDebugWindow = false;
		ImGui::End();
		return;
	}

	ImGui::End();
}
void ProjectManager::Project_Name_DebugWindow() {
	ImGui::Begin("项目重命名", &this->OpenProjectNameDebugWindow);

	if (this->ControllingProject) {
		ImGui::Text(("当前名称：" + this->ControllingProject->Name).c_str());
		ImGui::Separator();

		ImGui::Text("新项目名:");
		ImGui::SameLine();
		ImGui::InputText("##NewName", &this->Buffer_Name);

		//检查是否已经存在该名称
		if (this->Project_Get(this->Buffer_Name)) {
			ImGui::Text("该名称已经被占用");
		}
		else {//没有被使用才允许修改
			if (ImGui::Button("修改名称")) {
				this->ControllingProject->ResetName(this->Buffer_Name);
				this->OpenProjectNameDebugWindow = false;
				ImGui::End();
				return;
			}
		}
	}
	else {
		ImGui::Text("当前没有要调试的项目，无法修改项目名");
	}

	ImGui::Separator();

	if (ImGui::Button("关闭项目名调试页面")) {
		this->OpenProjectNameDebugWindow = false;
		ImGui::End();
		return;
	}

	ImGui::End();
}

//信息接口

const std::vector<std::string>* ProjectManager::Active_GetRoundStart() {
	if (!this->ActiveProject) {
		return nullptr;
	}
	return &this->ActiveProject->OnRoundStart;
}
const std::vector<std::string>* ProjectManager::Active_GetRoundEnd() {
	if (!this->ActiveProject) {
		return nullptr;
	}
	return &this->ActiveProject->OnRoundEnd;
}

bool ProjectManager::Playing_AutoCall(const CSATMessage& Msg) {
	this->CSATi->IDebugger().AddInfo("项目管理器正在处理消息！");
	if (!this->ActiveProject) {
		this->CSATi->IDebugger().AddWarning("无活跃项目，无法执行自动操作");
		return false;
	}
	switch (Msg.Type) {
	case MsgType::Game_NewRound: {
		const std::vector<std::string>& const OnNewRound = this->ActiveProject->OnNewRound;
		if (OnNewRound.empty()) {
			this->CSATi->IDebugger().AddWarning("无新回合解决方案可尝试调用");
			return false;
		}
		int temp = rand() % OnNewRound.size();
		return this->SManager->Playing_SetSolution(OnNewRound[temp], PlaybackMode::Reuse);
	}
	case MsgType::Game_RoundEnd: {
		const std::vector<std::string>& const OnEnd = this->ActiveProject->OnRoundEnd;
		if (OnEnd.empty()) {
			this->CSATi->IDebugger().AddWarning("无回合结束解决方案可尝试调用");
			return false;
		}
		int temp = rand() % OnEnd.size();
		return this->SManager->Playing_SetSolution(OnEnd[temp], PlaybackMode::Reuse);
	}
	}
	return false;
}
#include"IPCer.hpp"
#include"../CSATiManager.hpp"
#include"../Debugger/IDebugger.hpp"

#include<strstream>

bool IPCer::Init() {
    HWND hwnd = FindWindowW(NULL, L"Counter-Strike:Acute's Toolkit");
    if (!hwnd) {
        this->CSATi->IDebugger().AddError("没有找到窗口");
        return false; // 窗口未找到
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == 0) {
        this->CSATi->IDebugger().AddError("进程ID未找到");
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!hProcess) {
        this->CSATi->IDebugger().AddError("无法打开进程");
        return false;
    }

    WCHAR exePath[MAX_PATH] = { 0 };
    DWORD size = MAX_PATH;

    // 获取可执行文件路径
    if (!QueryFullProcessImageNameW(hProcess, 0, exePath, &size)) {
        this->CSATi->IDebugger().AddError("得到可执行文件路径失败");
        return false;
    }

    CloseHandle(hProcess);

     //设置CSAT.exe路径
    this->Paths.CSAT.CSAT_exe.Path = exePath;
    // 获取CSAT目录
    this->Paths.CSAT.Path = this->PathGet_CSAT_exe().parent_path();
    this->Paths.CSAT.CSATConfig.Path = this->PathGet_CSAT() / "CSATConfig";
    this->Paths.CSAT.DLLToCS.Path = this->PathGet_CSAT() / "DLLToCS";
    this->Paths.CSAT.DLLToCS.CSATDLL_dll.Path = this->PathGet_DLLToCS() / "CSATDLL.dll";
    this->Paths.CSAT.Saves.Path = this->PathGet_CSAT() / "Saves";
    this->Paths.CSAT.Saves.Internal.Path = this->PathGet_Saves() / "Internal";
    this->Paths.CSAT.Saves.Internal.Core.Path = this->PathGet_Internal() / "Core";
    this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Path = this->PathGet_Core() / "CameraSystem";
	this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Path = this->PathGet_CameraSystem() / "Workspaces";
	this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Path = this->PathGet_Workspaces() / "DefaultWorkspace";
    //this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Elements.Path = this->PathGet_CurrentWorkspace() / "Elements";
    //this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Solutions.Path = this->PathGet_CurrentWorkspace() / "Solutions";
    //this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Projects.Path = this->PathGet_CurrentWorkspace() / "Projects";


    this->CSATi->IDebugger().AddSucc("IPCer初始化成功!");
    this->Inited = true;
	return true;
}

bool IPCer::PathCreate_Workspace(const std::string& NewWorkspaceName) {
    if (NewWorkspaceName.empty()) {
        this->CSATi->IDebugger().AddError("工作区名称不能为空，无法创建工作区路径！");
        return false;
    }
	//拼接文件夹路径
    std::filesystem::path NewWorkspacePath = this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Path / NewWorkspaceName;
	//检验文件夹是否已存在
    if (std::filesystem::exists(NewWorkspacePath)) {
        this->CSATi->IDebugger().AddError("指定的工作区文件夹已存在，无法创建新的工作区文件夹！  路径：" + NewWorkspacePath.string());
        return false;
    }
    //创建文件夹
    try {
        std::filesystem::create_directory(NewWorkspacePath);
        //创建子文件夹
        //std::filesystem::create_directory(NewWorkspacePath / "Elements");
        //std::filesystem::create_directory(NewWorkspacePath / "Solutions");
        //std::filesystem::create_directory(NewWorkspacePath / "Projects");
    }
    catch (const std::filesystem::filesystem_error& e) {
        this->CSATi->IDebugger().AddError("创建工作区文件夹失败，错误信息：" + std::string(e.what()));
        return false;
    }
    this->CSATi->IDebugger().AddSucc("成功创建工作区文件夹，路径：" + NewWorkspacePath.string());
	return true;
}

bool IPCer::PathSet_Workspace(const std::string& NewWorkspaceName) {
    if (NewWorkspaceName.empty()) {
        this->CSATi->IDebugger().AddError("工作区名称不能为空，无法设置工作区路径！");
        return false;
    }
    //检验文件夹存在
	std::filesystem::path NewWorkspacePath = this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Path / NewWorkspaceName;
    if (!std::filesystem::exists(NewWorkspacePath) || !std::filesystem::is_directory(NewWorkspacePath)) {
        this->CSATi->IDebugger().AddError("指定的工作区文件夹不存在，无法设置工作区路径！  路径：" + NewWorkspacePath.string());
        return false;
	}
    this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Path = std::move(NewWorkspacePath);
    //this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Projects.Path = this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Path / "Projects";
    //this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Solutions.Path = this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Path / "Solutions";
    //this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Elements.Path = this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Path / "Elements";
    this->CSATi->IDebugger().AddSucc("成功设置工作区路径为：" + this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Path.string());
    return true;
}

bool IPCer::PathCreate_Project(const std::string& NewProjectName) {
    if (NewProjectName.empty()) {
        this->CSATi->IDebugger().AddError("项目名称不能为空，无法创建项目路径！");
        return false;
    }
    //拼接文件夹路径
    std::filesystem::path NewProjectPath = this->PathGet_CurrentWorkspace() / NewProjectName;
    //检验文件夹是否已存在
    if (std::filesystem::exists(NewProjectPath)) {
        this->CSATi->IDebugger().AddError("指定的项目文件夹已存在，无法创建新的项目文件夹！  路径：" + NewProjectPath.string());
        return false;
    }
    //创建文件夹
    try {
        std::filesystem::create_directory(NewProjectPath);
        //创建子文件夹
        std::filesystem::create_directory(NewProjectPath / "Elements");
        std::filesystem::create_directory(NewProjectPath / "Solutions");
    }
    catch (const std::filesystem::filesystem_error& e) {
        this->CSATi->IDebugger().AddError("创建项目文件夹失败，错误信息：" + std::string(e.what()));
        return false;
    }
    this->CSATi->IDebugger().AddSucc("成功创建项目文件夹，路径：" + NewProjectPath.string());
    return true;
}
bool IPCer::PathSet_Project(const std::string& LoadProjectName) {
    if (LoadProjectName.empty()) {
        this->CSATi->IDebugger().AddError("项目名称不能为空，无法设置项目路径！");
        return false;
    }
    //拼接完整路径
    //检验文件夹存在
    std::filesystem::path LoadProjectPath = this->PathGet_CurrentWorkspace() / LoadProjectName;
    if (!std::filesystem::exists(LoadProjectPath) || !std::filesystem::is_directory(LoadProjectPath)) {
        this->CSATi->IDebugger().AddError("指定的项目文件夹不存在，无法设置项目路径！  路径：" + LoadProjectPath.string());
        return false;
    }
    this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Project.Path = std::move(LoadProjectPath);
    //设置子文件夹路径
    this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Project.Solutions.Path = this->PathGet_CurrentProject() / "Solutions";
    this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Project.Elements.Path = this->PathGet_CurrentProject() / "Elements";
    this->CSATi->IDebugger().AddSucc("成功设置项目路径为：" + this->PathGet_CurrentProject().string());
    return true;
}



std::filesystem::path IPCer::PathGet_Workspaces() {
    return this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Path;
}
std::filesystem::path IPCer::PathGet_CurrentWorkspace() {
    return this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Path;
}
std::filesystem::path IPCer::PathGet_CurrentProject() {
    return this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Project.Path;
}
std::filesystem::path IPCer::PathGet_CurrentSolutions() {
    return this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Project.Solutions.Path;
}
std::filesystem::path IPCer::PathGet_CurrentElements() {
	return this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Workspaces.Workspace.Project.Elements.Path;
}


std::filesystem::path IPCer::PathGet_CSAT() {
    return this->Paths.CSAT.Path;
}
std::filesystem::path IPCer::PathGet_CSATConfig() {
    return this->Paths.CSAT.CSATConfig.Path;
}
std::filesystem::path IPCer::PathGet_DLLToCS() {
    return this->Paths.CSAT.DLLToCS.Path;
}
std::filesystem::path IPCer::PathGet_CSATDLL_dll() {
    return this->Paths.CSAT.DLLToCS.CSATDLL_dll.Path;
}
std::filesystem::path IPCer::PathGet_Saves() {
    return this->Paths.CSAT.Saves.Path;
}
std::filesystem::path IPCer::PathGet_Internal() {
    return this->Paths.CSAT.Saves.Internal.Path;
}
std::filesystem::path IPCer::PathGet_Core() {
    return this->Paths.CSAT.Saves.Internal.Core.Path;
}
std::filesystem::path IPCer::PathGet_CameraSystem() {
    return this->Paths.CSAT.Saves.Internal.Core.CameraSystem.Path;
}
std::filesystem::path IPCer::PathGet_CSAT_exe() {
    return this->Paths.CSAT.CSAT_exe.Path;
}



std::string IPCer::GetAllPathMsg() {
	std::ostringstream oss;
	//使用函数按文件夹结构获取路径信息
    oss << "CSAT文件夹路径：" << this->PathGet_CSAT()
        << "\n  CSATConfig文件夹路径：" << this->PathGet_CSATConfig()
        << "\n  DLLToCS文件夹路径：" << this->PathGet_DLLToCS()
        << "\n      CSATDLL.dll路径：" << this->PathGet_CSATDLL_dll()
        << "\n  Saves文件夹路径：" << this->PathGet_Saves()
        << "\n      Internal文件夹路径为：" << this->PathGet_Internal()
        << "\n          Core文件夹路径为：" << this->PathGet_Core()
        << "\n              CameraSystem文件夹路径为：" << this->PathGet_CameraSystem()
		<< "\n                  Workspaces文件夹路径：" << this->PathGet_Workspaces()
		<< "\n                      当前工作区文件夹路径：" << this->PathGet_CurrentWorkspace()
		<< "\n                          当前Projects文件夹路径：" << this->PathGet_CurrentProject()
		<< "\n                              当前Solutions文件夹路径：" << this->PathGet_CurrentSolutions()
		<< "\n                              当前Elements文件夹路径：" << this->PathGet_CurrentElements()
		<< "\n  CSAT.exe路径：" << this->PathGet_CSAT_exe();

	return oss.str();
}

std::vector<std::string> IPCer::GetProjectsNames() {
    std::vector<std::string> FileNames;
    //检查路径存在且为文件夹
    std::filesystem::path Path = this->PathGet_CurrentWorkspace();
    if (!std::filesystem::exists(Path) || !std::filesystem::is_directory(Path)) {
        this->CSATi->IDebugger().AddError("尝试获取当前工作区的所有项目名时出现错误：当前工作区文件夹不存在！");
        return FileNames;
    }
    //遍历目录
    for (const auto& entry : std::filesystem::directory_iterator(Path)) {
        if (entry.is_directory()) {
            std::string FileName = entry.path().filename().string();
            FileNames.push_back(std::move(FileName));
        }
    }
    return FileNames;
}
std::vector<std::string> IPCer::GetFileNamesByPath(std::filesystem::path& FolderPath) {
    std::vector<std::string> FileNames;
    //检查路径存在且为文件夹
    if (!std::filesystem::exists(FolderPath) || !std::filesystem::is_directory(FolderPath)) {
        this->CSATi->IDebugger().AddError("尝试获取指定文件夹内文件名时出现错误：目标文件夹不存在！\n文件夹路径：" + FolderPath.string());
        return FileNames;
    }
    //遍历目录
    for (const auto& entry : std::filesystem::directory_iterator(FolderPath)) {
        if (entry.is_regular_file()) {
            std::string FileName = entry.path().filename().string();
            FileNames.push_back(std::move(FileName));
        }
    }
    return FileNames;
}
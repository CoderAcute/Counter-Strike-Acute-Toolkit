#pragma once

#include"../ModuleBase/ModuleBase.hpp"

#include<filesystem>
#include<string>
#include<Windows.h>

class CSATsPaths {
public:
    //std::filesystem::path GetPath();
    // 主路径结构
    struct S_CSAT {
        std::filesystem::path Path;

        struct S_CSATConfig {
            std::filesystem::path Path;       // CSATConfig文件
        } CSATConfig;

        struct S_DLLToCS {
            std::filesystem::path Path;       // DLLToCS工具目录
            struct S_CSATDLL_dll {
                std::filesystem::path Path;
            }CSATDLL_dll;
        }DLLToCS;

        struct S_Saves {
            std::filesystem::path Path;       // Saves主目录
            struct S_Internal {
                std::filesystem::path Path;
                struct S_Core {
                    std::filesystem::path Path;
                    struct S_CameraSystem {
                        std::filesystem::path Path;
                        struct S_Workspaces {
                            std::filesystem::path Path;
                            struct S_Workspace {
                                std::filesystem::path Path;
                                struct S_Project {
                                    std::filesystem::path Path;
                                    struct S_Elements {
                                        std::filesystem::path Path;   // Elements子目录
                                    } Elements;
                                    struct S_Solutions {
                                        std::filesystem::path Path;   // Solutions子目录
                                    } Solutions;
                                } Project;
                            } Workspace;
                        } Workspaces;
                    } CameraSystem;
                } Core;
            } Internal;
        } Saves;

        struct S_CSAT_exe {
            std::filesystem::path Path;
        } CSAT_exe;

    } CSAT;

};

class IPCer final:public ModuleBase{
	CSATsPaths Paths{};
public:
    IPCer(CSATiManager* CSATi) :ModuleBase(CSATi) {
        this->Type = ModuleType::IPCer;
    }

	bool Inited = false;

	bool Init()override;

    std::filesystem::path PathGet_Workspaces();

	std::filesystem::path PathGet_CurrentWorkspace();
    std::filesystem::path PathGet_CurrentProject();
    std::filesystem::path PathGet_CurrentSolutions();
    std::filesystem::path PathGet_CurrentElements();
	
    std::filesystem::path PathGet_CSAT();
    std::filesystem::path PathGet_CSATConfig();
    std::filesystem::path PathGet_DLLToCS();
    std::filesystem::path PathGet_CSATDLL_dll();
    std::filesystem::path PathGet_Saves();
    std::filesystem::path PathGet_Internal();
    std::filesystem::path PathGet_Core();
    std::filesystem::path PathGet_CameraSystem();
    std::filesystem::path PathGet_CSAT_exe();

    bool PathCreate_Workspace(const std::string& NewWorkspaceName);
	bool PathSet_Workspace(const std::string& NewWorkspaceName);

	bool PathCreate_Project(const std::string& NewProjectName);
	bool PathSet_Project(const std::string& NewProjectName);
	
    std::string GetAllPathMsg();

    std::vector<std::string> GetProjectsNames();
    std::vector<std::string> GetFileNamesByPath(std::filesystem::path& FolderPath);
};
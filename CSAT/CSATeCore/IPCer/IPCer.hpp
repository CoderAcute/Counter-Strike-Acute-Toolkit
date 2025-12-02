#pragma once

#include<Windows.h>
#include<filesystem>

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
            struct S_CSATToolKit_dll {
                std::filesystem::path Path;
            }CSATToolKit_dll;
        }DLLToCS;

        struct S_Saves {
            std::filesystem::path Path;       // Saves主目录
            struct S_External {
                std::filesystem::path Path;
                struct S_Core {
                    std::filesystem::path Path;
                    struct S_Configs {
                        std::filesystem::path Path;
                    } Configs;
                } Core;
            } External;
        } Saves;

        struct S_CSAT_exe {
            std::filesystem::path Path;
        } CSAT_exe;

    } CSAT;

    struct cs2_exe {
        std::filesystem::path Path;
    } cs2_exe;
};

class CSATeCore;

class IPCer {
    CSATeCore* Core;
    CSATsPaths Paths;
public:
    bool Inited = false;
    bool Init(CSATeCore* Core);
    void VirtualMain();
private:
    bool HadCS2Path = false;

    bool NeedToTryInject = false;
    bool HasCS2hProcess = false;

    long long CS2OpenTime = 0;
public:

    bool HasCS2Path();

    bool SetCS2Path(const std::filesystem::path& CS2Path);
    bool SelectCS2Path(); // 通过对话框选择路径
    bool OpenCS2();
    bool TryCatchCS2();
    bool Inject();

    HWND CS2hWnd = nullptr;
    DWORD CS2PID = NULL;
    HANDLE CS2hProcess = nullptr;
    //std::filesystem::path CS2Path;


    std::filesystem::path PathGet_CSAT();
    std::filesystem::path PathGet_CSATConfig();
    std::filesystem::path PathGet_DLLToCS();
    std::filesystem::path PathGet_CSATToolKit_dll();

    std::filesystem::path PathGet_Saves();
    std::filesystem::path PathGet_External();
    std::filesystem::path PathGet_Core();
    std::filesystem::path PathGet_Configs();

    std::filesystem::path PathGet_CSAT_exe();

    std::filesystem::path PathGet_cs2_exe();
};
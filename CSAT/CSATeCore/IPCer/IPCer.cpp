#include"IPCer.hpp"

#include"../CSATeCore.hpp"
#include"../CSATeMainWindowManager/ConsoleManager/ConsoleManager.hpp"
#include"../ConfigManager/ConfigManager.hpp"

#include<Windows.h>
#include<ShObjIdl_core.h>
#include<chrono>

bool IPCer::Init(CSATeCore* Core) {
    this->Core = Core;
    HWND hwnd = FindWindowW(NULL, L"Counter-Strike:Acute's Toolkit");
    if (!hwnd) {
        this->Core->Console().PrintInfor("没有找到窗口\n");
        return false; // 窗口未找到
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == 0) {
        this->Core->Console().PrintInfor("进程ID未找到\n");
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!hProcess) {
        this->Core->Console().PrintInfor("无法打开进程\n");
        return false;
    }

    WCHAR exePath[MAX_PATH] = { 0 };
    DWORD size = MAX_PATH;

    // 获取可执行文件路径
    if (!QueryFullProcessImageNameW(hProcess, 0, exePath, &size)) {
        this->Core->Console().PrintInfor("得到可执行文件路径失败\n");
        return false;
    }

    CloseHandle(hProcess);

    //设置CSAT.exe路径
    this->Paths.CSAT.CSAT_exe.Path = exePath;
    // 获取CSAT目录
    this->Paths.CSAT.Path = this->PathGet_CSAT_exe().parent_path();
    this->Paths.CSAT.CSATConfig.Path = this->PathGet_CSAT() / "CSATConfig";
    this->Paths.CSAT.DLLToCS.Path = this->PathGet_CSAT() / "DLLToCS";
    this->Paths.CSAT.DLLToCS.CSATToolKit_dll.Path = this->PathGet_DLLToCS() / "CSATToolkit.dll";
    this->Paths.CSAT.Saves.Path = this->PathGet_CSAT() / "Saves";
    this->Paths.CSAT.Saves.External.Path = this->PathGet_Saves() / "External";
    this->Paths.CSAT.Saves.External.Core.Path = this->PathGet_External() / "Core";
    this->Paths.CSAT.Saves.External.Core.Configs.Path = this->PathGet_Core() / "Configs";


    this->Core->Console().PrintInfor("IPCer初始化成功!\n");
    this->Inited = true;
    return true;
}

bool IPCer::HasCS2Path() {
    return this->HadCS2Path;
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
std::filesystem::path IPCer::PathGet_CSATToolKit_dll() {
    return this->Paths.CSAT.DLLToCS.CSATToolKit_dll.Path;
}
std::filesystem::path IPCer::PathGet_Saves() {
    return this->Paths.CSAT.Saves.Path;
}
std::filesystem::path IPCer::PathGet_External() {
    return this->Paths.CSAT.Saves.External.Path;
}
std::filesystem::path IPCer::PathGet_Core() {
    return this->Paths.CSAT.Saves.External.Core.Path;
}
std::filesystem::path IPCer::PathGet_Configs() {
    return this->Paths.CSAT.Saves.External.Core.Configs.Path;
}
std::filesystem::path IPCer::PathGet_CSAT_exe() {
    return this->Paths.CSAT.CSAT_exe.Path;
}

std::filesystem::path IPCer::PathGet_cs2_exe() {
    return this->Paths.cs2_exe.Path;
}


void IPCer::VirtualMain() {
    if (this->NeedToTryInject) {
        if (this->CS2hProcess) {
            long long now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
            long long delta = now - this->CS2OpenTime;
            long long Countdown = 20 - delta;
            static long long CountdownBuffer;
            if (CountdownBuffer != Countdown) {
                CountdownBuffer = Countdown;
                this->Core->Console().PrintInfor("注入倒计时：" + std::to_string(Countdown) + "秒\n");
            }
            if (Countdown <= 0) {
                if (this->Inject()) {
                    this->NeedToTryInject = false;
                }
            }
        }
        else {
            this->TryCatchCS2();
        }
    }
}





bool IPCer::SetCS2Path(const std::filesystem::path& CS2Path) {
    this->Paths.cs2_exe.Path = CS2Path;

    return true;
}
bool IPCer::SelectCS2Path() { 
    //通过对话框选择路径
    
    //初始化COM库
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return false;
    }

    IFileOpenDialog* pFileOpen = NULL;

    // 创建文件打开对话框对象
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr)) {
        // 显示对话框
        hr = pFileOpen->Show(NULL);

        if (SUCCEEDED(hr)) {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);

            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr)) {
                    // 输出文件路径
                    this->Paths.cs2_exe.Path = pszFilePath;

                    // 释放字符串内存
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }

    // 清理COM库
    CoUninitialize();
    if (std::filesystem::exists(this->Paths.cs2_exe.Path) && this->Paths.cs2_exe.Path.filename() == L"cs2.exe") {
        return true;
    }
    return false;
}
bool IPCer::OpenCS2() {
    bool SafeOpen = true;
    if (this->Paths.cs2_exe.Path.empty()) {
        SafeOpen = false;
    }
    if (!std::filesystem::exists(this->Paths.cs2_exe.Path)) {
        SafeOpen = false;
    }
    if (!SafeOpen) {
        SafeOpen = this->SelectCS2Path();
    }
    if (!SafeOpen) {
        this->Core->Console().PrintInfor("选择错误\n");
        return false;
    }
    this->Core->ConfigManager().Config_SetCS2Path(this->PathGet_cs2_exe());
    this->Core->ConfigManager().Config_Save(this->PathGet_Configs());
    // 构建参数（不包含可执行文件路径）
    std::wstring parameters = L"-insecure -worldwide -window -w 1920 -h 1080 -novid";

    HINSTANCE result = ShellExecuteW(
        NULL,                                       // 父窗口句柄
        L"open",                                    // 操作
        this->Paths.cs2_exe.Path.c_str(),           // 应用程序路径
        parameters.c_str(),                         // 参数
        NULL,                                       // 工作目录
        SW_SHOWNORMAL                               // 显示方式
    );

    if (reinterpret_cast<INT_PTR>(result) > 32) {
        this->NeedToTryInject = true;
        return true;
    }
}
bool IPCer::TryCatchCS2() {
    if (this->CS2hProcess) {
        return true;
    }
    this->CS2hWnd = FindWindowW(nullptr, L"Counter-Strike 2");
    if (!this->CS2hWnd) {
        //this->Core->Console().PrintInfor("未找到CS窗口！\n");
        return false;
    }
    GetWindowThreadProcessId(this->CS2hWnd, &this->CS2PID);
    if (!this->CS2PID) {
        this->Core->Console().PrintInfor("获取CS窗口进程ID失败！\n");
        return false;
    }
    this->CS2hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->CS2PID);
    if (!this->CS2hProcess) {
        this->Core->Console().PrintInfor("打开CS进程失败！\n");
        return false;
    }
    this->CS2OpenTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    this->Core->Console().PrintInfor("CS相关模块初始化完成！\n");
    return true;
}

bool IPCer::Inject() {
    if (this->Paths.CSAT.DLLToCS.CSATToolKit_dll.Path.empty()) {
        this->Core->Console().PrintInfor("CSATToolKit.dll路径未设置！\n");
        return false;
    }
    if (!std::filesystem::exists(this->Paths.CSAT.DLLToCS.CSATToolKit_dll.Path)) {
        this->Core->Console().PrintInfor("CSATToolKit.dll文件不存在！\n");
        return false;
    }
    if (!this->CS2hProcess || this->CS2hProcess == INVALID_HANDLE_VALUE) {
        this->Core->Console().PrintInfor("CS进程句柄无效！\n");
        return false;
    }

    bool InjectResult;

    // 1. 在目标进程分配内存
    LPVOID pRemoteMem = VirtualAllocEx(
        this->CS2hProcess,
        NULL,
        (this->Paths.CSAT.DLLToCS.CSATToolKit_dll.Path.wstring().size() + 1) * sizeof(wchar_t),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!pRemoteMem) {
        this->Core->Console().PrintInfor("分配内存失败！\n");
        return false;
    }

    // 2. 写入DLL路径
    if (!WriteProcessMemory(
        this->CS2hProcess,
        pRemoteMem,
        this->Paths.CSAT.DLLToCS.CSATToolKit_dll.Path.c_str(),
        (this->Paths.CSAT.DLLToCS.CSATToolKit_dll.Path.wstring().size() + 1) * sizeof(wchar_t),
        NULL
    )) {
        VirtualFreeEx(this->CS2hProcess, pRemoteMem, 0, MEM_RELEASE);
        this->Core->Console().PrintInfor("写入路径失败！\n");
        return false;
    }

    // 3. 获取LoadLibraryW地址
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) {
        this->Core->Console().PrintInfor("获取kernel32.dll模块失败！\n");
        return false;
    }

    FARPROC pLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryW");
    if (!pLoadLibrary) {
        this->Core->Console().PrintInfor("获取LoadLibraryW地址失败！\n");
        return false;
    }

    // 4. 创建远程线程
    HANDLE hThread = CreateRemoteThread(
        this->CS2hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)pLoadLibrary,
        pRemoteMem,
        0,
        NULL
    );

    if (!hThread) {
        VirtualFreeEx(this->CS2hProcess, pRemoteMem, 0, MEM_RELEASE);
        this->Core->Console().PrintInfor("创建远程线程失败！\n");
        return false;
    }

    // 5. 等待线程执行完成
    WaitForSingleObject(hThread, INFINITE);

    // 6. 清理资源
    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);

    CloseHandle(hThread);
    VirtualFreeEx(this->CS2hProcess, pRemoteMem, 0, MEM_RELEASE);

    // 7. 检查是否加载成功
    // HMODULE 非零表示成功

    if (exitCode) {
        this->Core->Console().PrintInfor("注入DLL成功！\n");
        return true;
    }
    else {
        this->Core->Console().PrintInfor("注入DLL失败！\n");
        return false;
    }
}
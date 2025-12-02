#pragma once

#include<d3d11.h>
#include<mutex>
#include<string>

class CSATiManager;

class C_HookManager {
    friend CSATiManager;
private:
    static CSATiManager* CSATi;
public:
    ID3D11Device* pd3dDevice = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;
    ID3D11DeviceContext* pd3dContext = nullptr;
    ID3D11RenderTargetView* view = nullptr;
    HWND hwnd = nullptr;

    void* origin_present = nullptr;
    WNDPROC origin_wndProc;

    std::atomic<bool> NeedReHook = false;
    std::mutex hookMutex; // 保护 hook 操作的互斥锁

    bool d3dInited = false;
    void d3dInit(IDXGISwapChain* _this);

    void MyPresent(IDXGISwapChain* _this);

    DWORD CreateHook(void*);

    void* D3DPresent = nullptr;

    static LRESULT __stdcall MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static long __stdcall HookedPresentEntry(IDXGISwapChain* _this, UINT a, UINT b);
    static DWORD CreateHookEntry(void*);

    void StytlInit();

    // 存储创建的 ImGui 上下文，以便在 WndProc 中在调用线程上设置它
    struct ImGuiContext* ImGuiCtx = nullptr;

    // 保护对 ImGui API 调用（NewFrame/Render/WndProcHandler 等）的互斥量，避免跨线程并发访问导致的数据竞争
    std::mutex imguiMutex;

    // 持久化保存 imgui.ini 路径字符串，保证 io.IniFilename 指向有效内存
    std::string imguiIniPath;
};
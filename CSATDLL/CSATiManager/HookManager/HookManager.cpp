#include"HookManager.hpp"

#include"../../ThirdParty/All_ImGui.hpp"
#include"../../ThirdParty/All_MinHook.hpp"

#include"../CSATiManager.hpp"
#include"../IPCer/IPCer.hpp"

CSATiManager* C_HookManager::CSATi = nullptr;

void C_HookManager::MyPresent(IDXGISwapChain* _this) {
	this->d3dInit(_this);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	this->CSATi->VirtualMain();

	ImGui::EndFrame();
	ImGui::Render();
	this->pd3dContext->OMSetRenderTargets(1, &this->view, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

DWORD C_HookManager::CreateHook(void*) {
	while (this->NeedReHook) {
		const unsigned level_count = 2;
		D3D_FEATURE_LEVEL levels[level_count] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
		DXGI_SWAP_CHAIN_DESC sd{};
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = GetForegroundWindow();
		sd.SampleDesc.Count = 1;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		auto hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			levels,
			level_count,
			D3D11_SDK_VERSION,
			&sd,
			&this->pSwapChain,
			&this->pd3dDevice,
			nullptr,
			nullptr);

		if (this->pSwapChain) {
			auto vtable_ptr = (void***)(this->pSwapChain);
			auto vtable = *vtable_ptr;
			this->D3DPresent = vtable[8];
			MH_Initialize();
			MH_CreateHook(this->D3DPresent, CSATi->HookManager().HookedPresentEntry, &this->origin_present);
			MH_EnableHook(this->D3DPresent);
			this->pd3dDevice->Release();
			this->pSwapChain->Release();

			this->NeedReHook = false;
		}
	}
	return 0;
}

void C_HookManager::d3dInit(IDXGISwapChain* _this) {
	if (!this->d3dInited) {
		_this->GetDevice(__uuidof(ID3D11Device), (void**)&this->pd3dDevice);
		this->pd3dDevice->GetImmediateContext(&this->pd3dContext);

		DXGI_SWAP_CHAIN_DESC sd;
		_this->GetDesc(&sd);
		this->hwnd = sd.OutputWindow;

		ID3D11Texture2D* buf{};
		_this->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buf);
		this->pd3dDevice->CreateRenderTargetView(buf, nullptr, &this->view);
		buf->Release();

		this->origin_wndProc = (WNDPROC)SetWindowLongPtrW(this->hwnd, GWLP_WNDPROC, (LONG_PTR)CSATi->HookManager().MyWndProc);

		ImGui::CreateContext();
		// store created context so other threads (WndProc) can set it when needed
		this->ImGuiCtx = ImGui::GetCurrentContext();


		//设置ini文件路径
		ImGuiIO& io = ImGui::GetIO();
		this->imguiIniPath = (CSATi->IPCer().PathGet_Core() / "CSATUIConfig.ini").string();
		io.IniFilename = this->imguiIniPath.c_str();

		ImGui_ImplWin32_Init(this->hwnd);
		ImGui_ImplDX11_Init(this->pd3dDevice, this->pd3dContext);

		ImFont* font = io.Fonts->AddFontFromFileTTF(
			"C:/Windows/Fonts/msyh.ttc",  // 微软雅黑字体路径
			16.0f,                        // 字体大小
			nullptr,                      // 使用默认配置
			io.Fonts->GetGlyphRangesChineseFull() // 加载所有中文字符
		);
		ImGui_ImplDX11_CreateDeviceObjects();
		this->StytlInit();
		this->d3dInited = true;
	}
}

void C_HookManager::StytlInit() {
	ImGuiStyle& style = ImGui::GetStyle();

	// 设置颜色
	ImVec4 pink_title = ImVec4(0.91f, 0.42f, 0.71f, 0.80f); // 粉色标题
	ImVec4 pink_bg = ImVec4(1.00f, 0.92f, 0.96f, 1.0f);   // 更粉的背景
	ImVec4 purple_text = ImVec4(0.48f, 0.35f, 0.70f, 1.00f); // 紫色文本
	ImVec4 pink_input = ImVec4(0.94f, 0.80f, 0.88f, 0.95f); // 粉色输入框

	// 统一的头部颜色（用于标题栏、标签页、折叠头等）
	ImVec4 header_color = pink_title;
	ImVec4 header_hovered = ImVec4(0.84f, 0.35f, 0.64f, 1.00f); 
	ImVec4 header_active = ImVec4(0.85f, 0.35f, 0.65f, 0.90f);

	// 文本颜色
	style.Colors[ImGuiCol_Text] = purple_text;

	// 窗口相关颜色
	style.Colors[ImGuiCol_WindowBg] = pink_bg;

	// 统一的头部颜色控制
	style.Colors[ImGuiCol_TitleBg] = header_color;
	style.Colors[ImGuiCol_TitleBgActive] = header_color;
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(header_color.x, header_color.y, header_color.z, 0.6f);

	// 标签页颜色 - 使用统一的头部颜色
	style.Colors[ImGuiCol_Tab] = ImVec4(header_color.x, header_color.y, header_color.z, 0.6f);
	style.Colors[ImGuiCol_TabHovered] = header_hovered;
	style.Colors[ImGuiCol_TabActive] = header_active;
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(header_color.x, header_color.y, header_color.z, 0.4f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(header_color.x, header_color.y, header_color.z, 0.7f);

	// 折叠头颜色 - 使用统一的头部颜色
	style.Colors[ImGuiCol_Header] = header_color;
	style.Colors[ImGuiCol_HeaderHovered] = header_hovered;
	style.Colors[ImGuiCol_HeaderActive] = header_active;

	// 边框和框架颜色
	style.Colors[ImGuiCol_Border] = ImVec4(0.91f, 0.71f, 0.82f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.91f, 0.71f, 0.82f, 0.20f);

	// 输入框和复选框
	style.Colors[ImGuiCol_FrameBg] = pink_input;
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.96f, 0.88f, 0.92f, 0.80f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.94f, 0.80f, 0.88f, 0.90f);

	// 按钮颜色
	style.Colors[ImGuiCol_Button] = header_color;
	style.Colors[ImGuiCol_ButtonHovered] = header_hovered;
	style.Colors[ImGuiCol_ButtonActive] = header_active;

	// 滚动条
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(pink_bg.x, pink_bg.y, pink_bg.z, 0.60f);
	style.Colors[ImGuiCol_ScrollbarGrab] = header_color;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = header_hovered;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = header_active;

	// 滑块
	style.Colors[ImGuiCol_SliderGrab] = header_color;
	style.Colors[ImGuiCol_SliderGrabActive] = header_active;

	// 复选框
	style.Colors[ImGuiCol_CheckMark] = header_active;

	// 子窗口背景和间距设置
	style.Colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 0.94f, 0.97f, 1.0f);

	// 输入框相关颜色
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(header_color.x, header_color.y, header_color.z, 0.35f);

	// 调整样式参数
	style.Alpha = 0.95f; // 稍微提高不透明度以提高可读性

	// 圆角设置
	style.WindowRounding = 8.0f;
	style.ChildRounding = 6.0f;
	style.FrameRounding = 4.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 4.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;

	// 增加内边距设置 - 重点调整子窗口内边距
	style.WindowPadding = ImVec2(5, 5);           // 主窗口内边距
	//style.FramePadding = ImVec2(10, 6);           // 控件内边距
	style.CellPadding = ImVec2(8, 4);             // 表格单元格内边距
	style.ItemSpacing = ImVec2(10, 6);            // 控件间距
	//style.ItemInnerSpacing = ImVec2(6, 4);        // 控件内部元素间距

	// 特别增加子窗口的内边距
	//style.DisplayWindowPadding = ImVec2(15, 15);  // 显示区域内边距
	//style.DisplaySafeAreaPadding = ImVec2(8, 8);   // 安全区域内边距

	// 子窗口边框设置
	//style.ChildBorderSize = 1.2f;                 // 增加子窗口边框粗细
}




using Present = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);



bool IsMouseMessage(UINT uMsg) {
	switch (uMsg) {
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN: case WM_XBUTTONUP: case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL: case WM_MOUSEHWHEEL:
	case WM_NCMOUSEMOVE:
	case WM_NCLBUTTONDOWN: case WM_NCLBUTTONUP: case WM_NCLBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN: case WM_NCRBUTTONUP: case WM_NCRBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN: case WM_NCMBUTTONUP: case WM_NCMBUTTONDBLCLK:
	case WM_MOUSELEAVE: case WM_NCMOUSELEAVE:
		return true;
	default:
		return false;
	}
}

bool IsKeyboardMessage(UINT uMsg) {
	switch (uMsg) {
	case WM_KEYDOWN: case WM_KEYUP: case WM_CHAR:
	case WM_SYSKEYDOWN: case WM_SYSKEYUP: case WM_SYSCHAR:
	case WM_DEADCHAR: case WM_SYSDEADCHAR:
	case WM_HOTKEY:
	case WM_APPCOMMAND:
	case WM_INPUTLANGCHANGE:
	case WM_SETFOCUS: case WM_KILLFOCUS:
		return true;
	default:
		return false;
	}
}


LRESULT __stdcall C_HookManager::MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// Ensure the ImGui context created in the render thread is set on this (window) thread while handling Win32 messages
	ImGuiContext* prev_ctx = ImGui::GetCurrentContext();
	if (CSATi && CSATi->HookManager().ImGuiCtx) ImGui::SetCurrentContext(CSATi->HookManager().ImGuiCtx);

	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
		// restore previous context before returning
		ImGui::SetCurrentContext(prev_ctx);
		return true;
	}

	ImGuiIO& io = ImGui::GetIO();
	//鼠标：当ImGui想要捕获时总是拦截
	if (io.WantCaptureMouse && IsMouseMessage(uMsg)) {
		ImGui::SetCurrentContext(prev_ctx);
		return true;
	}
	//键盘：只在WantTextInput为true时拦截（表示输入框激活）
	else if (io.WantTextInput && IsKeyboardMessage(uMsg)) {
		ImGui::SetCurrentContext(prev_ctx);
		return true;
	}

	LRESULT ret = CallWindowProcW(CSATi->HookManager().origin_wndProc, hwnd, uMsg, wParam, lParam);
	ImGui::SetCurrentContext(prev_ctx);
	return ret;
}
long __stdcall C_HookManager::HookedPresentEntry(IDXGISwapChain* _this, UINT a, UINT b) {
	CSATi->HookManager().MyPresent(_this);

	return ((Present)CSATi->HookManager().origin_present)(_this, a, b);
}
DWORD C_HookManager::CreateHookEntry(void*) {
	CSATi->Init();
	CSATi->HookManager().CreateHook(nullptr);
	return 0;
}
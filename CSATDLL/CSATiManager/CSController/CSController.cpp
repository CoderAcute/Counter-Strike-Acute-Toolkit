#include"../../ThirdParty/All_ImGui.hpp"
#include"../../ThirdParty/All_cs2_dumper.hpp"

#include <syncstream>
#include<sstream>

#include"CSController.hpp"

#include"../CSATiManager.hpp"
#include"../MessageManager/IMessageManager.hpp"

#include"../GlobalVars/GlobalVars.hpp"

#include"../AbstractLayer3D/IAbstractLayer3D.hpp"
#include"../KeyTracker/KeyTracker.hpp"

//安全访问，避免访问冲突，可能在关闭游戏时报错，正常，因为本程序本身被中断了，来不及执行后面的内容
template<typename T>
inline bool SafeMemoryRead(T& Target, const uintptr_t& address) {
	__try {
		Target = *reinterpret_cast<T*>(address);
		return true;
	}
	//内核级保护
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}



bool CSController::GetEntityPositionFromEntityPawn(DirectX::XMFLOAT3& Position, const uintptr_t& EntityPawn) {
	if (!this->SafeRead)return false;
	Position = *reinterpret_cast<DirectX::XMFLOAT3*>(EntityPawn + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
	return true;
}
bool CSController::GetEyePositionFromEntityPawn(DirectX::XMFLOAT3& EyePosition, const uintptr_t& EntityPawn) {
	if (!this->SafeRead)return false;
	DirectX::XMFLOAT3 EntityPosition;
	if (!this->GetEntityPositionFromEntityPawn(EntityPosition, EntityPawn))return false;
	DirectX::XMFLOAT3 ViewOffset = *reinterpret_cast<DirectX::XMFLOAT3*>(EntityPawn + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);
    EyePosition = EntityPosition + ViewOffset;
	return true;
}
bool CSController::GetEyePositionFromIndex(DirectX::XMFLOAT3& EyePosition, const int& Index) {
	if (!this->SafeRead)return false;
	if (Index < 0 || Index >= this->EntityList.size())return false;
	uintptr_t EntityPawn = this->EntityList.at(Index).Pawn.Address;
	if (!EntityPawn)return false;
	return this->GetEyePositionFromEntityPawn(EyePosition, EntityPawn);
}

void CSController::Execute(const char* cmd) {
	CALL_VIRTUAL(void, 49, this->Interface, 0, cmd, 1);
	return;
}

void CSController::SetLocalViewAngle(const DirectX::XMFLOAT3& Angles) {
	*this->Local.dwViewAngles = Angles;
	return;
}

void CSController::InitInterface() {
	const char* ModuleName = "engine2.dll";
	const char* InterfaceName = "Source2EngineToClient001";
	HMODULE hModule = GetModuleHandleA(ModuleName);
	if (!hModule) return;
	auto pFunc = reinterpret_cast<void* (*)(const char*, int*)>(GetProcAddress(hModule, "CreateInterface"));
	if (!pFunc) return;
	this->Interface = pFunc(InterfaceName, nullptr);
}
bool CSController::Init() {
	this->AL3D = &this->CSATi->IAbstractLayer3D();
	this->GetModules();
	this->Catch();

	return true;
}

bool CSController::TryCreateUpdateThread() {
    for (;;) {
        if (this->ThreadRunning) return true;

        try {
            // Set running flag before starting the thread to avoid a race where the thread
            // observes a false flag and exits immediately while the caller thinks it's running.
            this->ThreadRunning = true;

            this->UpdateThread = std::thread([this]() {
                while (this->ThreadRunning) {
                    this->GetMsgResult = this->TryGetMsg();
                    std::this_thread::sleep_for(std::chrono::milliseconds(3));
                }
            });

            return true;
        }
        catch (...) {
            // If thread creation failed, ensure the running flag is cleared and try again.
            this->ThreadRunning = false;
            continue;
        }
    }

}
// Destructor: signal thread to stop without holding Mtx to avoid deadlocks.
CSController::~CSController() {
    // Signal the update thread to stop. Do not acquire Mtx here because the thread
    // may be blocked while holding Mtx; acquiring it here would deadlock.
    this->ThreadRunning = false;

    // Join the thread if it was started.
    if (this->UpdateThread.joinable()) {
        try {
            this->UpdateThread.join();
        }
        catch (...) {
            // swallow exceptions in destructor
        }
    }

    return;
}
// CS角色信息获取流程：
// 1. 检测LocalController是否存在并获取
// 2. 通过LocalController获取LocalHandlePawn（实体句柄）
// 3. 获取dwEntityList（实体列表基址）
// 4. 通过LocalHandlePawn获取LocalPawn（本地玩家实体）
// 5. 遍历实体列表：通过dwEntityList获取其他玩家的Controller
// 6. 通过Controller获取其Handle
// 7. 通过Handle获取对应的Pawn
// 8. 从Pawn读取各种游戏数据

// 索引结构说明：
// int类型32位索引的位分配：
//   [31:9] (高23位) - 段索引（Segment Index），用于定位内存中的地址段
//   [8:0]  (低9位)  - 段内偏移（Segment Offset），用于在地址段内定位具体对象

// 根据索引获取对应的内存地址段
// 注意：多个实体索引可能共享同一个地址段（当它们的高23位相同时）
uintptr_t CSController::GetEntityBaseFromIndex(int Index) {
	// 通过索引的高23位（Index >> 9）计算段基址在dwEntityList中的偏移
	// 公式：目标地址 = dwEntityList + (段索引 * 8) + 16
	uintptr_t BaseEntity;
	if (SafeMemoryRead(BaseEntity, this->Local.dwEntityList + 0x8 * (Index >> 9) + 0x10)) {
		return BaseEntity;
	}
	else {
		return 0;
	}
}

// 根据索引获取Controller对象
// 注意：索引的低9位决定了在地址段内的具体位置
uintptr_t CSController::GetEntityControllerFromIndex(int Index) {
	// 获取该索引对应的内存地址段
	uintptr_t entitylistbase = GetEntityBaseFromIndex(Index);
	if (!entitylistbase) return 0;
	// 使用索引的低9位（Index & 0x1FF）计算段内偏移
	// 每个Controller占用0x78字节，通过乘法定位具体位置
	uintptr_t Controller;
	if (SafeMemoryRead(Controller, entitylistbase + (0x70 * (Index & 0x1FF)))) {
		return Controller;
	}
	else {
		return 0;
	}
}

// 根据实体句柄(uHandle)获取Pawn对象
uintptr_t CSController::GetEntityPawnFromHandle(uint32_t uHandle) {
	// 实体句柄的低15位包含有效索引信息
	// 注意：0x7FFF = 32767 (二进制15个1)，提取低15位
	const int nIndex = uHandle & 0x7FFF;
	// 使用提取的索引获取对应的内存地址段
	uintptr_t entitylistbase = GetEntityBaseFromIndex(nIndex);
	if (!entitylistbase) return 0;
	// 使用索引的低9位计算段内偏移（与获取Controller方式相同）
	// Pawn对象同样占用0x78字节空间
	uintptr_t Pawn;
	if (SafeMemoryRead(Pawn, entitylistbase + (0x70 * (nIndex & 0x1FF)))) {
		return Pawn;
	}
	else {
		return 0;
	}
}

convar* ccvar::FindVarByIndex(uint64_t index) {
	return vmt::CallVirtual<convar*>(43, this, index);//一开始是37
}
convar* ccvar::FindVarByName(const char* var_name) {
	// Tip: There's logging in this function because this should run ONLY
		// once for every ConVar. If the console is spammed it means you haven't
		// made the variable static.

	uint64_t i = 0;
	GetFirstCvarIterator(i);
	while (i != 0xFFFFFFFF) {
		convar* pCvar = nullptr;
		pCvar = FindVarByIndex(i);
		if (strcmp(pCvar->m_name, var_name) == 0) {
			//LOG(xorstr_("CCvar::FindVarByName() found '%s' at -> %p\n"), var_name,
			 //   pCvar);
			return pCvar;
		}
		GetNextCvarIterator(i);
	}

	//LOG(xorstr_("CCvar::FindVarByName() couldn't find '%s'.\n"), var_name);
	return nullptr;
}
//convar* ccvar::FindVarByName(const char* var_name) {
//	// Tip: There's logging in this function because this should run ONLY
//	// once for every ConVar. If the console is spammed it means you haven't
//	// made the variable static.
//
//	uint64_t i = 0;
//	GetFirstCvarIterator(i);
//	size_t count = 0;
//	for (;; count++) {
//		if (i == 0xFFFFFFFF) {
//			MessageBoxW(NULL, L"i现在是0xFFFFFFFF", L"CVar 调试", MB_OK | MB_ICONINFORMATION);
//			break;
//		}
//		convar* pCvar = FindVarByIndex(i);
//		//Sleep(1000);
//		//if ((uintptr_t)pCvar == 0x6C) {
//		//	static uint64_t count = 0;
//		//	count++;
//		//	// 使用stringstream格式化浮点数
//		//	std::stringstream ss;
//		//	ss << std::fixed << std::setprecision(2) << i;
//		//	std::string message = ss.str();
//
//		//	// 转换为宽字符串
//		//	std::wstring wMessage(message.begin(), message.end());
//
//		//	//MessageBoxW(NULL, wMessage.c_str(), L"CVar 调试", MB_OK | MB_ICONINFORMATION);
//		//	GetNextCvarIterator(i);
//		//	continue;
//		//}
//		//else {
//			//// 使用stringstream格式化浮点数
//			//std::stringstream ss;
//			//ss << std::fixed << std::setprecision(2) << pCvar->m_name;
//			//std::string message = ss.str();
//
//			//// 转换为宽字符串
//			//std::wstring wMessage(message.begin(), message.end());
//
//			//MessageBoxW(NULL, wMessage.c_str(), L"CVar 调试", MB_OK | MB_ICONINFORMATION);
//		//}
//
//
//		//MessageBoxA(NULL, pCvar->m_name, "Cvar", NULL);
//
//
//
//
//		if (strcmp(pCvar->m_name, var_name) == 0) {
//
//			//LOG(xorstr_("CCvar::FindVarByName() found '%s' at -> %p\n"), var_name,
//			 //   pCvar);
//			return pCvar;
//		}
//
//
//
//		GetNextCvarIterator(i);
//	}
//
//
//	// 使用stringstream格式化浮点数
//	std::stringstream ss;
//	ss << count;
//	std::string message = ss.str();
//
//	// 转换为宽字符串
//	std::wstring wMessage(message.begin(), message.end());
//
//	MessageBoxW(NULL, wMessage.c_str(), L"CVar 调试", MB_OK | MB_ICONINFORMATION);
//
//	//LOG(xorstr_("CCvar::FindVarByName() couldn't find '%s'.\n"), var_name);
//	return nullptr;
//}

void CSController::GetModules() {
	this->Modules.client = reinterpret_cast<uintptr_t>(GetModuleHandleW(L"client.dll"));
	this->Modules.engine2 = reinterpret_cast<uintptr_t>(GetModuleHandleW(L"engine2.dll"));
	this->Modules.tier0 = reinterpret_cast<uintptr_t>(GetModuleHandleW(L"tier0.dll"));
	//if (this->Modules.tier0) {
	//	std::thread([]() {
	//		MessageBoxW(NULL, L"tier0.dll", L"获取到模块", MB_OK | MB_ICONINFORMATION);
	//		}).detach(); // 分离线程，使其独立运行
	//}

	const char* ModuleName = "tier0.dll";
	const char* InterfaceName = "VEngineCvar007";
	HMODULE hModule = GetModuleHandleA(ModuleName);
	auto pFunc = reinterpret_cast<void* (*)(const char*, int*)>(GetProcAddress(hModule, "CreateInterface"));
	this->CvarSystem = (ccvar*)pFunc(InterfaceName, nullptr);

	this->Local.pGlobalFOV = this->CvarSystem->FindVarByName("fov_cs_debug")->GetPtr<float>();

	if (!this->Modules.client || !this->Modules.engine2) {
		this->HasGetModules = false;
	}
}

void CSController::Catch() {
	//本地工作
	//获取本地视图投影矩阵
	this->Local.dwViewMatrix = reinterpret_cast<float*>(this->Modules.client + cs2_dumper::offsets::client_dll::dwViewMatrix);
	//获取本地欧拉角
	this->Local.dwViewAngles = reinterpret_cast<DirectX::XMFLOAT3*>(this->Modules.client + cs2_dumper::offsets::client_dll::dwViewAngles);
	
	return;
}
int CSController::BasicUpdate() {
	std::unique_lock<std::shared_mutex> lock(this->BasicMtx);
	//获取EntityList
	if (!SafeMemoryRead(this->Local.dwEntityList, this->Modules.client + cs2_dumper::offsets::client_dll::dwEntityList)) {
		return -1;
	}
	//获取全局变量
	if (!SafeMemoryRead(this->Local.GlobalVars.Address, this->Modules.client + cs2_dumper::offsets::client_dll::dwGlobalVars)) {
		return -2;
	}
	//获取本地控制器
	if (!SafeMemoryRead(this->Local.Player.Controller.Address, this->Modules.client + cs2_dumper::offsets::client_dll::dwLocalPlayerController)) {
		return -1001;
	}


	//判断本地控制器是否可用
	if (!this->Local.Player.Controller.Address) {
		return 1003;
	}
	{
		

		//将目标地址转换为 char*，然后用 strncpy 拷贝内容
		const char* LocalPlayerNamePtr = reinterpret_cast<const char*>(this->Local.Player.Controller.Address + cs2_dumper::schemas::client_dll::CBasePlayerController::m_iszPlayerName);
		strncpy_s(this->Local.Player.Controller.m_iszPlayerName, LocalPlayerNamePtr, sizeof(this->Local.Player.Controller.m_iszPlayerName) - 1);
		this->Local.Player.Controller.m_iszPlayerName[sizeof(this->Local.Player.Controller.m_iszPlayerName) - 1] = '\0'; // 确保字符串结尾
		//获取本地控制器的期望FOV
		this->Local.Player.Controller.m_ipDesiredFOV = reinterpret_cast<uint32_t*>(this->Local.Player.Controller.Address + cs2_dumper::schemas::client_dll::CBasePlayerController::m_iDesiredFOV);
	}
	//获取本地实体句柄
	this->Local.Player.Controller.hPawn = *reinterpret_cast<uint32_t*>(this->Local.Player.Controller.Address + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
	if (this->Local.Player.Controller.hPawn == 0xFFFFFFFF)return 2001;
	//获取本地实体
	this->Local.Player.Pawn.Address = this->GetEntityPawnFromHandle(this->Local.Player.Controller.hPawn);
	if (!this->Local.Player.Pawn.Address)return 3001;
	//摄像机模块
	{
		//获取本地实体的摄像机模块
		this->Local.Player.Pawn.CameraServices.Address = *reinterpret_cast<uintptr_t*>(this->Local.Player.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pCameraServices);
		//从摄像机服务获取fov
		this->Local.Player.Pawn.CameraServices.pFOV = reinterpret_cast<int*>(this->Local.Player.Pawn.CameraServices.Address + cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_iFOV);
		this->Local.Player.Pawn.CameraServices.pFOVStart = reinterpret_cast<uint32_t*>(this->Local.Player.Pawn.CameraServices.Address + cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_iFOVStart);
		this->Local.Player.Pawn.CameraServices.pFOVRate = reinterpret_cast<float*>(this->Local.Player.Pawn.CameraServices.Address + cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_flFOVRate);
		this->Local.Player.Pawn.CameraServices.pFOVTime = reinterpret_cast<float*>(this->Local.Player.Pawn.CameraServices.Address + cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_flFOVTime);
		//if (this->CSATi->KT().IsKeyPressed('F')) {
		//	//*this->Local.Player.Pawn.CameraServices.pFOV = 120;
		//	
		//	static float temp = 0;
		//	convar* pVar = this->CvarSystem->FindVarByName("fov_cs_debug");
		//	temp = pVar->GetValue<float>();

		//	// 使用stringstream格式化浮点数
		//	std::stringstream ss;
		//	ss << pVar->m_name << std::fixed << std::setprecision(2) << temp;
		//	std::string message = ss.str();

		//	// 转换为宽字符串
		//	std::wstring wMessage(message.begin(), message.end());

		//	std::thread([wMessage]() {
		//		MessageBoxW(NULL, wMessage.c_str(), L"CVar 调试", MB_OK | MB_ICONINFORMATION);
		//	}).detach();

		//	float* pfov = pVar->GetPtr<float>();
		//	*pfov = 120.0f;
		//	for (int i = 0; i < 10000; i++) {
		//		
		//	}
		//}
	}
	//其它
	{
		this->Local.Player.Pawn.m_iTeamNum = -10;
		this->Local.Player.Pawn.m_iTeamNum = *reinterpret_cast<int*>(this->Local.Player.Pawn.Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);//从本地实体获取team
		if (this->Local.Player.Pawn.m_iTeamNum < 0)return 4001;
	}


	this->CurrentTick = reinterpret_cast<int*>(this->Local.Player.Controller.Address + cs2_dumper::schemas::client_dll::CBasePlayerController::m_nTickBase);//从本地控制器获取当前tick
	//从本地实体获取GameSecneNode
	this->Local.Player.Pawn.GameSceneNode.Address = *reinterpret_cast<uintptr_t*>(this->Local.Player.Pawn.Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
	//获取本地坐标
	this->Local.Player.Pawn.m_vOldOrigin = *reinterpret_cast<DirectX::XMFLOAT3*>(this->Local.Player.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
	this->Local.Player.Pawn.m_iHideHUD = *reinterpret_cast<uint32_t*>(this->Local.Player.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_iHideHUD);
	this->Local.Player.Pawn.p_iHideHUD = reinterpret_cast<uint32_t*>(this->Local.Player.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_iHideHUD);
	this->Local.Player.Pawn.GameSceneNode.pPosition = reinterpret_cast<DirectX::XMFLOAT3*>(Local.Player.Pawn.GameSceneNode.Address + cs2_dumper::schemas::client_dll::CGameSceneNode::m_vecAbsOrigin);
	//获取本地旋转角
	this->Local.Player.Pawn.GameSceneNode.pRotationEuler = reinterpret_cast<DirectX::XMFLOAT3*>(Local.Player.Pawn.GameSceneNode.Address + cs2_dumper::schemas::client_dll::CGameSceneNode::m_angAbsRotation);

	//MessageBoxW(NULL, L"1", L"1", NULL);

	//获取客户端网络
	this->NetworkGameClient = *reinterpret_cast<uintptr_t*>(this->Modules.engine2 + cs2_dumper::offsets::engine2_dll::dwNetworkGameClient);
	if (this->NetworkGameClient) {
		float time = *reinterpret_cast<float*>(this->NetworkGameClient + 0xB8);
		this->RenderTime.store(time);
		if (this->RenderTime) {
			
		}
	}
	this->Local.GlobalVars.Update();
	this->CSATi->IAbstractLayer3D().PushTime(this->Local.GlobalVars.CurrentTime);

	this->ScreenWide = 1920.0f;

	this->ScreenHigh = 1080.0f;


	this->Local.Player.Pawn.m_pObserverServices.Address = *reinterpret_cast<uintptr_t*>(this->Local.Player.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pObserverServices);
	if (this->Local.Player.Pawn.m_pObserverServices.Address) {
		this->Local.Player.Pawn.m_pObserverServices.m_iObserverMode = reinterpret_cast<uint8_t*>(this->Local.Player.Pawn.m_pObserverServices.Address + cs2_dumper::schemas::client_dll::CPlayer_ObserverServices::m_iObserverMode);
		this->Local.Player.Pawn.m_pObserverServices.m_hObserverTarget = *reinterpret_cast<uintptr_t*>(this->Local.Player.Pawn.m_pObserverServices.Address + cs2_dumper::schemas::client_dll::CPlayer_ObserverServices::m_hObserverTarget);
		this->Local.Player.Pawn.m_pObserverServices.m_iObserverLastMode = reinterpret_cast<ObserverMode_t*>(this->Local.Player.Pawn.m_pObserverServices.Address + cs2_dumper::schemas::client_dll::CPlayer_ObserverServices::m_iObserverLastMode);
		this->Local.Player.Pawn.m_pObserverServices.m_bForcedObserverMode = reinterpret_cast<bool*>(this->Local.Player.Pawn.m_pObserverServices.Address + cs2_dumper::schemas::client_dll::CPlayer_ObserverServices::m_bForcedObserverMode);
		this->Local.Player.Pawn.m_pObserverServices.m_flObserverChaseDistance = reinterpret_cast<float*>(this->Local.Player.Pawn.m_pObserverServices.Address + cs2_dumper::schemas::client_dll::CPlayer_ObserverServices::m_flObserverChaseDistance);
		this->Local.Player.Pawn.m_pObserverServices.m_flObserverChaseDistanceCalcTime = reinterpret_cast<GameTime_t*>(this->Local.Player.Pawn.m_pObserverServices.Address + cs2_dumper::schemas::client_dll::CPlayer_ObserverServices::m_flObserverChaseDistanceCalcTime);
	}

	static int OldRoundStartCount = this->Local.CSGameRules.m_nRoundStartCount;
	if (OldRoundStartCount != this->Local.CSGameRules.m_nRoundStartCount) {
		CSATMessage* Msg = new CSATMessage;
		Msg->Type = MsgType::Game_NewRound;
		this->CSATi->IMessageManager().Publish(Msg);
		OldRoundStartCount = this->Local.CSGameRules.m_nRoundStartCount;
	}




	return 0;
}

int CSController::EntityListUpdate() {
	std::unique_lock<std::shared_mutex> lock(this->EntityListMtx);
	this->EntityList.clear();

	C_Entity tempEntity;
	for (int i = 0; i < 64; ++i, this->EntityList.push_back(std::move(tempEntity))) {
		tempEntity = {};
		auto player_co = this->GetEntityControllerFromIndex(i);//这里其实只访问了第一个地址段
		if (!player_co)continue;

		tempEntity.Index = i;
		tempEntity.Controller.Address = player_co;
		tempEntity.Controller.hPawn = *reinterpret_cast<uint32_t*>(player_co + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
		if (tempEntity.Controller.hPawn == 0xFFFFFFFF)continue;
		tempEntity.Pawn.Address = this->GetEntityPawnFromHandle(tempEntity.Controller.hPawn);
		if (!tempEntity.Pawn.Address)continue;
		tempEntity.Pawn.m_iTeamNum = *reinterpret_cast<int*>(tempEntity.Pawn.Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
		tempEntity.Pawn.m_iHealth = *reinterpret_cast<int*>(tempEntity.Pawn.Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
		tempEntity.Pawn.m_vOldOrigin = *reinterpret_cast<DirectX::XMFLOAT3*>(tempEntity.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
		tempEntity.Pawn.m_iHideHUD = *reinterpret_cast<uint32_t*>(tempEntity.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_iHideHUD);
		tempEntity.Pawn.p_iHideHUD = reinterpret_cast<uint32_t*>(tempEntity.Pawn.Address + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_iHideHUD);
		if (!SafeMemoryRead(tempEntity.Pawn.m_angEyeAngles, tempEntity.Pawn.Address + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_angEyeAngles)) {
			continue;
		}
		 //= *reinterpret_cast<DirectX::XMFLOAT3*>();
		//获取GameSecneNode
		//tempEntity.Pawn.GameSceneNode.Address = *reinterpret_cast<uintptr_t*>(tempEntity.Pawn.Address + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
		//获取位置指针
		//tempEntity.Pawn.GameSceneNode.pPosition = reinterpret_cast<DirectX::XMFLOAT3*>(tempEntity.Pawn.GameSceneNode.Address + cs2_dumper::schemas::client_dll::CGameSceneNode::m_vecAbsOrigin);
		//获取旋转角
		//tempEntity.Pawn.GameSceneNode.pRotationEuler = reinterpret_cast<DirectX::XMFLOAT3*>(tempEntity.Pawn.GameSceneNode.Address + cs2_dumper::schemas::client_dll::CGameSceneNode::m_angRotation);
		//将目标地址转换为 char*，然后用 strncpy 拷贝内容
		const char* playerNamePtr = reinterpret_cast<const char*>(player_co + cs2_dumper::schemas::client_dll::CBasePlayerController::m_iszPlayerName);
		strncpy_s(tempEntity.Controller.m_iszPlayerName, playerNamePtr, sizeof(tempEntity.Controller.m_iszPlayerName) - 1);
		tempEntity.Controller.m_iszPlayerName[sizeof(tempEntity.Controller.m_iszPlayerName) - 1] = '\0'; // 确保字符串结尾

	}

	int IndexInMap = 1;
	for (int i=0;i<64;++i) {
		if (IndexInMap == 11) {
			break;
		}
		C_Entity& Entity = this->EntityList.at(i);
		int team = Entity.GetPawn().m_iTeamNum;
		if (team == 2 || team == 3) {
			D_Player PlayerMsg{
			.Position = Entity.GetPawn().m_vOldOrigin,
			.Rotation = Entity.GetPawn().m_angEyeAngles,
			.HP = Entity.GetPawn().m_iHealth,
			.Team = team,
			.Alive = (Entity.GetPawn().m_iHealth > 0),
			.IndexInEntityList = Entity.Index,
			.IndexInMap = IndexInMap,
			};
			this->AL3D->UpdatePlayerMsg(std::move(PlayerMsg));
			++IndexInMap;
		}
	}
	return 0;
}
int CSController::GameRulesUpdate() {
	std::unique_lock<std::shared_mutex> lock(this->GameRulesMtx);
	this->Local.CSGameRules.Address = *reinterpret_cast<uintptr_t*>(this->Modules.client + cs2_dumper::offsets::client_dll::dwGameRules);
	this->Local.CSGameRules.Update();
	

	this->Local.PlantedC4.Address = *reinterpret_cast<uintptr_t*>(this->Modules.client + cs2_dumper::offsets::client_dll::dwPlantedC4);
	this->Local.PlantedC4.Update();

	if (this->Local.CSGameRules.m_bBombPlanted) {
		this->AL3D->SetPhaseStartTime(this->Local.PlantedC4.m_flC4Blow - this->Local.PlantedC4.m_flTimerLength);
		this->AL3D->SetPhaseDuration(this->Local.PlantedC4.m_flTimerLength);
	}
	else {
		this->AL3D->SetPhaseStartTime(this->Local.CSGameRules.m_fRoundStartTime);
		this->AL3D->SetPhaseDuration(this->Local.CSGameRules.m_iRoundTime);
	}
	
	return 0;
}

int CSController::TryGetMsg() {
	
	//切换当前状态不安全
	this->SafeRead = false;
	int Result;
	Result = this->BasicUpdate();
	if (Result) {
		this->CSATi->GlobalVars().InGamePlaying = false;
		return Result;
	}
	Result = this->EntityListUpdate();
	if (Result) {
		return Result;
	}
	Result = this->GameRulesUpdate();
	if (Result) {
		return Result;
	}
	this->CSATi->GlobalVars().InGamePlaying = true;
	this->SafeRead = true;

	return 0;
}
bool CSController::SetLocalFrame(const CSATMath::Frame& frame) {
	static uint8_t LastIndex;
	if (!this->CSATi->GlobalVars().InGamePlaying) {
		return false;
	}
	else {
		while (!this->SafeRead);
	}
	if (frame.TargetOBMode == 4) {
		if (*this->Local.Player.Pawn.m_pObserverServices.m_iObserverMode == 4) {
			DirectX::XMFLOAT4 PosAndFOV = frame.GetPositionAndFOV();
			DirectX::XMFLOAT3 RotEuler = frame.GetRotationEuler();

			this->SetLocalPosition(PosAndFOV);
			this->SetLocalFOV(PosAndFOV.w);
			this->SetLocalViewAngle(RotEuler);
		}
		else {
			this->Execute("spec_mode 4");
		}
		LastIndex = 0;
	}
	
	if (frame.TargetOBMode == 2) {
		if (LastIndex != frame.TargetPlayerIndexInMap) {
			this->Execute(("spec_mode 2;spec_player " + std::to_string(frame.TargetPlayerIndexInMap)).c_str());
			LastIndex = frame.TargetPlayerIndexInMap;
		}
	}


	return true;
}
bool CSController::SetLocalPosition(const DirectX::XMFLOAT4& PosAndFOV) {
	__try {
		this->Local.Player.Pawn.GameSceneNode.pPosition->x = PosAndFOV.x;
		this->Local.Player.Pawn.GameSceneNode.pPosition->y = PosAndFOV.y;
		this->Local.Player.Pawn.GameSceneNode.pPosition->z = PosAndFOV.z;

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}

void CSController::SetLocalFOV(float FOV) {
	*this->Local.pGlobalFOV = FOV;
}

CSATMath::SpatialState CSController::CSGetLocalSpatialState() {
	while (!this->SafeRead);
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	
	DirectX::XMFLOAT3 pos = this->Local.Player.Pawn.GetEyePos();
	CSATMath::SpatialState temp;
	temp.PositionAndFOV = DirectX::XMVectorSet(
		pos.x,
		pos.y,
		pos.z,
		this->CSGetFov()
	);

	DirectX::XMVECTOR quatVec = CSATMath::CSEulerToQuatVec(*this->Local.dwViewAngles);
	temp.RotationQuat = quatVec;

	return temp;
}

DirectX::XMFLOAT3 CSController::CSGetPosition() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	if (!this->SafeRead)return DirectX::XMFLOAT3{};
	return *this->Local.Player.Pawn.GameSceneNode.pPosition;
}
DirectX::XMFLOAT3 CSController::CSGetRotationEuler() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	if (!this->SafeRead)return DirectX::XMFLOAT3{};
	return *this->Local.dwViewAngles;
}

float CSController::CSGetCurrentTime() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	if (!this->SafeRead)return float{};
	return this->flCurrentTime;
}

float CSController::CSGetRenderTime() {
	return this->RenderTime;
}

float CSController::CSGetFov() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	static float FOV = static_cast<float>(this->Local.Player.Pawn.CameraServices.iFOV);
	if (FOV == 0)FOV = 90;
	if (!this->SafeRead)return FOV;
	FOV = static_cast<float>(this->Local.Player.Pawn.CameraServices.iFOV);
	if (FOV == 0)FOV = 90;
	return FOV;
}

int CSController::CSGetRenderTick() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	if (!this->SafeRead)return 0;
	return *this->CurrentTick;
}

float CSController::CSGetScreenWide() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);

	return this->ScreenWide;
}

float CSController::CSGetScreenHigh() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	return this->ScreenHigh;
}

float* CSController::CSGetMatrix() {
	return this->Local.dwViewMatrix;
}

void CSController::AimbotMain() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	if (!this->AimbotEnable)return;
	
	if (!this->SafeRead)return;

	DirectX::XMFLOAT3 LocalEyePos = this->Local.Player.Pawn.GetEyePos();
	DirectX::XMFLOAT3 TargetEyePos;
	if (!this->GetEyePositionFromIndex(TargetEyePos, this->AimTargetIndex))return;

	DirectX::XMFLOAT3 dir = TargetEyePos - LocalEyePos;
	DirectX::XMFLOAT3 TargetViewAngles{};

	ImGui::Text(to_string(LocalEyePos).c_str());
	CSATMath::CSDirToEuler(dir, TargetViewAngles);
	
	this->SetLocalViewAngle(TargetViewAngles);
}

std::ostringstream CSController::GetLocalPlayerMsg() {
	std::shared_lock<std::shared_mutex> lock(this->BasicMtx);
	std::ostringstream oss;
	if (!this->SafeRead) {
		oss << "error";
		return oss;
	}
	return this->Local.GetMsg();
}














//C_EntityList CSController::GetEntityList() {
//	std::shared_lock<std::shared_mutex> lock(this->EntityListMtx);
//	return this->EntityList;
//}


C_Entity CSController::GetEntity(size_t Index) {
	std::shared_lock<std::shared_mutex> lock(this->EntityListMtx);
	try {
		return this->EntityList.at(Index);
	}
	catch (const std::out_of_range&) {
		return C_Entity{};
	}
	//return this->EntityList.at(Index);
}
C_CSGameRules CSController::GetCSGameRules() {
	std::shared_lock<std::shared_mutex> lock(this->GameRulesMtx);
	return this->Local.CSGameRules;
}
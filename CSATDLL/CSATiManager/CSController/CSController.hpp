#pragma once

#include<Windows.h>
#include<cmath>
#include<vector>
#include<shared_mutex>

#include"../../CSATMath/CSATMath.hpp"
#include"../ModuleBase/ModuleBase.hpp"

#include"Classes/Classes.hpp"

#define CALL_VIRTUAL(retType,idx,...)\
vmt::CallVirtual<retType>(idx,__VA_ARGS__)
namespace vmt {
	template<typename T = void*>
	inline T GetVMethod(uint32_t uIndex, void* pClass) {
		void** pVTable = *static_cast<void***>(pClass);
		return reinterpret_cast<T>(pVTable[uIndex]);
	}

	template<typename T, typename ... Args>
	inline T CallVirtual(uint32_t uIndex, void* pClass, Args ... args) {
		auto pFunc = GetVMethod<T(__thiscall*)(void*, Args ...)>(uIndex, pClass);
		return pFunc(pClass, args ...);
	}
}
//namespace client_dll {
//    constexpr std::ptrdiff_t dwCSGOInput = 0x1E3DE30;
//    constexpr std::ptrdiff_t dwEntityList = 0x1D16758;
//    constexpr std::ptrdiff_t dwGameEntitySystem = 0x1FB7B48;
//    constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x20F0;
//    constexpr std::ptrdiff_t dwGameRules = 0x1E32C78;
//    constexpr std::ptrdiff_t dwGlobalVars = 0x1BE7220;
//    constexpr std::ptrdiff_t dwGlowManager = 0x1E2FB18;
//    constexpr std::ptrdiff_t dwLocalPlayerController = 0x1E1FC08;
//    constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1BF1FA0;
//    constexpr std::ptrdiff_t dwPlantedC4 = 0x1E38398;
//    constexpr std::ptrdiff_t dwPrediction = 0x1BF1ED0;
//    constexpr std::ptrdiff_t dwSensitivity = 0x1E30578;
//    constexpr std::ptrdiff_t dwSensitivity_sensitivity = 0x48;
//    constexpr std::ptrdiff_t dwViewAngles = 0x1E3E4E0;
//    constexpr std::ptrdiff_t dwViewMatrix = 0x1E339D0;
//    constexpr std::ptrdiff_t dwViewRender = 0x1E347F0;
//    constexpr std::ptrdiff_t dwWeaponC4 = 0x1DD0EE8;
//}
class C_Modules {
public:
	uintptr_t client{};
	uintptr_t engine2{};
};

class IAbstractLayer3D;

class CSController final:public ModuleBase{
	friend class Debugger;
private:
	IAbstractLayer3D* AL3D = nullptr;
	void* Interface = nullptr;
public:
	CSController(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::CSController;
	}
	~CSController();
	bool Init()override;
	

	void InitInterface();
    void Execute(const char* cmd);
private:
	uintptr_t GetEntityBaseFromIndex(int Index);
	uintptr_t GetEntityControllerFromIndex(int Index);
	uintptr_t GetEntityPawnFromHandle(uint32_t uHandle);
	bool GetEntityPositionFromEntityPawn(DirectX::XMFLOAT3& Position, const uintptr_t& EntityPawn);
    bool GetEyePositionFromEntityPawn(DirectX::XMFLOAT3& EyePosition, const uintptr_t& EntityPawn);
	bool GetEyePositionFromIndex(DirectX::XMFLOAT3& EyePosition, const int& Index);

	float ScreenWide{};
	float ScreenHigh{};
	
    C_EntityList EntityList{};
	C_Modules Modules{};
    int* CurrentTick{};
public:
	//C_EntityList GetEntityList();
	C_Entity GetEntity(size_t Index);
	C_CSGameRules GetCSGameRules();
    C_Local Local{};
private:
	float flCurrentTime{};
	std::atomic<float> CurrentTime30;
    
	uintptr_t NetworkGameClient{};
	std::atomic<float> RenderTime{};
public:
    bool HasGetModules = false;
    void GetModules();
    void Catch();
    int BasicUpdate();
	int EntityListUpdate();
	int GameRulesUpdate();

	mutable std::shared_mutex BasicMtx{};
	mutable std::shared_mutex EntityListMtx{};
	mutable std::shared_mutex GameRulesMtx{};

    std::atomic<bool> ThreadRunning{};//线程运行状态
    std::thread UpdateThread;//线程对象成员
    bool TryCreateUpdateThread();//尝试创建一个检测线程
	std::atomic<bool> SafeRead = false;

   

    std::atomic<int> GetMsgResult = 0;
	int TryGetMsg();
    

    CSATMath::SpatialState CSGetLocalSpatialState();
    DirectX::XMFLOAT3 CSGetPosition();
    DirectX::XMFLOAT3 CSGetRotationEuler();
	float CSGetCurrentTime();
	float CSGetRenderTime();
	int CSGetRenderTick();
	float CSGetFov();

	float CSGetScreenWide();
	float CSGetScreenHigh();
	float* CSGetMatrix();


	
	//设置本地欧拉角
	void SetLocalViewAngle(const DirectX::XMFLOAT3& Angles);
	bool SetLocalFrame(const CSATMath::Frame& frame);
	void SetLocalFOV(float FOV);
	bool SetLocalPosition(const DirectX::XMFLOAT4& PosAndFOV);
	
    bool AimbotEnable = false;
    void AimbotMain();

	int AimTargetIndex = -1;
    std::ostringstream GetLocalPlayerMsg();
};


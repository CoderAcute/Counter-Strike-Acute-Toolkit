#pragma once

#include<Windows.h>
#include<cmath>
#include<vector>
#include<shared_mutex>

#include<sstream>

#include"../../CSATMath/CSATMath.hpp"
#include"../ModuleBase/ModuleBase.hpp"

#include"Classes/Classes.hpp"

#include <iomanip>

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
	uintptr_t client = 0;
	uintptr_t engine2 = 0;
	uintptr_t tier0 = 0;
};

//class Convar {
//public:
//	template <typename T>
//	T get_value(void) {
//		return *(T*)(this + 0x50);
//	}
//
//	template <typename T>
//	void set_value(T value) {
//		*(T*)(this + 0x50) = value;
//	}
//};
//
//class C_CvarSystem {
//public:
//
//	Convar* get_convar(const char* convar_name) {
//		unsigned long objects = *(unsigned long*)(this + 0x40);
//		unsigned long length = *(unsigned long*)(this + 0xA0);
//
//		for (unsigned int i = 0; i < length; ++i) {
//			unsigned long object = *(unsigned long*)(objects + i * 0x10);
//			if (object == 0) break;
//
//			const char* name = *(const char**)(object);
//			if (strstr(name, convar_name)) {
//				return (Convar*)object;
//			}
//		}
//
//		return nullptr;
//	}
//};
/////////////////////////////////////////////////////////////////////////
// 
union CVValue_t
{
	bool i1;
	short i16;
	uint16_t u16;
	int i32;
	uint32_t u32;
	int64_t i64;
	uint64_t u64;
	float fl;
	double db;
	const char* sz;
	//color_t clr;
	//vector_2d vec2;
	//vector vec3;
	//vector_4d vec4;
	//vector ang;
};

class convar
{
public:
	const char* m_name; // 0x0000
	convar* m_pNext; // 0x0008
	const char pad000[0x10]; // 0x0010
	const char* szDescription; // 0x0020
	uint32_t nType; // 0x28
	uint32_t nRegistered; // 0x2C
	uint32_t nFlags; // 0x30
	const char pad001[0x8]; // 0x34
	// @note: read-only, mofify with caution
	CVValue_t value; // 0x40

	template <typename T>
	T GetValue() {
		// Tip: Do not modify the value by making this a reference.
		// It'll get your account flagged.
		return *reinterpret_cast<T*>((uintptr_t)(this) + 0x50);
	}
	template <typename T>
	T* GetPtr() {
		return reinterpret_cast<T*>((uintptr_t)(this) + 0x50);
	}
};

class ccvar {
public:
	auto GetFirstCvarIterator(uint64_t& idx) {
		return CALL_VIRTUAL(void*, 12, this, &idx);
	}

	auto GetNextCvarIterator(uint64_t& idx) {
		return CALL_VIRTUAL(void*, 13, this, &idx, idx);
	}

	convar* FindVarByIndex(uint64_t index);

	convar* FindVarByName(const char* var_name);
};


/////////////////////////////////////////////////////////////////////////

class IAbstractLayer3D;

class CSController final:public ModuleBase{
	friend class Debugger;
private:
	IAbstractLayer3D* AL3D = nullptr;
	void* Interface = nullptr;
	ccvar* CvarSystem = nullptr;
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


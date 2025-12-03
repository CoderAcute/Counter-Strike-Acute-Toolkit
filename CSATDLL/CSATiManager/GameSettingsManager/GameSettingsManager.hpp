#pragma once

#include"../ModuleBase/ModuleBase.hpp"

#include"../CSController/CSController.hpp"

class FPSSettings {
public:
	int MaxFPS = 200;

	bool operator==(const FPSSettings&)const = default;
};

class ScreenSettings {
public:
	bool XRay = true;
	bool ESPBox = false;

	bool operator==(const ScreenSettings&)const = default;
};

class SoundSettings {
public:
	bool snd_mute_losefocus = false;//游戏窗口失去焦点时静音

	float snd_menumusic_volume = 0; //主菜单音量
	float snd_roundstart_volume = 0; //回合开始音量
	float snd_roundaction_volume = 0; //回合开始行动音量
	float snd_roundend_volume = 0; //回合结束音量
	float snd_mvp_volume = 0; //MVP音量
	float snd_mapobjective_volume = 0; //炸弹/人质音量
	float snd_tensecondwarning_volume = 0; //十秒警告音量(0.04就是游戏设置中的20)
	float snd_deathcamera_volume = 0; //死亡视角音量
	bool snd_mute_mvp_music_live_players = true; //当双方团队成员都存活时关闭MVP音乐(0--否，1--是)
	
	bool operator==(const SoundSettings&)const = default;
};

class C_GameSettings {
public:
	bool HUD = 1;
	bool OnlyDeath = 0;
	int FPSShowMode = 0;
	int TickShowMode = 0;
	float GameSpeed = 1.0;
	SoundSettings SoundSettings{};
	ScreenSettings ScreenSettings{};
	FPSSettings FPSSettings{};

	bool operator==(const C_GameSettings&)const = default;
};

class GameSettingsManager final :public ModuleBase {
private:
	C_GameSettings GameSettings{};
	C_GameSettings OldGameSettings;
	uintptr_t pNow;
	uintptr_t pOld;
public:
	GameSettingsManager(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::GameSettingsManager;
	}

	//基类接口实现

	bool Init()override;

	void VirtualMain()override;
	void Menu()override;


	void Override();

	void SettingGraphFloat(const char* Label, float* V, const float& min, const float& max);
	void SettingGraphInt(const char* Label, int* V, const int& min, const int& max);

	void ESPDraw();


	bool CommandExecute(const char* cmd);


	//常数
	template<typename T>
	void CommandOverride(const char* Command, T* pNewValue) {
		// 计算成员在结构体中的偏移量
		auto offset = reinterpret_cast<uintptr_t>(pNewValue) - this->pNow;
		// 获取旧值指针
		T* pOldValue = reinterpret_cast<T*>(this->pOld + offset);
		if (*pNewValue != *pOldValue) {
			std::string commandStr = std::string(Command) + " " + std::to_string(*pNewValue);//布尔类型可以转化成0或1，true或false都可以正常兼容
			this->CommandExecute(commandStr.c_str());
			*pOldValue = *pNewValue;
		}
	}

	bool SafeCmdUse();
	//内置平方方法
	template<typename T>
	void CommandOverrideSquare(const char* Command, T* pNewValue) {
		// 计算成员在结构体中的偏移量
		auto offset = reinterpret_cast<uintptr_t>(pNewValue) - this->pNow;
		// 获取旧值指针
		T* pOldValue = reinterpret_cast<T*>(this->pOld + offset);
		if (*pNewValue != *pOldValue) {
			if (!SafeCmdUse())return;
			std::string commandStr = std::string(Command) + " " + std::to_string((*pNewValue) * (*pNewValue));
			this->CommandExecute(commandStr.c_str());
			*pOldValue = *pNewValue;
		}
	}


};
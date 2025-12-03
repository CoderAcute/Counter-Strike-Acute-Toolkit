#pragma once

#include<cstdint>

enum class MsgType :uint32_t {
	Core_Begin,
	Core_Shutdown,
	Core_Tick1,
	Core_Tick5,
	Core_Tick10,
	Core_Tick15,
	Core_Tick20,
	Core_Tick30,
	Core_Tick45,
	Core_Tick60,

	Game_NewRound,
	Game_RoundStart,
	
	Game_BombPlanted,
	Game_BombDefused,
	Game_Boomed,

	Game_RoundEnd,

	CameraSystem_CallSolution,
	CameraSystem_PlayingShutdown,

	Command_SpecPlayer//指定观战某个玩家，Param1存玩家索引
};

class CSATMessage {
public:
	MsgType Type;
	int Param1;
	float Param2;

	virtual ~CSATMessage() = default;
};
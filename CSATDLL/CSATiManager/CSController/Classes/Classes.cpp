#include"Classes.hpp"

#include"../../../CSATMath/CSATMath.hpp"

#include"../../../ThirdParty/All_cs2_dumper.hpp"

#include<string>
#include<sstream>

void C_PlantedC4::Update() {
	if (!this->Address)return;
	this->m_bBombTicking = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bBombTicking);
	this->m_nBombSite = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_nBombSite);
	this->m_flC4Blow = *reinterpret_cast<GameTime_t*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flC4Blow);
	this->m_bHasExploded = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bHasExploded);
	this->m_flTimerLength = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flTimerLength);
	this->m_bBeingDefused = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bBeingDefused);
	this->m_flDefuseLength = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flDefuseLength);
	this->m_flDefuseCountDown = *reinterpret_cast<GameTime_t*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flDefuseCountDown);
	this->m_bBombDefused = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bBombDefused);
	this->m_hBombDefuser = *reinterpret_cast<uint32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_hBombDefuser);
	this->m_fLastDefuseTime = *reinterpret_cast<GameTime_t*>(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_fLastDefuseTime);
}

void C_CSGameRules::Update() {
	this->m_bFreezePeriod = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bFreezePeriod);
	this->m_bWarmupPeriod = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bWarmupPeriod);
	this->m_fWarmupPeriodEnd = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fWarmupPeriodEnd);
	this->m_fWarmupPeriodStart = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fWarmupPeriodStart);
	this->m_bTerroristTimeOutActive = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bTerroristTimeOutActive);
	this->m_bCTTimeOutActive = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bCTTimeOutActive);
	this->m_flTerroristTimeOutRemaining = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flTerroristTimeOutRemaining);
	this->m_flCTTimeOutRemaining = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flCTTimeOutRemaining);
	this->m_nTerroristTimeOuts = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nTerroristTimeOuts);
	this->m_nCTTimeOuts = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nCTTimeOuts);
	this->m_bTechnicalTimeOut = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bTechnicalTimeOut);
	this->m_bMatchWaitingForResume = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMatchWaitingForResume);
	this->m_iRoundTime = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundTime);
	this->m_fMatchStartTime = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fMatchStartTime);
	this->m_fRoundStartTime = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fRoundStartTime);
	this->m_flRestartRoundTime = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flRestartRoundTime);
	this->m_bGameRestart = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bGameRestart);
	this->m_flGameStartTime = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flGameStartTime);
	this->m_timeUntilNextPhaseStarts = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_timeUntilNextPhaseStarts);
	this->m_gamePhase = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_gamePhase);
	this->m_totalRoundsPlayed = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_totalRoundsPlayed);
	this->m_nRoundsPlayedThisPhase = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundsPlayedThisPhase);
	this->m_nOvertimePlaying = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nOvertimePlaying);
	this->m_iHostagesRemaining = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iHostagesRemaining);
	this->m_bAnyHostageReached = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bAnyHostageReached);
	this->m_bMapHasBombTarget = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMapHasBombTarget);
	this->m_bMapHasRescueZone = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMapHasRescueZone);
	this->m_bMapHasBuyZone = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMapHasBuyZone);
	this->m_bIsQueuedMatchmaking = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsQueuedMatchmaking);
	this->m_nQueuedMatchmakingMode = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nQueuedMatchmakingMode);
	this->m_bIsValveDS = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsValveDS);
	this->m_bLogoMap = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bLogoMap);
	this->m_bPlayAllStepSoundsOnServer = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bPlayAllStepSoundsOnServer);
	this->m_iSpectatorSlotCount = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iSpectatorSlotCount);
	this->m_MatchDevice = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_MatchDevice);
	this->m_bHasMatchStarted = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bHasMatchStarted);
	this->m_nNextMapInMapgroup = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nNextMapInMapgroup);
	// 字符串类型需要特殊处理（此处省略具体实现）
	this->m_nTournamentPredictionsPct = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nTournamentPredictionsPct);
	this->m_flCMMItemDropRevealStartTime = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flCMMItemDropRevealStartTime);
	this->m_flCMMItemDropRevealEndTime = *reinterpret_cast<float*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flCMMItemDropRevealEndTime);
	this->m_bIsDroppingItems = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsDroppingItems);
	this->m_bIsQuestEligible = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsQuestEligible);
	this->m_bIsHltvActive = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsHltvActive);
	// 数组类型需要循环读取（此处省略具体实现）
	this->m_numBestOfMaps = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_numBestOfMaps);
	this->m_nHalloweenMaskListSeed = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nHalloweenMaskListSeed);
	this->m_bBombDropped = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bBombDropped);
	this->m_bBombPlanted = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bBombPlanted);
	this->m_iRoundWinStatus = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundWinStatus);
	this->m_eRoundWinReason = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_eRoundWinReason);
	this->m_bTCantBuy = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bTCantBuy);
	this->m_bCTCantBuy = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bCTCantBuy);
	// 数组类型需要循环读取（此处省略具体实现）
	this->m_iRoundEndTimerTime = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundEndTimerTime);
	// 字符串和特殊类型需要特殊处理（此处省略具体实现）
	this->m_iRoundEndPlayerCount = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundEndPlayerCount);
	this->m_bRoundEndNoMusic = *reinterpret_cast<bool*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bRoundEndNoMusic);
	this->m_iRoundEndLegacy = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundEndLegacy);
	this->m_nRoundEndCount = *reinterpret_cast<uint8_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundEndCount);
	this->m_iRoundStartRoundNumber = *reinterpret_cast<int32_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundStartRoundNumber);
	this->m_nRoundStartCount = *reinterpret_cast<uint8_t*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundStartCount);
	this->m_flLastPerfSampleTime = *reinterpret_cast<double*>(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flLastPerfSampleTime);
}

std::ostringstream C_PlayerController::GetMsg()const {
	std::ostringstream oss;
	oss << "   名称:   " << this->m_iszPlayerName;
	return oss;
}
std::ostringstream C_CameraServices::GetMsg()const {
	std::ostringstream oss;
	oss << " FOV:" << this->iFOV;
	return oss;
}
std::ostringstream C_GameSceneNode::GetMsg()const {
	std::ostringstream oss;
	if (this->pPosition) {
		oss << "   坐标: X:" << this->pPosition->x << "   Y:" << this->pPosition->y << "   Z:" << this->pPosition->z
			<< "   角度: X:" << this->pRotationEuler->x << "   Y:" << this->pRotationEuler->y << "   Z:" << this->pRotationEuler->z;
	}
	return oss;
}
std::ostringstream C_Pawn::GetMsg()const {
	std::ostringstream oss;
	//oss << this->GameSceneNode.GetMsg().str() << " 阵营:" << this->m_iTeamNum;
	oss << "   坐标: X:" << this->m_vOldOrigin.x << "   Y:" << this->m_vOldOrigin.y << "   Z:" << this->m_vOldOrigin.z
		<< "   角度: X:" << this->m_angEyeAngles.x << "   Y:" << this->m_angEyeAngles.y << "   Z:" << this->m_angEyeAngles.z
		<< "   阵营:" << this->m_iTeamNum << "   血量：" << this->m_iHealth
		<< "   HUD: " << this->m_iHideHUD;
	return oss;
}
DirectX::XMFLOAT3 C_Pawn::GetOriginPos()const {
	return *this->GameSceneNode.pPosition;
}
DirectX::XMFLOAT3 C_Pawn::GetEyePos()const {
	DirectX::XMFLOAT3 ViewOffset = *reinterpret_cast<DirectX::XMFLOAT3*>(this->Address + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);
	return this->GetOriginPos() + ViewOffset;
}
std::ostringstream C_Entity::GetMsg()const {
	std::ostringstream oss;
	oss << this->Pawn.GetMsg().str() << this->Controller.GetMsg().str();
	return oss;
}
std::ostringstream C_Local::GetMsg()const {
	std::ostringstream oss;
	oss << "   本地玩家信息:" << this->Player.GetMsg().str()
		<< "   HUD信息： " << this->Player.Pawn.m_iHideHUD;
	return oss;
}

void C_GlobalVars::Update() {
	this->Address = Address;
	this->RealTime = *reinterpret_cast<float*>(this->Address + 0x00);
	this->CurrentTime = *reinterpret_cast<float*>(this->Address + 0x30);
	this->TickCount = *reinterpret_cast<int*>(this->Address + 0x44);

	return;
}
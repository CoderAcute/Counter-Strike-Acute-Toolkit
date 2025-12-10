#pragma once
#include<cstdint>
#include<DirectXMath.h>
#include<string>
#include<vector>
#include<optional>

typedef float GameTime_t;
typedef uint32_t ObserverMode_t;

class C_PlantedC4 {
public:
    uintptr_t Address;

    bool m_bBombTicking; // bool
    bool m_nBombSite; // int32
    //constexpr std::ptrdiff_t m_nSourceSoundscapeHash = 0x1168; // int32
    //constexpr std::ptrdiff_t m_entitySpottedState = 0x1170; // EntitySpottedState_t
    //constexpr std::ptrdiff_t m_flNextGlow = 0x1188; // GameTime_t
    //constexpr std::ptrdiff_t m_flNextBeep = 0x118C; // GameTime_t
    GameTime_t m_flC4Blow; // GameTime_t
    //constexpr std::ptrdiff_t m_bCannotBeDefused = 0x1194; // bool
    bool m_bHasExploded; // bool
    float m_flTimerLength; // float32
    bool m_bBeingDefused; // bool
    //constexpr std::ptrdiff_t m_bTriggerWarning = 0x11A0; // float32
    //constexpr std::ptrdiff_t m_bExplodeWarning = 0x11A4; // float32
    //constexpr std::ptrdiff_t m_bC4Activated = 0x11A8; // bool
    //constexpr std::ptrdiff_t m_bTenSecWarning = 0x11A9; // bool
    float m_flDefuseLength; // float32
    GameTime_t m_flDefuseCountDown; // GameTime_t
    bool m_bBombDefused; // bool
    uint32_t m_hBombDefuser; // CHandle<C_CSPlayerPawn>
    //constexpr std::ptrdiff_t m_AttributeManager = 0x11C0; // C_AttributeContainer
    //constexpr std::ptrdiff_t m_hDefuserMultimeter = 0x1698; // CHandle<C_Multimeter>
    //constexpr std::ptrdiff_t m_flNextRadarFlashTime = 0x169C; // GameTime_t
    //constexpr std::ptrdiff_t m_bRadarFlash = 0x16A0; // bool
    //constexpr std::ptrdiff_t m_pBombDefuser = 0x16A4; // CHandle<C_CSPlayerPawn>
    GameTime_t m_fLastDefuseTime; // GameTime_t
    //constexpr std::ptrdiff_t m_pPredictionOwner = 0x16B0; // CBasePlayerController*
    //constexpr std::ptrdiff_t m_vecC4ExplodeSpectatePos = 0x16B8; // Vector
    //constexpr std::ptrdiff_t m_vecC4ExplodeSpectateAng = 0x16C4; // QAngle
    //constexpr std::ptrdiff_t m_flC4ExplodeSpectateDuration = 0x16D0; // float32

    void Update();
};

class C_CSGameRules {
public:
    uintptr_t Address;

    bool m_bFreezePeriod; // bool
    bool m_bWarmupPeriod; // bool
    GameTime_t m_fWarmupPeriodEnd; // GameTime_t
    GameTime_t m_fWarmupPeriodStart; // GameTime_t
    bool m_bTerroristTimeOutActive; // bool
    bool m_bCTTimeOutActive; // bool
    float m_flTerroristTimeOutRemaining; // float32
    float m_flCTTimeOutRemaining; // float32
    int32_t m_nTerroristTimeOuts; // int32
    int32_t m_nCTTimeOuts; // int32
    bool m_bTechnicalTimeOut; // bool
    bool m_bMatchWaitingForResume; // bool
    int32_t m_iRoundTime; // int32
    float m_fMatchStartTime; // float32
    GameTime_t m_fRoundStartTime; // GameTime_t
    GameTime_t m_flRestartRoundTime; // GameTime_t
    bool m_bGameRestart; // bool
    float m_flGameStartTime; // float32
    float m_timeUntilNextPhaseStarts; // float32
    int32_t m_gamePhase; // int32
    int32_t m_totalRoundsPlayed; // int32
    int32_t m_nRoundsPlayedThisPhase; // int32
    int32_t m_nOvertimePlaying; // int32
    int32_t m_iHostagesRemaining; // int32
    bool m_bAnyHostageReached; // bool
    bool m_bMapHasBombTarget; // bool
    bool m_bMapHasRescueZone; // bool
    bool m_bMapHasBuyZone; // bool
    bool m_bIsQueuedMatchmaking; // bool
    int32_t m_nQueuedMatchmakingMode; // int32
    bool m_bIsValveDS; // bool
    bool m_bLogoMap; // bool
    bool m_bPlayAllStepSoundsOnServer; // bool
    int32_t m_iSpectatorSlotCount; // int32
    int32_t m_MatchDevice; // int32
    bool m_bHasMatchStarted; // bool
    int32_t m_nNextMapInMapgroup; // int32
    //char m_szTournamentEventName[512]; // char[512]
    //char m_szTournamentEventStage[512]; // char[512]
    //char m_szMatchStatTxt[512]; // char[512]
    //char m_szTournamentPredictionsTxt[512]; // char[512]
    int32_t m_nTournamentPredictionsPct; // int32
    GameTime_t m_flCMMItemDropRevealStartTime; // GameTime_t
    GameTime_t m_flCMMItemDropRevealEndTime; // GameTime_t
    bool m_bIsDroppingItems; // bool
    bool m_bIsQuestEligible; // bool
    bool m_bIsHltvActive; // bool
    //uint16_t m_arrProhibitedItemIndices[100]; // uint16[100]
    //uint32_t m_arrTournamentActiveCasterAccounts[4]; // uint32[4]
    int32_t m_numBestOfMaps; // int32
    int32_t m_nHalloweenMaskListSeed; // int32
    bool m_bBombDropped; // bool
    bool m_bBombPlanted; // bool
    int32_t m_iRoundWinStatus; // int32
    int32_t m_eRoundWinReason; // int32
    bool m_bTCantBuy; // bool
    bool m_bCTCantBuy; // bool
    //int32_t m_iMatchStats_RoundResults[30]; // int32[30]
    //int32_t m_iMatchStats_PlayersAlive_CT[30]; // int32[30]
    //int32_t m_iMatchStats_PlayersAlive_T[30]; // int32[30]
    //float m_TeamRespawnWaveTimes[32]; // float32[32]
    //GameTime_t m_flNextRespawnWave[32]; // GameTime_t[32]
    //Vector m_vMinimapMins; // Vector
    //Vector m_vMinimapMaxs; // Vector
    //float m_MinimapVerticalSectionHeights[8]; // float32[8]
    uint64_t m_ullLocalMatchID; // uint64
    //int32_t m_nEndMatchMapGroupVoteTypes[10]; // int32[10]
    //int32_t m_nEndMatchMapGroupVoteOptions[10]; // int32[10]
    int32_t m_nEndMatchMapVoteWinner; // int32
    int32_t m_iNumConsecutiveCTLoses; // int32
    int32_t m_iNumConsecutiveTerroristLoses; // int32
    int32_t m_nMatchAbortedEarlyReason; // int32
    bool m_bHasTriggeredRoundStartMusic; // bool
    bool m_bSwitchingTeamsAtRoundReset; // bool
    //class CCSGameModeRules* m_pGameModeRules; // CCSGameModeRules*
    //class C_RetakeGameRules m_RetakeRules; // C_RetakeGameRules
    uint8_t m_nMatchEndCount; // uint8
    int32_t m_nTTeamIntroVariant; // int32
    int32_t m_nCTTeamIntroVariant; // int32
    bool m_bTeamIntroPeriod; // bool
    int32_t m_iRoundEndWinnerTeam; // int32
    int32_t m_eRoundEndReason; // int32
    bool m_bRoundEndShowTimerDefend; // bool
    int32_t m_iRoundEndTimerTime; // int32
    //class CUtlString m_sRoundEndFunFactToken; // CUtlString
    //CPlayerSlot m_iRoundEndFunFactPlayerSlot; // CPlayerSlot
    int32_t m_iRoundEndFunFactData1; // int32
    int32_t m_iRoundEndFunFactData2; // int32
    int32_t m_iRoundEndFunFactData3; // int32
    //class CUtlString m_sRoundEndMessage; // CUtlString
    int32_t m_iRoundEndPlayerCount; // int32
    bool m_bRoundEndNoMusic; // bool
    int32_t m_iRoundEndLegacy; // int32
    uint8_t m_nRoundEndCount; // uint8
    int32_t m_iRoundStartRoundNumber; // int32
    uint8_t m_nRoundStartCount; // uint8
    double m_flLastPerfSampleTime; // float64

    void Update();
};

class C_PlayerController {
public:
    std::ostringstream GetMsg()const;

    uintptr_t Address{};
    uint32_t hPawn{};
    char m_iszPlayerName[128];
    uint32_t* m_ipDesiredFOV{};
};

class C_CameraServices {
public:
    std::ostringstream GetMsg()const;

    uintptr_t Address{};
    int* pFOV{};
    uint32_t* pFOVStart; // uint32
    float* pFOVTime; // GameTime_t
    float* pFOVRate; // float32
    //constexpr std::ptrdiff_t m_hZoomOwner = 0x298; // CHandle<C_BaseEntity>
    float* pLastShotFOV; // float32
    uint32_t iFOV{};
};

class C_GameSceneNode {
public:
    std::ostringstream GetMsg()const;

    uintptr_t Address{};
    DirectX::XMFLOAT3* pPosition{};
    DirectX::XMFLOAT3* pRotationEuler{};
};

class C_ObserverServices {
public:
    uintptr_t Address{};

    uint8_t* m_iObserverMode; // uint8
    uintptr_t m_hObserverTarget; // CHandle<C_BaseEntity>
    ObserverMode_t* m_iObserverLastMode; // ObserverMode_t
    bool* m_bForcedObserverMode; // bool
    float* m_flObserverChaseDistance; // float32
    GameTime_t* m_flObserverChaseDistanceCalcTime; // GameTime_t
};

class C_Pawn {
public:
    std::ostringstream GetMsg()const;

    uintptr_t Address{};
    C_CameraServices CameraServices{};
    C_GameSceneNode GameSceneNode{};
    C_ObserverServices m_pObserverServices{};
    int m_iTeamNum;
    int m_iHealth;
    uint32_t m_iHideHUD; // uint32
    uint32_t* p_iHideHUD; // uint32

    DirectX::XMFLOAT3 m_vOldOrigin;
    DirectX::XMFLOAT3 m_angEyeAngles;

    DirectX::XMFLOAT3 GetOriginPos()const;
    DirectX::XMFLOAT3 GetEyePos()const;
};

class C_Entity {
public:
    int Index;

    std::ostringstream GetMsg()const;

    C_PlayerController Controller{};
    C_Pawn Pawn{};

    C_Pawn& GetPawn() {
        return this->Pawn;
    }
    C_PlayerController& GetController() {
        return this->Controller;
    }
};

class C_GlobalVars {
public:
    uintptr_t Address;

    float RealTime{};
    int FrameCount{};
    int MaxClients{};
    float IntervalPerTick{};
    float CurrentTime{};
    int TickCount{};
    float IntervalPerTick2{};
    char* CurrentMapName = nullptr;

    void Update();
};

class C_Local {
public:
    std::ostringstream GetMsg()const;

    uintptr_t dwEntityList{};
    C_GlobalVars GlobalVars{};
	C_PlantedC4 PlantedC4{};
    C_Entity Player{};
	C_CSGameRules CSGameRules{};
    DirectX::XMFLOAT3* dwViewAngles{};
    float* dwViewMatrix;

    float* pGlobalFOV = nullptr;
};

class C_EntityList {
public:
	std::ostringstream GetMsg()const;
private:
	std::vector<C_Entity> Entitys;
public:
    C_Entity& at(size_t index) {
        return Entitys.at(index);
	}
    void push_back(const C_Entity& entity) {
        Entitys.push_back(entity);
    }
    void push_back(C_Entity&& entity) {
        Entitys.push_back(entity);
    }
    void clear() {
        Entitys.clear();
    }
    size_t size() const {
        return Entitys.size();
	}
    bool empty() const {
        return Entitys.empty();
	}
    C_Entity GetEntity(size_t Index) {
        return this->Entitys[Index];
    }
};
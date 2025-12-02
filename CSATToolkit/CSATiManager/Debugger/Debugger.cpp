#pragma once

#include"../../ThirdParty/All_ImGui.hpp"

#include"Debugger.hpp"

#include"../CSATiManager.hpp"
#include"../CSController/CSController.hpp"
#include"../CameraSystem/ICameraSystem.hpp"
#include"../KeyTracker/KeyTracker.hpp"
#include"../GlobalVars/GlobalVars.hpp"
#include <bitset>

bool Debugger::Init() {
    return true;
}



//// 检查所有点是否都在屏幕内
//bool allPointsVisible = true;
//    for (int i = 0; i < 5; i++) {
//    if (points[i].x < 0 || points[i].y < 0) {
//        allPointsVisible = false;
//        break;
//    }
//}
//
//if (!allPointsVisible) {
//    // 如果有任何点在屏幕外，可以选择不绘制或只绘制可见部分
//    return;
//}


void Debugger::Menu() {
    if (!this->CSATi->GlobalVars().DebugMode) {
		ImGui::Text("调试模式未启用，无法使用该功能，请前往控制台打开");
        return;
    }
    if (!this->CSATi->CS().SafeRead)return;
    // 显示当前位置和旋转
    ImGui::Text("本地位置: X: %.6f, Y: %.6f, Z: %.6f",
        this->CSATi->CS().CSGetPosition().x,
        this->CSATi->CS().CSGetPosition().y,
        this->CSATi->CS().CSGetPosition().z);
    ImGui::Text("本地欧拉角: Pitch: %.6f, Yaw: %.6f, Roll: %.6f",
        this->CSATi->CS().CSGetRotationEuler().x,
        this->CSATi->CS().CSGetRotationEuler().y,
        this->CSATi->CS().CSGetRotationEuler().z);

    int FOV = this->CSATi->CS().CSGetFov();
    ImGui::Text("FOV:%d", FOV);
    
    ImGui::Text("本地控制器期望FOV： %d", *this->CSATi->CS().Local.Player.Controller.m_ipDesiredFOV);
	static int NewFOV = FOV;
	ImGui::InputInt("新FOV", &NewFOV);
    if(ImGui::Button("应用FOV"))
    {
        *this->CSATi->CS().Local.Player.Controller.m_ipDesiredFOV = NewFOV;
	}

    static DirectX::XMFLOAT3 TestPosition;
    static DirectX::XMFLOAT3 TestRotationEuler;
    static DirectX::XMFLOAT4 TestRotation4;

    // 检测K键按下
    if (ImGui::IsKeyPressed(ImGuiKey_K)) {
        TestPosition = this->CSATi->CS().CSGetPosition();
        TestRotationEuler = this->CSATi->CS().CSGetRotationEuler();
        CSATMath::CSEulerToQuat(TestRotationEuler, TestRotation4);
    }

    // 显示存储的测试值
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "按下K存储数值:");
    ImGui::Text("新位置: X: %.6f, Y: %.6f, Z: %.6f",
        TestPosition.x, TestPosition.y, TestPosition.z);
    ImGui::Text("新欧拉角: X: %.6f, Y: %.6f, Z: %.6f",
        TestRotationEuler.x, TestRotationEuler.y, TestRotationEuler.z);
    ImGui::Text("新四元数: X: %.6f, Y: %.6f, Z: %.6f, W: %.6f",
        TestRotation4.x, TestRotation4.y, TestRotation4.z, TestRotation4.w);

    DirectX::XMFLOAT3 back{};
	CSATMath::CSQuatToEuler(TestRotation4, back);
    ImGui::Text("回转欧拉角：X: %.6f, Y: %.6f, Z: %.6f",
        back.x, back.y, back.z);


    
    ImGui::Separator();
    ImGui::Separator();
    ImGui::Separator();

    static float CameraHigh = 20.0;
    static float CameraX = 30.0;
    static float CameraY = 15.0;
    static float AxisLenth = 10.0;

    ImGui::SliderFloat("摄像机高度", &CameraHigh, 1.0, 500.0);
    ImGui::SliderFloat("摄像机长度", &CameraX, 1.0, 500.0);
    ImGui::SliderFloat("摄像机宽度", &CameraY, 1.0, 500.0);
    ImGui::SliderFloat("摄像机坐标轴长度", &AxisLenth, 1.0, 100.0);

    ImGui::SliderFloat3("位置", &TestPosition.x, -2000.0, 2000, 0);
    ImGui::SliderFloat("俯仰角", &TestRotationEuler.x, -89.0, 89.0);
    ImGui::SliderFloat("偏航角", &TestRotationEuler.y, -179.0, 179.0);
    ImGui::SliderFloat("滚转角", &TestRotationEuler.z, -179.0, 179.0);

    static ImVec4 cameraColourVec = ImVec4(1.0f, 0.0f, 1.0f, 1.0f); // RGBA，每个分量0-1
    ImGui::ColorEdit4("相机颜色", (float*)&cameraColourVec);

    

    // 将ImVec4转换为ImU32
    ImU32 cameraColour = IM_COL32(
        (int)(cameraColourVec.x * 255),
        (int)(cameraColourVec.y * 255),
        (int)(cameraColourVec.z * 255),
        (int)(cameraColourVec.w * 255)
    );

    //this->CSATi->CameraSystem().CameraDrawer.Init(CameraHigh, CameraX, CameraY, AxisLenth, cameraColour);
    //this->CSATi->CameraSystem().CameraDrawer.Update(this->CSATi->CS().CSGetMatrix(), this->CSATi->CS().CSGetScreenWide(), this->CSATi->CS().CSGetScreenHigh());
    //this->CSATi->CameraSystem().CameraDrawer.DrawCamera(TestPosition, TestRotationEuler, "旧测试");
    this->CSATi->ICameraSystem().ResetCameraModule(CameraHigh, CameraX, CameraY, AxisLenth, cameraColour);
    this->CSATi->ICameraSystem().DrawCameraByPAR(TestPosition, TestRotationEuler, "测试摄像机");
    if (ImGui::Button("摄像机系统接口测试")) {
        this->CSATi->ICameraSystem().test();
        
    }
    ImGui::Separator();
    ImGui::Separator();
    ImGui::Separator();
    
    
    ImGui::Text(("OB模式：" + std::to_string(*this->CSATi->CS().Local.Player.Pawn.m_pObserverServices.m_iObserverMode)).c_str());
    ImGui::Text(("m_hObserverTarget：" + std::to_string(this->CSATi->CS().Local.Player.Pawn.m_pObserverServices.m_hObserverTarget)).c_str());
    ImGui::Text(("m_iObserverLastMode：" + std::to_string(*this->CSATi->CS().Local.Player.Pawn.m_pObserverServices.m_iObserverLastMode)).c_str());
    ImGui::Text(("m_bForcedObserverMode：" + std::to_string(*this->CSATi->CS().Local.Player.Pawn.m_pObserverServices.m_bForcedObserverMode)).c_str());
    ImGui::Text(("m_flObserverChaseDistance：" + std::to_string(*this->CSATi->CS().Local.Player.Pawn.m_pObserverServices.m_flObserverChaseDistance)).c_str());
    ImGui::Text(("m_flObserverChaseDistanceCalcTime：" + std::to_string(*this->CSATi->CS().Local.Player.Pawn.m_pObserverServices.m_flObserverChaseDistanceCalcTime)).c_str());


    static int TargetOBMode = 4;
    ImGui::InputInt("目标模式", &TargetOBMode);
    static bool OBModeOverride = false;
    ImGui::Checkbox("强制覆盖", &OBModeOverride);
    if (OBModeOverride) {
        *this->CSATi->CS().Local.Player.Pawn.m_pObserverServices.m_iObserverMode = static_cast<uint8_t>(TargetOBMode);
    }
    
    return;
}
void Debugger::VirtualMain() {
    return;
}



void Debugger::ResetMaxMsgCount(const int Max) {
    if (Max < 1) {
		this->AddError("最大信息条数不能小于一1!");
        return;
    }
    if (DebugMsg.size() > Max) {
        DebugMsg.erase(DebugMsg.begin(), DebugMsg.end() - Max);
    }
	this->MaxMsgCount = Max;
}

void Debugger::PushBack(const std::string& NewMsg, const std::string& prefix) {
    // 检查字符串是否包含换行符
    if (NewMsg.find('\n') == std::string::npos && NewMsg.find('\r') == std::string::npos) {
        // 没有换行符，直接添加（注意：这里需要加上前缀）
        if (this->DebugMsg.size() == this->MaxMsgCount) {
            this->DebugMsg.pop_front();
        }
        this->DebugMsg.push_back(prefix + NewMsg);
    }
    else {
        // 有换行符，分割字符串并为每一行添加前缀
        std::istringstream iss(NewMsg);
        std::string line;
        bool firstLine = true;
        int lineCount = 0;

        while (std::getline(iss, line)) {
            // 清理回车符
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            // 跳过空行
            if (line.empty()) continue;

            std::string formattedLine;
            if (firstLine) {
                // 第一行直接添加前缀
                formattedLine = prefix + line;
                firstLine = false;
            }
            else {
                // 后续行添加前缀//和缩进
                formattedLine = prefix + line;
            }

            // 添加到消息队列
            if (this->DebugMsg.size() == this->MaxMsgCount) {
                this->DebugMsg.pop_front();
            }
            this->DebugMsg.push_back(formattedLine);
            lineCount++;
        }
    }

    if (AutoScroll) {
        this->NeedAutoScroll = true;
    }
    return;
}

void Debugger::AddInfo(const std::string& NewMsg) {
    this->PushBack(NewMsg, this->Info);
}

void Debugger::AddSucc(const std::string& NewMsg) {
    this->PushBack(NewMsg, this->Succ);
}

void Debugger::AddWarning(const std::string& NewMsg) {
    this->PushBack(NewMsg, this->Warning);
}

void Debugger::AddError(const std::string& NewMsg) {
    this->PushBack(NewMsg, this->Error);
    if(this->ShowWhenError){
        this->IfShowWindow = true;
        this->IfShowStream = true;
    }
}


void Debugger::Windows() {
    if (!this->IfShowWindow)return;
    // 设置窗口属性
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    // 创建独立窗口
    ImGui::Begin("调试器", &this->IfShowWindow);

    if (ImGui::BeginTabBar("Debuger")) {
        
        if (ImGui::BeginTabItem("调试信息")) {
            this->MenuStream();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("监控")) {
            this->MenuMonitor();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
 
    ImGui::End();  
}

void Debugger::ShowWindow() {
    this->IfShowWindow = true;
}

void Debugger::HideWindow() {
    this->IfShowWindow = false;
}

void Debugger::MenuStream() {
    // 在标签页内创建一个子窗口
    ImVec2 childSize = ImGui::GetContentRegionAvail();
    childSize.y -= ImGui::GetStyle().ItemSpacing.y; // 留出一点空间

	
    // 开始子窗口，占据标签页的剩余空间
    ImGui::BeginChild("信息", childSize, true, ImGuiWindowFlags_HorizontalScrollbar);

    // 使用虚拟列表优化性能
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(DebugMsg.size()));
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            const auto& msg = DebugMsg[i];

            // 根据消息类型着色
            if (msg.find(Info) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 50, 255, 255));
            }
            else if (msg.find(Succ) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 100, 0, 255));
            }
            else if (msg.find(Warning) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 0, 255));
            }
            else if (msg.find(Error) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 50, 50, 255));
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
            }

            ImGui::TextUnformatted(msg.c_str());

            //弹出
            ImGui::PopStyleColor();
        }
    }

    // 自动滚动到最新消息
    if (this->NeedAutoScroll) {
        ImGui::SetScrollHereY(1.0f);
		this->NeedAutoScroll = false;
    }

    // 结束子窗口
    ImGui::EndChild();
}

void Debugger::ShowStream() {
    this->IfShowStream = true;
}

void Debugger::HideStream() {
    this->IfShowStream = false;
}

void Debugger::MenuMonitor() {
    
	ImGui::Text("调试监控");
    static int Result = 0;
    Result = this->CSATi->CS().GetMsgResult;
    ImGui::Text("CSController信息获取返回值: %d", Result);
    if (Result < 0) {
        this->AddError("成功捕获一次内核错误！");
    }
    if (!this->CSATi->GlobalVars().DebugMode) {
        ImGui::Text("调试模式未启用，无法使用该功能，请前往控制台打开");
        return;
    }
    ImGui::Text("新时间测试");
    float NewTime = this->CSATi->CS().CurrentTime30;
    ImGui::Text("%f", NewTime);
    static bool recordtime = false;
    ImGui::Checkbox("记录时间", &recordtime);
    if (recordtime) {
        this->AddInfo(std::to_string(NewTime));
    }
    ImGui::Separator();

    ImGui::Text("本地测试");
    std::ostringstream Msg = this->CSATi->CS().GetLocalPlayerMsg();
    ImGui::Text(Msg.str().c_str());
	static bool OverrideHideHUD = false;
	ImGui::Checkbox("覆盖HUD设置", &OverrideHideHUD);
    static int HideHUDValue = 0;
	//显示二进制的HideHUD值
	uint32_t HideHUDBinary = this->CSATi->CS().Local.Player.Pawn.m_iHideHUD;
	ImGui::Text("当前HideHUD二进制值: %s", std::bitset<32>(HideHUDBinary).to_string().c_str());
	ImGui::InputInt("HideHUD值", &HideHUDValue);
    static int Index = 1;
	ImGui::InputInt("实体列表索引", &Index);
    if(OverrideHideHUD){
        try {
            *this->CSATi->CS().EntityList.at(Index).Pawn.p_iHideHUD = HideHUDValue;
        }
        catch (...) {

        }
	}
    //return;
    ImGui::Separator();
    ImGui::Separator();

    ImGui::Text("当前时间： %f", this->CSATi->CS().CSGetCurrentTime());
    ImGui::Text("RenderTime： %f", this->CSATi->CS().CSGetRenderTime());
    ImGui::Text("RenderTick： %d", this->CSATi->CS().CSGetRenderTick());

    ImGui::Separator();
    ImGui::Separator();
    // 基本游戏状态
	C_CSGameRules CSGameRules = this->CSATi->CS().GetCSGameRules();

    ImGui::Text("冻结期： %s", CSGameRules.m_bFreezePeriod ? "是" : "否");
    ImGui::Text("热身期： %s", CSGameRules.m_bWarmupPeriod ? "是" : "否");
    ImGui::Separator();
    ImGui::Text("热身开始时间： %f", CSGameRules.m_fWarmupPeriodStart);
    ImGui::Text("热身结束时间： %f", CSGameRules.m_fWarmupPeriodEnd);
    static int TargetSkipTickForRenderTick = 0;
	ImGui::InputInt("TargetSkipTickForRenderTick", &TargetSkipTickForRenderTick);
    if(ImGui::Button("应用TargetSkipTickForRenderTick")){
        static int TickSchemaed = 0;
        TickSchemaed = TargetSkipTickForRenderTick - CSGameRules.m_fWarmupPeriodEnd * 64;
		this->CSATi->CS().Execute(("demo_gototick " + std::to_string(TickSchemaed)).c_str());
	}
    ImGui::Separator();

    ImGui::Text("T方暂停： %s", CSGameRules.m_bTerroristTimeOutActive ? "是" : "否");
    ImGui::Text("CT方暂停： %s", CSGameRules.m_bCTTimeOutActive ? "是" : "否");
    ImGui::Text("T方暂停剩余： %f", CSGameRules.m_flTerroristTimeOutRemaining);
    ImGui::Text("CT方暂停剩余： %f", CSGameRules.m_flCTTimeOutRemaining);
    ImGui::Text("T方暂停次数： %d", CSGameRules.m_nTerroristTimeOuts);
    ImGui::Text("CT方暂停次数： %d", CSGameRules.m_nCTTimeOuts);
    ImGui::Text("技术暂停： %s", CSGameRules.m_bTechnicalTimeOut ? "是" : "否");
    ImGui::Text("比赛等待恢复： %s", CSGameRules.m_bMatchWaitingForResume ? "是" : "否");
    ImGui::Separator();


    // 回合时间信息
    ImGui::Text("回合在未安装炸弹的情况下最长储蓄秒数： %d", CSGameRules.m_iRoundTime);
    ImGui::Text("比赛开始时间： %f", CSGameRules.m_fMatchStartTime);
    ImGui::Text("回合开始时间： %f", CSGameRules.m_fRoundStartTime);
    ImGui::Text("重启回合时间： %f", CSGameRules.m_flRestartRoundTime);
    ImGui::Separator();


    ImGui::Text("游戏重启： %s", CSGameRules.m_bGameRestart ? "是" : "否");
    ImGui::Text("游戏开始时间： %f", CSGameRules.m_flGameStartTime);
    ImGui::Separator();


    ImGui::Text("下一阶段开始时间： %f", CSGameRules.m_timeUntilNextPhaseStarts);
    ImGui::Text("游戏阶段： %d", CSGameRules.m_gamePhase);
    ImGui::Separator();


    // 回合计数信息
    ImGui::Text("总已完成回合数： %d", CSGameRules.m_totalRoundsPlayed);
    ImGui::Text("当前阶段已完成回合数： %d", CSGameRules.m_nRoundsPlayedThisPhase);
    ImGui::Text("加时回合： %d", CSGameRules.m_nOvertimePlaying);
    ImGui::Separator();


    ImGui::Text("剩余人质： %d", CSGameRules.m_iHostagesRemaining);
    ImGui::Text("有人质到达： %s", CSGameRules.m_bAnyHostageReached ? "是" : "否");
    ImGui::Separator();


    // 地图信息
    ImGui::Text("有爆破区： %s", CSGameRules.m_bMapHasBombTarget ? "是" : "否");
    ImGui::Text("有救援区： %s", CSGameRules.m_bMapHasRescueZone ? "是" : "否");
    ImGui::Text("有购买区： %s", CSGameRules.m_bMapHasBuyZone ? "是" : "否");
    ImGui::Separator();


    // 比赛设置
    ImGui::Text("m_bIsQueuedMatchmaking： %s", CSGameRules.m_bIsQueuedMatchmaking ? "是" : "否");
    ImGui::Text("m_nQueuedMatchmakingMode： %d", CSGameRules.m_nQueuedMatchmakingMode);
    ImGui::Text("Valve服务器： %s", CSGameRules.m_bIsValveDS ? "是" : "否");
    ImGui::Text("Logo地图： %s", CSGameRules.m_bLogoMap ? "是" : "否");
    ImGui::Text("m_bPlayAllStepSoundsOnServer： %s", CSGameRules.m_bPlayAllStepSoundsOnServer ? "是" : "否");
    ImGui::Text("m_iSpectatorSlotCount： %d", CSGameRules.m_iSpectatorSlotCount);
    ImGui::Text("m_MatchDevice： %d", CSGameRules.m_MatchDevice);
    ImGui::Text("比赛已开始： %s", CSGameRules.m_bHasMatchStarted ? "是" : "否");
    ImGui::Text("m_nNextMapInMapgroup： %d", CSGameRules.m_nNextMapInMapgroup);
    ImGui::Separator();


    // 炸弹状态
    ImGui::Text("炸弹掉落： %s", CSGameRules.m_bBombDropped ? "是" : "否");
    ImGui::Text("炸弹已安置： %s", CSGameRules.m_bBombPlanted ? "是" : "否");
    ImGui::Separator();


    // 回合结束信息
    ImGui::Text("上回合总时间： %d", CSGameRules.m_iRoundEndTimerTime);
    ImGui::Text("上回合结束玩家数： %d", CSGameRules.m_iRoundEndPlayerCount);
    ImGui::Text("上回合结束无音乐： %s", CSGameRules.m_bRoundEndNoMusic ? "是" : "否");
    ImGui::Separator();


    
    ImGui::Text("回合开始编号： %d", CSGameRules.m_iRoundStartRoundNumber);
    ImGui::Text("回合开始计数： %d", CSGameRules.m_nRoundStartCount);
    ImGui::Text("回合结束计数： %d", CSGameRules.m_nRoundEndCount);
    ImGui::Separator();


    // 其他信息
    ImGui::Text("最后性能采样时间： %f", CSGameRules.m_flLastPerfSampleTime);


    //   if(ImGui::Button("重置回合时间")){
//       *this->CSATi->CS.p_m_flRestartRoundTime = this->CSATi->CS.CSGetRenderTime() + 5.0f;
//       this->AddInfo("已将回合重置时间设置为当前时间+5秒");
   //}


    /*this->C_CSGameRules.m_bBombDropped = *reinterpret_cast<bool*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bBombDropped);
    this->C_CSGameRules.m_bBombPlanted = *reinterpret_cast<bool*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bBombPlanted);

    this->C_CSGameRules.m_flRestartRoundTime = *reinterpret_cast<GameTime_t*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flRestartRoundTime);
    this->C_CSGameRules.m_fRoundStartTime = *reinterpret_cast<GameTime_t*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fRoundStartTime);
    this->C_CSGameRules.m_iRoundTime = *reinterpret_cast<int32_t*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundTime);
    this->C_CSGameRules.m_iRoundStartRoundNumber = *reinterpret_cast<int32_t*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundStartRoundNumber);
    this->C_CSGameRules.m_nRoundStartCount = *reinterpret_cast<uint8_t*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundStartCount);
    this->C_CSGameRules.m_nRoundsPlayedThisPhase = *reinterpret_cast<int32_t*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundsPlayedThisPhase);

    this->C_CSGameRules.m_timeUntilNextPhaseStarts = *reinterpret_cast<float*>(this->GameRules + cs2_dumper::schemas::client_dll::C_CSGameRules::m_timeUntilNextPhaseStarts);*/


    ImGui::Separator();
    ImGui::Separator();

    
    /*if (this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pPosition) {
        ImGui::Text("本地位置: X: %.6f, Y: %.6f, Z: %.6f",
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pPosition->x,
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pPosition->y,
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pPosition->z);
        ImGui::Text("本地欧拉角: Pitch: %.6f, Yaw: %.6f, Roll: %.6f",
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pRotationEuler->x,
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pRotationEuler->y,
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pRotationEuler->z);

        static float tePosition[3]{ 0,0,0 };
        ImGui::InputFloat3("修改本地位置", tePosition);
        if (ImGui::Button("修改")) {
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pPosition->x = tePosition[0];
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pPosition->y = tePosition[1];
            this->CSATi->CS().Local.Player.Pawn.GameSceneNode.pPosition->z = tePosition[2];
        }
    }
    else {
        ImGui::Text("本地信息空指针");
    }*/
    

    ImGui::Separator();
    ImGui::Separator();

    //return;

    
    for (size_t i = 0; i < 64; ++i) {
        try {
			C_Entity Entity = this->CSATi->CS().GetEntity(i);
            ImGui::Text(Entity.GetMsg().str().c_str());
        }
        catch (...) {
            continue;
        }

    }
    
}
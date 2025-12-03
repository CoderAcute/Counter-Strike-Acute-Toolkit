#include"ElementDebugger.hpp"

#include"../../../../CSATiManager.hpp"
#include"../../../../Debugger/IDebugger.hpp"
#include"../../../../AbstractLayer3D/IAbstractLayer3D.hpp"
#include"../../../CameraDrawer/CameraDrawer.hpp"
#include"../../../../KeyTracker/KeyTracker.hpp"
#include"../../../../IPCer/IPCer.hpp"


#include"../ElementManager.hpp"

void ElementDebugger::Init(CSATiManager* CSATi, CameraDrawer* CamDrawer, ElementManager* EManager) {
	this->CSATi = CSATi;
    this->AL3D = &this->CSATi->IAbstractLayer3D();
	this->CamDrawer = CamDrawer;
    this->EManager = EManager;

    return;
}

void ElementDebugger::DebugMenus(ElementBase* const pElement) {
    //根据元素类型调用不同的调试菜单
    switch (pElement->Type) {
    case ElementType::None:
        ImGui::Text("错误元素类型");
        break;
    case ElementType::ElementBase:
        ImGui::Text("元素基类，无法调试");
        break;
    case ElementType::FreeCameraPath:
        this->FreeCameraPath_DebugMenu(static_cast<FreeCameraPath*>(pElement));
        break;
	case ElementType::FirstPersonCameraPath:
        this->FirstPersonCameraPath_DebugMenu(static_cast<FirstPersonCameraPath*>(pElement));
		break;
    }
}
//FreeCameeraPath

void ElementDebugger::FreeCameraPath_DebugMenu(FreeCameraPath* const FreeCamPath) {
    ImGui::Text(FreeCamPath->GetMsg().c_str());

    if (ImGui::Button("添加关键帧") || this->CSATi->KT().CheckComboClick(VK_CAPITAL, 1)) {
        CSATMath::CameraKeyFrame Frame;
        Frame.KeyTime = this->AL3D->GetTime();
        Frame.SpatialState = this->AL3D->GetSpatialState();
        FreeCamPath->AddKeyframe(Frame);
    }

    if (ImGui::Button("清空关键帧") || this->CSATi->KT().CheckComboClick(VK_DELETE, 2)) {
        FreeCamPath->Clear();
    }

    if (ImGui::Button("标准化轨道")) {
        FreeCamPath->TimeNormalize();
    }

    if (ImGui::Button("预览轨道")) {
        this->EManager->Preview_SetElement(FreeCamPath->Name);
        this->EManager->Preview_SetPreviewSchema(this->AL3D->GetTime());
        this->EManager->Preview_Enable();
    }

    ImGui::Separator();
    ImGui::Separator();
    ImGui::Separator();


    static int IndexForReset = -1;
    static int PreIndex = -2;
    ImGui::InputInt("要调整的关键帧", &IndexForReset);

    if (0 <= IndexForReset && IndexForReset < FreeCamPath->CameraKeyFrames.size()) {
        const CSATMath::CameraKeyFrame& Frame = FreeCamPath->GetKeyFrame(IndexForReset);
        ImGui::Text(("关键帧信息： 编号： " + std::to_string(IndexForReset) + Frame.GetMsg()).c_str());

        ImGui::Separator();

        static float temptime{};
        static DirectX::XMFLOAT4 tempPositionAndFOV{};
        static DirectX::XMFLOAT3 tempRotationEuler{};
        if (IndexForReset != PreIndex) {
            temptime = Frame.KeyTime;
            tempPositionAndFOV = Frame.SpatialState.GetPositionAndFOV();
            tempRotationEuler = Frame.SpatialState.GetRotationEuler();
        }

        ImGui::SliderFloat("时间", &temptime, 0, 20000);

        ImGui::SliderFloat3("位置", &tempPositionAndFOV.x, -2000.0, 2000, 0);
        ImGui::SliderFloat("俯仰角", &tempRotationEuler.x, -89.0, 89.0);
        ImGui::SliderFloat("偏航角", &tempRotationEuler.y, -179.0, 179.0);
        ImGui::SliderFloat("滚转角", &tempRotationEuler.z, -40, 40);
        ImGui::SliderFloat("FOV", &tempPositionAndFOV.w, 10, 170);

        this->CamDrawer->DrawCamera(DirectX::XMFLOAT3{ tempPositionAndFOV.x,tempPositionAndFOV.y ,tempPositionAndFOV.z }, tempRotationEuler, "目标摄像机关键帧");
        if (ImGui::Button("修改")) {
            CSATMath::CameraKeyFrame tempKey;

            tempKey.KeyTime = temptime;

            tempKey.SpatialState.PositionAndFOV = DirectX::XMLoadFloat4(&tempPositionAndFOV);

            DirectX::XMFLOAT4 tempRotationQuat;
            CSATMath::CSEulerToQuat(tempRotationEuler, tempRotationQuat);
            tempKey.SpatialState.RotationQuat = DirectX::XMLoadFloat4(&tempRotationQuat);

            FreeCamPath->CameraKeyFrames.erase(FreeCamPath->CameraKeyFrames.begin() + IndexForReset);

            FreeCamPath->AddKeyframe(std::move(tempKey));
        }
        if (ImGui::Button("删除")) {
            FreeCamPath->CameraKeyFrames.erase(FreeCamPath->CameraKeyFrames.begin() + IndexForReset);
            FreeCamPath->Refresh();
        }
        if (ImGui::Button("复制")) {
            FreeCamPath->AddKeyframe(FreeCamPath->GetKeyFrame(IndexForReset));
        }
    }
    PreIndex = IndexForReset;
}

void ElementDebugger::FirstPersonCameraPath_DebugMenu(FirstPersonCameraPath* const FirstPersonCamPath) {
	ImGui::Text(FirstPersonCamPath->GetMsg().c_str());
    ImGui::Separator();
	static int TargetPlayerIndex = FirstPersonCamPath->TargetPlayerIndex;
    ImGui::SliderInt("目标玩家索引", &TargetPlayerIndex, 0, 10);
	if (ImGui::Button("应用索引")) {
        FirstPersonCamPath->TargetPlayerIndex = static_cast<uint8_t>(TargetPlayerIndex);
        FirstPersonCamPath->Refresh();
	}
    static float StartBuffer;
	ImGui::InputFloat("开始时间", &StartBuffer);
	static float EndBuffer;
	ImGui::InputFloat("结束时间", &EndBuffer);
    if (ImGui::Button("应用时间")) {
        FirstPersonCamPath->StartTime = StartBuffer;
        FirstPersonCamPath->EndTime = EndBuffer;
        FirstPersonCamPath->Refresh();
    }
    if (ImGui::Button("测试切换")) {
		this->AL3D->ExecuteCommand("spec_mode 2");
		this->AL3D->ExecuteCommand("spec_player " + std::to_string(FirstPersonCamPath->TargetPlayerIndex));
    }

    return;
}
#include "CameraDrawer.hpp"

void CameraModel::Init(float CameraHigh, float CameraX, float CameraY, float AxisLenth, ImU32 Colour) {
    this->Origin = { 0,0,0 };

    this->Lens[0] = { CameraHigh,-CameraX / 2,CameraY / 2 };
    this->Lens[1] = { CameraHigh,-CameraX / 2,-CameraY / 2 };
    this->Lens[2] = { CameraHigh,CameraX / 2,-CameraY / 2 };
    this->Lens[3] = { CameraHigh,CameraX / 2,CameraY / 2 };

    this->X = { AxisLenth,0,0 };
    this->Y = { 0,AxisLenth,0 };
    this->Z = { 0,0,AxisLenth };

    this->Colour = Colour;
}


void CameraDrawer::Init(float CameraHigh, float CameraX, float CameraY, float AxisLenth, ImU32 Colour) {
    this->CameraModel.Init(CameraHigh, CameraX, CameraY, AxisLenth, Colour);
}

void CameraDrawer::Rotate(DirectX::XMFLOAT3 Rotation) {
    this->Origin = CSATMath::RotatePoint(this->Origin, Rotation.x, Rotation.y, Rotation.z);

    this->Lens[0] = CSATMath::RotatePoint(this->Lens[0], Rotation.x, Rotation.y, Rotation.z);
    this->Lens[1] = CSATMath::RotatePoint(this->Lens[1], Rotation.x, Rotation.y, Rotation.z);
    this->Lens[2] = CSATMath::RotatePoint(this->Lens[2], Rotation.x, Rotation.y, Rotation.z);
    this->Lens[3] = CSATMath::RotatePoint(this->Lens[3], Rotation.x, Rotation.y, Rotation.z);

    this->X = CSATMath::RotatePoint(this->X, Rotation.x, Rotation.y, Rotation.z);
    this->Y = CSATMath::RotatePoint(this->Y, Rotation.x, Rotation.y, Rotation.z);
    this->Z = CSATMath::RotatePoint(this->Z, Rotation.x, Rotation.y, Rotation.z);
} 

void CameraDrawer::Move(DirectX::XMFLOAT3 TargetPoint) {
    CSATMath::MovePoint(this->Origin, TargetPoint);

    CSATMath::MovePoint(this->Lens[0], TargetPoint);
    CSATMath::MovePoint(this->Lens[1], TargetPoint);
    CSATMath::MovePoint(this->Lens[2], TargetPoint);
    CSATMath::MovePoint(this->Lens[3], TargetPoint);

    CSATMath::MovePoint(this->X, TargetPoint);
    CSATMath::MovePoint(this->Y, TargetPoint);
    CSATMath::MovePoint(this->Z, TargetPoint);
}

void CameraDrawer::ToScreen() {
    CSATMath::XMWorldToScreen(this->Origin, this->Origin2D, this->pMatrixPtr, this->WinWidth, this->WinHeight);

    CSATMath::XMWorldToScreen(this->Lens[0], this->Lens2D[0], this->pMatrixPtr, this->WinWidth, this->WinHeight);
    CSATMath::XMWorldToScreen(this->Lens[1], this->Lens2D[1], this->pMatrixPtr, this->WinWidth, this->WinHeight);
    CSATMath::XMWorldToScreen(this->Lens[2], this->Lens2D[2], this->pMatrixPtr, this->WinWidth, this->WinHeight);
    CSATMath::XMWorldToScreen(this->Lens[3], this->Lens2D[3], this->pMatrixPtr, this->WinWidth, this->WinHeight);

    CSATMath::XMWorldToScreen(this->X, this->X2D, this->pMatrixPtr, this->WinWidth, this->WinHeight);
    CSATMath::XMWorldToScreen(this->Y, this->Y2D, this->pMatrixPtr, this->WinWidth, this->WinHeight);
    CSATMath::XMWorldToScreen(this->Z, this->Z2D, this->pMatrixPtr, this->WinWidth, this->WinHeight);
}

void CameraDrawer::Draw(const char* label) {
    if (!this->IfDraw)return;

    this->drawList = ImGui::GetBackgroundDrawList();
    //绘制坐标系
    this->drawList->AddLine(ImVec2(this->Origin2D.x, this->Origin2D.y), ImVec2(this->X2D.x, this->X2D.y), IM_COL32(255, 0, 0, 255), 4.0f);
    this->drawList->AddLine(ImVec2(this->Origin2D.x, this->Origin2D.y), ImVec2(this->Y2D.x, this->Y2D.y), IM_COL32(0, 255, 0, 255), 4.0f);
    this->drawList->AddLine(ImVec2(this->Origin2D.x, this->Origin2D.y), ImVec2(this->Z2D.x, this->Z2D.y), IM_COL32(0, 0, 255, 255), 4.0f);
    //绘制原点到四个角的连线
    this->drawList->AddLine(ImVec2(this->Origin2D.x, this->Origin2D.y), ImVec2(this->Lens2D[0].x, this->Lens2D[0].y), this->Colour, 4.0f);
    this->drawList->AddLine(ImVec2(this->Origin2D.x, this->Origin2D.y), ImVec2(this->Lens2D[1].x, this->Lens2D[1].y), this->Colour, 4.0f);
    this->drawList->AddLine(ImVec2(this->Origin2D.x, this->Origin2D.y), ImVec2(this->Lens2D[2].x, this->Lens2D[2].y), this->Colour, 4.0f);
    this->drawList->AddLine(ImVec2(this->Origin2D.x, this->Origin2D.y), ImVec2(this->Lens2D[3].x, this->Lens2D[3].y), this->Colour, 4.0f);
    //绘制镜头面
    this->drawList->AddLine(ImVec2(this->Lens2D[0].x, this->Lens2D[0].y), ImVec2(this->Lens2D[1].x, this->Lens2D[1].y), this->Colour, 4.0f);
    this->drawList->AddLine(ImVec2(this->Lens2D[1].x, this->Lens2D[1].y), ImVec2(this->Lens2D[2].x, this->Lens2D[2].y), this->Colour, 4.0f);
    this->drawList->AddLine(ImVec2(this->Lens2D[2].x, this->Lens2D[2].y), ImVec2(this->Lens2D[3].x, this->Lens2D[3].y), this->Colour, 4.0f);
    this->drawList->AddLine(ImVec2(this->Lens2D[3].x, this->Lens2D[3].y), ImVec2(this->Lens2D[0].x, this->Lens2D[0].y), this->Colour, 4.0f);
    //添加文本
    this->drawList->AddCircleFilled(ImVec2(this->Origin2D.x, this->Origin2D.y), 3.0f, IM_COL32(0, 0, 0, 255));
    if (label) {
        this->drawList->AddText(ImVec2(this->Origin2D.x, this->Origin2D.y - 20), IM_COL32(0, 0, 0, 255), label);
    }
}

void CameraDrawer::Update(float* pMatrixPtr, float pWinWidth, float pWinHeight) {
    this->pMatrixPtr = pMatrixPtr;
    this->WinWidth = pWinWidth;
    this->WinHeight = pWinHeight;
}

void CameraDrawer::DrawCamera(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT3 Rotation, const char* label) {
    //if(!)
    this->Origin = this->CameraModel.Origin;

    this->Lens[0] = this->CameraModel.Lens[0];
    this->Lens[1] = this->CameraModel.Lens[1];
    this->Lens[2] = this->CameraModel.Lens[2];
    this->Lens[3] = this->CameraModel.Lens[3];

    this->Colour = this->CameraModel.Colour;

    this->X = this->CameraModel.X;
    this->Y = this->CameraModel.Y;
    this->Z = this->CameraModel.Z;

    this->Rotate(Rotation);
    this->Move(Position);
    this->ToScreen();
    this->Draw(label);
}

void CameraDrawer::DrawFrameCamera(const CSATMath::Frame& frame, const char* label) {
    //if(!)
    this->Origin = this->CameraModel.Origin;

    this->Lens[0] = this->CameraModel.Lens[0];
    this->Lens[1] = this->CameraModel.Lens[1];
    this->Lens[2] = this->CameraModel.Lens[2];
    this->Lens[3] = this->CameraModel.Lens[3];

    this->Colour = this->CameraModel.Colour;

    this->X = this->CameraModel.X;
    this->Y = this->CameraModel.Y;
    this->Z = this->CameraModel.Z;

    this->Rotate(frame.SpatialState.GetRotationEuler());
    this->Move(frame.SpatialState.GetPosition());
    this->ToScreen();
    this->Draw(label);
}

void CameraDrawer::DrawFirstPersonCamera(const char* label) {
    if (!this->IfDraw) return;

    this->drawList = ImGui::GetBackgroundDrawList();

    // 使用固定的屏幕坐标绘制第一人称视角的摄像机镜面
    // 假设镜面位于屏幕中央，大小为屏幕的1/4
    float screenCenterX = this->WinWidth / 2.0f;
    float screenCenterY = this->WinHeight / 2.0f;
    float lensWidth = this->WinWidth / 4.0f;
    float lensHeight = this->WinHeight / 4.0f;

    // 计算镜面矩形的四个角点（屏幕坐标）
    ImVec2 lensCorners[4];
    lensCorners[0] = ImVec2(screenCenterX - lensWidth / 2, screenCenterY - lensHeight / 2); // 左上
    lensCorners[1] = ImVec2(screenCenterX - lensWidth / 2, screenCenterY + lensHeight / 2); // 左下
    lensCorners[2] = ImVec2(screenCenterX + lensWidth / 2, screenCenterY + lensHeight / 2); // 右下
    lensCorners[3] = ImVec2(screenCenterX + lensWidth / 2, screenCenterY - lensHeight / 2); // 右上

    // 使用固定的视图投影矩阵（单位矩阵）直接将模型投影到屏幕
    DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();

    // 定义第一人称镜面模型（相对于屏幕中心）
    DirectX::XMFLOAT3 fpLens[4];
    fpLens[0] = { -lensWidth / 2, lensHeight / 2, 0 };  // 左上
    fpLens[1] = { -lensWidth / 2, -lensHeight / 2, 0 }; // 左下
    fpLens[2] = { lensWidth / 2, -lensHeight / 2, 0 };  // 右下
    fpLens[3] = { lensWidth / 2, lensHeight / 2, 0 };   // 右上

    DirectX::XMFLOAT3 fpOrigin = { 0, 0, 0 };

    // 将模型直接投影到屏幕（使用固定矩阵）
    DirectX::XMFLOAT2 origin2D, lens2D[4];
    CSATMath::XMWorldToScreen(fpOrigin, origin2D, (float*)&identityMatrix, this->WinWidth, this->WinHeight);
    for (int i = 0; i < 4; i++) {
        CSATMath::XMWorldToScreen(fpLens[i], lens2D[i], (float*)&identityMatrix, this->WinWidth, this->WinHeight);
    }

    // 调整到屏幕中心
    origin2D.x += screenCenterX;
    origin2D.y += screenCenterY;
    for (int i = 0; i < 4; i++) {
        lens2D[i].x += screenCenterX;
        lens2D[i].y += screenCenterY;
    }

    // 绘制镜面矩形
    this->drawList->AddLine(ImVec2(lens2D[0].x, lens2D[0].y), ImVec2(lens2D[1].x, lens2D[1].y), this->Colour, 2.0f);
    this->drawList->AddLine(ImVec2(lens2D[1].x, lens2D[1].y), ImVec2(lens2D[2].x, lens2D[2].y), this->Colour, 2.0f);
    this->drawList->AddLine(ImVec2(lens2D[2].x, lens2D[2].y), ImVec2(lens2D[3].x, lens2D[3].y), this->Colour, 2.0f);
    this->drawList->AddLine(ImVec2(lens2D[3].x, lens2D[3].y), ImVec2(lens2D[0].x, lens2D[0].y), this->Colour, 2.0f);

    // 绘制从原点到四个角的连线（可选，模拟视角锥体）
    this->drawList->AddLine(ImVec2(origin2D.x, origin2D.y), ImVec2(lens2D[0].x, lens2D[0].y), this->Colour, 1.0f);
    this->drawList->AddLine(ImVec2(origin2D.x, origin2D.y), ImVec2(lens2D[1].x, lens2D[1].y), this->Colour, 1.0f);
    this->drawList->AddLine(ImVec2(origin2D.x, origin2D.y), ImVec2(lens2D[2].x, lens2D[2].y), this->Colour, 1.0f);
    this->drawList->AddLine(ImVec2(origin2D.x, origin2D.y), ImVec2(lens2D[3].x, lens2D[3].y), this->Colour, 1.0f);

    // 添加标签
    this->drawList->AddCircleFilled(ImVec2(origin2D.x, origin2D.y), 2.0f, IM_COL32(255, 255, 255, 255));
    if (label) {
        this->drawList->AddText(ImVec2(origin2D.x + 10, origin2D.y - 10), IM_COL32(255, 255, 255, 255), label);
    }
}
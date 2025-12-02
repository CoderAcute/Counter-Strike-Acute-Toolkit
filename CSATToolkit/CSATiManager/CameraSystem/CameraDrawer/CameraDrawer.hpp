#pragma once

#include"../../../ThirdParty/All_ImGui.hpp"
#include"../../../CSATMath/CSATMath.hpp"

class CameraModel {
public:

    DirectX::XMFLOAT3 Origin{};
    DirectX::XMFLOAT3 Lens[4]{};
    DirectX::XMFLOAT3 X{};
    DirectX::XMFLOAT3 Y{};
    DirectX::XMFLOAT3 Z{};

    ImU32 Colour{};

    void Init(float CameraHigh, float CameraX, float CameraY, float AxisLenth, ImU32 Colour);
};

class CameraDrawer {
public:
    bool IfDraw = true;
private:
    

    CameraModel CameraModel{};

    DirectX::XMFLOAT3 Origin{};
    DirectX::XMFLOAT3 Lens[4]{};
    DirectX::XMFLOAT3 X{};
    DirectX::XMFLOAT3 Y{};
    DirectX::XMFLOAT3 Z{};

    DirectX::XMFLOAT2 Origin2D{};
    DirectX::XMFLOAT2 Lens2D[4]{};
    DirectX::XMFLOAT2 X2D{};
    DirectX::XMFLOAT2 Y2D{};
    DirectX::XMFLOAT2 Z2D{};

    ImU32 Colour{};

    float CameraHigh;
    float CameraX;
    float CameraY;
    float AxisLenth;

    ImDrawList* drawList{};

    float* pMatrixPtr{};
    float WinWidth{};
    float WinHeight{};
public:
    void Init(float CameraHigh, float CameraX, float CameraY, float AxisLenth, ImU32 Colour);
    void Rotate(DirectX::XMFLOAT3 Rotation);
    void Move(DirectX::XMFLOAT3 TargetPoint);
    void ToScreen();
    void Draw(const char* label);
    void Update(float* pMatrixPtr, float pWinWidth, float pWinHeight);
    void DrawCamera(DirectX::XMFLOAT3 Position, DirectX::XMFLOAT3 Rotation, const char* label);
    void DrawFrameCamera(const CSATMath::Frame& frame, const char* label);
    void DrawFirstPersonCamera(const char* label);
};
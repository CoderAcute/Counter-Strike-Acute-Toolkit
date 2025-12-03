#include"CSATMath.hpp"

#include<sstream>
#include<iomanip>

constexpr float PI = 3.141592653f;

DirectX::XMFLOAT3 CSATMath::SpatialState::GetPosition()const {
	DirectX::XMFLOAT3 position;
	DirectX::XMStoreFloat3(&position, this->PositionAndFOV);
	return position;
}

DirectX::XMFLOAT4 CSATMath::SpatialState::GetRotationQuat() const {
	DirectX::XMFLOAT4 Quat;
	DirectX::XMStoreFloat4(&Quat, this->RotationQuat);
	return Quat;
}

float CSATMath::SpatialState::GetFOV() const {
	return DirectX::XMVectorGetW(this->PositionAndFOV);
}

DirectX::XMFLOAT3 CSATMath::SpatialState::GetRotationEuler()const {
	DirectX::XMFLOAT3 Euler;
	CSQuatToEuler(this->GetRotationQuat(), Euler);
	return Euler;
}

DirectX::XMFLOAT4 CSATMath::SpatialState::GetPositionAndFOV()const {
	DirectX::XMFLOAT4 PositionAndFOV;
	DirectX::XMStoreFloat4(&PositionAndFOV, this->PositionAndFOV);
	return PositionAndFOV;
}

std::string CSATMath::SpatialState::GetMsg()const {
	DirectX::XMFLOAT4 PositionAndFOV = this->GetPositionAndFOV();
	DirectX::XMFLOAT3 Euler = this->GetRotationEuler();
	std::ostringstream oss;

	oss << std::fixed << std::setprecision(6);

	oss << "  坐标： X：" << PositionAndFOV.x
		<< "  Y：" << PositionAndFOV.y
		<< "  Z：" << PositionAndFOV.z
		<< "  FOV:" << PositionAndFOV.w
		<< "  角度： 俯仰：" << Euler.x
		<< "  偏航：" << Euler.y
		<< "  滚转：" << Euler.z;
		
	return oss.str();
}

std::string CSATMath::CameraKeyFrame::GetMsg()const {
	std::ostringstream oss;

	oss << std::fixed << std::setprecision(6);

	oss << "  时间： " << this->KeyTime
		<< this->SpatialState.GetMsg()
		<< "  景深：" << this->Depth;
	return oss.str();
}

std::string CSATMath::Frame::GetMsg()const {
	std::ostringstream oss;

	oss << std::fixed << std::setprecision(6);

	oss << this->SpatialState.GetMsg()
		<< "  景深：" << Depth;
	return oss.str();
}

void CSATMath::CSEulerToQuat(const DirectX::XMFLOAT3& Euler, DirectX::XMFLOAT4& QuaT) {
	// 将角度转换为弧度
	float yawRad = DirectX::XMConvertToRadians(Euler.y);   // 偏航角（绕Z轴，向左为正）
	float pitchRad = DirectX::XMConvertToRadians(-Euler.x); // 俯仰角（绕Y轴，向下为正，取负以符合右手系）
	float rollRad = DirectX::XMConvertToRadians(-Euler.z);  // 滚转角（绕X轴，顺时针为正，取负以符合右手系）

	// 创建旋转轴
	DirectX::XMVECTOR axisZ = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR axisY = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR axisX = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	// 计算各旋转的四元数
	DirectX::XMVECTOR quatYaw = DirectX::XMQuaternionRotationAxis(axisZ, yawRad);
	DirectX::XMVECTOR quatPitch = DirectX::XMQuaternionRotationAxis(axisY, pitchRad);
	DirectX::XMVECTOR quatRoll = DirectX::XMQuaternionRotationAxis(axisX, rollRad);

	// 按顺序组合旋转：先偏航，再俯仰，最后滚转
	DirectX::XMVECTOR quatTemp = DirectX::XMQuaternionMultiply(quatPitch, quatYaw);
	DirectX::XMVECTOR quatResult = DirectX::XMQuaternionMultiply(quatRoll, quatTemp);

	// 存储并返回结果
	DirectX::XMStoreFloat4(&QuaT, quatResult);
	return;
}

DirectX::XMVECTOR CSATMath::CSEulerToQuatVec(const DirectX::XMFLOAT3& Euler) {
	// 将角度转换为弧度
	float yawRad = DirectX::XMConvertToRadians(Euler.y);   // 偏航角（绕Z轴，向左为正）
	float pitchRad = DirectX::XMConvertToRadians(-Euler.x); // 俯仰角（绕Y轴，向下为正，取负以符合右手系）
	float rollRad = DirectX::XMConvertToRadians(-Euler.z);  // 滚转角（绕X轴，顺时针为正，取负以符合右手系）

	// 创建旋转轴
	DirectX::XMVECTOR axisZ = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR axisY = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR axisX = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	// 计算各旋转的四元数
	DirectX::XMVECTOR quatYaw = DirectX::XMQuaternionRotationAxis(axisZ, yawRad);
	DirectX::XMVECTOR quatPitch = DirectX::XMQuaternionRotationAxis(axisY, pitchRad);
	DirectX::XMVECTOR quatRoll = DirectX::XMQuaternionRotationAxis(axisX, rollRad);

	// 按顺序组合旋转：先偏航，再俯仰，最后滚转
	DirectX::XMVECTOR quatTemp = DirectX::XMQuaternionMultiply(quatPitch, quatYaw);
	DirectX::XMVECTOR quatResult = DirectX::XMQuaternionMultiply(quatRoll, quatTemp);

	return quatResult;
}

void CSATMath::CSQuatToEuler(const DirectX::XMFLOAT4& Quat, DirectX::XMFLOAT3& Euler) {
	//提取四元数分量
	const float& x = Quat.x;
	const float& y = Quat.y;
	const float& z = Quat.z;
	const float& w = Quat.w;

	//计算偏航角（yaw）
	float yawRad = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));

	//计算中间值用于俯仰角和万向锁检测
	float test = 2.0f * (w * y - z * x);
	const float threshold = 0.9999999f; // 万向锁阈值

	float pitchRad, rollRad;

	//处理万向锁（俯仰角接近±90度）
	if (test > threshold) {
		pitchRad = -DirectX::XM_PIDIV2; // -90度（向上）
		rollRad = 0.0f;
	}
	else if (test < -threshold) {
		pitchRad = DirectX::XM_PIDIV2;  // +90度（向下）
		rollRad = 0.0f;
	}
	else {
		//计算俯仰角和滚转角
		float pitchStd = std::asin(test);
		float rollStd = std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));

		//转换旋转方向
		pitchRad = -pitchStd; // 俯仰角取负
		rollRad = -rollStd;   // 滚转角取负
	}

	//将弧度转换为角度
	Euler.y = DirectX::XMConvertToDegrees(yawRad);   // 偏航角
	Euler.x = DirectX::XMConvertToDegrees(pitchRad); // 俯仰角
	Euler.z = DirectX::XMConvertToDegrees(rollRad);  // 滚转角

	return;
}

void CSATMath::CSDirToEuler(const DirectX::XMFLOAT3& Dir, DirectX::XMFLOAT3& Euler){
	Euler.x = std::atan2(-Dir.z, std::sqrt(Dir.x * Dir.x + Dir.y * Dir.y)) * (180.0 / PI);
	Euler.y = std::atan2(Dir.y, Dir.x) * (180.0 / PI);

	Euler.x = (Euler.x < -89.0f) ? -89.0f : Euler.x;
	Euler.x = (Euler.x > 89.f) ? 89.0f : Euler.x;
	
	while (Euler.y > 180.f)Euler.y -= 360.f;
	while (Euler.y < -180.f)Euler.y += 360.f;
}


bool CSATMath::XMWorldToScreen(const DirectX::XMFLOAT3& pWorldPos, DirectX::XMFLOAT2& pScreenPos, float* pMatrixPtr, const float pWinWidth, const float pWinHeight)
{
	if (!pMatrixPtr) return false;

	const float mX{ pWinWidth / 2 };
	const float mY{ pWinHeight / 2 };

	// 直接使用原始矩阵指针，通过索引访问
	const float w{
		pMatrixPtr[12] * pWorldPos.x +    // matrix[3][0]
		pMatrixPtr[13] * pWorldPos.y +    // matrix[3][1] 
		pMatrixPtr[14] * pWorldPos.z +    // matrix[3][2]
		pMatrixPtr[15] };                 // matrix[3][3]

	if (w <= 0.0f) return false;

	const float x{
		pMatrixPtr[0] * pWorldPos.x +     // matrix[0][0]
		pMatrixPtr[1] * pWorldPos.y +     // matrix[0][1]
		pMatrixPtr[2] * pWorldPos.z +     // matrix[0][2]
		pMatrixPtr[3] };                  // matrix[0][3]

	const float y{
		pMatrixPtr[4] * pWorldPos.x +     // matrix[1][0]
		pMatrixPtr[5] * pWorldPos.y +     // matrix[1][1]
		pMatrixPtr[6] * pWorldPos.z +     // matrix[1][2]
		pMatrixPtr[7] };                  // matrix[1][3]

	pScreenPos.x = (mX + mX * x / w);
	pScreenPos.y = (mY - mY * y / w);

	return true;
}


DirectX::XMFLOAT3 CSATMath::RotatePoint(
	const DirectX::XMFLOAT3& inputPoint,
	float pitchDegrees,  // 绕Y轴旋转（俯仰）
	float yawDegrees,    // 绕Z轴旋转（偏航）
	float rollDegrees    // 绕X轴旋转（滚转）
)
{
	// === 第1步：将输入点转换为SIMD向量 ===
	DirectX::XMVECTOR point = DirectX::XMLoadFloat3(&inputPoint);

	// === 第2步：角度转弧度 ===
	float pitchRadians = DirectX::XMConvertToRadians(pitchDegrees);  // Y轴
	float yawRadians = DirectX::XMConvertToRadians(yawDegrees);      // Z轴
	float rollRadians = DirectX::XMConvertToRadians(rollDegrees);     // X轴

	// === 第3步：初始化旋转四元数 ===
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionIdentity();

	// === 第4步：按顺序应用旋转 ===
	// 应用顺序：先Roll → 再Pitch → 最后Yaw

	// 4.1 应用绕X轴旋转（Roll - 滚转）
	if (rollDegrees != 0.0f)
	{
		// 绕X轴旋转
		DirectX::XMVECTOR rollQuaternion = DirectX::XMQuaternionRotationAxis(
			DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), // X轴
			rollRadians
		);
		rotationQuaternion = DirectX::XMQuaternionMultiply(rotationQuaternion, rollQuaternion);
	}

	// 4.2 应用绕Y轴旋转（Pitch - 俯仰）
	if (pitchDegrees != 0.0f)
	{
		// 绕Y轴旋转
		DirectX::XMVECTOR pitchQuaternion = DirectX::XMQuaternionRotationAxis(
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), // Y轴
			pitchRadians
		);
		rotationQuaternion = DirectX::XMQuaternionMultiply(rotationQuaternion, pitchQuaternion);
	}

	// 4.3 应用绕Z轴旋转（Yaw - 偏航）
	if (yawDegrees != 0.0f)
	{
		// 绕Z轴旋转
		DirectX::XMVECTOR yawQuaternion = DirectX::XMQuaternionRotationAxis(
			DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), // Z轴
			yawRadians
		);
		rotationQuaternion = DirectX::XMQuaternionMultiply(rotationQuaternion, yawQuaternion);
	}

	// === 第5步：将四元数转换为旋转矩阵 ===
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotationQuaternion);

	// === 第6步：应用旋转到点 ===
	DirectX::XMVECTOR rotatedVector = DirectX::XMVector3Transform(point, rotationMatrix);

	// === 第7步：将结果转回常规格式 ===
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, rotatedVector);

	return result;
}

bool CSATMath::MovePoint(DirectX::XMFLOAT3& SourcePoint, const DirectX::XMFLOAT3& TargetPoint) {
	SourcePoint += TargetPoint;
	return true;
}

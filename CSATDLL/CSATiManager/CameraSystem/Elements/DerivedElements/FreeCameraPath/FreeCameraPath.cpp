#include"FreeCameraPath.hpp"

#include"../../../CameraDrawer/CameraDrawer.hpp"
#include"../../../../../ThirdParty/All_pugixml.hpp"
#include"../../../../../ThirdParty/All_ImGui.hpp"

//拷贝语义版
void FreeCameraPath::AddKeyframe(const CSATMath::CameraKeyFrame& KeyFrame) {
	//按照时间排序插入
    auto it = std::lower_bound(CameraKeyFrames.begin(), CameraKeyFrames.end(), KeyFrame,
        [&](const CSATMath::CameraKeyFrame& a, const CSATMath::CameraKeyFrame& b) {//引用捕获加速
            return a.KeyTime < b.KeyTime;
        });
    CameraKeyFrames.insert(it, KeyFrame); // 拷贝插入
    this->Refresh();

    return;
}
//移动语义版
void FreeCameraPath::AddKeyframe(CSATMath::CameraKeyFrame&& KeyFrame) {
	//按照时间排序插入
    auto it = std::lower_bound(CameraKeyFrames.begin(), CameraKeyFrames.end(), KeyFrame,
        [&](const CSATMath::CameraKeyFrame& a, const CSATMath::CameraKeyFrame& b) {//引用捕获加速
            return a.KeyTime < b.KeyTime;
        });
    CameraKeyFrames.insert(it, std::move(KeyFrame)); // 移动插入
    this->Refresh();

    return;
}
void FreeCameraPath::Refresh() {
    this->Size_Frames = this->CameraKeyFrames.size();
    if (this->Size_Frames == 0) {
        this->StartTime = 0;
        this->EndTime = 0;
        this->DurationTime = 0;
        return;
    }
    else {
        this->StartTime = this->CameraKeyFrames.front().KeyTime;
        this->EndTime = this->CameraKeyFrames.back().KeyTime;
        this->DurationTime = this->EndTime - this->StartTime;
        return;
    }
}
void FreeCameraPath::TimeNormalize() {
    if (this->CameraKeyFrames.front().KeyTime == 0)return;
    size_t Size = this->CameraKeyFrames.size();
    if (!Size)return;
    if (Size == 1) {
        this->CameraKeyFrames.at(0).KeyTime = 0;
        return;
    }
    float Schema = this->CameraKeyFrames.front().KeyTime;
    for (int i = 1; i < Size; ++i) {
        this->CameraKeyFrames.at(i).KeyTime -= Schema;
    }
    this->CameraKeyFrames.front().KeyTime = 0;
}
bool FreeCameraPath::Call(CSATMath::Frame& Frame, float Time, const PlaybackMode Mode)const {
    //如果不在理论影响范围内，应当直接返回而不做任何修改
    //是与被遍历的其它call一起工作

    //处理空关键帧的情况
    if (CameraKeyFrames.empty())return false;

    if (!this->TimeReform(Time, Mode)) {
        return false;
    }

    // 查找当前时间所在的关键帧区间（使用绝对时间来搜索以匹配以绝对时间存储的关键帧）
    auto it = std::lower_bound(CameraKeyFrames.begin(), CameraKeyFrames.end(), Time,
        [](const CSATMath::CameraKeyFrame& k, float t) {
            return k.KeyTime < t;
        });

    // 确保迭代器有效并安全地计算索引
    ptrdiff_t dist = std::distance(CameraKeyFrames.begin(), it);
    size_t index = (dist == 0) ? 0 : static_cast<size_t>(dist - 1);

    // 保护：如果 index 位于最后一个元素，则没有下一个关键帧可用于插值
    if (index + 1 >= this->Size_Frames) return false;

    //获取相邻的四个关键帧用于插值
    const CSATMath::CameraKeyFrame& k1 = CameraKeyFrames[index];
    const CSATMath::CameraKeyFrame& k2 = CameraKeyFrames[index + 1];
    const CSATMath::CameraKeyFrame& k0 = (index > 0) ? CameraKeyFrames[index - 1] : k1;
    const CSATMath::CameraKeyFrame& k3 = (index + 2 < this->Size_Frames) ? CameraKeyFrames[index + 2] : k2;

    //计算当前片段的时间比例 (0~1)
    float segmentDuration = k2.KeyTime - k1.KeyTime;
    if (segmentDuration <= 0.0f) return false; // 避免除以零或无效的段

    float segmentTime = (Time - k1.KeyTime) / segmentDuration;

    //位置和FOV插值 (Catmull-Rom)
    Frame.SpatialState.PositionAndFOV = DirectX::XMVectorCatmullRom(
        k0.SpatialState.PositionAndFOV,
        k1.SpatialState.PositionAndFOV,
        k2.SpatialState.PositionAndFOV,
        k3.SpatialState.PositionAndFOV,
        segmentTime);

    //旋转插值 (使用Squad提供高阶连续性)

    //使用DirectXMath内置函数计算Squad控制点
    DirectX::XMVECTOR s1, s2, s3;
    DirectX::XMQuaternionSquadSetup(&s1, &s2, &s3,
        k0.SpatialState.RotationQuat,
        k1.SpatialState.RotationQuat,
        k2.SpatialState.RotationQuat,
        k3.SpatialState.RotationQuat);

    //使用Squad进行插值
    DirectX::XMVECTOR rotation = DirectX::XMQuaternionSquad(k1.SpatialState.RotationQuat, s1, s2, s3, segmentTime);
    Frame.SpatialState.RotationQuat = DirectX::XMQuaternionNormalize(rotation);

    Frame.TargetOBMode = 4;

    //DirectX::XMFLOAT3 output = Frame.SpatialState.GetPosition();
    //ImGui::Text(("X: " + std::to_string(output.x) + "  Y: " + std::to_string(output.y) + "  Z: " + std::to_string(output.z)).c_str());

    return true;
}
//绘制函数（虚），各个元素按需实现
bool FreeCameraPath::Draw(CameraDrawer* CamDrawer, const float* Matrix, const float WinWidth, const float WinHeight)const {
    const std::vector<CSATMath::CameraKeyFrame>& Frames = this->GetAllKeyFrames();
    int Size = Frames.size();

    // 存储上一个关键帧的位置（用于连线）
    DirectX::XMFLOAT3 prevPosition{};
    bool firstFrame = true;

    // 遍历当前路径的所有关键帧
    for (int i = 0; i < Size; ++i) {
        if (!Matrix)return false;
        const auto& Frame = Frames.at(i);
        // 绘制关键帧的摄像机
        std::string label = this->Name + " #" + std::to_string(i);
        CamDrawer->DrawCamera(Frame.SpatialState.GetPosition(), Frame.SpatialState.GetRotationEuler(), label.c_str());

        // 获取ImDrawList用于绘制连线
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        // 如果不是第一个关键帧，绘制连线
        if (!firstFrame) {
            // 将3D位置转换为屏幕坐标
            DirectX::XMFLOAT2 prevScreenPos, currentScreenPos;

            // 使用CameraDrawer中的转换方法
            CSATMath::XMWorldToScreen(prevPosition, prevScreenPos, Matrix, WinWidth, WinHeight);

            CSATMath::XMWorldToScreen(Frame.SpatialState.GetPosition(), currentScreenPos, Matrix, WinWidth, WinHeight);

            // 绘制连线
            drawList->AddLine(
                ImVec2(prevScreenPos.x, prevScreenPos.y),
                ImVec2(currentScreenPos.x, currentScreenPos.y),
                IM_COL32(0, 255, 255, 255), // 青色连线
                2.0f // 线宽
            );
        }
        // 保存当前位置用于下一次连线
        prevPosition = Frame.SpatialState.GetPosition();
        firstFrame = false;
    }
    return true;
}

std::string FreeCameraPath::GetMsg()const {
    std::ostringstream oss;
    oss << this->GetBaseMsg()
        << "\n详细信息：\n";
    const size_t& Size = this->CameraKeyFrames.size();
    oss << "  关键帧总数： " << std::to_string(Size) << "\n";
    for (size_t i = 0; i < Size; ++i) {
        const CSATMath::CameraKeyFrame& Frame = this->CameraKeyFrames.at(i);
        DirectX::XMFLOAT3 Euler;
        CSATMath::CSQuatToEuler(Frame.SpatialState.GetRotationQuat(), Euler);
        oss << "编号： " << std::to_string(i) << Frame.GetMsg() << "\n";
    }
	return oss.str();
}

size_t FreeCameraPath::GetKeyFrameCount() const {
    return this->CameraKeyFrames.size();
}
const CSATMath::CameraKeyFrame& FreeCameraPath::GetKeyFrame(const size_t& index)const {
    return this->CameraKeyFrames[index];
}
const std::vector<CSATMath::CameraKeyFrame>& FreeCameraPath::GetAllKeyFrames()const {
    return this->CameraKeyFrames;
}
void FreeCameraPath::Clear() {
    this->CameraKeyFrames.clear();
    this->Refresh();
}
bool FreeCameraPath::ReadElementMain(const pugi::xml_node& node_ElementMain, std::string& strRuselt) {
    //获取关键帧节点
    pugi::xml_node node_KeyFrame = node_ElementMain.child("KeyFrame");
    if (!node_KeyFrame) {
        strRuselt = "找不到关键帧节点！ 自由摄像机轨道 名：" + this->Name;
        return false;
    }
    else {
        //在有信息可读取时，先清空自身信息，准备进入读取流程
        this->Clear();
        //读取流程
        for (; node_KeyFrame; node_KeyFrame = node_KeyFrame.next_sibling("KeyFrame")) {
            CSATMath::CameraKeyFrame CameraKeyFrame;
            // 读取时间
            CameraKeyFrame.KeyTime = node_KeyFrame.attribute("Time").as_float();

            // 读取坐标和FOV
            DirectX::XMFLOAT4 PositionAndFOV;// = Frame.SpatialState.GetPositionAndFOV();

            PositionAndFOV.x = node_KeyFrame.attribute("Px").as_float();
            PositionAndFOV.y = node_KeyFrame.attribute("Py").as_float();
            PositionAndFOV.z = node_KeyFrame.attribute("Pz").as_float();
            PositionAndFOV.w = node_KeyFrame.attribute("FOV").as_float();

            CameraKeyFrame.SpatialState.PositionAndFOV = DirectX::XMLoadFloat4(&PositionAndFOV);

            // 读取角度
            // 欧拉角 → 四元数转换
            DirectX::XMFLOAT3 temEuler{
                node_KeyFrame.attribute("Pitch").as_float(),
                node_KeyFrame.attribute("Yaw").as_float(),
                node_KeyFrame.attribute("Roll").as_float()
            };
            DirectX::XMFLOAT4 temQuat;
            CSATMath::CSEulerToQuat(temEuler, temQuat);

            CameraKeyFrame.SpatialState.RotationQuat = DirectX::XMLoadFloat4(&temQuat);

            this->AddKeyframe(std::move(CameraKeyFrame));
        }
    }

    strRuselt = "成功从XML文件加载自由摄像机轨道信息！ 自由摄像机轨道 名：" + this->Name;

    return true;
}
bool FreeCameraPath::SaveToXML(const std::filesystem::path& FolderPath, std::string& strRuselt)const {
    if (FolderPath.empty()) {
        strRuselt = "文件夹路径为空，无法保存元素到XML文件！";
        return false;
    }
    pugi::xml_document XML;
    pugi::xml_node node_ElementMain;
    if (!this->SaveBase(XML, node_ElementMain, strRuselt)) {
		return false;
    }
    std::filesystem::path FullPath = FolderPath / (this->Name + ".xml");
    // 添加注释
    //node_ElementMain.append_child(pugi::node_comment).set_value("采用欧拉角存储角度，但工具实际使用四元数系统，如果你觉得加载速度慢，可以向作者反馈");

    // 遍历关键帧并添加到XML
    for (const auto& CameraKeyFrame : this->CameraKeyFrames) {
        // 创建KeyFrame节点并设置属性
        pugi::xml_node node_KeyFrame = node_ElementMain.append_child("KeyFrame");

        //时间
        node_KeyFrame.append_attribute("Time") = CameraKeyFrame.KeyTime;

        //坐标和FOV
        DirectX::XMFLOAT4 PositionAndFOV = CameraKeyFrame.SpatialState.GetPositionAndFOV();

        node_KeyFrame.append_attribute("Px") = PositionAndFOV.x;
        node_KeyFrame.append_attribute("Py") = PositionAndFOV.y;
        node_KeyFrame.append_attribute("Pz") = PositionAndFOV.z;
        node_KeyFrame.append_attribute("FOV") = PositionAndFOV.w;

        //欧拉角
        DirectX::XMFLOAT3 Euler = CameraKeyFrame.SpatialState.GetRotationEuler();
        node_KeyFrame.append_attribute("Pitch") = Euler.x;
        node_KeyFrame.append_attribute("Yaw") = Euler.y;
        node_KeyFrame.append_attribute("Roll") = Euler.z;
        
        
        //node_KeyFrame.append_attribute("Depth") = Frame.Depth;
    }
    // 保存XML到文件
    if (!XML.save_file(FullPath.c_str())) {
        strRuselt = "尝试保存自由摄像机轨道到XML文件失败，无法保存XML文件！ 文件路径：" + FullPath.string();
        return false;
    }
    strRuselt = "成功保存自由摄像机轨道到XML文件！ 文件路径：" + FullPath.string();
    return true;
}

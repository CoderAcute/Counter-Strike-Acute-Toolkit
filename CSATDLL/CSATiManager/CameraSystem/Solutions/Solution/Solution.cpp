#include"Solution.hpp"

//#include"../../../../ThirdParty/All_ImGui.hpp"

bool Solution::AddElement(const std::shared_ptr<ElementBase> const element, const float Offset) {
    // 检查重复
    auto it = std::find_if(this->Elements.begin(), this->Elements.end(),
        [&element](const ElementWithOffset& ew) {
            if (auto el = ew.Element) {
                return el == element;
            }
            return false;
        });

    if (it != this->Elements.end()) {
        return false;
    }

    // 创建新元素
    ElementWithOffset newElement{ element, Offset };

    // 找到正确的插入位置以保持排序
    auto insertPos = std::lower_bound(Elements.begin(), Elements.end(), newElement,
        [](const ElementWithOffset& a, const ElementWithOffset& b) {
            return a.Offset < b.Offset;
        });

    // 插入元素
    Elements.insert(insertPos, std::move(newElement));

    // 计算自身大小同时检验所有元素有效性
    this->Refresh();
    return true;
}
bool Solution::RemoveElementAt(const size_t Index) {
    if (Index < 0 || Index >= this->Elements.size()) {
        return false;
    }
    this->Elements.erase(this->Elements.begin() + Index);
    this->Refresh();
    return true;
}
bool Solution::SaveToXML(const std::filesystem::path& FolderPath, std::string& stRuselt) {
    //先刷新确保安全有效
    this->Refresh();
    //检查文件路径和名称存在性
    if (FolderPath.empty()) {
        stRuselt = "文件夹路径为空，无法保存解决方案！";
        return false;
    }

    //拼接完整路径
    std::filesystem::path FullPath = FolderPath / (this->Name + ".xml");

    //创建临时XML文件
    pugi::xml_document NewXML;
    //初始化
    pugi::xml_node declarationNode = NewXML.prepend_child(pugi::node_declaration);
    declarationNode.append_attribute("version") = "1.0";
    declarationNode.append_attribute("encoding") = "utf-8";

    //开始保存信息

    //保存Solution节点
    pugi::xml_node node_Solution= NewXML.append_child("Solution");

    //保存解决方案名称
    node_Solution.append_attribute("Name").set_value(this->Name);

    //保存SolutionMain节点
    pugi::xml_node node_SolutionMain = node_Solution.append_child("SolutionMain");
    node_SolutionMain.append_attribute("DurationTime").set_value(this->TotalDurationTime);

    //保存KeyCheckPack信息
    pugi::xml_node node_KeyCheckPack = node_SolutionMain.append_child("KeyCheckPack");
    this->KCPack.WriteXMLNode(node_KeyCheckPack);


    //保存Elements节点
    pugi::xml_node node_Elements = node_SolutionMain.append_child("Elements");
    node_Elements.append_attribute("Size").set_value(this->Size_Elements);

    //保存所有元素
    for (size_t i = 0; i < this->Size_Elements; ++i) {
        std::shared_ptr<ElementBase> element = this->Elements[i].Element;
        if (!element) {
            stRuselt = "疑似有元素在保存过程中被删除，保存终止！";
            return false;
        }
        pugi::xml_node node_Element = node_Elements.append_child("Element");
        node_Element.append_attribute("Name").set_value(element->Name);
        node_Element.append_attribute("Offset").set_value(this->Elements[i].Offset);
    }

    //保存XML文件到磁盘
    if (!NewXML.save_file(FullPath.c_str())) {
        stRuselt = "保存文件失败！路径：" + FullPath.string();
        return false;
    }

    stRuselt = "保存成功  解决方案名：" + this->Name + "  元素个数：" + std::to_string(this->Size_Elements);
    return true;
}

void Solution::Refresh() {
    //判空处理
    if (this->Elements.empty()) {
        this->SafeUse = false;
        this->StartTime = 0;
        this->EndTime = 0;
        this->TotalDurationTime = 0;
        return;
    }

    //清理过期元素的同时查找整个解决方案的结束时间点
    for (auto It = Elements.begin(); It != Elements.end();) {
        if (It->Element->NeedBeDelete) {
            It = Elements.erase(It);//erase返回下一个有效迭代器
        }
        else {
            float ElementEndTime = It->Offset + It->Element->DurationTime;
            if (this->EndTime < ElementEndTime) {
                this->EndTime = ElementEndTime;
            }

            ++It;//未删除时递增
        }
    }

    //更新大小
    this->Size_Elements = Elements.size();

    //更新后判空
    if (!this->Size_Elements) {
        this->SafeUse = false;
        this->StartTime = 0;
        this->EndTime = 0;
        this->TotalDurationTime = 0;
        return;
    }

    //获取第一个元素的开始时间
    this->StartTime = Elements.front().Offset;

    //计算总持续时间（从第一个元素开始到最后一个元素结束）
    this->TotalDurationTime = this->EndTime - this->StartTime;
    this->SafeUse = true;
}
void Solution::SetSolutionOffset(const float Offset) {
    this->SolutionOffset = Offset;
}
bool Solution::TimeLineGenerate() {
    this->Refresh();

    if (this->Elements.empty()) {
        return false;
    }
    float TimeReference = this->Elements[0].Offset;
    for (int i = 0; i < this->Elements.size(); ++i) {
        //先获取最早的元素的开始时间作为参考时间
        if (TimeReference > this->Elements[i].Element->GetStartTime()) {
            TimeReference = this->Elements[i].Element->GetStartTime();
        }
    }
    //然后设置元素偏移
    for (int i = 0; i < this->Elements.size(); ++i) {
        this->Elements[i].Offset = this->Elements[i].Element->GetStartTime() - TimeReference;
    }

    this->Refresh();

    return true;
}
std::string Solution::GetMsg() {
    this->Refresh();

    if (!this->SafeUse)return "不安全的解决方案";

    std::ostringstream oss;
    oss << "解决方案名称：" << this->Name
        << "   元素数量：" << this->Elements.size()
        << "   总时长：" << this->TotalDurationTime
        << "\n详细信息："
        << "\n";

    for (size_t i = 0; i < this->Size_Elements; ++i) {
        std::shared_ptr<ElementBase> Element = this->Elements.at(i).Element;
        if (Element) {
            oss << i << ".  "
                "  |元素编号：" << i <<
                "  元素名称：" << Element->Name <<
                "  元素类型：" << Element->TypeGet_String() <<
                "  元素持续时间：" << Element->DurationTime <<
                "  元素偏移时间：" << this->Elements[i].Offset << "\n";
        }
    }

    return oss.str();
}
void Solution::Clear() {
    this->Elements.clear();
    this->Refresh();
    this->SolutionOffset = 0;
}
void Solution::ResetName(std::string_view NewName) {
    this->Name = NewName;
}
std::string Solution::GetName()const {
    return this->Name;
}
bool Solution::Call(CSATMath::Frame& frame, const float Time, const float PlayBackRate, bool& isPlaying) {
    if (!this->SafeUse) {
        isPlaying = false;
        return false;
    }
    //对时间依次进行两次偏移：解决方案偏移，具体元素具体偏移

    //判断是否有插值结果。
    //有结果，Manager才可能覆盖
    bool bResult = false;

    //尝试依次调用所有的元素插值
    //编号靠后的元素有更高的决定权
    this->Refresh();//调用前立刻更新，智能共享指针锁定状态

    //先分开播放模式逻辑
    switch (this->Playmode) {
    case PlaybackMode::Reuse: {
        //偏移时间轴播放

        //判断偏移后的时间是否位于解决方案持续范围之中（先统一计算偏移后时间，无论究竟有没有偏移）
        float SolutionOffsetedTime = (Time - this->SolutionOffset) * PlayBackRate;

        if (this->EndTime < SolutionOffsetedTime) {
            //播放结束
            this->SolutionOffset = 0;//归位时间
            isPlaying = false;//播放结束
            return false;//无插值结果
        }
        for (size_t i = 0; i < this->Size_Elements; ++i) {
            //这里用减法得到相对于元素的时间
            //尝试该元素插值，如果有结果则代表可以应用
            //这里传入的时间已经是相对时间
            //模式1自动减去头时间
            bResult = bResult || this->Elements[i].Element->Call(frame, SolutionOffsetedTime - this->Elements[i].Offset, PlaybackMode::Reuse);
        }
        break;
    }
    case PlaybackMode::Composition: {
        //默认游戏时间轴播放
        //if (Time < this->StartTime || this->EndTime < Time) {
        //    return false;//无插值结果
        //    //不修改isPlaying状态，使用者任意跳转时间，如果在范围内，仍能给出插值结果
        //}
        for (size_t i = 0; i < this->Size_Elements; ++i) {
            //模式0为组合效果，不减去头时间
            bResult = bResult || this->Elements[i].Element->Call(frame, Time, PlaybackMode::Composition);
        }
        break;
    }
    }

    return bResult;
}
void Solution::SetKeyCheckPack(KeyCheckPack& KCPack) {
    this->KCPack = KCPack;
}
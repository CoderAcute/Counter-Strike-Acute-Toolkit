#include"KeyTracker.hpp"

#include"../../ThirdParty/All_pugixml.hpp"

#include <unordered_map>

std::string KeyCheckPack::GetMsg()const {
    std::ostringstream oss;

    if (this->Ctrl) oss << "Ctrl + ";
    if (this->Shift) oss << "Shift + ";
    if (this->Alt) oss << "Alt + ";

    //虚拟键码到中文的映射
    static const std::unordered_map<unsigned char, std::string> keyMap = {
        {0x70, "F1"}, {0x71, "F2"}, {0x72, "F3"}, {0x73, "F4"},
        {0x74, "F5"}, {0x75, "F6"}, {0x76, "F7"}, {0x77, "F8"},
        {0x78, "F9"}, {0x79, "F10"}, {0x7A, "F11"}, {0x7B, "F12"}
    };

    //处理字母键 (A-Z)
    if (this->vkCode >= 0x41 && this->vkCode <= 0x5A) {
        oss << static_cast<char>(this->vkCode);
    }
    //处理功能键 (F1-F12)
    else if (keyMap.find(this->vkCode) != keyMap.end()) {
        oss << keyMap.at(this->vkCode);
    }
    // 其他键 - 格式化为两位十六进制
    else {
        unsigned char uc = static_cast<unsigned char>(this->vkCode);
        oss << "键码[0x" << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(uc) << "]";
    }

    // 始终显示连击数，包括x1
    oss << "x" << static_cast<int>(this->ComboClick);

    return oss.str();
}
void KeyCheckPack::Refresh() {
    this->Usable = this->vkCode && this->ComboClick;
    return;
}

bool KeyCheckPack::WriteXMLNode(pugi::xml_node& node_KeyCheckPack) {
    this->Refresh();
    node_KeyCheckPack.append_attribute("Usable").set_value(this->Usable ? 1 : 0);
    node_KeyCheckPack.append_attribute("Ctrl").set_value(this->Ctrl ? 1 : 0);
    node_KeyCheckPack.append_attribute("Shift").set_value(this->Shift ? 1 : 0);
    node_KeyCheckPack.append_attribute("Alt").set_value(this->Alt ? 1 : 0);
    node_KeyCheckPack.append_attribute("Key").set_value(this->vkCode);
    node_KeyCheckPack.append_attribute("ComboClick").set_value(this->ComboClick);
    return true;
}
bool KeyCheckPack::ReadXMLNode(pugi::xml_node& node_KeyCheckPack) {
    try {
        this->Usable = node_KeyCheckPack.attribute("Usable").as_bool();
        this->Ctrl = node_KeyCheckPack.attribute("Ctrl").as_bool();
        this->Shift = node_KeyCheckPack.attribute("Shift").as_bool();
        this->Alt = node_KeyCheckPack.attribute("Alt").as_bool();
        this->vkCode = static_cast<unsigned char>(node_KeyCheckPack.attribute("Key").as_uint());
        this->ComboClick = static_cast<unsigned char>(node_KeyCheckPack.attribute("ComboClick").as_uint());
    }
    catch (...) {
        return false;
    }
    this->Refresh();
    return true;
}













KeyTracker::KeyTracker() : IfCreated(false), ThreadRunning(false) {};

KeyTracker::~KeyTracker() {
    this->IfCreated = false;

    //退出线程
    if (this->ThreadRunning) {
        this->ThreadRunning = false;
        this->UpdateThread.join();
    }
}

bool KeyTracker::UpdateKeysState() {
    unsigned int Threshold = this->Threshold;
    unsigned int DThreshold = Threshold * 2;
    this->CurrentTimeMs = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->ClockEpoch).count());
    for (int vkCode = 0; vkCode < 256; ++vkCode) {
        SHORT keyState = GetAsyncKeyState(vkCode);
        this->KeysState[vkCode].Current = (keyState & 0x8000) != 0;
        //this->KeysState[vkCode].Current = (keyboardState[vkCode] & 0x80) != 0;
        if (this->KeysState[vkCode].BufferComboClick) {
            if (this->CurrentTimeMs - this->KeysState[vkCode].LastPressTimeMs > DThreshold) {
                this->KeysState[vkCode].BufferComboClick = 0;
            }
        }
        if (this->KeysState[vkCode].ComboClick) {
            if (this->CurrentTimeMs - this->KeysState[vkCode].LastPressTimeMs > Threshold) {
                this->KeysState[vkCode].BufferComboClick.store(this->KeysState[vkCode].ComboClick.load());
                this->KeysState[vkCode].ComboClick = 0;
            }
        }
        if (this->KeysState[vkCode].Current) {
            if (!this->KeysState[vkCode].Previous) {
                this->KeysState[vkCode].ComboClick++;
            }
            this->KeysState[vkCode].Previous = true;
            this->KeysState[vkCode].LastPressTimeMs = this->CurrentTimeMs;
        }
        else {
            this->KeysState[vkCode].Previous = false;
        }
    }
    return true;
}

bool KeyTracker::TryCreateUpdateThread() {
    if (this->ThreadRunning) return false;

    try {
        this->ThreadRunning = true;
        UpdateThread = std::thread([this]() {
            while (this->ThreadRunning) {
                this->UpdateKeysState();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            });
        return true;
    }
    catch (const std::system_error& e) {
        this->ThreadRunning = false;
        return false;
    }
}

bool KeyTracker::IsKeyPressed(const unsigned char vkCode)const {
    return KeysState[vkCode].Current;//返回指定键的当前状态
}

bool KeyTracker::CheckComboClick(const unsigned char vkCode, unsigned char TargetCombo) {
    if (TargetCombo < 0)return false;
    return this->KeysState[vkCode].BufferComboClick.compare_exchange_strong(TargetCombo, 0);
}

//要求按键检测包与当前状态完全匹配
bool KeyTracker::CheckWithPack(const KeyCheckPack& Pack) {
    if (!Pack.Usable)return false;
    // 检查Ctrl状态必须完全匹配
    bool ctrlPressed = this->IsKeyPressed(VK_CONTROL) ||
        this->IsKeyPressed(VK_LCONTROL) ||
        this->IsKeyPressed(VK_RCONTROL);
    if (Pack.Ctrl != ctrlPressed) return false;

    // 检查Shift状态必须完全匹配
    bool shiftPressed = this->IsKeyPressed(VK_SHIFT) ||
        this->IsKeyPressed(VK_LSHIFT) ||
        this->IsKeyPressed(VK_RSHIFT);
    if (Pack.Shift != shiftPressed) return false;

    // 检查Alt状态必须完全匹配
    bool altPressed = this->IsKeyPressed(VK_MENU) ||
        this->IsKeyPressed(VK_LMENU) ||
        this->IsKeyPressed(VK_RMENU);
    if (Pack.Alt != altPressed) return false;

    return this->CheckComboClick(Pack.vkCode, Pack.ComboClick);
}

unsigned char KeyTracker::CheckComboClickUnremove(const unsigned char vkCode)const {
    return this->KeysState[vkCode].ComboClick;
}

void KeyTracker::ResetThreshold(const unsigned int Threshold) {
    if (Threshold < 1)return;
    this->Threshold = Threshold;
}
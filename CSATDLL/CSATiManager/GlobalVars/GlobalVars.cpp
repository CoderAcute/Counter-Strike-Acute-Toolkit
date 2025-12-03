#include"GlobalVars.hpp"

#include"../CSATiManager.hpp"
#include"../MessageManager/IMessageManager.hpp"

#include<chrono>

bool GlobalVars::Init() {
    this->MsgManager = &this->CSATi->IMessageManager();
	return true;
}

void GlobalVars::VirtualMain() {
	static std::chrono::steady_clock::time_point StartTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point CurrentTime = std::chrono::steady_clock::now();
    static long long LastDelta = 0;
    long long Delta = std::chrono::duration_cast<std::chrono::seconds>(CurrentTime - StartTime).count();
    if (Delta > LastDelta) {
        if (Delta - LastDelta >= 60) {
            this->PublishTickAll();
            this->CoreTick += Delta - LastDelta;
            LastDelta = Delta;
            return;
        }
        for (long long i = 0; i < Delta - LastDelta; ++i) {
            ++this->CoreTick;
            this->Tick();
        }
        LastDelta = Delta;
    }
    return;
}

void GlobalVars::PublishTick(MsgType type) {
    CSATMessage* Msg = new CSATMessage();
    Msg->Type = type;
    this->MsgManager->Publish(Msg);
    return;
}
void GlobalVars::PublishTickAll() {
    this->PublishTick(MsgType::Core_Tick1);
    this->PublishTick(MsgType::Core_Tick5);
    this->PublishTick(MsgType::Core_Tick10);
    this->PublishTick(MsgType::Core_Tick15);
    this->PublishTick(MsgType::Core_Tick20);
    this->PublishTick(MsgType::Core_Tick30);
    this->PublishTick(MsgType::Core_Tick45);
    this->PublishTick(MsgType::Core_Tick60);
}
void GlobalVars::Tick() {
    this->PublishTick(MsgType::Core_Tick1);
    if (this->CoreTick % 5 == 0) this->PublishTick(MsgType::Core_Tick5);
    if (this->CoreTick % 10 == 0) this->PublishTick(MsgType::Core_Tick10);
    if (this->CoreTick % 15 == 0) this->PublishTick(MsgType::Core_Tick15);
    if (this->CoreTick % 20 == 0) this->PublishTick(MsgType::Core_Tick20);
    if (this->CoreTick % 30 == 0) this->PublishTick(MsgType::Core_Tick30);
    if (this->CoreTick % 45 == 0) this->PublishTick(MsgType::Core_Tick45);
    if (this->CoreTick % 60 == 0) this->PublishTick(MsgType::Core_Tick60);
    return;
}
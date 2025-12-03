#pragma once

#include"CSATMessage/CSATMessage.hpp"

#include"../ModuleBase/ModuleBase.hpp"

#include<atomic>
#include<string>

class RegistePack {
public:
	std::atomic<bool>* ptrID{};
	std::string Name{};
};

class IMessageManager :public ModuleBase {
public:
	IMessageManager(CSATiManager* CSATi) :ModuleBase(CSATi) {
		this->Type = ModuleType::MessageManager;
	}

	//注册，获取消息指针
	virtual bool Registe(CSATMessage**& CurrentMessage, RegistePack&& Pack) = 0;
	//订阅，获得对应类型消息推送
	virtual bool Subscribe(MsgType MsgType, std::atomic<bool>* flag) = 0;
	//取消订阅
	virtual bool Unsubscribe(MsgType MsgType, std::atomic<bool>* flag) = 0;
	//发布，在堆空间创建消息后传递，所有权转移至总线
	virtual bool Publish(CSATMessage* Msg) = 0;
	//释放，处理消息后必须调用，减少引用计数，直到为0后由总线释放空间
	virtual bool Release() = 0;
};
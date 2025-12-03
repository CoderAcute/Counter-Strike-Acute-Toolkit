//MessageManager.hpp
//跨线程安全的消息总线
//生命周期管理：由发布者手动在堆new消息，发布后管理权归总线，总线在其任务完成后自动回收

#include"IMessageManager.hpp"

#include<deque>
#include<shared_mutex>
#include<unordered_map>
#include<atomic>

class MessageManager final :public IMessageManager {
private:
	//线程类
	std::shared_mutex MessagerManagerMutex;
	std::atomic<bool> ThreadRunning{};//线程运行状态
	std::thread UpdateThread;//线程对象成员
	//存储类
	std::deque<CSATMessage*> Messages;//类型擦除，准备实现多态（后续增加按type转换后销毁的能力）
	CSATMessage* CurrentMessage = nullptr;
	std::unordered_map<MsgType, std::vector<std::atomic<bool>*>>map;
	std::unordered_map<std::atomic<bool>*, RegistePack>RegisteMsg;
	std::atomic<uint32_t>RefCount = 0;
public:
	MessageManager(CSATiManager* CSATi) :IMessageManager(CSATi) {};
	~MessageManager();

	bool Init()override;
	bool TryCreateUpdateThread();
	//返回true表示正在处理消息，false表示没有消息可处理
	bool NextMsg();
	void VirtualMain()override;

	//接口实现：

	//注册，获取消息指针
	bool Registe(CSATMessage**& CurrentMessage, RegistePack&& Pack)override;
	//订阅，获得对应类型消息推送
	bool Subscribe(MsgType MsgType, std::atomic<bool>* flag)override;
	//取消订阅
	bool Unsubscribe(MsgType MsgType, std::atomic<bool>* flag)override;
	//需要在堆中构建消息，消息的创建由发送者负责，消息的销毁由消息总线负责
	bool Publish(CSATMessage* Msg)override;
	//释放消息，降低引用计数
	bool Release()override;
};
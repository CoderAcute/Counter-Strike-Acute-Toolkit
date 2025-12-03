#include"MessageManager.hpp"

bool MessageManager::Registe(CSATMessage**& CurrentMessage, RegistePack&& Pack) {
	std::unique_lock<std::shared_mutex>lock(this->MessagerManagerMutex);
	//指针引用以重定向，得到访问当前消息服务
	CurrentMessage = &this->CurrentMessage;
	//存储注册信息
	this->RegisteMsg[Pack.ptrID] = std::move(Pack);
	return true;
}

bool MessageManager::Subscribe(MsgType MsgType, std::atomic<bool>* flag) {
	std::unique_lock<std::shared_mutex>lock(this->MessagerManagerMutex);
	this->map[MsgType].push_back(flag);
	return true;
}
bool MessageManager::Unsubscribe(MsgType MsgType, std::atomic<bool>* flag) {
	std::unique_lock<std::shared_mutex>lock(this->MessagerManagerMutex);
	auto it = this->map.find(MsgType);
	if (it == this->map.end()) return false;
	auto& vec = it->second;
	auto vec_it = std::find(vec.begin(), vec.end(), flag);
	if (vec_it == vec.end())return false;
	vec.erase(vec_it);

	return true;
}
bool MessageManager::Publish(CSATMessage* Msg) {
	std::unique_lock<std::shared_mutex>lock(this->MessagerManagerMutex);
	this->Messages.push_back(Msg);
	return true;
}

bool MessageManager::Release() {
	//原子变量不需要锁
	//只会由那些被通知的组件调用，不用担心向下溢出风险
	this->RefCount.fetch_sub(1);
	return true;
}

bool MessageManager::NextMsg() {
	std::unique_lock<std::shared_mutex>lock(this->MessagerManagerMutex);//只有切换消息时加锁，而等待组件处理消息时不加锁，不阻塞发布订阅
	if (this->Messages.empty())return false;
	
	//检测当前引用计数
	if (this->RefCount.load()) {
		//有引用计数
		//说明当前还有消息在被处理
		//执行空闲操作，目前没有
		return true;
	}
	else {
		//如果没有引用计数
		//说明当前没有消息在被处理
		//获取锁
		
		//先检查当前是否存在消息，如果存在则弹出
		if (this->CurrentMessage) {
			this->Messages.pop_front();
			delete this->CurrentMessage;//引用计数已经归零，可以安全释放
			this->CurrentMessage = nullptr;
			if (this->Messages.empty())return false;
		}
		
		//处理下一条消息
		//分发消息给订阅者
		
		auto it = map.find(this->Messages.front()->Type);
		if (it == map.end()) {
			//没有订阅者，设置计数为零，下一帧删除
			this->CurrentMessage = this->Messages.front();
			this->RefCount.store(0);
		}
		else {
			//获取vector容器的大小
			size_t subscriberCount = it->second.size();
			//设置引用计数为订阅者数量
			this->RefCount.store(subscriberCount);
			//修改当前消息指针传递消息
			this->CurrentMessage = this->Messages.front();
			for (auto& flag : it->second) {
				flag->store(true); //通知订阅者
			}
		}
		return true;
	}
	
	//return;
}

bool MessageManager::Init() {
	return true;
}
bool MessageManager::TryCreateUpdateThread() {
	for (;;) {
		if (this->ThreadRunning) return true;

		try {
			this->UpdateThread = std::thread([this]() {
				while (this->ThreadRunning) {
					if (this->NextMsg()) {
						//有消息在处理,快速轮询
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					else {
						//没有消息在处理，降低轮询频率
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
				}
				});
			this->ThreadRunning = true;
			return true;
		}
		catch (...) {
			this->ThreadRunning = false;
			continue;
		}
	}
	
}
MessageManager::~MessageManager() {
	//退出线程
	std::unique_lock<std::shared_mutex> lock(this->MessagerManagerMutex);
	if (this->ThreadRunning) {
		this->ThreadRunning = false;
		this->UpdateThread.join();
	}
	//释放当前消息（如果存在）
	if (this->CurrentMessage) {
		delete this->CurrentMessage;
		this->CurrentMessage = nullptr;
	}
	//释放所有剩余消息
	while (!this->Messages.empty()) {
		delete this->Messages.front();
		this->Messages.pop_front();
	}
	this->map.clear();
	this->RegisteMsg.clear();
	return;
}
void MessageManager::VirtualMain() {
	
	return;
}
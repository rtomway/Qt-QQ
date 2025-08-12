#ifndef NETWORKSERVICELOCATOR_H_
#define NETWORKSERVICELOCATOR_H_

#include <QObject>
#include <atomic>

#include "NetWorkService.h"

class NetWorkServiceLocator :public QObject
{
	Q_OBJECT
public:
	static NetWorkService* instance();
	static void setNetService(NetWorkService* service);

	// 禁止拷贝和移动
	NetWorkServiceLocator(const NetWorkServiceLocator&) = delete;
	NetWorkServiceLocator& operator=(const NetWorkServiceLocator&) = delete;
	NetWorkServiceLocator(NetWorkServiceLocator&&) = delete;
	NetWorkServiceLocator& operator=(NetWorkServiceLocator&&) = delete;
private:
	static std::atomic<NetWorkService*> m_instance;
};


#endif // !NETWORKSERVICELOCATOR_H_

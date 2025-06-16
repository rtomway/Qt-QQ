#include "NetWorkServiceLocator.h"

std::atomic<NetWorkService*> NetWorkServiceLocator::m_instance{ nullptr };
NetWorkService* NetWorkServiceLocator::instance()
{
	return m_instance.load(std::memory_order_acquire);
}


void NetWorkServiceLocator::setNetService(NetWorkService* service)
{
	if (service) {
		m_instance.store(service, std::memory_order_release);
	}
}

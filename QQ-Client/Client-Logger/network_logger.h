#ifndef NETWORK_LOGGER_H_
#define NETWORK_LOGGER_H_

#include <spdlog/spdlog.h>
#include <memory>

class Network_Logger
{
private:
	static std::shared_ptr<spdlog::logger> instance;
};



#endif // !NETWORK_LOGGER_H_



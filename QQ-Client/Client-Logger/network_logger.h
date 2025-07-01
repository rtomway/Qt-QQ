#ifndef NETWORK_LOGGER_H_
#define NETWORK_LOGGER_H_

#include <spdlog/spdlog.h>
#include <memory>
#include <QString>

class Network_Logger
{
public:
	static void init();
	static void debug(const QString& text);
	static void info(const QString& text);
	static void error(const QString& text);
private:
	static std::shared_ptr<spdlog::logger> m_instance;
};



#endif // !NETWORK_LOGGER_H_



#include "Network_logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>  
#include <locale>
#include <QStringConverter>

std::shared_ptr<spdlog::logger> Network_Logger::m_instance = nullptr;

void Network_Logger::init()
{
	if (!m_instance)
	{
		m_instance = spdlog::stdout_color_mt("NETWORK");
		m_instance->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
		m_instance->set_level(spdlog::level::debug);  // 允许显示debug级别
	}
}

void Network_Logger::debug(const QString& text)
{
	if (!m_instance)
		init();
	m_instance->debug(text.toLocal8Bit().constData());
}

void Network_Logger::info(const QString& text)
{
	if (!m_instance)
		init();
	m_instance->info(text.toLocal8Bit().constData());
}

void Network_Logger::error(const QString& text)
{
	if (!m_instance)
		init();
	m_instance->error(text.toLocal8Bit().constData());
}



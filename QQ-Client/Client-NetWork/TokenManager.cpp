#include "TokenManager.h"
#include "SConfigFile.h"

QString TokenManager::m_token = QString();

void TokenManager::setToken(const QString& token)
{
	m_token = token;
	SConfigFile config("config.ini");
	config.setValue("token", m_token);
}

const QString& TokenManager::getToken()
{
	return m_token;
}

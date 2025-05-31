#include "TokenManager.h"
#include "SConfigFile.h"

QString TokenManager::m_token = QString();

//保存token
void TokenManager::saveToken(const QString& token)
{
	m_token = token;
	SConfigFile config("config.ini");
	config.setValue("token", m_token);
}

//获取token
const QString& TokenManager::getToken()
{
	return m_token;
}

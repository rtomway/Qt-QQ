#ifndef TOKENMANAGER_H_
#define TOKENMANAGER_H_

#include <QString>

class TokenManager
{
public:
	static void saveToken(const QString& token);
	static const QString& getToken();
private:
	static QString m_token;
};

#endif // !TOKENMANAGER_H_

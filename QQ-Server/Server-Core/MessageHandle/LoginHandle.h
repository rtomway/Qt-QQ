#ifndef LOGINHANDLE_H_
#define LOGINHANDLE_H_

#include <QJsonObject>
#include <QByteArray>

class LoginHandle
{
public:
	static void handle_login(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static QJsonArray getFriendsMessage(const QString& user_id);
};

#endif // !LOGINHANDLE_H_
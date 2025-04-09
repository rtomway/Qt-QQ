#ifndef LOGINHANDLE_H_
#define LOGINHANDLE_H_

#include <QJsonObject>
#include <QByteArray>
#include <QHttpServerResponder>

class LoginHandle
{
public:
	static void handle_login(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_loginValidation(const QJsonObject& paramsObj,const QByteArray& data, QHttpServerResponder& responder);
	static QJsonArray getFriendsMessage(const QString& user_id);
};

#endif // !LOGINHANDLE_H_
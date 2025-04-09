#ifndef USERHANDLE_H_
#define USERHANDLE_H_

#include <QJsonObject>
#include <QByteArray>
#include <QHttpServerResponder>

class UserHandle
{
public:
	static void handle_searchUser(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_updateUserAvatar(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_updateUserMessage(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder);
	static QStringList getFriendId(const QString& user_id);
};

#endif // !USERHANDLE_H_
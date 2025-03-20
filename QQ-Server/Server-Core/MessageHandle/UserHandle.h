#ifndef USERHANDLE_H_
#define USERHANDLE_H_

#include <QJsonObject>
#include <QByteArray>

class UserHandle
{
public:
	static void handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_updateUserGrouping(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static QStringList getFriendId(const QString& user_id);
};

#endif // !USERHANDLE_H_
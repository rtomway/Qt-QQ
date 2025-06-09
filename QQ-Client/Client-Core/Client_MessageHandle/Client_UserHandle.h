#ifndef CLIENT_USERHANDLE_H_
#define CLIENT_USERHANDLE_H_

#include <QJsonObject>

class Client_UserHandle
{
public:
	void handle_searchUser(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_searchGroup(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_updateUserMessage(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_updateUserAvatar(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
};

#endif // !CLIENT_USERHANDLE_H_
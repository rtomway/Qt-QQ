#ifndef CLIENT_LOGINHANDLE_H_
#define CLIENT_LOGINHANDLE_H_

#include <QJsonObject>

class Client_LoginHandle
{
public:
	void handle_loginSuccess(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_loadFriendList(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_loadFriendAvatars(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_loadGroupList(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_loadGroupMember(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_loadGroupMemberAvatar(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_loadGroupAvatars(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_registerSuccess(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
};

#endif // !CLIENT_LOGINHANDLE_H_
#ifndef CLIENT_GROUPHANDLE_H_
#define CLIENT_GROUPHANDLE_H_

#include <QJsonObject>

class Client_GroupHandle
{
public:
	void handle_createGroupSuccess(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_groupInvite(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_groupTextCommunication(const QJsonObject& paramsObjec = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_groupPictureCommunication(const QJsonObject& paramsObjec = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_newGroupMember(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_groupInviteSuccess(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_groupMemberLoad(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_newGroup(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
};

#endif // !CLIENT_GROUPHANDLE_H_
#ifndef CLIENT_FRIENDHANDLE_H_
#define CLIENT_FRIENDHANDLE_H_

#include <QJsonObject>

class Client_FriendHandle
{
public:
	void handle_addFriend(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_newFriend(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_friendDeleted(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_rejectAddFriend(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_textCommunication(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
	void handle_pictureCommunication(const QJsonObject& paramsObject = QJsonObject(), const QByteArray& data = QByteArray());
};

#endif // !CLIENT_FRIENDHANDLE_H_
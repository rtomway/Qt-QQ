#ifndef FRIENDHANDLE_H_
#define FRIENDHANDLE_H_

#include <QJsonObject>
#include <QByteArray>
#include <QHttpServerResponder>

class FriendHandle
{
public:
	static void handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_friendAddSuccess(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_friendAddFail(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static QVariantMap getUserMessage(const QString& user_id);
	static void handle_updateFriendGrouping(const QJsonObject& params, const QByteArray& data, QHttpServerResponder& responder);
	static void handle_deleteFriend(const QJsonObject&params,const QByteArray& data, QHttpServerResponder& responder);
private:
	static QString m_sendGrouping;
	static QString m_receiveGrouping;
};

#endif // !FRIENDHANDLE_H_
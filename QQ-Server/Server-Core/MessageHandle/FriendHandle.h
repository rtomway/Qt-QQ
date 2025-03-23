#ifndef FRIENDHANDLE_H_
#define FRIENDHANDLE_H_

#include <QJsonObject>
#include <QByteArray>

class FriendHandle
{
public:
	static void handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_deleteFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static void handle_resultOfAddFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	static QVariantMap getUserMessage(const QString& user_id);
	static void handle_updateFriendGrouping(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
private:
	static QString m_sendGrouping;
	static QString m_receiveGrouping;
};

#endif // !FRIENDHANDLE_H_
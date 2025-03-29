#ifndef MESSAGEHANDLE_H_
#define MESSAGEHANDLE_H_

#include <QByteArray>
#include <QJsonObject>
#include <QObject>

class MessageHandle :public QObject
{
	Q_OBJECT
public:
	MessageHandle(QObject* parent = nullptr);
	//消息处理接口
	void handle_message(const QString& message);
	void handle_message(const QByteArray& message);
private:
	//消息处理函数表
	QHash<QString, void(MessageHandle::*)(const QJsonObject&, const QByteArray&)>requestHash{};
private:
	void initRequestHash();
	//各种消息处理函数
	void handle_loginSuccess(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_registerSuccess(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_newFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_rejectAddFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_createGroupSuccess(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_groupInvite(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
};

#endif // !MESSAGEHANDLE_H_

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
	void handle_communication(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_newFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_rejectAddFriend(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
	void handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data = QByteArray());
signals:   //接受到消息发送信号通知UI界面更新同步
	void loginSuccess();
	void communication(const QJsonObject& paramsObject);
	void addFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void newFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void addGroup(const QJsonObject& paramsObject);
	void rejectAddFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void searchUser(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void updateUserMessage(const QJsonObject& paramsObject);
	void updateUserAvatar(const QString& user_id, const QPixmap& pixmap);
};

#endif // !MESSAGEHANDLE_H_

#ifndef EVENTBUS_H_
#define EVENTBUS_H_

#include <QObject>

class EventBus:public QObject
{
	Q_OBJECT
public:
	static EventBus* instance();
private:
	EventBus() {};
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

#endif // !EVENTBUS_H_

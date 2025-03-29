#ifndef EVENTBUS_H_
#define EVENTBUS_H_

#include <QObject>

class EventBus :public QObject
{
	Q_OBJECT
public:
	// 获取单例实例的静态方法
	static EventBus* instance();
	// 禁止拷贝构造函数和赋值操作符
	EventBus(const EventBus&) = delete;
	EventBus& operator=(const EventBus&) = delete;
private:
	EventBus() {};
signals:   //接受到消息发送信号通知UI界面更新同步
	void loginSuccess();
	void registerSuccess(const QJsonObject& obj);
	void textCommunication(const QJsonObject& paramsObject);
	void pictureCommunication(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void addFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void newFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void addGroup(const QJsonObject& paramsObject);
	void rejectAddFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void searchUser(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void updateUserMessage(const QJsonObject& paramsObject);
	void updateUserAvatar(const QString& user_id, const QPixmap& pixmap);
	void deleteFriend(const QString& user_id);
	void createGroupSuccess(const QJsonObject& paramsObject);
	void groupInvite(const QJsonObject& paramsObject, const QPixmap& pixmap);
};

#endif // !EVENTBUS_H_

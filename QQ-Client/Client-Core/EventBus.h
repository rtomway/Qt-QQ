#ifndef EVENTBUS_H_
#define EVENTBUS_H_

#include <QObject>

#include "GlobalTypes.h"

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
	//登录
	void initLoginUser(const QJsonObject& paramsObject);
	void initFriendManager(const QJsonObject& paramsObject);
	void initGroupManager(const QJsonObject& paramsObject);
	void loadGroupMember(const QJsonObject& paramsObject);
	void loadLocalAvatarGroup(const QStringList& group_idList);
	void loadLocalAvatarFriend(const QStringList& friend_idList);
	void registerSuccess(const QJsonObject& obj);
	//好友
	void textCommunication(const QJsonObject& paramsObject);
	void pictureCommunication(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void addFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void newFriend(const QJsonObject& paramsObject);
	void rejectAddFriend(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void deleteFriend(const QString& user_id);
	//群组
	void addGroup(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void createGroupSuccess(const QJsonObject& paramsObject);
	void groupInvite(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void groupPictureCommunication(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void groupTextCommunication(const QJsonObject& paramsObject);
	void groupInviteSuccess(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void newGroupMember(const QJsonObject& paramsObject);
	void groupMemberload(const QJsonObject& paramsObject);
	void newGroup(const QJsonObject& paramsObject);
	void exitGroup(const QString& group_id, const QString& user_id);
	void groupMemberExitGroup(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void removeGroupMember(const QJsonObject& paramsObject);
	//用户
	void searchUser(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void searchGroup(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void updateUserMessage(const QJsonObject& paramsObject);
	void updateUserAvatar(const QString& user_id, const QPixmap& pixmap);
	//头像
	void saveAvatar(const QString& id, ChatType type, const QByteArray& avatarData);
};

#endif // !EVENTBUS_H_

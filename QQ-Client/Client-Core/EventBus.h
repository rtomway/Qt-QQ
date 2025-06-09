#ifndef EVENTBUS_H_
#define EVENTBUS_H_

#include <QObject>

#include "GlobalTypes.h"

class EventBus :public QObject
{
	Q_OBJECT
public:
	static EventBus* instance();
	EventBus(const EventBus&) = delete;
	EventBus& operator=(const EventBus&) = delete;
private:
	EventBus() {};
signals:   //接受到消息发送信号通知UI界面更新同步
	//登录
	void loginValidationSuccess(const QJsonObject& paramsObject);
	void initLoginUser(const QJsonObject& paramsObject);
	void initFriendManager(const QJsonObject& paramsObject);
	void initGroupManager(const QJsonObject& paramsObject);
	void loadGroupMember(const QJsonObject& paramsObject);
	void loadLocalAvatarGroup(const QStringList& group_idList);
	void loadLocalAvatarFriend(const QStringList& friend_idList);
	void registerSuccess(const QJsonObject& obj);
	//好友操作
	void textCommunication(const QJsonObject& paramsObject);
	void pictureCommunication(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void newFriend(const QJsonObject& paramsObject);
	void deleteFriend(const QString& user_id);
	//群组操作
	void createGroupSuccess(const QJsonObject& paramsObject);
	void groupPictureCommunication(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void groupTextCommunication(const QJsonObject& paramsObject);
	void newGroupMember(const QJsonObject& paramsObject);
	void groupMemberload(const QJsonObject& paramsObject);
	void newGroup(const QJsonObject& paramsObject);
	void exitGroup(const QString& group_id, const QString& user_id);
	void removeGroup(const QJsonObject& paramsObject);
	void disbandGroup(const QString& group_id);
	void groupMemberDeleted(const QJsonObject& paramsObject);
	void batch_groupMemberDeleted(const QJsonObject& paramsObject);
	//用户操作
	void searchUser(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void searchGroup(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void updateUserMessage(const QJsonObject& paramsObject);
	void updateUserAvatar(const QString& user_id, const QPixmap& pixmap);
	//头像
	void saveAvatar(const QString& id, ChatType type, const QByteArray& avatarData);
	//通知
	void notice_groupReply(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void notice_groupRequest(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void notice_friendReply(const QJsonObject& paramsObject, const QPixmap& pixmap);
	void notice_friendRequest(const QJsonObject& paramsObject, const QPixmap& pixmap);

	//内部
	void updateChatWidget(ChatType type, const QString& group_id);
	void hideGroupSetPannel();
};

#endif // !EVENTBUS_H_

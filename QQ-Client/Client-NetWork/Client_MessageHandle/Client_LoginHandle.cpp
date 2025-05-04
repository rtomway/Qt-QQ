#include "Client_LoginHandle.h"
#include <QPixmap>
#include <QJsonArray>
#include <QJsonDocument>
#include <FriendManager.h>
#include <Friend.h>
#include <GroupManager.h>
#include <Group.h>

#include "MessageSender.h"
#include "EventBus.h"
#include "ImageUtil.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"

//登录成功
void Client_LoginHandle::handle_loginSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	QJsonObject loginUser = paramsObject;
	qDebug() << "handle_loginSuccess" << loginUser;
	EventBus::instance()->emit initLoginUser(loginUser);
	auto user_id = loginUser["user_id"].toString();
	ImageUtils::saveAvatarToLocal(data, user_id, ChatType::User, [=](bool result)
		{
			if (!result)
				qDebug() << "头像保存失败";
		});
}
//加载好友列表数据
void Client_LoginHandle::handle_loadFriendList(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------handle_loadFriendList--------------------" << paramsObject;
	EventBus::instance()->emit initFriendManager(paramsObject);
}
//加载好友头像
void Client_LoginHandle::handle_loadFriendAvatars(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------handle_loadFriendAvatars--------------------";
	auto friend_id = paramsObject["friend_id"].toString();
	ImageUtils::saveAvatarToLocal(data, friend_id, ChatType::User, [=](bool result)
		{
			if (!result)
				qDebug() << "头像保存失败";
			else
				AvatarManager::instance()->emit loadFriendAvatarSuccess(friend_id);
		});
}
//加载群组列表数据
void Client_LoginHandle::handle_loadGroupList(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------handle_loadGroupList--------------------";
	EventBus::instance()->emit initGroupManager(paramsObject);
}
//加载群成员
void Client_LoginHandle::handle_loadGroupMember(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit loadGroupMember(paramsObject);
}
//加载群成员头像
void Client_LoginHandle::handle_loadGroupMemberAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------handle_loadGroupAvatars--------------------";
	auto user_id = paramsObject["user_id"].toString();
	ImageUtils::saveAvatarToLocal(data, user_id, ChatType::User, [=](bool result)
		{
			if (!result)
				qDebug() << "头像保存失败";
		});
}
//加载群组头像
void Client_LoginHandle::handle_loadGroupAvatars(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------handle_loadGroupAvatars--------------------";
	auto group_id = paramsObject["group_id"].toString();
	ImageUtils::saveAvatarToLocal(data, group_id, ChatType::Group, [=](bool result)
		{
			if (!result)
				qDebug() << "头像保存失败";
			else
				AvatarManager::instance()->emit loadGroupAvatarSuccess(group_id);
		});
}
//注册成功
void Client_LoginHandle::handle_registerSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit registerSuccess(paramsObject);
}

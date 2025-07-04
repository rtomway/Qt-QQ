#include "Client_LoginHandle.h"
#include <QPixmap>
#include <QJsonArray>
#include <QJsonDocument>
#include <FriendManager.h>
#include <Friend.h>
#include <GroupManager.h>
#include <Group.h>

#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include "EventBus.h"
#include "ImageUtil.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"

//登录认证成功
void Client_LoginHandle::handle_loginValidationSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit loginValidationSuccess(paramsObject);
}

//登录成功
void Client_LoginHandle::handle_loginSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	QJsonObject loginUser = paramsObject;
	auto user_id = loginUser["user_id"].toString();
	ImageUtils::saveAvatarToLocal(data, user_id, ChatType::User);
		
	EventBus::instance()->emit initLoginUser(loginUser);
}

//加载好友列表数据
void Client_LoginHandle::handle_loadFriendList(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit initFriendManager(paramsObject);
}

//加载好友头像
void Client_LoginHandle::handle_loadFriendAvatars(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto friend_id = paramsObject["friend_id"].toString();
	ImageUtils::saveAvatarToLocal(data, friend_id, ChatType::User, [=]()
		{
				AvatarManager::instance()->emit loadFriendAvatarSuccess(friend_id);
		});
}

//加载群组列表数据
void Client_LoginHandle::handle_loadGroupList(const QJsonObject& paramsObject, const QByteArray& data)
{
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
	auto user_id = paramsObject["user_id"].toString();
	ImageUtils::saveAvatarToLocal(data, user_id, ChatType::User);
}

//加载群组头像
void Client_LoginHandle::handle_loadGroupAvatars(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto group_id = paramsObject["group_id"].toString();
	ImageUtils::saveAvatarToLocal(data, group_id, ChatType::Group, [=]()
		{
				AvatarManager::instance()->emit loadGroupAvatarSuccess(group_id);
		});
}

//注册成功
void Client_LoginHandle::handle_registerSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit registerSuccess(paramsObject);
}

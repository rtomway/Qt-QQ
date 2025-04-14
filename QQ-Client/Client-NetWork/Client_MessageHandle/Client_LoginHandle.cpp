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

void Client_LoginHandle::handle_loginSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	QJsonObject loginUser = paramsObject["loginUser"].toObject();
	auto user_name = loginUser["username"].toString();
	auto user_id = loginUser["user_id"].toString();
	//当前登录用户信息
	auto user = QSharedPointer<Friend>::create();
	user->setFriend(loginUser);
	qDebug() << "loginUser" << user->getFriend();
	FriendManager::instance()->addFriend(user);
	FriendManager::instance()->setOneselfID(user_id);
	//头像保存文件指定
	ImageUtils::setLoginUser(user_id);
	ImageUtils::saveAvatarToLocal(data, user_id, ChatType::User, [=](bool result)
		{
			if (!result)
				qDebug() << "头像保存失败";
			else
				EventBus::instance()->emit loginSuccess();
		});

	//登录成功申请好友信息
	QJsonObject loadFriendListObj;
	loadFriendListObj["user_id"] = user_id;
	QJsonDocument loadFriendListDoc(loadFriendListObj);
	QByteArray loadFriendListData = loadFriendListDoc.toJson(QJsonDocument::Compact);
	MessageSender::instance()->sendHttpRequest("loadFriendList", loadFriendListData, "application/json");
	//申请群组基本信息
	QJsonObject loadGroupListObj;
	loadGroupListObj["user_id"] = user_id;
	QJsonDocument loadGroupListDoc(loadGroupListObj);
	QByteArray loadGroupListData = loadGroupListDoc.toJson(QJsonDocument::Compact);
	MessageSender::instance()->sendHttpRequest("loadGroupList", loadGroupListData, "application/json");
}

void Client_LoginHandle::handle_loadFriendList(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------handle_loadFriendList--------------------";
	QJsonArray friendArray = paramsObject["friendList"].toArray();
	QStringList friend_IdList;
	//加载好友信息
	for (const QJsonValue& value : friendArray)
	{
		QJsonObject friendObject = value.toObject();
		auto friendUser = QSharedPointer<Friend>::create();
		friendUser->setFriend(friendObject);
		auto& friend_id = friendUser->getFriendId();
		friend_IdList.append(friend_id);
		FriendManager::instance()->addFriend(friendUser);
	}
	//申请好友头像的加载
	const int batchSize = 10;
	int totalSize = friend_IdList.size();
	for (int i = 0; i < totalSize; i += batchSize)
	{
		QJsonArray loadFriendAvatarIdArray;
		QStringList currentBatch = friend_IdList.mid(i, batchSize);
		for (const auto& friendId : currentBatch)
		{
			loadFriendAvatarIdArray.append(friendId);
		}
		QJsonObject loadFriendAvatarIdObj;
		loadFriendAvatarIdObj["friend_ids"] = loadFriendAvatarIdArray;
		QJsonDocument loadFriendAvatarIdDoc(loadFriendAvatarIdObj);
		auto data = loadFriendAvatarIdDoc.toJson(QJsonDocument::Compact);
		MessageSender::instance()->sendHttpRequest("loadFriendAvatars", data, "application/json");
	}
}

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

void Client_LoginHandle::handle_loadGroupList(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------handle_loadGroupList--------------------";
	auto groupListArray = paramsObject["groupList"].toArray();
	QStringList group_IdList;
	for (auto groupValue : groupListArray)
	{
		auto groupObj = groupValue.toObject();
		group_IdList.append(groupObj["group_id"].toString());
		auto myGroup = QSharedPointer<Group>::create();
		myGroup->setGroup(groupObj);
		GroupManager::instance()->addGroup(myGroup);
	}
	const int batchSize = 10;
	int totalSize = group_IdList.size();
	// 按批次处理 group_IdList
	for (int i = 0; i < totalSize; i += batchSize)
	{
		QJsonArray loadGroupAvatarIdArray;
		// 获取当前批次的 10 个 group_id
		QStringList currentBatch = group_IdList.mid(i, batchSize);
		// 处理当前批次的 group_id
		for (const auto& groupId : currentBatch)
		{
			loadGroupAvatarIdArray.append(groupId);
		}
		QJsonObject loadGroupAvatarIdObj;
		loadGroupAvatarIdObj["group_ids"] = loadGroupAvatarIdArray;
		QJsonDocument loadGroupAvatarIdDoc(loadGroupAvatarIdObj);
		auto data = loadGroupAvatarIdDoc.toJson(QJsonDocument::Compact);
		MessageSender::instance()->sendHttpRequest("loadGroupAvatars", data, "application/json");
	}
}

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

void Client_LoginHandle::handle_registerSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit registerSuccess(paramsObject);
}

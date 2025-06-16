#include "FriendService.h"
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>
#include <QStandardPaths>

#include "ImageUtil.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include "PacketCreate.h"
#include "ChatRecordManager.h"
#include "EventBus.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"


constexpr int BATCHSIZE_LOADAVATAR = 10;

FriendService::FriendService(FriendRespository* friendRespository, QObject* parent)
	:QObject(parent)
	, m_friendRespository(friendRespository)
{


}

FriendService::~FriendService()
{

}

//申请好友基本信息
void FriendService::loadFriendList()
{
	QJsonObject loadListObj;
	loadListObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
	QJsonDocument loadListDoc(loadListObj);
	QByteArray loadListData = loadListDoc.toJson(QJsonDocument::Compact);
	NetWorkServiceLocator::instance()->sendHttpRequest("loadFriendList", loadListData, "application/json");
}

//初始化好友中心
void FriendService::initFriendRespository(const QJsonObject& obj)
{
	QJsonArray friendArray = obj["friendList"].toArray();
	QStringList localFriend_IdList;
	QStringList needLoadFriend_IdList;
	for (const QJsonValue& value : friendArray)
	{
		//好友数据加载
		QJsonObject friendObject = value.toObject();
		auto friendUser = QSharedPointer<Friend>::create();
		friendUser->setFriend(friendObject);
		m_friendRespository->addFriend(friendUser);

		//头像是否申请的分组
		auto& friend_id = friendUser->getFriendId();
		if (AvatarManager::instance()->hasLocalAvatar(friend_id, ChatType::User))
		{
			localFriend_IdList.append(friend_id);
		}
		else
		{
			needLoadFriend_IdList.append(friend_id);
		}
	}

	//头像加载及请求
	emit loadLocalAvatarFriend(localFriend_IdList);
	if (!needLoadFriend_IdList.isEmpty())
	{
		loadFriendAvatarFromServer(needLoadFriend_IdList);
	}
}

//更新用户信息
void FriendService::updateUserMessage(const QJsonObject& obj)
{
	auto user_id = obj["user_id"].toString();
	auto user = m_friendRespository->findFriend(user_id);
	user->setFriend(obj);
	emit UpdateFriendMessage(user_id);
}

//新增好友
void FriendService::addNewFriend(const QJsonObject& obj)
{
	auto user = QSharedPointer<Friend>::create();
	user->setFriend(obj);
	auto& user_id = user->getFriendId();
	auto grouping = user->getGrouping();
	m_friendRespository->addFriend(user);
	emit NewFriend(user_id, grouping);
}

//好友删除
void FriendService::removeFriend(const QString& user_id)
{
	emit deleteFriend(user_id);
}

//申请头像的加载(10个一批)
void FriendService::loadFriendAvatarFromServer(const QStringList& friend_idList)
{
	int totalSize = friend_idList.size();
	for (int i = 0; i < totalSize; i += BATCHSIZE_LOADAVATAR)
	{
		QJsonArray loadFriendAvatarIdArray;
		QStringList currentBatch = friend_idList.mid(i, BATCHSIZE_LOADAVATAR);
		for (const auto& friendId : currentBatch)
		{
			loadFriendAvatarIdArray.append(friendId);
		}

		QJsonObject loadFriendAvatarIdObj;
		loadFriendAvatarIdObj["friend_ids"] = loadFriendAvatarIdArray;
		QJsonDocument loadFriendAvatarIdDoc(loadFriendAvatarIdObj);
		auto data = loadFriendAvatarIdDoc.toJson(QJsonDocument::Compact);
		NetWorkServiceLocator::instance()->sendHttpRequest("loadFriendAvatars", data, "application/json");
	}
}

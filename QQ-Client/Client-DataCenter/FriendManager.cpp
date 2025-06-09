#include "FriendManager.h"
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>
#include <QStandardPaths>

#include "ImageUtil.h"
#include "../Client-ServiceLocator/NetWorkServiceLocator.h"
#include "PacketCreate.h"
#include "ChatRecordManager.h"
#include "EventBus.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"

constexpr int BATCHSIZE_LOADAVATAR = 10;

FriendManager* FriendManager::instance() 
{
	static FriendManager instance;
	return &instance;
}

FriendManager::FriendManager()
{
	//申请好友基本信息
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, this, [=]
		{
			QJsonObject loadListObj;
			loadListObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
			QJsonDocument loadListDoc(loadListObj);
			QByteArray loadListData = loadListDoc.toJson(QJsonDocument::Compact);
			NetWorkServiceLocator::instance()->sendHttpRequest("loadFriendList", loadListData, "application/json");
		});
	//初始化好友中心
	connect(EventBus::instance(), &EventBus::initFriendManager, this, [=](const QJsonObject& paramsObject)
		{
			QJsonArray friendArray = paramsObject["friendList"].toArray();
			QStringList localFriend_IdList;
			QStringList needLoadFriend_IdList;
			for (const QJsonValue& value : friendArray)
			{
				//好友数据加载
				QJsonObject friendObject = value.toObject();
				auto friendUser = QSharedPointer<Friend>::create();
				friendUser->setFriend(friendObject);
				FriendManager::instance()->addFriend(friendUser);

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
		});
	//更新用户信息
	connect(EventBus::instance(), &EventBus::updateUserMessage, this, [=](const QJsonObject& obj)
		{
			auto user_id = obj["user_id"].toString();
			auto user = findFriend(user_id);
			user->setFriend(obj);
			emit UpdateFriendMessage(user_id);
		});
	//新增好友
	connect(EventBus::instance(), &EventBus::newFriend, this, [=](const QJsonObject& obj)
		{
			auto user = QSharedPointer<Friend>::create();
			user->setFriend(obj);
			auto& user_id = user->getFriendId();
			auto grouping = user->getGrouping();
			this->addFriend(user);
			emit NewFriend(user_id, grouping);
		});
	//好友删除
	connect(EventBus::instance(), &EventBus::deleteFriend, this, [=](const QString& user_id)
		{
			emit deleteFriend(user_id);
		});
}

//申请头像的加载(10个一批)
void FriendManager::loadFriendAvatarFromServer(const QStringList& friend_idList)
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

//添加好友
void FriendManager::addFriend(const QSharedPointer<Friend>& user)
{
	QString user_id = user->getFriendId();
	if (!m_friends.contains(user_id))
	{
		m_friends.insert(user_id, user);
		ChatRecordManager::instance()->addUserChat(user_id, std::make_shared<ChatRecordMessage>(LoginUserManager::instance()->getLoginUserID(), user_id, ChatType::User));
	}

	if (!m_grouping.contains(user->getGrouping()) && !user->getGrouping().isEmpty())
	{
		m_grouping.insert(user_id, user->getGrouping());
	}
}

//搜索好友
QSharedPointer<Friend> FriendManager::findFriend(const QString& id) const
{
	auto it = m_friends.find(id);
	if (it != m_friends.end()) 
	{
		return it.value(); // 返回找到的用户
	}
	else 
	{
		return nullptr; // 如果未找到，返回空用户
	}
}

//获取相关好友
QHash<QString, QSharedPointer<Friend>> FriendManager::findFriends(const QString& text) const
{
	QHash<QString, QSharedPointer<Friend>> result;
	for (auto it = m_friends.begin(); it != m_friends.end(); ++it) 
	{
		if (it.key().contains(text, Qt::CaseInsensitive)) 
		{
			result.insert(it.key(), it.value());
		}
	}
	return result;
}

//获取所有好友信息
QList<QSharedPointer<Friend>> FriendManager::getAllFriends() const
{
	return m_friends.values();
}

//获取所有好友分组
QStringList FriendManager::getAllFriendGroupings() const
{
	QSet<QString> uniqueGroupings;
	for (const QString& grouping : m_grouping.values())
	{
		uniqueGroupings.insert(grouping);
	}
	return QStringList(uniqueGroupings.begin(), uniqueGroupings.end());
}

//获取所有好友Id
QStringList FriendManager::getAllFriendIdList() const
{
	return m_friends.keys();
}

//判断是否是好友
bool FriendManager::isFriend(const QString& user_id)
{
	if (m_friends.keys().contains(user_id) &&
		user_id != LoginUserManager::instance()->getLoginUserID())
	{
		return true;
	}
	return false;
}

//删除好友
void FriendManager::removeFriend(const QString& friend_id)
{
	m_friends.remove(friend_id);
}

//将好友中心清空(切换用户)
void FriendManager::clearFriendManager()
{
	m_friends.clear();
	m_grouping.clear();
}



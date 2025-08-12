#include "FriendManager.h"

#include "ImageUtil.h"
#include "../Client-ServiceLocator/NetWorkServiceLocator.h"
#include "PacketCreate.h"
#include "ChatRecordManager.h"
#include "EventBus.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"


FriendManager* FriendManager::instance()
{
	static FriendManager instance;
	return &instance;
}

FriendManager::FriendManager()
	:m_friendRespository(new FriendRespository(this))
	, m_friendService(new FriendService(m_friendRespository, this))
{
	init();
}

void FriendManager::init()
{
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, m_friendService, &FriendService::loadFriendList);
	connect(EventBus::instance(), &EventBus::initFriendManager, m_friendService, &FriendService::initFriendRespository);
	connect(EventBus::instance(), &EventBus::updateUserMessage, m_friendService, &FriendService::updateUserMessage);
	connect(EventBus::instance(), &EventBus::newFriend, m_friendService, &FriendService::addNewFriend);
	connect(EventBus::instance(), &EventBus::deleteFriend, m_friendService, &FriendService::removeFriend);

	connect(m_friendService, &FriendService::loadLocalAvatarFriend, this, [=](const QStringList& load_idList)
		{
			this->emit loadLocalAvatarFriend(load_idList);
		});
	connect(m_friendService, &FriendService::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			this->emit UpdateFriendMessage(user_id);
		});
	connect(m_friendService, &FriendService::updateFriendGrouping, this, [=](const QString& user_id, const QString& oldGrouping)
		{
			this->emit updateFriendGrouping(user_id, oldGrouping);
		});
	connect(m_friendService, &FriendService::NewFriend, this, [=](const QString& user_id, const QString& grouping)
		{
			this->emit NewFriend(user_id, grouping);
		});
	connect(m_friendService, &FriendService::deleteFriend, this, [=](const QString& user_id)
		{
			this->emit deleteFriend(user_id);
		});
	connect(m_friendService, &FriendService::chatWithFriend, this, [=](const QString& user_id)
		{
			this->emit chatWithFriend(user_id);
		});
}

//添加好友
void FriendManager::addFriend(const QSharedPointer<Friend>& user)
{
	m_friendRespository->addFriend(user);
}

//搜索好友
QSharedPointer<Friend> FriendManager::findFriend(const QString& id) const
{
	return m_friendRespository->findFriend(id);
}

//获取相关好友
QHash<QString, QSharedPointer<Friend>> FriendManager::findFriends(const QString& text) const
{
	return m_friendRespository->findFriends(text);
}

//获取所有好友信息
QList<QSharedPointer<Friend>> FriendManager::getAllFriends() const
{
	return m_friendRespository->getAllFriends();
}

//获取所有好友分组
QStringList FriendManager::getAllFriendGroupings() const
{
	return m_friendRespository->getAllFriendGroupings();
}

//获取所有好友Id
QStringList FriendManager::getAllFriendIdList() const
{
	return m_friendRespository->getAllFriendIdList();
}

//判断是否是好友
bool FriendManager::isFriend(const QString& user_id)
{
	return m_friendRespository->isFriend(user_id);
}

//删除好友
void FriendManager::removeFriend(const QString& friend_id)
{
	m_friendRespository->removeFriend(friend_id);
}

//将好友中心清空(切换用户)
void FriendManager::clearFriendManager()
{
	m_friendRespository->clearFriendManager();
}





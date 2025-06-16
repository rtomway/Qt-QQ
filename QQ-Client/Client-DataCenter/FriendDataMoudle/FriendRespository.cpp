#include "FriendRespository.h"

#include "ChatRecordManager.h"
#include "LoginUserManager.h"

FriendRespository::FriendRespository(QObject* parent)
{

}

FriendRespository::~FriendRespository()
{


}

//添加好友
void FriendRespository::addFriend(const QSharedPointer<Friend>& user)
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
QSharedPointer<Friend> FriendRespository::findFriend(const QString& id) const
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
QHash<QString, QSharedPointer<Friend>> FriendRespository::findFriends(const QString& text) const
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
QList<QSharedPointer<Friend>> FriendRespository::getAllFriends() const
{
	return m_friends.values();
}

//获取所有好友分组
QStringList FriendRespository::getAllFriendGroupings() const
{
	QSet<QString> uniqueGroupings;
	for (const QString& grouping : m_grouping.values())
	{
		uniqueGroupings.insert(grouping);
	}
	return QStringList(uniqueGroupings.begin(), uniqueGroupings.end());
}

//获取所有好友Id
QStringList FriendRespository::getAllFriendIdList() const
{
	return m_friends.keys();
}

//判断是否是好友
bool FriendRespository::isFriend(const QString& user_id)
{
	if (m_friends.keys().contains(user_id) &&
		user_id != LoginUserManager::instance()->getLoginUserID())
	{
		return true;
	}
	return false;
}

//删除好友
void FriendRespository::removeFriend(const QString& friend_id)
{
	m_friends.remove(friend_id);
}

//将好友中心清空(切换用户)
void FriendRespository::clearFriendManager()
{
	m_friends.clear();
	m_grouping.clear();
}

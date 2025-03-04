#include "FriendManager.h"
#include "ImageUtil.h"

FriendManager* FriendManager::instance() {
	static FriendManager instance;
	return &instance;
}
QString FriendManager::m_oneselfID = QString();

//设置当前用户id
void FriendManager::setOneselfID(const QString& id)
{
	m_oneselfID = id;
	qDebug() << "当前用户ID:" << m_oneselfID;
}

const QString FriendManager::getOneselfID() const
{
	return m_oneselfID;
}

//添加用户信息
void FriendManager::addFriend(const QSharedPointer<Friend>& user)
{
	QString user_id = user->getFriendId();
	if (!m_user.contains(user_id))
		m_user.insert(user_id, user);
}

QSharedPointer<Friend> FriendManager::findFriend(const QString& id) const
{
	auto it = m_user.find(id);
	if (it != m_user.end()) {
		return it.value(); // 返回找到的用户
	}
	else {
		return nullptr; // 如果未找到，返回空用户
	}
}

const QHash<QString, QSharedPointer<Friend>>& FriendManager::getFriends() const
{
	return m_user;
}

void FriendManager::loadAvatar(const QString& user_id)
{
	//找到该用户
	QSharedPointer<Friend> user = m_user.value(user_id);
	user->loadAvatar();
	auto avatar = user->getAvatar();
	qDebug() << "avatar" << avatar;
	if (m_oneselfID == user_id)
	{
		emit UserAvatarLoaded(avatar);
	}
	else
	{
		emit FriendAvatarLoaded(avatar);
	}
}


void FriendManager::clearFriendManager()
{
	m_oneselfID = QString();
	m_user.clear();
}



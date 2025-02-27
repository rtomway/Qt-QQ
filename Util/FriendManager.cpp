#include "FriendManager.h"

FriendManager* FriendManager::instance() {
    static FriendManager instance;
    return &instance;
}

//设置当前用户id
void FriendManager::setOneselfID(const QString& id)
{
    m_oneselfID = id;
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





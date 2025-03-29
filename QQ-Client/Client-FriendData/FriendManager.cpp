#include "FriendManager.h"
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QStandardPaths>

#include "ImageUtil.h"
#include "MessageSender.h"
#include "PacketCreate.h"
#include "ChatRecordManager.h"
#include "EventBus.h"
#include "AvatarManager.h"


QString FriendManager::m_oneselfID = QString();

FriendManager::FriendManager()
{
	connect(EventBus::instance(), &EventBus::updateUserMessage, this, [=](const QJsonObject& obj)
		{
			auto user_id = obj["user_id"].toString();
			auto user = findFriend(user_id);
			qDebug() << "updateUserMessage前" << user->getFriend();
			user->setFriend(obj);
			qDebug() << "updateUserMessage后" << user->getFriend();
			emit UpdateFriendMessage(user_id);
		});
	connect(EventBus::instance(), &EventBus::newFriend, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto user = QSharedPointer<Friend>::create();
			user->setFriend(obj);
			auto user_id = user->getFriendId();
			auto grouping = user->getGrouping();
			//保存目录
			if (ImageUtils::saveAvatarToLocal(pixmap, user_id, ChatType::User))
			{
				AvatarManager::instance()->updateAvatar(user_id, ChatType::User);
			}
			else
			{
				qDebug() << "头像接受失败";
			}
			this->addFriend(user);
			emit NewFriend(user_id, grouping);
		});
	connect(EventBus::instance(), &EventBus::deleteFriend, this, [=](const QString& user_id)
		{
			m_user.remove(user_id);
		});
}
//单例
FriendManager* FriendManager::instance() {
	static FriendManager instance;
	return &instance;
}
//设置当前用户id
void FriendManager::setOneselfID(const QString& id)
{
	m_oneselfID = id;
	qDebug() << "当前用户ID:" << m_oneselfID;
}
//获取当前用户id
const QString FriendManager::getOneselfID() const
{
	return m_oneselfID;
}
//添加用户信息
void FriendManager::addFriend(const QSharedPointer<Friend>& user)
{
	QString user_id = user->getFriendId();
	if (!m_user.contains(user_id))
	{
		m_user.insert(user_id, user);
		ChatRecordManager::instance()->addChat(user_id, std::make_shared<ChatRecordMessage>(m_oneselfID, user_id, ChatType::User));
	}

}
//搜索好友
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
//获取好友
const QHash<QString, QSharedPointer<Friend>>& FriendManager::getFriends() const
{
	return m_user;
}
//好友搜索
QHash<QString, QSharedPointer<Friend>> FriendManager::findFriends(const QString& text) const
{
	QHash<QString, QSharedPointer<Friend>> result;
	for (auto it = m_user.begin(); it != m_user.end(); ++it) {
		if (it.key().contains(text, Qt::CaseInsensitive)) {
			result.insert(it.key(), it.value());
		}
	}
	return result;
}
//将好友中心清空(切换用户)
void FriendManager::clearFriendManager()
{
	m_oneselfID = QString();
	m_user.clear();
}



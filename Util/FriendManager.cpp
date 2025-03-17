#include "FriendManager.h"
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QStandardPaths>

#include "ImageUtil.h"
#include "Client.h"


QString FriendManager::m_oneselfID = QString();

FriendManager::FriendManager()
{
	connect(Client::instance(), &Client::updateUserMessage, this, [=](const QJsonObject& obj)
		{
			auto user_id = obj["user_id"].toString();
			auto user = findFriend(user_id);
			qDebug() << "updateUserMessage前"<<user->getFriend();
			user->setFriend(obj);
			qDebug() << "updateUserMessage后" << user->getFriend();
			emit UpdateFriendMessage(user_id);
		});
	connect(Client::instance(), &Client::updateUserAvatar, this, [=](const QString& user_id, const QPixmap& pixmap)
		{
			auto user = findFriend(user_id);
			qDebug() << "----------F好友头像更新-----------";
			if (ImageUtils::saveAvatarToLocal(pixmap,user_id))
			{
				user->loadAvatar();
				emit UpdateFriendMessage(user_id);
			}
			else
			{
				qDebug() << "头像接收失败";
			}
		});
	connect(Client::instance(), &Client::newFriend, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto user = QSharedPointer<Friend>::create();
			user->setFriend(obj);
			auto user_id = user->getFriendId();
			auto grouping = user->getGrouping();
			//保存目录
			if (ImageUtils::saveAvatarToLocal(pixmap, user_id))
			{
				user->loadAvatar();
			}
			else
			{
				qDebug() << "头像接受失败";
			}
			this->addFriend(user);
			emit NewFriend(user_id, grouping);
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
		m_user.insert(user_id, user);
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
//删除好友
void FriendManager::removeFriend(const QString& user_id)
{
	m_user.remove(user_id);
	deleteFriendToServer(user_id);
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
//用户信息更新向服务端发送
void FriendManager::updateUserMessageToServer(const QJsonObject& obj)
{
	QVariantMap friendObj = obj.toVariantMap();
	//客户端独立信息删除
	friendObj.remove("grouping");
	friendObj.remove("avatar_path"); 
	qDebug() << "--------------用户更新的信息json：" << friendObj;
	Client::instance()->sendMessage("updateUserMessage", friendObj);
}
//用户头像更新向服务端发送
void FriendManager::updateUserAvatarToServer(const QPixmap& pixmap)
{
	if (pixmap.isNull()) {
		qDebug() << "Avatar is empty, no update needed.";
		return;
	}

	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);

	// 将 QPixmap 转换为 PNG 并存入 QByteArray
	if (!pixmap.save(&buffer, "PNG")) {
		qDebug() << "Failed to convert avatar to PNG format.";
		return;
	}
	QVariantMap params;
	params["user_id"] = m_oneselfID;
	params["size"] = byteArray.size();

	Client::instance()->sendBinaryMessage("updateUserAvatar", params, byteArray);

}
//好友分组修改
void FriendManager::updateUserGroupingToServer(const QString& user_id, const QString& friend_id, const QString& grouping)
{
	QVariantMap groupingMap;
	groupingMap["user_id"] = user_id;
	groupingMap["friend_id"] = friend_id;
	groupingMap["grouping"] = grouping;
	Client::instance()->sendMessage("updateUserGrouping", groupingMap);
}
//删除好友
void FriendManager::deleteFriendToServer(const QString& user_id)
{
	QVariantMap deleteMap;
	deleteMap["user_id"] = m_oneselfID;
	deleteMap["friend_id"] = user_id;
	Client::instance()->sendMessage("deleteFriend", deleteMap);
}
//接受到其他用户信息更新信号

//将好友中心清空(切换用户)
void FriendManager::clearFriendManager()
{
	m_oneselfID = QString();
	m_user.clear();
}



#include "FriendManager.h"
#include "ImageUtil.h"
#include "Client.h"
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QStandardPaths>

FriendManager* FriendManager::instance() {
	static FriendManager instance;
	return &instance;
}
QString FriendManager::m_oneselfID = QString();

FriendManager::FriendManager()
{
	connect(Client::instance(), &Client::updateUserMessage, this, [=](const QJsonObject& obj)
		{
			auto user_id = obj["user_id"].toString();
			auto user = findFriend(user_id);
			user->setFriend(obj);
			emit UpdateFriendMessage(user_id);
		});
	connect(Client::instance(), &Client::updateUserAvatar, this, [=](const QString& user_id,const QPixmap&pixmap)
		{
			auto user = findFriend(user_id);
			qDebug() << "----------F好友头像更新-----------";
			//保存目录
			QString avatarFolder = QStandardPaths::writableLocation
			(QStandardPaths::AppDataLocation)+"/avatars";
			auto avatarPath = avatarFolder + "/" + user_id +".png";
			auto isSave= pixmap.save(avatarPath);
			if (isSave)
			{
				user->loadAvatar();
				emit UpdateFriendMessage(user_id);
			}
		});
}

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

//用户信息更新向服务端发送
void FriendManager::updateUserMessageToServer(const QJsonObject& obj)
{
	QVariantMap friendObj=obj.toVariantMap();
	qDebug() << "用户更新的信息json：" << friendObj;
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

//接受到其他用户信息更新信号


//将好友中心清空(切换用户)
void FriendManager::clearFriendManager()
{
	m_oneselfID = QString();
	m_user.clear();
}



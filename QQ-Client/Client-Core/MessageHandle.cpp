#include "MessageHandle.h"
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonArray>
#include <QIODevice>

#include "FriendManager.h"
#include "Friend.h"
#include "SConfigFile.h"
#include "EventBus.h"
#include "AvatarManager.h"


MessageHandle::MessageHandle(QObject* parent)
	:QObject(parent)
{
	initRequestHash();
}
void MessageHandle::initRequestHash()
{
	requestHash["loginSuccess"] = &MessageHandle::handle_loginSuccess;
	requestHash["registerSuccess"] = &MessageHandle::handle_registerSuccess;
	requestHash["textCommunication"] = &MessageHandle::handle_textCommunication;
	requestHash["pictureCommunication"] = &MessageHandle::handle_pictureCommunication;
	requestHash["addFriend"] = &MessageHandle::handle_addFriend;
	requestHash["newFriend"] = &MessageHandle::handle_newFriend;
	requestHash["rejectAddFriend"] = &MessageHandle::handle_rejectAddFriend;
	requestHash["searchUser"] = &MessageHandle::handle_searchUser;
	requestHash["updateUserMessage"] = &MessageHandle::handle_updateUserMessage;
	requestHash["updateUserAvatar"] = &MessageHandle::handle_updateUserAvatar;
	requestHash["createGroupSuccess"] = &MessageHandle::handle_createGroupSuccess;
	requestHash["groupInvite"] = &MessageHandle::handle_groupInvite;
}
//消息处理接口
void MessageHandle::handle_message(const QString& message)
{
	qDebug() << "接受到服务端的文本消息:";
	QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
	if (doc.isObject())
	{
		QJsonObject obj = doc.object();
		auto type = obj["type"].toString();
		auto paramsObject = obj["params"].toObject();
		qDebug() << type << requestHash.contains(type);

		QByteArray data;
		if (requestHash.contains(type))
		{
			auto handle = requestHash[type];
			(this->*handle)(paramsObject, data);
		}
	}
}
void MessageHandle::handle_message(const QByteArray& message)
{
	qDebug() << "接受到服务端的数据消息:";
	QDataStream stream(message);
	stream.setByteOrder(QDataStream::BigEndian);
	stream.setVersion(QDataStream::Qt_6_5);
	// 1️⃣ 读取 `allData` 的总大小（4 字节）
	if (message.size() < sizeof(qint32)) {
		qDebug() << "Incomplete packet: waiting for more data...";
		return;
	}
	qint32 totalSize;
	stream >> totalSize;
	qDebug() << "Total data size:" << totalSize;
	// 2️⃣ 开始循环解析多个数据包
	while (!stream.atEnd()) {
		if (message.size() - stream.device()->pos() < sizeof(qint32)) {
			qDebug() << "Incomplete packet header size!";
			return;
		}
		// 读取当前数据包的大小
		qint32 packetSize;
		stream >> packetSize;
		if (message.size() - stream.device()->pos() < packetSize) {
			qDebug() << "Incomplete full packet!";
			return;
		}
		// 读取 JSON 头部大小
		qint32 headerSize;
		stream >> headerSize;
		// 读取 JSON 头部
		QByteArray headerData(headerSize, Qt::Uninitialized);
		stream.readRawData(headerData.data(), headerSize);
		// 解析 JSON
		QJsonDocument jsonDoc = QJsonDocument::fromJson(headerData);
		if (jsonDoc.isNull()) {
			qDebug() << "Invalid JSON data!";
			return;
		}
		QJsonObject jsonObj = jsonDoc.object();
		QString type = jsonObj["type"].toString();
		QJsonObject params = jsonObj["params"].toObject();
		qDebug() << "Received packet type:" << type;
		qDebug() << "Params:" << params;
		// 读取二进制数据（图片）
		int imageSize = params["size"].toInt();
		QByteArray imageData(imageSize, Qt::Uninitialized);
		stream.readRawData(imageData.data(), imageSize);
		// 根据类型给处理函数处理
		if (requestHash.contains(type)) {
			auto handle = requestHash[type];
			(this->*handle)(params, imageData);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << type;
		}
	}
}
//消息处理
void MessageHandle::handle_loginSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	QJsonObject loginUser = paramsObject["loginUser"].toObject();
	QJsonArray friendArray = paramsObject["friendArray"].toArray();
	auto user_name = loginUser["username"].toString();
	auto user_id = loginUser["user_id"].toString();
	//当前登录用户信息
	auto user = QSharedPointer<Friend>::create();
	user->setFriend(loginUser);
	qDebug() << "loginUser" << user->getFriend();
	FriendManager::instance()->addFriend(user);
	AvatarManager::instance()->updateAvatar(user_id, ChatType::User);
	FriendManager::instance()->setOneselfID(user_id);
	//加载好友信息
	for (const QJsonValue& value : friendArray)
	{
		QJsonObject friendObject = value.toObject();
		auto friendUser = QSharedPointer<Friend>::create();
		friendUser->setFriend(friendObject);
		auto friend_id = friendUser->getFriendId();
		FriendManager::instance()->addFriend(friendUser);
		AvatarManager::instance()->updateAvatar(friend_id, ChatType::User);
	}
	FriendManager::instance()->emit FriendManagerLoadSuccess();
	EventBus::instance()->emit loginSuccess();
}
void MessageHandle::handle_registerSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit registerSuccess(paramsObject);
}
void MessageHandle::handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit textCommunication(paramsObject);
}
void MessageHandle::handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap pixmap;
	if (data.isEmpty())
	{
		qDebug() << "无数据";
	}
	else if (!pixmap.loadFromData(data))
	{
		qDebug() << "client-头像加载失败";
		return;
	};
	EventBus::instance()->emit pictureCommunication(paramsObject, pixmap);
}
void MessageHandle::handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap pixmap;
	if (data.isEmpty())
	{
		qDebug() << "无数据";
		pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	else if (!pixmap.loadFromData(data))
	{
		qDebug() << "client-头像加载失败";
		return;
	};
	EventBus::instance()->emit addFriend(paramsObject, pixmap);
}
void MessageHandle::handle_newFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar";
		avatar = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	EventBus::instance()->emit newFriend(paramsObject, avatar);
}
void MessageHandle::handle_rejectAddFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap pixmap;
	if (data.isEmpty())
	{
		qDebug() << "无数据";
		pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
		EventBus::instance()->emit rejectAddFriend(paramsObject, pixmap);
	}
	if (!pixmap.loadFromData(data))
	{
		qDebug() << "client-头像加载失败";
		return;
	};
	EventBus::instance()->emit rejectAddFriend(paramsObject, pixmap);
}
void MessageHandle::handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar";
		avatar = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	EventBus::instance()->emit searchUser(paramsObject, avatar);
}
void MessageHandle::handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit updateUserMessage(paramsObject);
}
void MessageHandle::handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	qDebug() << "好友" << user_id << "的头像更新---------- - ";
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}

	EventBus::instance()->emit updateUserAvatar(user_id, avatar);
}
void MessageHandle::handle_createGroupSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "handle_createGroupSuccess:" << paramsObject;
	EventBus::instance()->emit createGroupSuccess(paramsObject);
}
void MessageHandle::handle_groupInvite(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	qDebug() << "群聊邀请";
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}
	EventBus::instance()->emit groupInvite(paramsObject, avatar);
}
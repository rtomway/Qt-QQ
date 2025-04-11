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
#include "SConfigFile.h"


MessageHandle::MessageHandle(QObject* parent)
	:QObject(parent)
{
	initRequestHash();
}
// 注册请求类型和对应的处理函数
template <typename T>
void MessageHandle::registerHandle(const QString& key, T& instance, void (T::* handler)(const QJsonObject&, const QByteArray&))
{
	requestHash[key] = [&instance, handler](const QJsonObject& obj, const QByteArray& data)
		{
			(instance.*handler)(obj, data);
		};
}
//注册表
void MessageHandle::initRequestHash()
{
	//登录界面
	registerHandle("loginSuccess", m_loginHandle, &Client_LoginHandle::handle_loginSuccess);
	registerHandle("registerSuccess", m_loginHandle, &Client_LoginHandle::handle_registerSuccess);
	//好友处理
	registerHandle("addFriend", m_friendHandle, &Client_FriendHandle::handle_addFriend);
	registerHandle("newFriend", m_friendHandle, &Client_FriendHandle::handle_newFriend);
	registerHandle("rejectAddFriend", m_friendHandle, &Client_FriendHandle::handle_rejectAddFriend);
	registerHandle("textCommunication", m_friendHandle, &Client_FriendHandle::handle_textCommunication);
	registerHandle("pictureCommunication", m_friendHandle, &Client_FriendHandle::handle_pictureCommunication);
	//用户
	registerHandle("searchUser", m_userHandle, &Client_UserHandle::handle_searchUser);
	registerHandle("updateUserMessage", m_userHandle, &Client_UserHandle::handle_updateUserMessage);
	registerHandle("updateUserAvatar", m_userHandle, &Client_UserHandle::handle_updateUserAvatar);
	//群组
	registerHandle("groupTextCommunication", m_groupHandle, &Client_GroupHandle::handle_groupTextCommunication);
	registerHandle("newGroupMember", m_groupHandle, &Client_GroupHandle::handle_newGroupMember);
	registerHandle("groupInviteSuccess", m_groupHandle, &Client_GroupHandle::handle_groupInviteSuccess);
	registerHandle("groupMemberLoad", m_groupHandle, &Client_GroupHandle::handle_groupMemberLoad);
	registerHandle("groupLoad", m_groupHandle, &Client_GroupHandle::handle_groupLoad);
	registerHandle("createGroupSuccess", m_groupHandle, &Client_GroupHandle::handle_createGroupSuccess);
	registerHandle("groupInvite", m_groupHandle, &Client_GroupHandle::handle_groupInvite);
}
//记录token
void MessageHandle::token(const QString& token)
{
	SConfigFile config("config.ini");
	config.setValue("token", token);
	emit connectToServer();
}
//消息处理接口
void MessageHandle::handle_textMessage(const QJsonDocument& messageDoc)
{
	qDebug() << "接受到服务端的文本消息:";
	if (messageDoc.isObject())
	{
		QJsonObject obj = messageDoc.object();

		if (obj.contains("token"))
		{
			auto token = obj["token"].toString();
			this->token(token);
			return;
		}
		auto type = obj["type"].toString();
		auto paramsObject = obj["params"].toObject();
		qDebug() << type << requestHash.contains(type);

		QByteArray data;
		if (requestHash.contains(type))
		{
			auto handle = requestHash[type];
			handle(paramsObject, data);
		}
	}
}
void MessageHandle::handle_binaryMessage(const QByteArray& message)
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
			handle(params, imageData);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << type;
		}
	}
}

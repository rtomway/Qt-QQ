#include "MessageHandle.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QIODevice>
#include <QWebSocket>
#include <QRandomGenerator>

#include "ImageUtil.h"
#include "CreateId.h"
#include "PacketCreate.h"
#include "DataBaseQuery.h"
#include "Server-MessageHandle/LoginHandle.h"
#include "Server-MessageHandle/FriendHandle.h"
#include "Server-MessageHandle/GroupHandle.h"
#include "Server-MessageHandle/RegisterHandle.h"
#include "Server-MessageHandle/UserHandle.h"

MessageHandle::MessageHandle(QObject* parent)
	:QObject(parent)
{
	initWebRequestHash();
	initHttpRequestHash();
}
//映射表
void MessageHandle::initWebRequestHash()
{
	//登录
	webRequestHash["login"] = &LoginHandle::handle_login;
	//好友处理
	webRequestHash["textCommunication"] = &FriendHandle::handle_textCommunication;
	webRequestHash["pictureCommunication"] = &FriendHandle::handle_pictureCommunication;
	webRequestHash["addFriend"] = &FriendHandle::handle_addFriend;
	webRequestHash["friendAddSuccess"] = &FriendHandle::handle_friendAddSuccess;
	webRequestHash["friendAddFail"] = &FriendHandle::handle_friendAddFail;
	//群组处理
	webRequestHash["createGroup"] = &GroupHandle::handle_createGroup;
	webRequestHash["groupTextCommunication"] = &GroupHandle::handle_groupTextCommunication;
	webRequestHash["groupInviteSuccess"] = &GroupHandle::handle_groupInviteSuccess;
}
void MessageHandle::initHttpRequestHash()
{
	//登录注册
	httpRequestHash["loginValidation"] = &LoginHandle::handle_loginValidation;
	httpRequestHash["loadFriendList"] = &LoginHandle::handle_loadFriendList;
	httpRequestHash["loadFriendAvatars"] = &LoginHandle::handle_loadFriendAvatars;
	httpRequestHash["loadGroupList"] = &LoginHandle::handle_loadGroupList;
	httpRequestHash["loadGroupAvatars"] = &LoginHandle::handle_loadGroupAvatars;
	httpRequestHash["register"] = &RegisterHandle::handle_register;
	//搜索
	httpRequestHash["serachUser"] = &UserHandle::handle_searchUser;
	httpRequestHash["searchGroup"] = &GroupHandle::handle_searchGroup;
	//更新
	httpRequestHash["updateFriendGrouping"] = &FriendHandle::handle_updateFriendGrouping;
	httpRequestHash["updateUserMessage"] = &UserHandle::handle_updateUserMessage;
	httpRequestHash["updateUserAvatar"] = &UserHandle::handle_updateUserAvatar;
	httpRequestHash["deleteFriend"] = &FriendHandle::handle_deleteFriend;
}
//token验证
bool MessageHandle::tokenRight(const QString& token)
{
	qDebug() << "TOKEN:" << token;
	return true;
}

//消息处理接口
void MessageHandle::handle_message(const QString& message, QWebSocket* socket)
{
	QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
	if (doc.isObject())
	{
		qDebug() << "客户端发来消息:";
		QJsonObject obj = doc.object();
		QString client_id;
		auto type = obj["type"].toString();
		auto paramsObject = obj["params"].toObject();
		client_id = paramsObject["user_id"].toString();
		auto token = obj["token"].toString();
		if (tokenRight(token))
		{
			qDebug() << "tokenRight";
			emit this->addClient(client_id, socket);
		}
		qDebug() << "客户端发来消息:" << client_id << "type:" << type << webRequestHash.contains(type);
		//根据类型给处理函数处理
		if (webRequestHash.contains(type))
		{
			auto handle = webRequestHash[type];
			handle(paramsObject, QByteArray());
		}
		else
		{

		}

	}
}
void MessageHandle::handle_message(const QByteArray& message, QWebSocket* socket)
{
	qDebug() << message;
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
		qDebug() << "客户端发来消息:" << "type:" << type << webRequestHash.contains(type);
		// 根据类型给处理函数处理
		if (webRequestHash.contains(type)) {
			auto handle = webRequestHash[type];
			handle(params, imageData);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << type;
		}
	}
}
void MessageHandle::handle_message(const QString& type, const QHttpServerRequest& request, QHttpServerResponder& response)
{
	auto requestBody = request.body();
	auto paramsObj = QJsonDocument::fromJson(requestBody).object();
	// 根据类型给处理函数处理
	if (httpRequestHash.contains(type)) {
		auto handle = httpRequestHash[type];
		handle(paramsObj, QByteArray(), response);  // 调用对应的处理函数
	}
	else {
		qDebug() << "未知的类型:" << type;
	}
}
void MessageHandle::handle_message(const QHttpServerRequest& request, QHttpServerResponder& responder)
{
	auto message = request.body();
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
		qDebug() << "客户端发来消息:" << "type:" << type << httpRequestHash.contains(type);
		// 根据类型给处理函数处理
		if (httpRequestHash.contains(type)) {
			auto handle = httpRequestHash[type];
			handle(params, imageData, responder);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << type;
		}
	}
}


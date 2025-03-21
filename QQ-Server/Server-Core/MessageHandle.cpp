#include "MessageHandle.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QIODevice>
#include <QWebSocket>
#include <QRandomGenerator>

#include "ImageUtil.h"
#include "PacketCreate.h"
#include "DataBaseQuery.h"
#include "MessageHandle/LoginHandle.h"
#include "MessageHandle/FriendHandle.h"
#include "MessageHandle/GroupHandle.h"
#include "MessageHandle/RegisterHandle.h"
#include "MessageHandle/UserHandle.h"

MessageHandle::MessageHandle(QObject* parent)
	:QObject(parent)
{
	initRequestHash();
}
//映射表
void MessageHandle::initRequestHash()
{
	requestHash["login"] = &LoginHandle::handle_login;
	requestHash["register"] = &RegisterHandle::handle_register;
	requestHash["communication"] = &FriendHandle::handle_communication;
	requestHash["addFriend"] = &FriendHandle::handle_addFriend;
	requestHash["resultOfAddFriend"] = &FriendHandle::handle_resultOfAddFriend;
	requestHash["deleteFriend"] = &FriendHandle::handle_deleteFriend;
	requestHash["searchUser"] = &UserHandle::handle_searchUser;
	requestHash["updateUserMessage"] = &UserHandle::handle_updateUserMessage;
	requestHash["updateUserAvatar"] = &UserHandle::handle_updateUserAvatar;
	requestHash["updateUserGrouping"] = &UserHandle::handle_updateUserGrouping;
	requestHash["searchGroup"] = &GroupHandle::handle_searchGroup;
	requestHash["addGroup"] = &GroupHandle::handle_addGroup;
}
//用户id生成
QString MessageHandle::getRandomID(int length)
{
	QString user_id;
	QRandomGenerator randomID = QRandomGenerator::securelySeeded(); // 手动初始化种子;
	while (user_id.size() < length)
	{
		int ram = randomID.bounded(0, 9);
		user_id.append(QString::number(ram));
	}
	qDebug() << "RandomID:" << user_id;
	return user_id;
}
QString MessageHandle::generateUserID()
{
	QString user_id;
	//注册唯一id
	while (true) {
		//服务器随机生成10位数用户id
		user_id = getRandomID(10);
		qDebug() << "RandomID后的:" << user_id;
		//先查询生成id是否已存在
		DataBaseQuery query;
		QString queryStr = "select user_id from user where user_id=?";
		QVariantList bindvalues;
		bindvalues.append(user_id);
		auto allQueryObj = query.executeQuery(queryStr, bindvalues);
		//错误返回
		if (allQueryObj.contains("error")) {
			qDebug() << "Error executing query:" << allQueryObj["error"].toString();
			return QString();
		}
		//未找到，id唯一跳出
		QJsonArray userArray = allQueryObj["data"].toArray();
		if (userArray.isEmpty())
		{
			return user_id;
		}
	}
}
//消息处理接口
void MessageHandle::handle_message(const QString& message, QWebSocket* socket)
{
	QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
	if (doc.isObject())
	{
		QJsonObject obj = doc.object();
		QString client_id;
		auto type = obj["type"].toString();
		auto paramsObject = obj["params"].toObject();
		if (paramsObject["user_id"].toString().isEmpty())
		{
			//注册时没有user_id服务器产生
			client_id = generateUserID();
			paramsObject["user_id"] = client_id;
		}
		else
		{
			client_id = paramsObject["user_id"].toString();
		}
		QByteArray data;
		
		emit this->addClient(client_id, socket);
		qDebug() << "客户端发来消息:" << client_id << "type:" << type << requestHash.contains(type);
		//根据类型给处理函数处理
		if (requestHash.contains(type))
		{
			auto handle = requestHash[type];
			handle(paramsObject, data);
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
		qDebug() << "客户端发来消息:" << "type:" << type << requestHash.contains(type);
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


#include "FriendHandle.h"
#include "DataBaseQuery.h"
#include <QJsonArray>
#include <QJsonDocument>
#include "ConnectionManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"

QString FriendHandle::m_sendGrouping=QString();
QString FriendHandle::m_receiveGrouping= QString();

void FriendHandle::handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "添加好友";
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	auto client_id = receive_id;
	//保存分组信息
	m_sendGrouping = paramsObject["grouping"].toString();
	//数据包装入信息
	QVariantMap senderMessage;
	senderMessage["user_id"] = send_id;
	senderMessage["message"] = paramsObject["message"].toString();
	senderMessage["username"] = paramsObject["username"].toString();
	senderMessage["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	senderMessage["addFriend"] = "请求加为好友";
	auto imageData = ImageUtils::loadImage(send_id);
	senderMessage["size"] = imageData.size();
	//包装数据包
	auto userPacket = PacketCreate::binaryPacket("addFriend", senderMessage, imageData);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//发送数据
	ConnectionManager::instance()->sendBinaryMessage(client_id, allData);
	qDebug() << "发送了申请信息";
}

void FriendHandle::handle_communication(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	QJsonObject jsondate;
	jsondate["type"] = "communication";
	jsondate["params"] = paramsObject;
	auto client_id = paramsObject["to"].toString();

	QJsonDocument doc(jsondate);
	QString message = QString(doc.toJson(QJsonDocument::Compact));
	ConnectionManager::instance()->sendTextMessage(client_id, message);
}

void FriendHandle::handle_resultOfAddFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "好友添加结果";
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	auto result = paramsObject["result"].toBool();
	if (result)
	{
		//保存分组信息
		m_receiveGrouping = paramsObject["grouping"].toString();
		//好友列表新增
		DataBaseQuery query;
		QString queryStr = "insert into friendship (user_id,friend_id,Fgrouping)values(?,?,?),(?,?,?)";
		QVariantList bindvalues;
		bindvalues.append(send_id);
		bindvalues.append(receive_id);
		bindvalues.append(m_receiveGrouping);
		bindvalues.append(receive_id);
		bindvalues.append(send_id);
		bindvalues.append(m_sendGrouping);
		auto queryResult = query.executeNonQuery(queryStr, bindvalues);
		bindvalues.clear();
		//错误返回
		if (!queryResult) {
			qDebug() << "Error query:";
			return;
		}
		//通知两边客户端
		QVariantMap receiveMap = getUserMessage(receive_id);
		receiveMap["grouping"] = m_sendGrouping;
		QVariantMap sendMap = getUserMessage(send_id);
		sendMap["grouping"] = m_receiveGrouping;
		auto reveiveImage = ImageUtils::loadImage(receive_id);
		auto sendImage = ImageUtils::loadImage(send_id);
		receiveMap["size"] = reveiveImage.size();
		sendMap["size"] = sendImage.size();
		//打包
		auto receivePacket = PacketCreate::binaryPacket("newFriend", receiveMap, reveiveImage);
		QByteArray receiveData;
		PacketCreate::addPacket(receiveData, receivePacket);
		auto receiveAllData = PacketCreate::allBinaryPacket(receiveData);
		auto sendPacket = PacketCreate::binaryPacket("newFriend", sendMap, sendImage);
		QByteArray sendData;
		PacketCreate::addPacket(sendData, sendPacket);
		auto sendAllData = PacketCreate::allBinaryPacket(sendData);
		//发送
		ConnectionManager::instance()->sendBinaryMessage(send_id, receiveAllData);
		ConnectionManager::instance()->sendBinaryMessage(receive_id, sendAllData);
	}
	else
	{
		//数据包装入信息
		QVariantMap senderMessage;
		senderMessage["user_id"] = send_id;
		senderMessage["username"] = paramsObject["username"].toString();
		senderMessage["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
		senderMessage["addFriend"] = "拒绝了你的好友申请";
		auto imageData = ImageUtils::loadImage(send_id);
		senderMessage["size"] = imageData.size();
		//包装数据包
		auto userPacket = PacketCreate::binaryPacket("rejectAddFriend", senderMessage, imageData);
		QByteArray userData;
		PacketCreate::addPacket(userData, userPacket);
		auto allData = PacketCreate::allBinaryPacket(userData);
		//发送数据
		ConnectionManager::instance()->sendBinaryMessage(receive_id, allData);
	}
}

QVariantMap FriendHandle::getUserMessage(const QString& user_id)
{
	//查询用户信息
	DataBaseQuery query;
	QString queryStr = "select * from user where user_id=?";
	QVariantList bindvalues;
	bindvalues.append(user_id);
	auto allQueryObj = query.executeQuery(queryStr, bindvalues);
	//错误返回
	if (allQueryObj.contains("error")) {
		qDebug() << "Error executing query:" << allQueryObj["error"].toString();
		return QVariantMap();
	}
	//查询数据验证
	QJsonObject userDataObj; //存放返回客户端的所有信息
	QJsonArray userArray = allQueryObj["data"].toArray();
	for (const auto& userValue : userArray)
	{
		QJsonObject userObj = userValue.toObject();
		userObj.remove("password");
		return userObj.toVariantMap();
	}
}

void FriendHandle::handle_deleteFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "删除好友";
	auto user_id = paramsObject["user_id"].toString();
	auto friend_id = paramsObject["friend_id"].toString();
	// 数据库查询
	//注册信息插入
	DataBaseQuery query;
	QString queryStr = "delete from friendship where user_id=? and friend_id=?";
	QVariantList bindvalues;
	bindvalues.append(user_id);
	bindvalues.append(friend_id);
	auto queryResult = query.executeNonQuery(queryStr, bindvalues);
	bindvalues.clear();
	//错误返回
	if (!queryResult) {
		qDebug() << "Error query:";
		return;
	}
}

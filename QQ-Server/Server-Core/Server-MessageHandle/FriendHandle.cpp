#include "FriendHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"

QString FriendHandle::m_sendGrouping = QString();
QString FriendHandle::m_receiveGrouping = QString();
//好友添加
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
	senderMessage = paramsObject.toVariantMap();
	senderMessage["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	auto imageData = ImageUtils::loadImage(send_id, ChatType::User);
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
//文字交流
void FriendHandle::handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	QJsonObject jsondate;
	jsondate["type"] = "textCommunication";
	jsondate["params"] = paramsObject;
	auto client_id = paramsObject["to"].toString();

	QJsonDocument doc(jsondate);
	QString message = QString(doc.toJson(QJsonDocument::Compact));
	ConnectionManager::instance()->sendTextMessage(client_id, message);
}
//图片交流
void FriendHandle::handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	//数据打包
	auto userPacket = PacketCreate::binaryPacket("pictureCommunication", paramsObject.toVariantMap(), data);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	auto client_id = paramsObject["to"].toString();
	ConnectionManager::instance()->sendBinaryMessage(client_id, allData);
	qDebug() << "图片发送";
}
//好友添加成功
void FriendHandle::handle_friendAddSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "好友添加结果";
	qDebug() << "发送方:" << paramsObject["user_id"].toString();
	qDebug() << "接受方:" << paramsObject["to"].toString();
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
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
	auto reveiveImage = ImageUtils::loadImage(receive_id, ChatType::User);
	auto sendImage = ImageUtils::loadImage(send_id, ChatType::User);
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
//好友添加失败
void FriendHandle::handle_friendAddFail(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	//数据包装入信息
	QVariantMap senderMessage;
	senderMessage = paramsObject.toVariantMap();
	auto imageData = ImageUtils::loadImage(send_id, ChatType::User);
	senderMessage["size"] = imageData.size();
	//包装数据包
	auto userPacket = PacketCreate::binaryPacket("rejectAddFriend", senderMessage, imageData);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//发送数据
	ConnectionManager::instance()->sendBinaryMessage(receive_id, allData);
}
//获取用户信息
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
//好友分组更新
void FriendHandle::handle_updateFriendGrouping(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	auto friend_id = paramsObj["friend_id"].toString();
	auto grouping = paramsObj["grouping"].toString();
	//数据库查询
	//注册信息插入
	DataBaseQuery query;
	QString queryStr = "update friendship f set Fgrouping = ? where f.user_id=? and f.friend_id=?";
	QVariantList bindvalues;
	bindvalues.append(grouping);
	bindvalues.append(user_id);
	bindvalues.append(friend_id);
	auto queryResult = query.executeNonQuery(queryStr, bindvalues);
	bindvalues.clear();
	//错误返回
	if (!queryResult) {
		qDebug() << "Error query:";
		return;
	}
	responder.write(QHttpServerResponder::StatusCode::NoContent);
}
//好友删除
void FriendHandle::handle_deleteFriend(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	qDebug() << "删除好友";
	//auto paramsObject = QJsonDocument::fromJson(data);
	auto user_id = paramsObj["user_id"].toString();
	auto friend_id = paramsObj["friend_id"].toString();
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
	responder.write(QHttpServerResponder::StatusCode::NoContent);
}

#include "FriendHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ImageUtil.h"
#include "PacketCreate.h"
#include "FriendDBUtils.h"
#include "UserDBUtils.h"

#include "../Server-ServiceLocator/NetWorkServiceLocator.h"

QString FriendHandle::m_sendGrouping = QString();
QString FriendHandle::m_receiveGrouping = QString();

//好友添加
void FriendHandle::handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	//保存分组信息
	m_sendGrouping = paramsObject["Fgrouping"].toString();
	//数据包装入信息
	QVariantMap senderMessage;
	senderMessage = paramsObject.toVariantMap();
	senderMessage["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	auto imageData = ImageUtils::loadImage(send_id, ChatType::User);
	//包装数据包
	auto userPacket = PacketCreate::binaryPacket("addFriend", senderMessage, imageData);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//发送数据
	NetWorkServiceLocator::instance()->sendBinaryMessage(receive_id, allData);
}

//文字交流
void FriendHandle::handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	QJsonObject jsondate;
	jsondate["type"] = "textCommunication";
	jsondate["params"] = paramsObject;
	auto client_id = paramsObject["to"].toString();

	QJsonDocument doc(jsondate);
	QString message = QString(doc.toJson(QJsonDocument::Compact));
	NetWorkServiceLocator::instance()->sendTextMessage(client_id, message);
}

//图片交流
void FriendHandle::handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	//数据打包
	auto userPacket = PacketCreate::binaryPacket("pictureCommunication", paramsObject.toVariantMap(), data);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	auto client_id = paramsObject["to"].toString();
	NetWorkServiceLocator::instance()->sendBinaryMessage(client_id, allData);
}

//好友添加成功
void FriendHandle::handle_friendAddSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto send_id = paramsObject["user_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	//保存分组信息
	m_receiveGrouping = paramsObject["grouping"].toString();
	MyFriend friend_1(send_id, receive_id, m_sendGrouping);
	MyFriend friend_2(receive_id, send_id, m_receiveGrouping);
	//好友列表新增
	DataBaseQuery query;
	QVariantMap sendMap;
	QVariantMap receiveMap;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!FriendDBUtils::insertFriend(friend_1, query, queryPtr))
			{
				return false;
			}
			if (!FriendDBUtils::insertFriend(friend_2, query, queryPtr))
			{
				return false;
			}
			sendMap = UserDBUtils::queryUserDetail(send_id, query, queryPtr).toVariantMap();
			sendMap["Fgrouping"] = m_receiveGrouping;
			if (sendMap.contains("error"))
			{
				return false;
			}
			receiveMap = UserDBUtils::queryUserDetail(receive_id, query, queryPtr).toVariantMap();
			receiveMap["Fgrouping"] = m_sendGrouping;
			if (receiveMap.contains("error"))
			{
				return false;
			}
		});
	if (!result)
		return;
	//通知两边客户端
	auto reveiveImage = ImageUtils::loadImage(receive_id, ChatType::User);
	auto sendImage = ImageUtils::loadImage(send_id, ChatType::User);
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
	NetWorkServiceLocator::instance()->sendBinaryMessage(send_id, receiveAllData);
	NetWorkServiceLocator::instance()->sendBinaryMessage(receive_id, sendAllData);
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
	//包装数据包
	auto userPacket = PacketCreate::binaryPacket("rejectAddFriend", senderMessage, imageData);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//发送数据
	NetWorkServiceLocator::instance()->sendBinaryMessage(receive_id, allData);
}

//好友分组更新
void FriendHandle::handle_updateFriendGrouping(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	auto friend_id = paramsObj["friend_id"].toString();
	auto grouping = paramsObj["grouping"].toString();
	MyFriend myFriend(user_id, friend_id, grouping);
	DataBaseQuery query;
	if (!FriendDBUtils::updateFriendGrouping(myFriend, query))
	{

	}
	responder.write(QHttpServerResponder::StatusCode::NoContent);
}

//好友删除
void FriendHandle::handle_deleteFriend(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	auto friend_id = paramsObj["friend_id"].toString();
	MyFriend myFriend_1(user_id, friend_id);
	MyFriend myFriend_2(friend_id, user_id);
	DataBaseQuery query;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!FriendDBUtils::deleteFriend(myFriend_1, query, queryPtr))
			{
				return false;
			}
			if (!FriendDBUtils::deleteFriend(myFriend_2, query, queryPtr))
			{
				return false;
			}
		});
	if (!result)
	{
		return;
	}
	//好友移除通知
	QVariantMap deleteNoticeMap;
	deleteNoticeMap["user_id"] = user_id;
	deleteNoticeMap["username"] = paramsObj["username"].toString();
	deleteNoticeMap["noticeMessage"] = "将你从好友中移除";
	deleteNoticeMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	auto packet = PacketCreate::binaryPacket("friendDeleted", deleteNoticeMap, imageData);
	QByteArray deleteNoticeData;
	PacketCreate::addPacket(deleteNoticeData, packet);
	auto allData = PacketCreate::allBinaryPacket(deleteNoticeData);
	//通知被移除好友
	NetWorkServiceLocator::instance()->sendBinaryMessage(friend_id, allData);
	//回复客户端
	responder.write(QHttpServerResponder::StatusCode::NoContent);
}

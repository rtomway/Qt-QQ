#include "UserHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"

//用户搜索
void UserHandle::handle_searchUser(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	auto client_id = user_id;
	auto search_id = paramsObj["search_id"].toString();
	//数据库查询
	DataBaseQuery query;
	auto user = "%" + search_id + "%";
	QString queryStr = "select user_id,username from user where user_id like ?";
	QVariantList bindvalues;
	bindvalues.append(user);
	auto allQueryObj = query.executeQuery(queryStr, bindvalues);
	//错误返回
	if (allQueryObj.contains("error")) {
		qDebug() << "Error executing query:" << allQueryObj["error"].toString();
		return;
	}
	//查询数据验证
	QByteArray userData;
	auto friendList = getFriendId(user_id);
	QJsonArray userArray = allQueryObj["data"].toArray();
	for (const auto& userValue : userArray)
	{
		QJsonObject userObj = userValue.toObject();
		search_id = userObj["user_id"].toString();
		//判断是不是好友
		if (friendList.contains(search_id))
			userObj["hasAdd"] = true;
		else
			userObj["hasAdd"] = false;
		QByteArray image = ImageUtils::loadImage(search_id, ChatType::User);
		userObj["size"] = image.size();
		qDebug() << "搜索信息" << userObj;
		//打包
		auto userPacket = PacketCreate::binaryPacket("searchUser", userObj.toVariantMap(), image);
		PacketCreate::addPacket(userData, userPacket);
	}
	//最总发送
	auto allData = PacketCreate::allBinaryPacket(userData);
	qDebug() << "发送";
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
}
//群组搜索
void UserHandle::handle_searchGroup(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto search_id = paramsObj["search_id"].toString();
	auto user_id = paramsObj["user_id"].toString();
	//数据库查询
	DataBaseQuery query;
	QStringList groupIdList;
	QByteArray groupData;
	auto groupResult = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			//查询相似群组
			auto group_id = "%" + search_id + "%";
			QString queryStr = "select group_id,group_name from `group` where group_id like ?";
			QVariantList bindvalues;
			bindvalues.append(group_id);
			auto allQueryObj = query.executeQuery(queryStr, bindvalues, queryPtr);
			//错误返回
			if (allQueryObj.contains("error")) {
				qDebug() << "Error executing query:" << allQueryObj["error"].toString();
				return false;
			}
			//查询已加入群组
			QString queryGroupStr = "select group_id from groupmembers where user_id=?";
			QVariantList bindvalues_2;
			bindvalues_2.append(user_id);
			auto allQueryObj_2 = query.executeQuery(queryGroupStr, bindvalues_2, queryPtr);
			//错误返回
			if (allQueryObj_2.contains("error")) {
				qDebug() << "Error executing query:" << allQueryObj_2["error"].toString();
				return false;
			}
			QJsonArray groupIdArray = allQueryObj_2["data"].toArray();
			for (const auto& groupValue : groupIdArray)
			{
				auto groupIdObj = groupValue.toObject();
				auto addedGroup_id = groupIdObj["group_id"].toString();
				groupIdList.append(addedGroup_id);
				qDebug() << "dddddddddddddd" << addedGroup_id;
			}
			//包装查询数据
			QJsonArray groupArray = allQueryObj["data"].toArray();
			for (const auto& groupValue : groupArray)
			{
				auto groupObj = groupValue.toObject();
				auto groupId = groupObj["group_id"].toString();
				if (groupIdList.contains(groupId))
				{
					qDebug()<<"true---------"<<groupObj["group_id"].toString();
					groupObj["hasAdd"] = true;
				}
				else
				{
					qDebug() << "false---------" << groupObj["group_id"].toString();
					groupObj["hasAdd"] = false;
				}
				auto imageData = ImageUtils::loadImage(groupId, ChatType::Group);
				groupObj["size"] = imageData.size();
				auto groupPacket = PacketCreate::binaryPacket("searchGroup", groupObj.toVariantMap(), imageData);
				PacketCreate::addPacket(groupData, groupPacket);
			}
			return true;
		});
	if (groupResult)
	{
		auto allData = PacketCreate::allBinaryPacket(groupData);
		QByteArray mimeType = "application/octet-stream";
		responder.write(allData, mimeType);
	}
}
//用户信息更新
void UserHandle::handle_updateUserMessage(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	//客户端
	auto user_id = paramsObj["user_id"].toString();
	auto username = paramsObj["username"].toString();
	auto gender = paramsObj["gender"].toInt();
	auto age = paramsObj["age"].toInt();
	auto phone_number = paramsObj["phone_number"].toString();
	auto email = paramsObj["email"].toString();
	auto birthday = QDate::fromString(paramsObj["birthday"].toString(), "MM-dd");
	auto signature = paramsObj["signature"].toString();
	phone_number = phone_number.isEmpty() ? QVariant(QVariant::String).toString() : phone_number;
	email = email.isEmpty() ? QVariant(QVariant::String).toString() : email;
	signature = signature.isEmpty() ? QVariant(QVariant::String).toString() : signature;
	//更新信息
	DataBaseQuery query;
	QString queryStr = "UPDATE user SET username=?,\
		gender = ?, age =?, phone_number = ? , \
		email = ? , birthday = ? , signature = ? \
		WHERE user_id = ?";
	QVariantList bindvalues;
	bindvalues.append(username);
	bindvalues.append(gender);
	bindvalues.append(age);
	bindvalues.append(phone_number);
	bindvalues.append(email);
	bindvalues.append(birthday);
	bindvalues.append(signature);
	bindvalues.append(user_id);
	auto queryResult = query.executeNonQuery(queryStr, bindvalues);
	bindvalues.clear();
	//错误返回
	if (!queryResult) {
		qDebug() << "Error query:";
		return;
	}
	responder.write(QHttpServerResponder::StatusCode::NoContent);

	QJsonObject jsondata;
	jsondata["type"] = "updateUserMessage";
	jsondata["params"] = paramsObj;
	QJsonDocument doc(jsondata);
	QString message = QString(doc.toJson(QJsonDocument::Compact));

	QStringList friendIdList = getFriendId(user_id);
	for (auto friend_id : friendIdList)
	{
		ConnectionManager::instance()->sendTextMessage(friend_id, message);
	}
}
//用户头像更新
void UserHandle::handle_updateUserAvatar(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	//将头像存储到服务器中
	QImage image;
	if (!image.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}
	//图片保存
	ImageUtils::saveAvatarToLocal(image, user_id, ChatType::User);
	//转发头像信息
	//数据打包
	auto userPacket = PacketCreate::binaryPacket("updateUserAvatar", paramsObj.toVariantMap(), data);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//转发
	QStringList friendIdList = getFriendId(user_id);
	for (const auto& friend_id : friendIdList)
	{
		ConnectionManager::instance()->sendBinaryMessage(friend_id, allData);
	}
}
//获取好友ID
QStringList UserHandle::getFriendId(const QString& user_id)
{
	QStringList friendIdList;
	//数据库查询
	DataBaseQuery query;
	QString queryStr = "select friend_id from friendship where user_id=?";
	QVariantList bindvalues;
	bindvalues.append(user_id);
	auto allQueryObj = query.executeQuery(queryStr, bindvalues);
	//错误返回
	if (allQueryObj.contains("error")) {
		qDebug() << "Error executing query:" << allQueryObj["error"].toString();
		return QStringList();
	}
	//好友id
	QJsonArray userArray = allQueryObj["data"].toArray();
	for (const auto& userValue : userArray)
	{
		QJsonObject userObj = userValue.toObject();
		auto friend_id = userObj["friend_id"].toString();
		friendIdList.append(friend_id);
		qDebug() << "好友Id" << friend_id;
	}
	return friendIdList;
}

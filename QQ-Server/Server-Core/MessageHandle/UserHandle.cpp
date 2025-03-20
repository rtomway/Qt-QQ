#include "UserHandle.h"
#include "DataBaseQuery.h"
#include <QJsonArray>
#include <QJsonDocument>
#include "ConnectionManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"

void UserHandle::handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << paramsObject;
	auto user_id = paramsObject["user_id"].toString();
	auto client_id = user_id;
	auto search_id = paramsObject["search_id"].toString();
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
			userObj["isFriend"] = true;
		else
			userObj["isFriend"] = false;
		QByteArray image = ImageUtils::loadImage(search_id);
		userObj["size"] = image.size();
		qDebug() << "搜索信息" << userObj;
		//打包
		auto userPacket = PacketCreate::binaryPacket("searchUser", userObj.toVariantMap(), image);
		PacketCreate::addPacket(userData, userPacket);
	}
	//最总发送
	auto allData = PacketCreate::allBinaryPacket(userData);
	ConnectionManager::instance()->sendBinaryMessage(client_id, allData);
}

void UserHandle::handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data)
{
	//客户端
	auto user_id = paramsObject["user_id"].toString();
	auto username = paramsObject["username"].toString();
	auto gender = paramsObject["gender"].toInt();
	auto age = paramsObject["age"].toInt();
	auto phone_number = paramsObject["phone_number"].toString();
	auto email = paramsObject["email"].toString();
	auto birthday = QDate::fromString(paramsObject["birthday"].toString(), "yyyy-MM-dd");
	auto signature = paramsObject["signature"].toString();
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
	QJsonObject jsondata;
	jsondata["type"] = "updateUserMessage";
	jsondata["params"] = paramsObject;
	QJsonDocument doc(jsondata);
	QString message = QString(doc.toJson(QJsonDocument::Compact));

	QStringList friendIdList = getFriendId(user_id);
	for (auto friend_id : friendIdList)
	{
		ConnectionManager::instance()->sendTextMessage(friend_id, message);
	}
}

void UserHandle::handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	//将头像存储到服务器中
	QImage image;
	if (!image.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}
	//图片保存
	ImageUtils::saveImage(user_id, image);
	//转发头像信息
	//数据打包
	auto userPacket = PacketCreate::binaryPacket("updateUserAvatar", paramsObject.toVariantMap(), data);
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

void UserHandle::handle_updateUserGrouping(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	auto friend_id = paramsObject["friend_id"].toString();
	auto grouping = paramsObject["grouping"].toString();
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
}

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

#include "UserHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"
#include "UserDBUtils.h"
#include "FriendDBUtils.h"
#include "GroupDBUtils.h"

//用户搜索
void UserHandle::handle_searchUser(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	auto search_text = paramsObj["search_id"].toString();
	searchPage page;
	page.page= paramsObj["page"].toInt();
	page.pageSize = paramsObj["pageSize"].toInt();
	//数据库查询
	DataBaseQuery query;
	QJsonObject searchUserObj;
	QStringList friend_idList;
	QJsonArray searchUserListArray;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			searchUserObj = UserDBUtils::searchUser(search_text,page, query, queryPtr);
			if (searchUserObj.contains("error"))
			{
				return false;
			}
			searchUserListArray = searchUserObj["searchUserList"].toArray();
			friend_idList = FriendDBUtils::queryFriendIdList(user_id, query, queryPtr);
		});
	//查询数据验证
	QByteArray userData;
	for (const auto& userValue : searchUserListArray)
	{
		QJsonObject userObj = userValue.toObject();
		QString user_id = userObj["user_id"].toString();
		//判断是不是好友
		if (friend_idList.contains(user_id))
			userObj["hasAdd"] = true;
		else
			userObj["hasAdd"] = false;

		QByteArray image = ImageUtils::loadImage(user_id, ChatType::User);
		//打包
		auto userPacket = PacketCreate::binaryPacket("searchUser", userObj.toVariantMap(), image);
		PacketCreate::addPacket(userData, userPacket);
	}
	//最总发送
	auto allData = PacketCreate::allBinaryPacket(userData);
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
}

//查询user信息
void UserHandle::handle_queryUser(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto query_id = paramsObj["query_id"].toString();
	auto user_id= paramsObj["user_id"].toString();
	DataBaseQuery query;
	auto queryUserObj= UserDBUtils::queryUserDetail(query_id, query);
	if (queryUserObj.contains("error"))
	{
		
	}
	QByteArray userData;
	auto packet = PacketCreate::binaryPacket("queryUser", queryUserObj.toVariantMap(), QByteArray());
	PacketCreate::addPacket(userData, packet);
	auto allData = PacketCreate::allBinaryPacket(userData);
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
}

//群组搜索
void UserHandle::handle_searchGroup(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto search_id = paramsObj["search_id"].toString();
	auto user_id = paramsObj["user_id"].toString();
	searchPage page;
	page.page = paramsObj["page"].toInt();
	page.pageSize = paramsObj["pageSize"].toInt();
	//数据库查询
	DataBaseQuery query;
	QByteArray groupData;
	QJsonObject groupObj;
	QJsonArray searchGroupListArray;
	QStringList group_idList;
	auto groupResult = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			//查询相似群组
			auto group_id = "%" + search_id + "%";
			groupObj = UserDBUtils::searchGroup(group_id,page, query, queryPtr);
			//错误返回
			if (groupObj.contains("error")) {
				return false;
			}
			searchGroupListArray = groupObj["searchGroupList"].toArray();
			//查询已加入群组
			group_idList = GroupDBUtils::queryGroupIdList(user_id, query, queryPtr);
			return true;
		});
	if (!groupResult)
		return;
	//包装查询数据
	for (const auto& groupValue : searchGroupListArray)
	{
		auto groupObj = groupValue.toObject();
		auto groupId = groupObj["group_id"].toString();
		if (group_idList.contains(groupId))
		{
			groupObj["hasAdd"] = true;
		}
		else
		{
			groupObj["hasAdd"] = false;
		}
		auto imageData = ImageUtils::loadImage(groupId, ChatType::Group);
		groupObj["size"] = imageData.size();
		auto groupPacket = PacketCreate::binaryPacket("searchGroup", groupObj.toVariantMap(), imageData);
		PacketCreate::addPacket(groupData, groupPacket);
	}
	auto allData = PacketCreate::allBinaryPacket(groupData);
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
}

//用户信息更新
void UserHandle::handle_updateUserMessage(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	UserInfo userInfo;
	userInfo.user_id = paramsObj["user_id"].toString();
	userInfo.username = paramsObj["username"].toString();
	userInfo.gender = paramsObj["gender"].toInt();
	userInfo.age = paramsObj["age"].toInt();
	userInfo.phone_number = paramsObj["phone_number"].toString();
	userInfo.email = paramsObj["email"].toString();
	userInfo.birthday = QDateTime::fromString(paramsObj["birthday"].toString(), "MM-dd");
	userInfo.signature = paramsObj["signature"].toString();
	DataBaseQuery query;
	auto queryResult = UserDBUtils::updateUserMessage(userInfo, query);
	//错误返回
	if (!queryResult) {
		qDebug() << "Error query:";
		return;
	}
	responder.write(QHttpServerResponder::StatusCode::NoContent);
	//向好友发送更新信息
	QJsonObject jsondata;
	jsondata["type"] = "updateUserMessage";
	jsondata["params"] = paramsObj;
	QJsonDocument doc(jsondata);
	QString message = QString(doc.toJson(QJsonDocument::Compact));
	QStringList friend_idList = FriendDBUtils::queryFriendIdList(userInfo.user_id,query);
	for (auto& friend_id : friend_idList)
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
	DataBaseQuery query;
	QStringList friend_idList = FriendDBUtils::queryFriendIdList(user_id, query);
	for (const auto& friend_id : friend_idList)
	{
		ConnectionManager::instance()->sendBinaryMessage(friend_id, allData);
	}
}

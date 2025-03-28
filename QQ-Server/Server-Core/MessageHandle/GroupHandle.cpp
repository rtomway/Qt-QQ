﻿#include "GroupHandle.h"
#include <QDate>
#include <QJSonDocument>
#include <QJsonArray>

#include "CreateId.h"
#include "DataBaseQuery.h"
#include "ImageUtil.h"
#include "ConnectionManager.h"
#include "PacketCreate.h"


//群组搜索
void GroupHandle::handle_searchGroup(const QJsonObject& paramsObject, const QByteArray& data)
{

}
//添加群组
void GroupHandle::handle_createGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto groupOwerId = paramsObject["user_id"].toString();
	auto groupOwerName = paramsObject["username"].toString();
	auto groupId = CreateId::generateUserID(CreateId::Id::Group);
	auto groupName = paramsObject["group_name"].toString();
	auto groupAvatar = groupId + ".png";

	DataBaseQuery query;
	auto result = query.executeTransaction([&](QSqlQuery& sqlquery)->bool
		{
			QString queryStr_1 = "insert into `group` (group_id,group_name,ower_id,create_time,group_avatar)values(?,?,?,?,?)";
			QVariantList bindvalues_1;
			bindvalues_1.append(groupId);
			bindvalues_1.append(groupName);
			bindvalues_1.append(groupOwerId);
			bindvalues_1.append(QDate::currentDate());
			bindvalues_1.append(groupAvatar);
			auto insertResult_1 = query.executeNonQuery(queryStr_1, bindvalues_1);
			if (!insertResult_1) {
				qDebug() << "insertResult_1失败";
				return false;  // 查询失败，回滚事务
			}
			QString queryStr_2 = "insert into groupMembers (group_id,user_id,role)values(?,?,?)";
			QVariantList bindvalues_2;
			bindvalues_2.append(groupId);
			bindvalues_2.append(groupOwerId);
			bindvalues_2.append("ower");
			auto insertResult_2 = query.executeNonQuery(queryStr_2, bindvalues_2);
			if (!insertResult_2) {
				qDebug() << "insertResult_2失败";
				return false;  // 查询失败，回滚事务
			}
			//所有操作都成功
			return true;
		});

	if (result)
	{
		//将创建成功返回客户端
		QJsonObject groupObj = paramsObject;
		groupObj["group_id"] = groupId;
		QJsonArray memberArray;
		QJsonObject memberObj;
		memberObj["user_id"] = groupOwerId;
		memberObj["username"] = groupOwerName;
		memberObj["group_role"] = "ower";
		memberArray.append(memberObj);
		groupObj["members"] = memberArray;
		QJsonObject allObj;
		allObj["type"] = "createGroupSuccess";
		allObj["params"] = groupObj;

		QJsonDocument doc(allObj);
		QString message = doc.toJson(QJsonDocument::Compact);
		ConnectionManager::instance()->sendTextMessage(groupOwerId, message);

		//头像保存服务器
		auto imageData = ImageUtils::loadImage(groupOwerId, ChatType::User);
		QImage groupImage;
		groupImage.loadFromData(imageData);
		ImageUtils::saveAvatarToLocal(groupImage, groupId, ChatType::Group);

		//通知邀请的好友
		QVariantMap groupInviteMap;
		groupInviteMap["user_id"] = groupOwerId;
		groupInviteMap["username"] = groupOwerName;
		groupInviteMap["group_id"] = groupId;
		groupInviteMap["group_name"] = groupName;
		groupInviteMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
		groupInviteMap["requestMessage"] = "邀请你加入他的群聊";
		groupInviteMap["size"] = imageData.size();

		auto groupInvitePacket = PacketCreate::binaryPacket("groupInvite", groupInviteMap, imageData);
		QByteArray groupData;
		PacketCreate::addPacket(groupData, groupInvitePacket);
		auto allData = PacketCreate::allBinaryPacket(groupData);

		auto friendIdListArray = paramsObject["inviteMembers"].toArray();
		qDebug() << "inviteMembers:" << friendIdListArray;
		for (const auto& friendIdValue : friendIdListArray)
		{
			QString friendId = friendIdValue.toString();  // 将 QJsonValue 转换为 QString
			qDebug() << "Sending to friendId:" << friendId;
			ConnectionManager::instance()->sendBinaryMessage(friendId, allData);
		}
	}
}

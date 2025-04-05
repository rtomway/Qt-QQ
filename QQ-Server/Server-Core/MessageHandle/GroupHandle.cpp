#include "GroupHandle.h"
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
		groupInviteMap["groupType"] = 0;
		groupInviteMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
		groupInviteMap["noticeMessage"] = "邀请你加入群聊" + groupName;
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
//群聊交流
void GroupHandle::handle_groupTextCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	//转发的信息
	QJsonObject messageObj;
	messageObj["params"] = paramsObject;
	messageObj["type"] = "groupTextCommunication";
	QJsonDocument doc(messageObj);
	QString message = QString(doc.toJson(QJsonDocument::Compact));
	//转发
	auto group_id = paramsObject["group_id"].toString();
	DataBaseQuery query;
	QStringList groupMembers;
	QString queryStr1("select user_id from groupmembers where group_id=?");
	QVariantList bindvalues;
	bindvalues.append(group_id);
	auto memberObj = query.executeQuery(queryStr1, bindvalues);
	if (memberObj.contains("error"))
	{

	}
	else
	{
		auto memberArray = memberObj["data"].toArray();
		for (const auto& memberValue : memberArray)
		{
			QJsonObject userObj = memberValue.toObject();
			auto member_id = userObj["user_id"].toString();
			ConnectionManager::instance()->sendTextMessage(member_id, message);
		}
	}
}
//群聊邀请成员成功
void GroupHandle::handle_groupInviteSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	auto userName = paramsObject["username"].toString();
	auto group_id = paramsObject["group_id"].toString();
	auto groupOwerId = paramsObject["groupOwerId"].toString();
	auto groupName = paramsObject["group_name"].toString();
	//成员加入信息
	QVariantMap newMemberMap = paramsObject.toVariantMap();
	newMemberMap["group_role"] = "群成员";
	qDebug() << "newMemberMap" << newMemberMap;
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	newMemberMap["size"] = imageData.size();
	auto newMemberPacket = PacketCreate::binaryPacket("newGroupMember", newMemberMap, imageData);
	QByteArray newMemberData;
	PacketCreate::addPacket(newMemberData, newMemberPacket);
	auto newMemberAllData = PacketCreate::allBinaryPacket(newMemberData);
	//通知群管理信息包
	QVariantMap inviteReplyMap;
	inviteReplyMap["user_id"] = user_id;
	inviteReplyMap["username"] = userName;
	inviteReplyMap["noticeMessage"] = "同意加入群组" + groupName;
	inviteReplyMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	inviteReplyMap["size"] = imageData.size();
	auto replyPacket = PacketCreate::binaryPacket("groupInviteSuccess", inviteReplyMap, imageData);
	QByteArray replyData;
	PacketCreate::addPacket(replyData, replyPacket);
	auto replyAllData = PacketCreate::allBinaryPacket(replyData);
	//查询该群成员
	DataBaseQuery query;
	QStringList memberIdList;
	auto result = query.executeTransaction([&](QSqlQuery& sqlquery)->bool
		{
			QString queryStr2("select user_id from groupmembers where group_id=?");
			QVariantList bindvalues2;
			bindvalues2.append(group_id);
			auto queryResult = query.executeQuery(queryStr2, bindvalues2);
			if (queryResult.contains("error"))
			{
				qDebug() << "queryResult failed";
				return false;
			}
			else
			{
				auto memberArray = queryResult["data"].toArray();
				for (const auto& memberValue : memberArray)
				{
					QJsonObject userObj = memberValue.toObject();
					memberIdList.append(userObj["user_id"].toString());
				}
			}
			QString queryStr("insert into groupmembers (group_id,user_id)values(?,?)");
			QVariantList bindvalues;
			bindvalues.append(group_id);
			bindvalues.append(user_id);
			auto insertResult = query.executeNonQuery(queryStr, bindvalues);
			if (!insertResult)
			{
				qDebug() << "insertResult failed";
				return false;
			}
			return true;
		});
	if (result)
	{
		ConnectionManager::instance()->sendBinaryMessage(groupOwerId, replyAllData);
		for (auto& member_id : memberIdList)
		{
			ConnectionManager::instance()->sendBinaryMessage(member_id, newMemberAllData);
		}
	}
}

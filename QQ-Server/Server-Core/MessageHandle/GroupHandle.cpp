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
void GroupHandle::handle_searchGroup(const QJsonObject& paramsObj,const QByteArray& data, QHttpServerResponder& responder)
{
	//auto paramsObject = QJsonDocument::fromJson(data);
	auto search_id = paramsObj["search_id"].toString();
	auto user_id= paramsObj["user_id"].toString();
	//数据库查询
	DataBaseQuery query;
	auto groupResult= query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
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
			QString queryGroupStr = "select group_id from group_id where user_id=?";
			QVariantList bindvalues_2;
			bindvalues_2.append(user_id);
			auto allQueryObj_2 = query.executeQuery(queryStr, bindvalues,queryPtr);
			//错误返回
			if (allQueryObj_2.contains("error")) {
				qDebug() << "Error executing query:" << allQueryObj_2["error"].toString();
				return false;
			}
			QJsonArray groupIdArray = allQueryObj_2["data"].toArray();
			QStringList groupIdList;
			for (const auto& groupValue : groupIdArray)
			{
				auto groupIdObj = groupValue.toObject();
				auto addedGroup_id = groupIdObj["group_id"].toString();
				groupIdList.append(addedGroup_id);
			}
			//包装查询数据
			QJsonArray groupArray = allQueryObj["data"].toArray();
			QByteArray groupData;
			for (const auto& groupValue : groupArray)
			{
				auto groupObj = groupValue.toObject();
				auto groupId = groupObj["group_id"].toString();
				if (groupIdList.contains(groupId))
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
			QByteArray mimeType="application/octet-stream";
			responder.write(allData, mimeType);
			return false;
		});
}
//添加群组
void GroupHandle::handle_createGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "群聊创建（服务器）";
	auto groupOwerId = paramsObject["user_id"].toString();
	auto groupOwerName = paramsObject["username"].toString();
	auto groupId = CreateId::generateUserID(CreateId::Id::Group);
	auto groupName = paramsObject["group_name"].toString();
	auto groupAvatar = groupId + ".png";

	DataBaseQuery query;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			QString queryStr_1 = "insert into `group` (group_id,group_name,ower_id,create_time,group_avatar)values(?,?,?,?,?)";
			QVariantList bindvalues_1;
			bindvalues_1.append(groupId);
			bindvalues_1.append(groupName);
			bindvalues_1.append(groupOwerId);
			bindvalues_1.append(QDate::currentDate());
			bindvalues_1.append(groupAvatar);
			auto insertResult_1 = query.executeNonQuery(queryStr_1, bindvalues_1, queryPtr);
			if (!insertResult_1) {
				qDebug() << "insertResult_1失败";
				return false;  // 查询失败，回滚事务
			}
			QString queryStr_2 = "insert into groupMembers (group_id,user_id,username,group_role)values(?,?,?,?)";
			QVariantList bindvalues_2;
			bindvalues_2.append(groupId);
			bindvalues_2.append(groupOwerId);
			bindvalues_2.append(groupOwerName);
			bindvalues_2.append("ower");
			auto insertResult_2 = query.executeNonQuery(queryStr_2, bindvalues_2, queryPtr);
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
		qDebug() << "群聊创建成功发送邀请好友";
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
	//查询需转发群成员
	auto group_id = paramsObject["group_id"].toString();
	auto user_id= paramsObject["user_id"].toString();
	DataBaseQuery query;
	QString queryStr("select user_id from groupmembers where group_id=? and user_id!=?");
	QVariantList bindvalues;
	bindvalues.append(group_id);
	bindvalues.append(user_id);
	auto memberObj = query.executeQuery(queryStr, bindvalues);
	if (memberObj.contains("error"))
	{
		qDebug() << "查询群成员失败";
	}
	else
	{
		//转发
		auto memberArray = memberObj["data"].toArray();
		qDebug() << "发送者：" << user_id;
		for (const auto& memberValue : memberArray)
		{
			QJsonObject userObj = memberValue.toObject();
			auto member_id = userObj["user_id"].toString();
			qDebug() << "发送者：" << user_id << "----群成员接收:" << member_id;
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
	newMemberMap["group_role"] = "member";
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
	//向群主发送通知信息
	ConnectionManager::instance()->sendBinaryMessage(groupOwerId, replyAllData);
	//查询该群成员
	DataBaseQuery query;
	QStringList memberIdList;
	QByteArray groupMembersData;
	QByteArray groupMembersAllData;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			QString queryStr("insert into groupmembers (group_id,user_id,username)values(?,?,?)");
			QVariantList bindvalues;
			bindvalues.append(group_id);
			bindvalues.append(user_id);
			bindvalues.append(userName);
			auto insertResult = query.executeNonQuery(queryStr, bindvalues, queryPtr);
			if (!insertResult)
			{
				qDebug() << "insertResult failed";
				return false;
			}
			QString queryStr2("select user_id,username,group_role from groupmembers where group_id=?");
			QVariantList bindvalues2;
			bindvalues2.append(group_id);
			auto queryResult = query.executeQuery(queryStr2, bindvalues2, queryPtr);
			if (queryResult.contains("error"))
			{
				qDebug() << "queryResult failed";
				return false;
			}
			else
			{
				//包装群聊信息
				QVariantMap groupMap;
				groupMap["group_id"] = group_id;
				groupMap["group_name"] = groupName;
				groupMap["user_id"] = groupOwerId;
				auto imageData = ImageUtils::loadImage(group_id, ChatType::Group);
				groupMap["size"] = imageData.size();
				auto packet = PacketCreate::binaryPacket("groupLoad", groupMap, imageData);
				PacketCreate::addPacket(groupMembersData, packet);
		
				auto memberArray = queryResult["data"].toArray();
				for (const auto& memberValue : memberArray)
				{
					//获取群成员
					QJsonObject userObj = memberValue.toObject();
					auto member_id = userObj["user_id"].toString();
					memberIdList.append(member_id);
					//包装群成员信息
					auto imageData = ImageUtils::loadImage(member_id, ChatType::User);
					QVariantMap memberMap= userObj.toVariantMap();
					memberMap["group_id"] = group_id;
					memberMap["size"] = imageData.size();
					auto packet = PacketCreate::binaryPacket("groupMemberLoad", memberMap,imageData);
					PacketCreate::addPacket(groupMembersData, packet);
				}
				groupMembersAllData = PacketCreate::allBinaryPacket(groupMembersData);
			}
			
			return true;
		});
	if (result)
	{
		//向新成员发送群组信息
		ConnectionManager::instance()->sendBinaryMessage(user_id, groupMembersAllData);
		//向群成员客户端发送新成员信息
		for (auto& member_id : memberIdList)
		{
			ConnectionManager::instance()->sendBinaryMessage(member_id, newMemberAllData);
		}
	}
}

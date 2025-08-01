﻿#include "GroupHandle.h"
#include <QDate>
#include <QJSonDocument>
#include <QJsonArray>

#include "CreateId.h"
#include "DataBaseQuery.h"
#include "ImageUtil.h"
#include "ConnectionManager.h"
#include "PacketCreate.h"
#include "GroupDBUtils.h"

//添加群组
void GroupHandle::handle_createGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	Group groupParams;
	groupParams.owner_id = paramsObject["user_id"].toString();
	groupParams.group_id = CreateId::generateUserID(CreateId::Id::Group);
	groupParams.group_name = paramsObject["group_name"].toString();
	groupParams.group_avatarPath = ImageUtils::getGroupAvatarFolderPath() + "/" + groupParams.group_id + ".png";
	groupParams.create_time = QDateTime::currentDateTime();
	GroupMember groupMemberParams;
	groupMemberParams.group_id = groupParams.group_id;
	groupMemberParams.user_id = groupParams.owner_id;
	groupMemberParams.username = paramsObject["username"].toString();
	groupMemberParams.group_role = "owner";
	//数据库操作
	DataBaseQuery query;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!GroupDBUtils::insertGroup(groupParams, query, queryPtr))
			{
				return false;
			}
			if (!GroupDBUtils::insertGroupMember(groupMemberParams, query, queryPtr))
			{
				return false;
			}
			if (!GroupDBUtils::groupMemberCountAdd(groupParams.group_id, query, queryPtr))
			{
				return false;
			}
			return true;
		});
	if (!result)
	{
		return;
	}
	//头像保存服务器
	auto imageData = ImageUtils::loadImage(groupParams.owner_id, ChatType::User);
	QImage groupImage;
	groupImage.loadFromData(imageData);
	ImageUtils::saveAvatarToLocal(groupImage, groupParams.group_id, ChatType::Group);
	//群组数据包
	QJsonObject groupObj;
	groupObj["group_id"] = groupParams.group_id;
	groupObj["group_name"] = groupParams.group_name;
	groupObj["owner_id"] = groupParams.owner_id;
	groupObj["groupMemberCount"] = 1;
	auto groupPacket = PacketCreate::binaryPacket("createGroupSuccess", groupObj.toVariantMap(), imageData);
	QByteArray groupData;
	PacketCreate::addPacket(groupData, groupPacket);
	auto groupAllData = PacketCreate::allBinaryPacket(groupData);
	ConnectionManager::instance()->sendBinaryMessage(groupParams.owner_id, groupAllData);
	//通知邀请好友
	QVariantMap groupInviteMap;
	groupInviteMap["user_id"] = groupParams.owner_id;
	groupInviteMap["username"] = groupMemberParams.username;
	groupInviteMap["group_id"] = groupParams.group_id;
	groupInviteMap["group_name"] = groupParams.group_name;
	groupInviteMap["groupType"] = 0;
	groupInviteMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	groupInviteMap["noticeMessage"] = "邀请你加入群聊" + groupParams.group_name;
	auto groupInvitePacket = PacketCreate::binaryPacket("groupInvite", groupInviteMap, imageData);
	QByteArray groupInviteData;
	PacketCreate::addPacket(groupInviteData, groupInvitePacket);
	auto allData = PacketCreate::allBinaryPacket(groupInviteData);
	auto friendIdListArray = paramsObject["inviteMembers"].toArray();
	for (const auto& friendIdValue : friendIdListArray)
	{
		QString friendId = friendIdValue.toString();
		ConnectionManager::instance()->sendBinaryMessage(friendId, allData);
	}
}

//申请添加群组
void GroupHandle::handle_addGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto send_id = paramsObject["user_id"].toString();
	auto group_id = paramsObject["to"].toString();
	DataBaseQuery query;
	Group queryGroup;
	queryGroup = GroupDBUtils::queryGroup(group_id, query);
	//数据包装入信息
	QVariantMap senderMessage;
	senderMessage = paramsObject.toVariantMap();
	senderMessage["group_id"] = group_id;
	senderMessage["groupType"] = 1;
	senderMessage["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	auto imageData = ImageUtils::loadImage(send_id, ChatType::User);
	senderMessage["size"] = imageData.size();
	//包装数据包
	auto userPacket = PacketCreate::binaryPacket("addGroup", senderMessage, imageData);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//发送数据
	ConnectionManager::instance()->sendBinaryMessage(queryGroup.owner_id, allData);
}

//群聊文本交流
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
	auto user_id = paramsObject["user_id"].toString();
	DataBaseQuery query;
	auto memberId_list = GroupDBUtils::queryGroupMemberIdList(group_id, query);
	for (auto& member_id : memberId_list)
	{
		if (member_id != user_id)
			ConnectionManager::instance()->sendTextMessage(member_id, message);
	}
}

//群聊图片发送
void GroupHandle::handle_groupPictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	//数据打包
	auto userPacket = PacketCreate::binaryPacket("groupPictureCommunication", paramsObject.toVariantMap(), data);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//查询需转发群成员
	auto group_id = paramsObject["group_id"].toString();
	auto user_id = paramsObject["user_id"].toString();
	DataBaseQuery query;
	auto memberId_list = GroupDBUtils::queryGroupMemberIdList(group_id, query);
	for (auto& member_id : memberId_list)
	{
		if (member_id != user_id)
			ConnectionManager::instance()->sendBinaryMessage(member_id, allData);
	}
}

//单个成员加载
void GroupHandle::handle_groupMemberLoad(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto member_id = paramsObj["member_id"].toString();
	auto group_id = paramsObj["group_id"].toString();
	//获取群成员
	DataBaseQuery query;
	auto groupMember = GroupDBUtils::queryGroupMember(group_id, member_id, query);
	//包装群成员信息
	QVariantMap memberMap;
	memberMap["group_id"] = groupMember.group_id;
	memberMap["user_id"] = groupMember.user_id;
	memberMap["username"] = groupMember.username;
	memberMap["group_role"] = groupMember.group_role;
	auto imageData = ImageUtils::loadImage(member_id, ChatType::User);
	auto packet = PacketCreate::binaryPacket("groupMemberLoad", memberMap, imageData);

	QByteArray groupMemberData;
	PacketCreate::addPacket(groupMemberData, packet);
	auto allData = PacketCreate::allBinaryPacket(groupMemberData);
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
}

//群聊邀请成员成功
void GroupHandle::handle_groupInviteSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	auto group_id = paramsObject["group_id"].toString();
	GroupMember groupMember;
	groupMember.user_id = paramsObject["user_id"].toString();
	groupMember.username = paramsObject["username"].toString();
	groupMember.group_id = paramsObject["group_id"].toString();
	groupMember.group_role = "member";
	DataBaseQuery query;
	Group queryGroup;
	QStringList member_idList;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!GroupDBUtils::insertGroupMember(groupMember, query, queryPtr))
			{
				return false;
			}
			if (!GroupDBUtils::groupMemberCountAdd(group_id, query, queryPtr))
			{
				return false;
			}
			queryGroup = GroupDBUtils::queryGroup(group_id, query, queryPtr);
			if (queryGroup.group_id.isEmpty())
			{
				return false;
			}
			member_idList = GroupDBUtils::queryGroupMemberIdList(group_id, query, queryPtr);
			if (member_idList.isEmpty())
			{
				return false;
			}
		});
	if (!result)
		return;
	//新成员信息
	QVariantMap newMemberMap = paramsObject.toVariantMap();
	newMemberMap["group_role"] = groupMember.group_role;
	auto newMemberImageData = ImageUtils::loadImage(user_id, ChatType::User);
	auto newMemberPacket = PacketCreate::binaryPacket("newGroupMember", newMemberMap, newMemberImageData);
	QByteArray newMemberData;
	PacketCreate::addPacket(newMemberData, newMemberPacket);
	auto newMemberAllData = PacketCreate::allBinaryPacket(newMemberData);
	//向群成员客户端发送新成员信息
	for (auto& member_id : member_idList)
	{
		if (member_id != groupMember.user_id)
			ConnectionManager::instance()->sendBinaryMessage(member_id, newMemberAllData);
	}
	//通知群主邀请成功
	QVariantMap inviteReplyMap;
	inviteReplyMap["user_id"] = user_id;
	inviteReplyMap["username"] = groupMember.username;
	inviteReplyMap["noticeMessage"] = "同意加入群组" + queryGroup.group_name;
	inviteReplyMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	auto replyPacket = PacketCreate::binaryPacket("groupInviteSuccess", inviteReplyMap, newMemberImageData);
	QByteArray replyData;
	PacketCreate::addPacket(replyData, replyPacket);
	auto replyAllData = PacketCreate::allBinaryPacket(replyData);
	//向群主发送通知信息
	ConnectionManager::instance()->sendBinaryMessage(queryGroup.owner_id, replyAllData);
	//包装群聊信息
	QVariantMap groupMap;
	groupMap["group_id"] = group_id;
	groupMap["group_name"] = queryGroup.group_name;
	groupMap["owner_id"] = queryGroup.owner_id;
	groupMap["groupMemberCount"] = queryGroup.groupMemberCount;
	auto imageData = ImageUtils::loadImage(group_id, ChatType::Group);
	auto packet = PacketCreate::binaryPacket("newGroup", groupMap, imageData);
	QByteArray groupData;
	PacketCreate::addPacket(groupData, packet);
	auto groupAllData = PacketCreate::allBinaryPacket(groupData);
	//向新成员发送群组信息
	ConnectionManager::instance()->sendBinaryMessage(user_id, groupAllData);
}

//群组申请加入成功
void GroupHandle::handle_groupAddSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto group_id = paramsObject["group_id"].toString();
	auto user_id = paramsObject["user_id"].toString();
	GroupMember newGroupMember;
	newGroupMember.user_id = paramsObject["applicant_id"].toString();
	newGroupMember.username = paramsObject["applicant_name"].toString();
	newGroupMember.group_id = paramsObject["group_id"].toString();
	newGroupMember.group_role = "member";
	//查询该群成员
	DataBaseQuery query;
	QStringList member_idList;
	Group queryGroup;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!GroupDBUtils::insertGroupMember(newGroupMember, query, queryPtr))
			{
				return false;
			}
			if (!GroupDBUtils::groupMemberCountAdd(group_id, query, queryPtr))
			{
				return false;
			}
			queryGroup = GroupDBUtils::queryGroup(group_id, query, queryPtr);
			if (queryGroup.group_id.isEmpty())
			{
				return false;
			}
			member_idList = GroupDBUtils::queryGroupMemberIdList(group_id, query, queryPtr);
			if (member_idList.isEmpty())
			{
				return false;
			}
		});
	if (!result)
		return;
	//新成员信息
	QVariantMap newMemberMap;
	newMemberMap["group_role"] = newGroupMember.group_role;
	newMemberMap["user_id"] = newGroupMember.user_id;
	newMemberMap["username"] = newGroupMember.username;
	newMemberMap["group_id"] = newGroupMember.group_id;
	auto newMemberImageData = ImageUtils::loadImage(user_id, ChatType::User);
	auto newMemberPacket = PacketCreate::binaryPacket("newGroupMember", newMemberMap, newMemberImageData);
	QByteArray newMemberData;
	PacketCreate::addPacket(newMemberData, newMemberPacket);
	auto newMemberAllData = PacketCreate::allBinaryPacket(newMemberData);
	//向群成员客户端发送新成员信息
	for (auto& member_id : member_idList)
	{
		if (member_id != newGroupMember.user_id)
			ConnectionManager::instance()->sendBinaryMessage(member_id, newMemberAllData);
	}
	//包装群聊信息
	QVariantMap groupMap;
	groupMap["group_id"] = group_id;
	groupMap["group_name"] = queryGroup.group_name;
	groupMap["owner_id"] = queryGroup.owner_id;
	groupMap["groupMemberCount"] = queryGroup.groupMemberCount;
	auto imageData = ImageUtils::loadImage(group_id, ChatType::Group);
	auto packet = PacketCreate::binaryPacket("newGroup", groupMap, imageData);
	QByteArray groupData;
	PacketCreate::addPacket(groupData, packet);
	auto groupAllData = PacketCreate::allBinaryPacket(groupData);
	//向新成员发送群组信息
	ConnectionManager::instance()->sendBinaryMessage(newGroupMember.user_id, groupAllData);
}

//群聊邀请成员失败
void GroupHandle::handle_groupInviteFailed(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto groupOwner_id = paramsObject["groupOwnerId"].toString();
	auto user_id = paramsObject["user_id"].toString();
	//数据包装入信息
	QVariantMap senderMessage;
	senderMessage = paramsObject.toVariantMap();
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	//包装数据包
	auto userPacket = PacketCreate::binaryPacket("rejectAddGroup", senderMessage, imageData);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//发送数据
	ConnectionManager::instance()->sendBinaryMessage(groupOwner_id, allData);
}

//群组申请加入失败
void GroupHandle::handle_groupAddFailed(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	auto group_id = paramsObject["group_id"].toString();
	auto receive_id = paramsObject["to"].toString();
	//数据包装入信息
	QVariantMap senderMessage;
	senderMessage = paramsObject.toVariantMap();
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	//包装数据包
	auto userPacket = PacketCreate::binaryPacket("groupAddFailed", senderMessage, imageData);
	QByteArray userData;
	PacketCreate::addPacket(userData, userPacket);
	auto allData = PacketCreate::allBinaryPacket(userData);
	//发送数据
	ConnectionManager::instance()->sendBinaryMessage(receive_id, allData);
}

//群聊邀请
void GroupHandle::handle_groupInvite(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	auto username = paramsObject["username"].toString();
	auto group_id = paramsObject["group_id"].toString();
	auto group_name = paramsObject["group_name"].toString();
	QVariantMap groupInviteMap = paramsObject.toVariantMap();
	groupInviteMap["groupType"] = 0;
	groupInviteMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	groupInviteMap["noticeMessage"] = "邀请你加入群聊" + group_name;
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	auto groupInvitePacket = PacketCreate::binaryPacket("groupInvite", groupInviteMap, imageData);
	QByteArray groupInviteData;
	PacketCreate::addPacket(groupInviteData, groupInvitePacket);
	auto allData = PacketCreate::allBinaryPacket(groupInviteData);
	auto friendIdListArray = paramsObject["inviteMembers"].toArray();
	for (const auto& friendIdValue : friendIdListArray)
	{
		QString friendId = friendIdValue.toString();
		ConnectionManager::instance()->sendBinaryMessage(friendId, allData);
	}
}

//移除群成员
void GroupHandle::handle_groupMemberRemove(const QJsonObject& paramsObj, const QByteArray& data)
{
	auto user_id = paramsObj["user_id"].toString();
	auto username = paramsObj["username"].toString();
	auto group_id = paramsObj["group_id"].toString();
	auto group_name = paramsObj["group_name"].toString();
	auto removeListArray = paramsObj["removeMembers"].toArray();
	QStringList removedMember_idList;
	QStringList member_idList;
	for (auto memberValue : removeListArray)
	{
		auto member_id = memberValue.toString();
		removedMember_idList.append(member_id);
	}
	DataBaseQuery query;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!GroupDBUtils::batch_deleteGroupMember(group_id, removedMember_idList, query, queryPtr))
			{
				return false;
			}
			for (auto i = 0; i < removedMember_idList.size(); i++)
			{
				if (!GroupDBUtils::groupMemberCountSub(group_id, query, queryPtr))
				{
					return false;
				}
			}
			member_idList = GroupDBUtils::queryGroupMemberIdList(group_id, query, queryPtr);
			if (member_idList.isEmpty())
			{
				return false;
			}
		});
	if (!result)
	{
		return;
	}
	//通知被移除群成员
	auto removeNoticeObj = paramsObj.toVariantMap();
	removeNoticeObj["noticeMessage"] = "将你移除群组" + group_name;
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	auto packet = PacketCreate::binaryPacket("beRemovedGroup", removeNoticeObj, imageData);
	QByteArray disbandNoticeData;
	PacketCreate::addPacket(disbandNoticeData, packet);
	auto allData = PacketCreate::allBinaryPacket(disbandNoticeData);

	for (auto& groupMember_id : removedMember_idList)
	{
		ConnectionManager::instance()->sendBinaryMessage(groupMember_id, allData);
	}
	member_idList.removeOne(user_id);
	//通知其余成员
	QJsonObject deleteMembersObj;
	deleteMembersObj["group_id"] = group_id;
	QJsonArray deleteMemberArray;
	for (auto& removeMemberId : removedMember_idList)
	{
		deleteMemberArray.append(removeMemberId);
	}
	deleteMembersObj["deleteMemberList"] = deleteMemberArray;

	QJsonObject exitGroupObj;
	exitGroupObj["type"] = "batch_groupMemberDeleted";
	exitGroupObj["params"] = deleteMembersObj;
	QJsonDocument doc(exitGroupObj);
	auto message = QString(doc.toJson(QJsonDocument::Compact));
	for (auto& member_id : member_idList)
	{
		if (!removedMember_idList.contains(member_id))
			ConnectionManager::instance()->sendTextMessage(member_id, message);
	}
}

//退出群组
void GroupHandle::handle_exitGroup(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	GroupMember groupMember;
	groupMember.user_id = paramsObj["user_id"].toString();
	groupMember.username = paramsObj["username"].toString();
	groupMember.group_id = paramsObj["group_id"].toString();
	DataBaseQuery query;
	Group queryGroup;
	QString groupOwner_id;
	QStringList groupMember_idList;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			if (!GroupDBUtils::deleteGroupMember(groupMember, query, queryPtr))
			{
				return false;
			}
			if (!GroupDBUtils::groupMemberCountSub(groupMember.group_id, query, queryPtr))
			{
				return false;
			}
			groupMember_idList = GroupDBUtils::queryGroupMemberIdList(groupMember.group_id, query, queryPtr);
			queryGroup = GroupDBUtils::queryGroup(groupMember.group_id, query, queryPtr);
			if (queryGroup.group_id.isEmpty())
			{
				return false;
			}
			groupOwner_id = queryGroup.owner_id;
			return true;
		});
	if (!result)
		return;
	//给群主发送成员退出通知
	QVariantMap MemberExitGroupMap;
	MemberExitGroupMap["user_id"] = groupMember.user_id;
	MemberExitGroupMap["username"] = groupMember.username;
	MemberExitGroupMap["group_id"] = groupMember.group_id;
	MemberExitGroupMap["noticeMessage"] = "退出群组" + queryGroup.group_name;
	MemberExitGroupMap["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	auto imageData = ImageUtils::loadImage(groupMember.user_id, ChatType::User);
	auto packet = PacketCreate::binaryPacket("groupMemberExitGroup", MemberExitGroupMap, imageData);
	QByteArray MemberExitGroupData;
	PacketCreate::addPacket(MemberExitGroupData, packet);
	auto allData = PacketCreate::allBinaryPacket(MemberExitGroupData);
	//向群主发送通知信息
	ConnectionManager::instance()->sendBinaryMessage(queryGroup.owner_id, allData);
	//通知群组其他成员
	QJsonObject exitGroupObj;
	exitGroupObj["type"] = "groupMemberDeleted";
	exitGroupObj["params"] = paramsObj;
	QJsonDocument doc(exitGroupObj);
	auto message = QString(doc.toJson(QJsonDocument::Compact));
	for (auto& member_id : groupMember_idList)
	{
		ConnectionManager::instance()->sendTextMessage(member_id, message);
	}
	//返回客户端操作结果
	responder.write(QHttpServerResponder::StatusCode::NoContent);
}

//解散群组
void GroupHandle::handle_disbandGroup(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	auto username = paramsObj["username"].toString();
	auto group_id = paramsObj["group_id"].toString();
	auto group_name = paramsObj["group_name"].toString();
	//删除群组
	DataBaseQuery query;
	QStringList groupMember_idList;
	auto result = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			groupMember_idList = GroupDBUtils::queryGroupMemberIdList(group_id, query, queryPtr);
			if (!GroupDBUtils::deleteGroup(group_id, query, queryPtr))
			{
				return false;
			}
			return true;
		});
	if (!result)
	{

	}
	//通知所有群成员群组解散
	auto disbandNoticeObj = paramsObj.toVariantMap();
	disbandNoticeObj["noticeMessage"] = "解散了群组" + group_name;
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	auto packet = PacketCreate::binaryPacket("disbandGroup", disbandNoticeObj, imageData);
	QByteArray disbandNoticeData;
	PacketCreate::addPacket(disbandNoticeData, packet);
	auto allData = PacketCreate::allBinaryPacket(disbandNoticeData);
	for (auto& groupMember_id : groupMember_idList)
	{
		if (groupMember_id != user_id)
			ConnectionManager::instance()->sendBinaryMessage(groupMember_id, allData);
	}

}

//更改群组头像
void GroupHandle::handle_updateGroupAvatar(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto group_id = paramsObj["group_id"].toString();
	//将头像存储到服务器中
	QImage image;
	if (!image.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for group:" << group_id;
		return;
	}
	//图片保存
	ImageUtils::saveAvatarToLocal(image, group_id, ChatType::Group);
	//转发头像信息
	//数据打包
	auto groupPacket = PacketCreate::binaryPacket("updateGroupAvatar", paramsObj.toVariantMap(), data);
	QByteArray groupData;
	PacketCreate::addPacket(groupData, groupPacket);
	auto allData = PacketCreate::allBinaryPacket(groupData);
	//转发
	DataBaseQuery query;
	QStringList groupMember_idList = GroupDBUtils::queryGroupMemberIdList(group_id, query);
	for (const auto& member_id : groupMember_idList)
	{
		ConnectionManager::instance()->sendBinaryMessage(member_id, allData);
	}
}

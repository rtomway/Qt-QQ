﻿#include "Group.h"
#include <QJsonArray>

//设置群聊基本信息
void Group::setGroup(const QJsonObject& groupObj)
{
	m_groupId = groupObj["group_id"].toString();
	m_groupName = groupObj["group_name"].toString();
	m_groupOwnerId = groupObj["owner_id"].toString();
	m_groupMemberCount = groupObj["groupMemberCount"].toInt();
	m_groupJson = groupObj;
}

//获取群组基本信息
const QJsonObject& Group::getGroupProfile()
{
	return m_groupJson;
}

//获取群Id
const QString& Group::getGroupId() const
{
	return m_groupId;
}

//获取群主id
const QString& Group::getGroupOwerId() const
{
	return m_groupOwnerId;
}

//获取群组名
const QString& Group::getGroupName() const
{
	return m_groupName;
}

//批量加载群成员
void Group::batchLoadGroupMember(const QJsonArray& memberArray)
{
	for (auto memberValue : memberArray)
	{
		auto memberObj = memberValue.toObject();
		loadGroupMember(memberObj);
	}
}

//加载单个群成员
void Group::loadGroupMember(const QJsonObject& memberObj)
{
	Member member;
	member.member_id = memberObj["user_id"].toString();
	member.member_name = memberObj["username"].toString();
	member.member_role = memberRole(memberObj["group_role"].toString());
	m_members.insert(member.member_id, member);
}

//获取群成员id列表
QStringList Group::getGroupMembersIdList() const
{
	QStringList groupMemberIdList;
	for (auto it = m_members.cbegin(); it != m_members.cend(); it++)
	{
		groupMemberIdList.append(it.key());
	}
	return groupMemberIdList;
}

//群聊添加新成员
void Group::addMember(const QJsonObject& memberObj)
{
	Member member;
	member.member_id = memberObj["user_id"].toString();
	member.member_name = memberObj["username"].toString();
	auto role = memberObj["group_role"].toString();
	member.member_role = memberRole(role);

	m_members.insert(member.member_id, member);
	m_groupMemberCount++;
	m_groupJson["groupMemberCount"] = m_groupMemberCount;
}

//移除成员
void Group::removeMember(const QString& user_id)
{
	m_members.remove(user_id);
	m_groupMemberCount--;
	m_groupJson["groupMemberCount"] = m_groupMemberCount;
}

//设置群组分组
void Group::setGrouping(const QString& grouping)
{
	m_grouping = grouping;
}

//获取群组分组
const QString& Group::getGrouping() const
{
	return m_grouping;
}

//获取指定成员
Member Group::getMember(const QString& member_id) const
{
	auto it = m_members.find(member_id);
	if (it != m_members.end())
	{
		return it.value();
	}
	else
	{
		return Member();
	}
}

//获取全部成员
const QHash<QString, Member>& Group::getMembers() const
{
	return m_members;
}

//群人数
const int& Group::count() const
{
	return m_groupMemberCount;
}

//群角色转成中文
QString Group::memberRole(const QString& role)
{
	QString memberRole;
	if (role == "owner")
	{
		memberRole = "群主";
	}
	else if (role == "admin")
	{
		memberRole = "群管理";
	}
	else
	{
		memberRole = "群成员";
	}
	return memberRole;
}

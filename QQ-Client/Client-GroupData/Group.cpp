#include "Group.h"
#include "Group.h"
#include "Group.h"
#include "QJsonArray"

void Group::setGroup(const QJsonObject& groupObj)
{
	qDebug() << "setGroup1" << groupObj;
	m_groupId = groupObj["group_id"].toString();
	m_groupName = groupObj["group_name"].toString();
	m_groupOwnerId = groupObj["user_id"].toString();
	// 解析成员列表
	QJsonArray membersArray = groupObj["members"].toArray();
	for (const auto& memberVal : membersArray)
	{
		addMember(memberVal.toObject());
	}
	qDebug() << m_groupId;
}
//群聊添加成员
void Group::addMember(const QJsonObject& memberObj)
{
	QString member_id = memberObj["user_id"].toString();
	Member member;
	member.member_id = member_id;
	member.member_name = memberObj["username"].toString();
	member.member_role = memberObj["group_role"].toString();
	m_members.insert(member_id, member);
}
//设置成员用户名
void Group::setMemberName(const QString& member_id, const QString& member_name)
{
	m_members[member_id].member_name = member_name;
}
//移除成员
void Group::removeMember(const QString& user_id)
{
	m_members.remove(user_id);
}
//获取群Id
const QString& Group::getGroupId() const
{
	return m_groupId;
}
//获取群组id
const QString& Group::getGroupOwerId() const
{
	return m_groupOwnerId;
}
//获取群组名
const QString& Group::getGroupName() const
{
	return m_groupName;
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

const QHash<QString, Member>& Group::getMembers() const
{
	return m_members;
}

const int& Group::count() const
{
	return m_members.count();
}

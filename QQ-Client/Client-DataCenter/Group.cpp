#include "Group.h"
#include "QJsonArray"

void Group::setGroup(const QJsonObject& groupObj)
{
	qDebug() << "setGroup" << groupObj;
	m_groupId = groupObj["group_id"].toString();
	m_groupName = groupObj["group_name"].toString();
	m_groupOwnerId = groupObj["user_id"].toString();
	// 解析成员列表
	QJsonArray membersArray = groupObj["members"].toArray();
	for (const auto& memberVal : membersArray)
	{
		addMember(memberVal.toObject());
	}
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
	qDebug() << "addmember:" << member.member_id << member.member_name << member.member_role;
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
const Member& Group::getMember(const QString& member_id) const
{
	auto it = m_members.find(member_id);  // 使用 find() 查找元素
	if (it != m_members.end()) {
		qDebug() << "有该成员";
		return it.value();  // 返回元素的引用
	}
	else {
		qDebug() << "没有该成员";
		static const Member defaultMember;  // 使用静态的默认成员对象
		return defaultMember;  // 返回默认成员对象的引用
	}
}



const QHash<QString, Member>& Group::getMembers() const
{
	return m_members;
}

const int& Group::count() const
{
	return m_members.count();
}

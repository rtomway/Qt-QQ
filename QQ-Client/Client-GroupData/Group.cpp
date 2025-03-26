#include "Group.h"
#include "QJsonArray"

void Group::setGroup(const QJsonObject& groupObj)
{
    m_groupId = groupObj["group_id"].toString();
    m_groupName = groupObj["group_name"].toString();
    m_groupOwnerId = groupObj["owner_id"].toString();

    // 解析成员列表
    QJsonArray membersArray = groupObj["members"].toArray();
    for (const auto& memberVal : membersArray)
    {
        addMember(memberVal.toObject());
    }
}

void Group::addMember(const QJsonObject& memberObj)
{
    QString member_id = memberObj["user_id"].toString();
    Member member;
    member.member_id = member_id;
    member.member_name = memberObj["user_name"].toString();
    member.member_avatarPath = memberObj["avatarPath"].toString();
    m_members.insert(member_id, member);
}

void Group::setMemberName(QString& member_id,QString& member_name)
{
    m_members[member_id].member_name = member_name;
}

void Group::removeMember(const QString& user_id)
{
    m_members.remove(user_id);
}

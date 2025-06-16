#include "GroupRespository.h"
#include "LoginUserDataMoudle/LoginUserManager.h"
#include "ChatRecordManager.h"

GroupRespository::GroupRespository(QObject* parent)
{

}

GroupRespository::~GroupRespository()
{

}

//添加群组
void GroupRespository::addGroup(const QSharedPointer<Group>& group)
{
	if (!group || group->getGroupId().isEmpty())
		return;

	auto& group_id = group->getGroupId();
	if (!m_groups.contains(group_id))
	{
		m_groups.insert(group_id, group);
		auto& loginUserId = LoginUserManager::instance()->getLoginUserID();
		ChatRecordManager::instance()->addGroupChat(group_id, std::make_shared<ChatRecordMessage>(loginUserId, group_id, ChatType::Group));
	}
}

//移除群组
void GroupRespository::removeGroup(const QString& groupId)
{
	m_groups.remove(groupId);
}

//find指定群组
QSharedPointer<Group> GroupRespository::findGroup(const QString& groupId) const
{
	return m_groups.value(groupId, nullptr);
}

//获取群组列表
const QHash<QString, QSharedPointer<Group>>& GroupRespository::getGroups() const
{
	return m_groups;
}

//清除群组中心
void GroupRespository::clearGroupRespository()
{
	m_groups.clear();
}


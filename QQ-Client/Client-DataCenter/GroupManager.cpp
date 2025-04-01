#include "GroupManager.h"
#include "EventBus.h"
#include "AvatarManager.h"
#include "ImageUtil.h"

GroupManager::GroupManager()
{
	connect(EventBus::instance(), &EventBus::createGroupSuccess, this, [=](const QJsonObject& obj)
		{
			auto group_id = obj["group_id"].toString();
			auto group_name = obj["group_name"].toString();
			auto group_pixmap = AvatarManager::instance()->getAvatar(obj["user_id"].toString(), ChatType::User);
			ImageUtils::saveAvatarToLocal(group_pixmap, group_id, ChatType::Group);
			auto myGroup = QSharedPointer<Group>::create();
			myGroup->setGroup(obj);
			GroupManager::instance()->addGroup(myGroup);
			emit newGroup(group_id);
			qDebug() << myGroup->getGroupId() << myGroup->getGroupName();
		});
}
GroupManager* GroupManager::instance()
{
	static GroupManager ins;
	return &ins;
}

void GroupManager::addGroup(const QSharedPointer<Group>& group)
{
	if (group && !group->getGroupId().isEmpty())
	{
		m_groups.insert(group->getGroupId(), group);
	}
}

void GroupManager::removeGroup(const QString& groupId)
{
	m_groups.remove(groupId);
}

QSharedPointer<Group> GroupManager::findGroup(const QString& groupId) const
{
	qDebug() << m_groups << groupId << "lalallalalala";
	return m_groups.value(groupId, nullptr);
}




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
			auto& group_pixmap = AvatarManager::instance()->getAvatar(obj["user_id"].toString(), ChatType::User);
			if (!ImageUtils::saveAvatarToLocal(group_pixmap, group_id, ChatType::Group))
			{
				qDebug() << "图片保存本地失败";
			}
			auto myGroup = QSharedPointer<Group>::create();
			myGroup->setGroup(obj);
			GroupManager::instance()->addGroup(myGroup);
			qDebug() << "群组管理中心加载完成·······";
			emit newGroup(group_id);
			qDebug() <<"新建群组：" << myGroup->getGroupId() << myGroup->getGroupName();
		});
	connect(EventBus::instance(), &EventBus::newGroupMember, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			qDebug() << "GroupManager:" << obj;
			auto group_id = obj["group_id"].toString();
			auto user_id = obj["user_id"].toString();
			auto group = findGroup(group_id);
			group->addMember(obj);
			ImageUtils::saveAvatarToLocal(pixmap, user_id, ChatType::User);
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




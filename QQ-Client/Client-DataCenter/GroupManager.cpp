#include "GroupManager.h"
#include "EventBus.h"
#include "AvatarManager.h"
#include "ImageUtil.h"

GroupManager::GroupManager()
{
	//群组创建成功
	connect(EventBus::instance(), &EventBus::createGroupSuccess, this, [=](const QJsonObject& obj)
		{
			auto group_id = obj["group_id"].toString();
			auto group_name = obj["group_name"].toString();
			AvatarManager::instance()->getAvatar(obj["user_id"].toString(), ChatType::User,
				[=](const QPixmap&pixmap)
			{
				ImageUtils::saveAvatarToLocal(pixmap.toImage(), group_id, ChatType::User, [=](bool result)
				{
					if (!result)
					{
						qDebug() << "头像保存失败";
						return;
					}
					auto myGroup = QSharedPointer<Group>::create();
					myGroup->setGroup(obj);
					GroupManager::instance()->addGroup(myGroup);
					qDebug() << "群组管理中心加载完成·······";
					emit createGroupSuccess(group_id);
					qDebug() << "新建群组：" << myGroup->getGroupId() << myGroup->getGroupName();

				});
			});
		});
	//新成员添加
	connect(EventBus::instance(), &EventBus::newGroupMember, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			qDebug() << "GroupManager:" << obj;
			auto group_id = obj["group_id"].toString();
			auto user_id = obj["user_id"].toString();
			auto group = findGroup(group_id);
			group->addMember(obj);
			ImageUtils::saveAvatarToLocal(pixmap.toImage(), user_id, ChatType::User, [](bool result)
				{
					if (!result)
						qDebug() << "头像保存失败";
				});
		});
	//加载群组
	connect(EventBus::instance(), &EventBus::loadGroup, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto group_id = obj["group_id"].toString();
			auto group_name = obj["group_name"].toString();
			ImageUtils::saveAvatarToLocal(pixmap.toImage(), group_id, ChatType::Group, [](bool result)
				{
					if (!result)
						qDebug() << "头像保存失败";
				});
			auto myGroup = QSharedPointer<Group>::create();
			myGroup->setGroup(obj);
			GroupManager::instance()->addGroup(myGroup);
			emit newGroup(group_id);
		});
	//加载群聊
	connect(EventBus::instance(), &EventBus::loadGroupMember, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			qDebug() << "loadGroupMember:" << obj;
			auto group_id = obj["group_id"].toString();
			auto user_id = obj["user_id"].toString();
			auto group = findGroup(group_id);
			group->addMember(obj);
			ImageUtils::saveAvatarToLocal(pixmap.toImage(), user_id, ChatType::User, [](bool result)
				{
					if (!result)
						qDebug() << "头像保存失败";
				});
		});
}
GroupManager* GroupManager::instance()
{
	static GroupManager ins;
	return &ins;
}
//添加群组
void GroupManager::addGroup(const QSharedPointer<Group>& group)
{
	if (group && !group->getGroupId().isEmpty())
	{
		m_groups.insert(group->getGroupId(), group);
	}
}
//移除群组
void GroupManager::removeGroup(const QString& groupId)
{
	m_groups.remove(groupId);
}
//find指定群组
QSharedPointer<Group> GroupManager::findGroup(const QString& groupId) const
{
	return m_groups.value(groupId, nullptr);
}




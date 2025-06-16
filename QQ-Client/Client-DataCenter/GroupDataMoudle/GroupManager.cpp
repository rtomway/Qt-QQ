#include "GroupManager.h"
#include <QJsonObject>

#include "EventBus.h"
#include "LoginUserDataMoudle/LoginUserManager.h"


GroupManager* GroupManager::instance()
{
	static GroupManager ins;
	return &ins;
}

GroupManager::GroupManager()
	:m_groupRespository(new GroupRespository(this))
	,m_groupService(new GroupService(m_groupRespository,this))
{
	init();
}

void GroupManager::init()
{
	//外部信号
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, m_groupService, &GroupService::initAllGroupRequest);
	connect(EventBus::instance(), &EventBus::initGroupManager, m_groupService, &GroupService::initAllGroup);
	connect(EventBus::instance(), &EventBus::loadGroupMember, m_groupService, &GroupService::loadGroupMember);
	connect(EventBus::instance(), &EventBus::createGroupSuccess, m_groupService, &GroupService::createGroupSuccess);
	connect(EventBus::instance(), &EventBus::groupPictureCommunication, m_groupService, &GroupService::groupPictureCommunication);
	connect(EventBus::instance(), &EventBus::groupTextCommunication, m_groupService, &GroupService::groupTextCommunication);
	connect(EventBus::instance(), &EventBus::newGroupMember, m_groupService, &GroupService::newGroupMember);
	connect(EventBus::instance(), &EventBus::newGroup, m_groupService, &GroupService::newGroup);
	connect(EventBus::instance(), &EventBus::exitGroup, m_groupService, &GroupService::exitGroup);
	connect(EventBus::instance(), &EventBus::removeGroup, m_groupService, &GroupService::removeGroup);
	connect(EventBus::instance(), &EventBus::disbandGroup, m_groupService, &GroupService::disbandGroup);
	connect(EventBus::instance(), &EventBus::groupMemberDeleted, m_groupService, &GroupService::groupMemberDeleted);
	connect(EventBus::instance(), &EventBus::batch_groupMemberDeleted, m_groupService, &GroupService::batch_groupMemberDeleted);

	//业务信号
	connect(m_groupService, &GroupService::g_loadGroupListSuccess, this, [=]
		{
			emit loadGroupListSuccess();
		});
	connect(m_groupService, &GroupService::g_loadLocalAvatarGroup, this, [=](const QStringList&list)
		{
			emit loadLocalAvatarGroup(list);
		});
	connect(m_groupService, &GroupService::g_createGroupSuccess, this, [=](const QString&user_id)
		{
			emit createGroupSuccess(user_id);
		});
	connect(m_groupService, &GroupService::g_newGroup, this, [=](const QString&group_id)
		{
			emit newGroup(group_id);
		});
	connect(m_groupService, &GroupService::g_newGroupMember, this, [=](const QString& group_id, const QString& user_id)
		{
			emit newGroupMember(group_id, user_id);
		});
	connect(m_groupService, &GroupService::g_groupTextCommunication, this, [=](const QJsonObject&obj)
		{
			emit groupTextCommunication(obj);
		});
	connect(m_groupService, &GroupService::g_groupPictureCommunication, this, [=](const QJsonObject&obj,const QPixmap&pixmap)
		{
			emit groupPictureCommunication(obj, pixmap);
		});
	connect(m_groupService, &GroupService::g_chatWithGroup, this, [=](const QString&group_id)
		{
			emit chatWithGroup(group_id);
		});
	connect(m_groupService, &GroupService::g_exitGroup, this, [=](const QString& group_id, const QString& user_id)
		{
			emit exitGroup(group_id, user_id);
		});
	connect(m_groupService, &GroupService::g_updateGroupProfile, this, [=](const QString& group_id)
		{
			emit updateGroupProfile(group_id);
		});

}

//添加群组
void GroupManager::addGroup(const QSharedPointer<Group>& group)
{
	m_groupRespository->addGroup(group);
}

//移除群组
void GroupManager::removeGroup(const QString& group_id)
{
	m_groupRespository->removeGroup(group_id);
}

//find指定群组
QSharedPointer<Group> GroupManager::findGroup(const QString& group_id) const
{
	return m_groupRespository->findGroup(group_id);
}

//获取群组列表
const QHash<QString, QSharedPointer<Group>>& GroupManager::getGroups() const
{
	return m_groupRespository->getGroups();
}

//清除群组中心
void GroupManager::clearGroupRespository()
{
	m_groupRespository->clearGroupRespository();
}


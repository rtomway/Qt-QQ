#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_

#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QSharedPointer>

#include "Group.h"

class GroupManager :public QObject
{
	Q_OBJECT
public:
	static GroupManager* instance();
	//群组操作
	void addGroup(const QSharedPointer<Group>& group);
	void removeGroup(const QString& groupId);
	QSharedPointer<Group> findGroup(const QString& groupId) const;
	const QHash<QString, QSharedPointer<Group>>& getGroups()const;
	void clearGroupManager();
private:
	explicit GroupManager();
	GroupManager(const GroupManager&) = delete;
	GroupManager& operator=(const GroupManager&) = delete;
private:
	void loadGroupInfoFromServer(const QString& group_id, const QString& requestType);
	void loadGroupAvatarFromServer(const QStringList& group_idList, const QString& requestType);
	void ensureGroupMemberLoad(const QString& group_id, const QString& user_id, std::function<void()> callback)const;
private:
	QHash<QString, QSharedPointer<Group>> m_groups{};
signals:
	void loadLocalAvatarGroup(const QStringList& group_idList);
	void createGroupSuccess(const QString& user_id);
	void newGroup(const QString& user_id);
	void newGroupMember(const QString& group_id, const QString& user_id);
	void loadGroupListSuccess();
	void groupTextCommunication(const QJsonObject& obj);
	void groupPictureCommunication(const QJsonObject& obj, const QPixmap& pixmap);
	void chatWithGroup(const QString& group_id);
	void exitGroup(const QString& group_id, const QString& user_id);
	void updateGroupProfile(const QString& group_id);
};

#endif // !GROUPMANAGER_H_

#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_


#include <QObject>
#include <QSharedPointer>

#include "Group.h"
#include "GroupRespository.h"
#include "GroupService.h"

class GroupManager :public QObject
{
	Q_OBJECT
public:
	static GroupManager* instance();
	void init();
	//仓库操作接口
	void addGroup(const QSharedPointer<Group>& group);
	void removeGroup(const QString& group_id);
	QSharedPointer<Group> findGroup(const QString& group_id) const;
	const QHash<QString, QSharedPointer<Group>>& getGroups()const;
	void clearGroupRespository();
private:
	explicit GroupManager();
	GroupManager(const GroupManager&) = delete;
	GroupManager& operator=(const GroupManager&) = delete;
private:
	GroupRespository* m_groupRespository;
	GroupService* m_groupService;

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

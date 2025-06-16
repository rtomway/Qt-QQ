#ifndef GROUPSERVICE_H_
#define GROUPSERVICE_H_

#include <QObject>
#include "GroupRespository.h"

class GroupService :public QObject
{
	Q_OBJECT
public:
	GroupService(GroupRespository* groupRespository,QObject* parent = nullptr);
	~GroupService();
public:
	void initAllGroupRequest();
	void initAllGroup(const QJsonObject& obj);
	void loadGroupMember(const QJsonObject&obj);
	void createGroupSuccess(const QJsonObject& obj);
	void newGroupMember(const QJsonObject& obj);
	void newGroup(const QJsonObject& obj);
	void groupTextCommunication(const QJsonObject& obj);
	void groupPictureCommunication(const QJsonObject& obj,const QPixmap& pixmap);
	void exitGroup(const QString& group_id, const QString& user_id);
	void removeGroup(const QJsonObject& obj);
	void groupMemberDeleted(const QJsonObject& obj);
	void disbandGroup(const QString& group_id);
	void batch_groupMemberDeleted(const QJsonObject& obj);

	void loadGroupInfoFromServer(const QString& group_id, const QString& requestType);
	void loadGroupAvatarFromServer(const QStringList& group_idList, const QString& requestType);
	void ensureGroupMemberLoad(const QString& group_id, const QString& user_id, std::function<void()> callback)const;
private:
	GroupRespository* m_groupRespository;

signals:
	void g_loadLocalAvatarGroup(const QStringList& group_idList);
	void g_createGroupSuccess(const QString& user_id);
	void g_newGroup(const QString& user_id);
	void g_newGroupMember(const QString& group_id, const QString& user_id);
	void g_loadGroupListSuccess();
	void g_groupTextCommunication(const QJsonObject& obj);
	void g_groupPictureCommunication(const QJsonObject& obj, const QPixmap& pixmap);
	void g_chatWithGroup(const QString& group_id);
	void g_exitGroup(const QString& group_id, const QString& user_id);
	void g_updateGroupProfile(const QString& group_id);

};


#endif // !GROUPSERVICE_H_

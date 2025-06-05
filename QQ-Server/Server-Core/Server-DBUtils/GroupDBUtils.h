#ifndef GROUPDBUTILS_H_
#define GROUPDBUTILS_H_

#include <QVariantList>
#include <QSqlQuery>
#include <QDateTime>
#include <QJsonArray>
#include "DataBaseQuery.h"

struct Group
{
	QString group_id;
	QString group_name;
	QString owner_id;
	QDateTime create_time;
	QString group_avatarPath;
	int groupMemberCount;
};
struct GroupMember
{
	QString group_id;
	QString user_id;
	QString group_role;
	QString username;
};

class GroupDBUtils
{
public:
	static bool insertGroup(const Group& groupParams,DataBaseQuery&query, std::shared_ptr<QSqlQuery> queryPtr=nullptr);
	static bool insertGroupMember(const GroupMember& groupMemberParams, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static bool groupMemberCountAdd(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static bool groupMemberCountSub(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static Group queryGroup(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static QStringList queryGroupMemberIdList(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static GroupMember queryGroupMember(const QString& group_id, const QString&member_id,DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static QStringList queryGroupIdList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static bool deleteGroupMember(const GroupMember& groupMember, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static bool batch_deleteGroupMember(const QString& group_id,const QStringList&id_list, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static bool deleteGroup(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
};

#endif // !GROUPDBUTILS_H_

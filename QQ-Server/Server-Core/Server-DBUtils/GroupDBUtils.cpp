#include "GroupDBUtils.h"
#include <QJsonObject>

#include "ImageUtil.h"

//插入新群组
bool GroupDBUtils::insertGroup(const Group& groupParams,DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"insert into `group` \
		(group_id,group_name,owner_id,create_time,group_avatarPath)\
		values(?,?,?,?,?)"
	);
	QVariantList bindValues;
	bindValues.append(groupParams.group_id);
	bindValues.append(groupParams.group_name);
	bindValues.append(groupParams.owner_id);
	bindValues.append(groupParams.create_time);
	bindValues.append(groupParams.group_avatarPath);
	auto insertResult= query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!insertResult)
	{
		qDebug() << "insertGroup failed";
		return false;
	}
	return true;
}

//插入群成员
bool GroupDBUtils::insertGroupMember(const GroupMember& groupMemberParams, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"insert into groupMembers\
		(group_id, user_id, username, group_role)\
		values(? , ? , ? , ? )"
	);
	QVariantList bindValues;
	bindValues.append(groupMemberParams.group_id);
	bindValues.append(groupMemberParams.user_id);
	bindValues.append(groupMemberParams.username);
	bindValues.append(groupMemberParams.group_role);
	auto insertResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!insertResult)
	{
		qDebug() << "insertGroupMember failed";
		return false;
	}
	return true;
}

//群人数加1
bool GroupDBUtils::groupMemberCountAdd(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"update `group`\
		set groupMemberCount=groupMemberCount+1\
		where group_id=?"
	);
	QVariantList bindValues;
	bindValues.append(group_id);
	auto insertResult = query.executeNonQuery(queryStr, bindValues,queryPtr);
	if (!insertResult)
	{
		qDebug() << "groupMemberCountAdd failed";
		return false;
	}
	return true;
}

//群人数减1
bool GroupDBUtils::groupMemberCountSub(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"update `group`\
		set groupMemberCount=groupMemberCount-1\
		where group_id=?"
	);
	QVariantList bindValues;
	bindValues.append(group_id);
	auto updateResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!updateResult)
	{
		qDebug() << "groupMemberCountSub failed";
		return false;
	}
	return true;
}

//群组查询
Group GroupDBUtils::queryGroup(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString("select* from `group` where group_id=?");
	QVariantList bindValues;
	bindValues.append(group_id);
	auto queryResult = query.executeQuery(queryStr, bindValues, queryPtr);
	Group queryGroup;
	if (queryResult.contains("error"))
	{
		qDebug() << "queryGroup failed";
		return queryGroup;
	}
	QJsonArray dataArray = queryResult["data"].toArray();
	if (dataArray.isEmpty()) {
		qDebug() << "群不存在或查询结果为空";
		return queryGroup;
	}
	auto groupOwnerObj = dataArray.at(0).toObject();
	queryGroup.group_id = groupOwnerObj["group_id"].toString();
	queryGroup.group_name = groupOwnerObj["group_name"].toString();
	queryGroup.owner_id = groupOwnerObj["owner_id"].toString();
	queryGroup.groupMemberCount = groupOwnerObj["groupMemberCount"].toInt();
	queryGroup.create_time = QDateTime::fromString(groupOwnerObj["create_time"].toString());
	queryGroup.group_avatarPath = groupOwnerObj["group_avatarPath"].toString();
	return queryGroup;
}

//查询所有群成员ID
QStringList GroupDBUtils::queryGroupMemberIdList(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString("select user_id from groupmembers where group_id=?");
	QVariantList bindValues;
	bindValues.append(group_id);
	auto queryResult = query.executeQuery(queryStr, bindValues, queryPtr);
	if (queryResult.contains("error"))
	{
		qDebug() << "查询群成员id失败";
	}
	QStringList groupMember_idList;
	auto memberArray = queryResult["data"].toArray();
	for (auto memberIdValues: memberArray)
	{
		auto member_idObj = memberIdValues.toObject();
		auto member_id = member_idObj["user_id"].toString();
		groupMember_idList.append(member_id);
	}
	return groupMember_idList;
}

//查询单个群成员信息
GroupMember GroupDBUtils::queryGroupMember(const QString& group_id, const QString& member_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString("select * from groupmembers where group_id=? and user_id=?");
	QVariantList bindValues;
	bindValues.append(group_id);
	bindValues.append(member_id);
	auto queryResult = query.executeQuery(queryStr, bindValues, queryPtr);
	GroupMember groupMember;
	if (queryResult.contains("error"))
	{
		qDebug() << "queryGroup failed";
		return groupMember;
	}
	QJsonArray dataArray = queryResult["data"].toArray();
	if (dataArray.isEmpty()) {
		qDebug() << "群成员不存在或查询结果为空";
		return groupMember;
	}
	auto groupMemberObj = dataArray.at(0).toObject();
	groupMember.user_id = groupMemberObj["user_id"].toString();
	groupMember.username = groupMemberObj["username"].toString();
	groupMember.group_id = groupMemberObj["group_id"].toString();
	groupMember.group_role = groupMemberObj["group_role"].toString();
	return groupMember;
}

//查询群组id
QStringList GroupDBUtils::queryGroupIdList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString("select group_id from groupmembers where user_id=?");
	QVariantList bindValues;
	bindValues.append(user_id);
	auto queryResult = query.executeQuery(queryStr, bindValues, queryPtr);
	if (queryResult.contains("error"))
	{
		qDebug() << "查询所有群组id失败";
	}
	QStringList group_idList;
	auto groupArray = queryResult["data"].toArray();
	for (auto groupIdValues : groupArray)
	{
		auto group_idObj = groupIdValues.toObject();
		auto group_id = group_idObj["group_id"].toString();
		group_idList.append(group_id);
	}
	return group_idList;
}

//删除群成员
bool GroupDBUtils::deleteGroupMember(const GroupMember& groupMember, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"delete from groupMembers\
		where group_id=? and user_id=?"
	);
	QVariantList bindValues;
	bindValues.append(groupMember.group_id);
	bindValues.append(groupMember.user_id);
	auto deleteResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!deleteResult)
	{
		qDebug() << "deleteGroupMember failed";
		return false;
	}
	return true;
}

//批量删除群成员
bool GroupDBUtils::batch_deleteGroupMember(const QString& group_id, const QStringList& id_list, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{

	if (id_list.isEmpty())
		return true;

	QString placeholders;
	for (int i = 0; i < id_list.size(); ++i) {
		placeholders += "?,";
	}
	placeholders.chop(1); 

	QString queryStr = QString(
		"delete from groupMembers\
		where group_id=? and user_id in (%1)"
	).arg(placeholders);

	QVariantList bindValues;
	bindValues.append(group_id);
	for (auto& id : id_list)
	{
		bindValues.append(id);
	}
	auto deleteResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!deleteResult)
	{
		qDebug() << "deleteGroupMember failed";
		return false;
	}
	return true;
}

//删除群组
bool GroupDBUtils::deleteGroup(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"delete from `group` where group_id=?"
	);
	QVariantList bindValues;
	bindValues.append(group_id);
	auto deleteResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!deleteResult)
	{
		qDebug() << "deleteGroupMember failed";
		return false;
	}
	return true;
}

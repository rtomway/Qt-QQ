#include "UserDBUtils.h"
#include <QJsonArray>

//查看指定指定用户个人信息
QJsonObject UserDBUtils::queryUserDetail(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"select f.Fgrouping,u.* from user u join friendship f \
		on (u.user_id=f.user_id) \
		where u.user_id=?"
	);
	QVariantList bindValues;
	bindValues.append(user_id);
	auto queryResult = query.executeQuery(queryStr, bindValues,queryPtr);
	QJsonObject queryUserObj;
	qDebug() << "queryUserDetail-------------:"<<queryResult;
	if (queryResult.contains("error"))
	{
		qDebug() << "queryUserDetail failed";
		queryUserObj["error"] = "";
		return queryUserObj;
	}
	QJsonArray dataArray = queryResult["data"].toArray();
	if (dataArray.isEmpty())
	{
		qDebug() << "queryUserDetail查询结果为空";
		return queryUserObj;
	}
	queryUserObj = dataArray.at(0).toObject();
	queryUserObj.remove("password");
	return queryUserObj;
}
//搜索相关用户
QJsonObject UserDBUtils::searchUser(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"select user_id,username from user where user_id like ?"
	);
	QVariantList bindValues;
	bindValues.append(user_id);
	auto queryResult = query.executeQuery(queryStr, bindValues, queryPtr);
	QJsonObject searchUserObj;
	if (queryResult.contains("error"))
	{
		qDebug() << "searchUser failed";
		searchUserObj["error"] = "";
		return searchUserObj;
	}
	QJsonArray searchUserListArray = queryResult["data"].toArray();
	searchUserObj["searchUserList"] = searchUserListArray;
	return searchUserObj;
}

QJsonObject UserDBUtils::searchGroup(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"select group_id,group_name from `group` where group_id like ?"
	);
	QVariantList bindValues;
	bindValues.append(group_id);
	auto queryResult = query.executeQuery(queryStr, bindValues, queryPtr);
	QJsonObject searchGroupObj;
	if (queryResult.contains("error"))
	{
		qDebug() << "searchUser failed";
		searchGroupObj["error"] = "";
		return searchGroupObj;
	}
	QJsonArray searchGroupListArray = queryResult["data"].toArray();
	searchGroupObj["searchGroupList"] = searchGroupListArray;
	return searchGroupObj;
}

bool UserDBUtils::updateUserMessage(const UserInfo& userInfo, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr =QString(
		"UPDATE user \
		SET username=?, gender=?, age=?, phone_number=?, \
		email=?, birthday=?, signature=? \
		WHERE user_id=?"
	);
	QVariantList bindValues;
	bindValues.append(userInfo.username);
	bindValues.append(userInfo.gender);
	bindValues.append(userInfo.age);
	// 使用 QVariant() 表示 NULL
	bindValues.append(userInfo.phone_number.isEmpty() ? QVariant(QVariant::String).toString() : userInfo.phone_number);
	bindValues.append(userInfo.email.isEmpty() ? QVariant(QVariant::String).toString() :userInfo.email);
	bindValues.append(userInfo.birthday);
	bindValues.append(userInfo.signature.isEmpty() ? QVariant(QVariant::String).toString() : userInfo.signature);
	bindValues.append(userInfo.user_id);

	return query.executeNonQuery(queryStr, bindValues, queryPtr);
}

#include "LoginDBUtils.h"
#include <QJsonArray>
#include "ImageUtil.h"

//密码验证
bool LoginDBUtils::validationPassWord(const QString& user_id, const QString& password, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString("select password from user where user_id=?");
	QVariantList bindValues;
	bindValues.append(user_id);
	auto queryResult = query.executeQuery(queryStr, bindValues);
	if (queryResult.contains("error"))
	{
		qDebug() << "validationPassWord failed";
		return false;
	}
	QJsonArray dataArray = queryResult["data"].toArray();
	if (dataArray.isEmpty()) 
	{
		qDebug() << "查询结果为空";
		return false;
	}
	auto passwordObj = dataArray.at(0).toObject();
	auto queryPassWord = passwordObj["password"].toString();
	if (queryPassWord == password)
	{
		return true;
	}
	return false;
}

//查询好友列表所有信息
QJsonObject LoginDBUtils::queryFriendList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"select f.Fgrouping,user.* from friendship f \
		join user ON f.friend_id = user.user_id \
		where f.user_id=? and f.friend_id != f.user_id"
	);
	QVariantList bindValues;
	bindValues.append(user_id);
	auto queryResult = query.executeQuery(queryStr, bindValues);
	QJsonObject friendListObj;
	if (queryResult.contains("error"))
	{
		qDebug() << "queryFriendList failed";
		friendListObj["error"] = "";
		return QJsonObject();
	}
	QJsonArray friendListArray = queryResult["data"].toArray();
	friendListObj["friendList"] = friendListArray;
	return friendListObj;
}

//查询群组列表所有信息
QJsonObject LoginDBUtils::queryGroupList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"SELECT g.*,gm.group_role\
		FROM `group` g\
		JOIN groupmembers gm ON g.group_id = gm.group_id\
		WHERE gm.user_id =?;"
	);
	QVariantList bindValues;
	bindValues.append(user_id);
	auto queryResult = query.executeQuery(queryStr, bindValues);
	QJsonObject groupListObj;
	if (queryResult.contains("error"))
	{
		qDebug() << "queryGroupList failed";
		groupListObj["error"] = "";
		return QJsonObject();
	}
	QJsonArray groupListArray = queryResult["data"].toArray();
	groupListObj["groupList"] = groupListArray;
	return groupListObj;
}

//查询指定群组的所有成员信息
QJsonObject LoginDBUtils::queryGroupMemberList(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"select user_id,username,group_role from groupmembers\
		where group_id=?"
	);
	QVariantList bindValues;
	bindValues.append(group_id);
	auto queryResult = query.executeQuery(queryStr, bindValues);
	QJsonObject groupMemberListObj;
	if (queryResult.contains("error"))
	{
		qDebug() << "queryGroupMemberList failed";
		groupMemberListObj["error"] = "";
		return QJsonObject();
	}
	QJsonArray groupMemberListArray = queryResult["data"].toArray();
	groupMemberListObj["groupMemberArray"] = groupMemberListArray;
	return groupMemberListObj;
}

//注册插入新用户
bool LoginDBUtils::insertUser(const RegisterMessage& registerMessage, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString("insert into user (user_id,username,password,avatar_path)values(?,?,?,?)");
	auto avatarPath = ImageUtils::getUserAvatarFolderPath() + "/" + registerMessage.user_id + ".png";
	QVariantList bindValues;
	bindValues.append(registerMessage.user_id);
	bindValues.append(registerMessage.username);
	bindValues.append(registerMessage.password);
	bindValues.append(avatarPath);
	auto insertResult = query.executeNonQuery(queryStr, bindValues,queryPtr);
	if (!insertResult)
	{
		qDebug() << "insertUser failed";
		return false;
	}
	return true;
}

#include "FriendDBUtils.h"
#include <QJsonArray>

//添加好友
bool FriendDBUtils::insertFriend(const MyFriend& myFriend, DataBaseQuery& query, std::shared_ptr<QSqlQuery>queryPtr)
{
	QString queryStr = QString(
		"insert into friendship \
		(user_id,friend_id,Fgrouping)\
		values(?,?,?)"
	);
	QVariantList bindValues;
	bindValues.append(myFriend.user_id);
	bindValues.append(myFriend.friend_id);
	bindValues.append(myFriend.friendGrouping);
	auto insertResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!insertResult)
	{
		qDebug() << "insertGroup failed";
		return false;
	}
	return true;
}

//更新好友分组
bool FriendDBUtils::updateFriendGrouping(const MyFriend& myFriend, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"update friendship f \
		set Fgrouping = ? \
		where f.user_id=? and f.friend_id=?"
	);
	QVariantList bindValues;
	bindValues.append(myFriend.friendGrouping);
	bindValues.append(myFriend.user_id);
	bindValues.append(myFriend.friend_id);
	auto updateResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!updateResult)
	{
		qDebug() << "updateFriendGrouping failed";
		return false;
	}
	return true;
}

//删除好友
bool FriendDBUtils::deleteFriend(const MyFriend& myFriend, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	QString queryStr = QString(
		"delete from friendship where user_id=? and friend_id=?"
	);
	QVariantList bindValues;
	bindValues.append(myFriend.user_id);
	bindValues.append(myFriend.friend_id);
	auto deleteResult = query.executeNonQuery(queryStr, bindValues, queryPtr);
	if (!deleteResult)
	{
		qDebug() << "deleteFriend failed";
		return false;
	}
	return true;
}

//查询好友id
QStringList FriendDBUtils::queryFriendIdList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr)
{
	qDebug() << "------------------------------------查询所有好友ID---------------------------------------";
	QString queryStr = QString("select friend_id from friendship where user_id=?");
	QVariantList bindValues;
	bindValues.append(user_id);
	auto queryResult = query.executeQuery(queryStr, bindValues, queryPtr);
	if (queryResult.contains("error"))
	{
		qDebug() << "查询好友id失败";
	}
	QStringList friend_idList;
	auto friendArray = queryResult["data"].toArray();
	for (auto friendIdValues : friendArray)
	{
		auto friend_idObj = friendIdValues.toObject();
		auto friend_id = friend_idObj["friend_id"].toString();
		friend_idList.append(friend_id);
	}
	return friend_idList;
}
 
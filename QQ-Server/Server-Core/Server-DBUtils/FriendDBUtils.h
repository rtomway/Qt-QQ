#ifndef FRIENDDBUTILS_H_
#define FRIENDDBUTILS_H_

#include <QSqlQuery>
#include <QString>
#include "DataBaseQuery.h"

struct MyFriend
{
	QString user_id;
	QString friend_id;
	QString friendGrouping=QString();
	MyFriend(const QString& user_id, const QString& friend_id, const QString& friendGrouping=QString())
		:user_id(user_id), friend_id(friend_id), friendGrouping(friendGrouping)
	{};
};

class FriendDBUtils
{
public:
	static bool insertFriend(const MyFriend& myFriend, DataBaseQuery& query, std::shared_ptr<QSqlQuery>queryPtr = nullptr);
	static bool updateFriendGrouping(const MyFriend& myFriend, DataBaseQuery& query, std::shared_ptr<QSqlQuery>queryPtr = nullptr);
	static bool deleteFriend(const MyFriend& myFriend, DataBaseQuery& query, std::shared_ptr<QSqlQuery>queryPtr = nullptr);
	static QStringList queryFriendIdList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
};

#endif // !FRIENDDBUTILS_H_

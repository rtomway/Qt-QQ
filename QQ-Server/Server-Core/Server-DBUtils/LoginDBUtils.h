#ifndef LOGINDBUTILS_H_
#define LOGINDBUTILS_H_

#include <QSqlQuery>
#include "DataBaseQuery.h"
#include <QJsonObject>

struct RegisterMessage
{
	QString user_id;
	QString username;
	QString password;
};

class LoginDBUtils
{
public:
	static bool validationPassWord(const QString& user_id, const QString& password, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static QJsonObject queryFriendList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static QJsonObject queryGroupList(const QString& user_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static QJsonObject queryGroupMemberList(const QString& group_id, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
	static bool insertUser(const RegisterMessage& registerMessage, DataBaseQuery& query, std::shared_ptr<QSqlQuery> queryPtr = nullptr);
};

#endif // !LOGINDBUTILS_H_

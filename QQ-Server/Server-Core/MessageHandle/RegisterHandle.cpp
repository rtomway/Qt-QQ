#include "RegisterHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
#include "CreateId.h"
#include "GlobalTypes.h"

//注册
void RegisterHandle::handle_register(const QJsonObject& paramsObj,const QByteArray& data, QHttpServerResponder& responder)
{
	//auto paramsObject = QJsonDocument::fromJson(data);
	auto user_id = CreateId::generateUserID(CreateId::Id::User);
	QString username = paramsObj["username"].toString();
	QString password = paramsObj["password"].toString();
	auto avatarPath = user_id + ".png";
	//数据库查询
	//注册信息插入
	DataBaseQuery query;
	auto registerResult = query.executeTransaction([&](std::shared_ptr<QSqlQuery>queryPtr)->bool
		{
			QString queryStr = "insert into user (user_id,username,password,avatar_path)values(?,?,?,?)";
			QVariantList bindvalues;
			bindvalues.append(user_id);
			bindvalues.append(username);
			bindvalues.append(password);
			bindvalues.append(avatarPath);
			auto queryResult = query.executeNonQuery(queryStr, bindvalues,queryPtr);
			bindvalues.clear();
			//错误返回
			if (!queryResult) {
				qDebug() << "register insert into user  Error query:";
				return false;
			}
			//好友表插入
			QString insertStr = "insert into friendship(user_id, friend_id, create_time, Fgrouping)values(? , ? , ? , ? )";
			bindvalues.append(user_id);
			auto friend_id = user_id;
			bindvalues.append(friend_id);
			bindvalues.append(QDate::currentDate());
			bindvalues.append("我的好友");
			auto insertResult = query.executeNonQuery(insertStr, bindvalues,queryPtr);
			//错误返回
			if (!insertResult) {
				qDebug() << "register insert into friendship Error query:";
				return false;
			}
		});
	//注册成功后返回该用户账号与密码
	if (registerResult)
	{
		QJsonObject resgisterObj;
		resgisterObj["type"] = "registerSuccess";
		QJsonObject params;
		params["user_id"] = user_id;
		params["password"] = password;
		resgisterObj["params"] = params;
		QJsonDocument doc(resgisterObj);
		//http响应
		responder.write(doc);
	}
}

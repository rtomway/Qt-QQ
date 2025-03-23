#include "RegisterHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"

//注册
void RegisterHandle::handle_register(const QJsonObject& paramsObject, const QByteArray& data)
{
	QString user_id = paramsObject["user_id"].toString();
	auto client_id = user_id;
	QString username = paramsObject["username"].toString();
	QString password = paramsObject["password"].toString();
	auto avatarPath = user_id + ".png";
	//数据库查询
	//注册信息插入
	DataBaseQuery query;
	QString queryStr = "insert into user (user_id,username,password,avatar_path)values(?,?,?,?)";
	QVariantList bindvalues;
	bindvalues.append(user_id);
	bindvalues.append(username);
	bindvalues.append(password);
	bindvalues.append(avatarPath);
	auto queryResult = query.executeNonQuery(queryStr, bindvalues);
	bindvalues.clear();
	//错误返回
	if (!queryResult) {
		qDebug() << "Error query:";
		return;
	}
	//好友表插入
	QString insertStr = "insert into friendship (user_id,friend_id,create_time,Fgrouping)values(?,?,?,?)";
	bindvalues.append(user_id);
	auto friend_id = user_id;
	bindvalues.append(friend_id);
	bindvalues.append(QDate::currentDate());
	bindvalues.append("我的好友");
	auto insertResult = query.executeNonQuery(insertStr, bindvalues);
	//错误返回
	if (!insertResult) {
		qDebug() << "Error query:";
		return;
	}
	//注册成功后返回该用户账号与密码
	QJsonObject resgisterObj;
	resgisterObj["type"] = "registerSuccess";
	QJsonObject params;
	params["user_id"] = user_id;
	params["password"] = password;
	resgisterObj["params"] = params;

	QJsonDocument doc(resgisterObj);
	QString message = QString(doc.toJson(QJsonDocument::Compact));
	ConnectionManager::instance()->sendTextMessage(client_id, message);
}

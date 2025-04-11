#include "LoginHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
//#include "jwt-cpp/jwt.h"

void LoginHandle::handle_login(const QJsonObject& paramsObject, const QByteArray& data)
{
	//auto paramsObject = QJsonDocument::fromJson(data);
	qDebug() << "登录数据";
	auto user_id = paramsObject["user_id"].toString();
	auto password = paramsObject["password"].toString();
	auto client_id = user_id;
	//数据库查询
	DataBaseQuery query;
	QString queryStr = "select f.Fgrouping,u.* from user u join friendship f on (u.user_id=f.user_id and u.user_id=f.friend_id) where u.user_id=?";
	QVariantList bindvalues;
	bindvalues.append(user_id);
	auto allQueryObj = query.executeQuery(queryStr, bindvalues);
	//错误返回
	if (allQueryObj.contains("error")) {
		qDebug() << "Error executing query:" << allQueryObj["error"].toString();
		return;
	}
	//查询数据验证
	QJsonObject userDataObj; //存放返回客户端的所有信息
	QJsonArray userArray = allQueryObj["data"].toArray();
	for (const auto& userValue : userArray)
	{
		QJsonObject userObj = userValue.toObject();
		userObj.remove("password");
		userObj["grouping"] = userObj["Fgrouping"].toString();
		qDebug() << "我的信息---------------------" << userObj;
		userDataObj["loginUser"] = userObj;
	}
	userDataObj["friendArray"] = getFriendsMessage(user_id);
	QJsonObject allData;
	allData["type"] = "loginSuccess";
	allData["params"] = userDataObj;
	QJsonDocument doc(allData);

	QString message = QString(doc.toJson(QJsonDocument::Compact));
	ConnectionManager::instance()->sendTextMessage(client_id, message);
}
void LoginHandle::handle_loginValidation(const QJsonObject& paramsObj,const QByteArray& data, QHttpServerResponder& responder)
{
	//auto paramsObject = QJsonDocument::fromJson(data);
	auto user_id = paramsObj["user_id"].toString();
	auto password = paramsObj["password"].toString();
	auto client_id = user_id;
	//数据库查询
	DataBaseQuery query;
	QString queryStr = "select password from user where user_id=?";
	QVariantList bindvalues;
	bindvalues.append(user_id);
	auto allQueryObj = query.executeQuery(queryStr, bindvalues);
	//错误返回
	if (allQueryObj.contains("error")) {
		qDebug() << "Error executing query:" << allQueryObj["error"].toString();
		return;
	}
	//查询数据验证
	QJsonObject userDataObj; //存放返回客户端的所有信息
	QJsonArray userArray = allQueryObj["data"].toArray();
	for (const auto& userValue : userArray)
	{
		QJsonObject userObj = userValue.toObject();
		if (userObj["password"].toString() != password)
		{
			qDebug() << "密码错误";
			return;
		}
	}
	//token
	// 创建 JWT
	//auto token = jwt::create()
	//	.set_issuer("my_service")  // 设置 JWT 发行者
	//	.set_payload_claim("user_id", jwt::claim(std::string("1234")))  // 设置自定义负载字段
	//	.set_payload_claim("username", jwt::claim(std::string("john_doe")))  // 设置用户名
	//	.set_payload_claim("role", jwt::claim(std::string("admin")))  // 设置角色信息
	//	.set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(1))  // 设置过期时间为 1 小时后
	//	.sign(jwt::algorithm::hs256{ "my_secret_key" });  // 使用密钥 "my_secret_key" 进行签名
	auto token = "xu";
	QJsonObject allData;
	allData["token"] = QString::fromStdString(token);
	QJsonDocument responseDoc(allData);
	// 发送 HTTP 响应
	responder.write(responseDoc);

}
//好友信息
QJsonArray LoginHandle::getFriendsMessage(const QString& user_id)
{
	//数据库查询
	DataBaseQuery query;
	QString queryStr = QString("select f.Fgrouping,user.* from friendship f \
			join user ON f.friend_id = user.user_id \
	where f.user_id=? and f.friend_id != f.user_id ");
	QVariantList bindvalues;
	bindvalues.append(user_id);
	auto allQueryObj = query.executeQuery(queryStr, bindvalues);
	//错误返回
	if (allQueryObj.contains("error")) {
		qDebug() << "Error executing query:" << allQueryObj["error"].toString();
		return QJsonArray();
	}
	//查询数据验证
	QJsonArray friendArray;
	QJsonArray userArray = allQueryObj["data"].toArray();
	for (const auto& userValue : userArray)
	{
		QJsonObject userObj = userValue.toObject();
		userObj.remove("password");
		userObj["grouping"] = userObj["Fgrouping"].toString();
		qDebug() << "好友信息---------------------" << userObj;
		friendArray.append(userObj);
	}
	return friendArray;
}

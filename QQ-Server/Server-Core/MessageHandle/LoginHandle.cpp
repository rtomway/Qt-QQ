#include "LoginHandle.h"
#include <QJsonArray>
#include <QJsonDocument>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"

//登录请求处理
//void LoginHandle::handle_login(const QJsonObject& paramsObject, const QByteArray& data)
//{
//	auto user_id = paramsObject["user_id"].toString();
//	auto password = paramsObject["password"].toString();
//	auto client_id = user_id;
//	//数据库查询
//	DataBaseQuery query;
//	QString queryStr = "select f.Fgrouping,u.* from user u join friendship f on (u.user_id=f.user_id and u.user_id=f.friend_id) where u.user_id=?";
//	QVariantList bindvalues;
//	bindvalues.append(user_id);
//	auto allQueryObj = query.executeQuery(queryStr, bindvalues);
//	//错误返回
//	if (allQueryObj.contains("error")) {
//		qDebug() << "Error executing query:" << allQueryObj["error"].toString();
//		return;
//	}
//	//查询数据验证
//	QJsonObject userDataObj; //存放返回客户端的所有信息
//	QJsonArray userArray = allQueryObj["data"].toArray();
//	for (const auto& userValue : userArray)
//	{
//		QJsonObject userObj = userValue.toObject();
//		if (userObj["password"].toString() != password)
//		{
//			return;
//		}
//		userObj.remove("password");
//		userObj["grouping"] = userObj["Fgrouping"].toString();
//		qDebug() << "我的信息---------------------"<< userObj;
//		userDataObj["loginUser"] = userObj;
//	}
//	userDataObj["friendArray"] = getFriendsMessage(user_id);
//	QJsonObject allData;
//	allData["type"] = "loginSuccess";
//	allData["params"] = userDataObj;
//
//	QJsonDocument doc(allData);
//	QString message = QString(doc.toJson(QJsonDocument::Compact));
//	ConnectionManager::instance()->sendTextMessage(client_id, message);
//}
void LoginHandle::handle_login(const QByteArray& data, QHttpServerResponder& reponse)
{
	auto paramsObject = QJsonDocument::fromJson(data);
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
		if (userObj["password"].toString() != password)
		{
			return;
		}
		userObj.remove("password");
		userObj["grouping"] = userObj["Fgrouping"].toString();
		qDebug() << "我的信息---------------------" << userObj;
		userDataObj["loginUser"] = userObj;
	}
	userDataObj["friendArray"] = getFriendsMessage(user_id);
	QJsonObject allData;
	allData["type"] = "loginSuccess";
	allData["params"] = userDataObj;
	QJsonDocument responseDoc(allData);

	// 发送 HTTP 响应
	//reponse.writeHeader("Content-Type", "application/json");
	reponse.write(responseDoc);
	//qDebug() << "Processing login request for user:" << user_id;

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

#include "LoginHandle.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QImage>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"

//#include "jwt-cpp/jwt.h"

//登录认证
void LoginHandle::handle_loginValidation(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
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
	//responder.writeHeader("Content-Type", "application/json");
	responder.write(responseDoc);

}
//登录
void LoginHandle::handle_login(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------登录用户数据的加载------------------------";
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
		userDataObj["loginUser"] = userObj;
	}
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	userDataObj["size"] = imageData.size();
	auto packet = PacketCreate::binaryPacket("loginSuccess", userDataObj.toVariantMap(), imageData);
	QByteArray loginUserData;
	PacketCreate::addPacket(loginUserData, packet);
	auto allData = PacketCreate::allBinaryPacket(loginUserData);
	ConnectionManager::instance()->sendBinaryMessage(user_id, allData);
}
//加载好友列表
void LoginHandle::handle_loadFriendList(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	qDebug() << "--------------------------加载好友列表------------------------";
	qDebug() << paramsObj;
	auto user_id = paramsObj["user_id"].toString();
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
		return;
	}
	//查询数据验证
	QJsonArray friendListArray;
	QJsonArray userListArray = allQueryObj["data"].toArray();
	for (const auto& friendValue : userListArray)
	{
		QJsonObject friendObj = friendValue.toObject();
		friendObj.remove("password");
		friendObj["grouping"] = friendObj["Fgrouping"].toString();
		qDebug() << "--------------------------loadFriend------------------------" << friendObj;
		friendListArray.append(friendObj);
	}
	QJsonObject friendListObj;
	friendListObj["friendList"] = friendListArray;
	QJsonObject replyObj;
	replyObj["params"] = friendListObj;
	replyObj["type"] = "loadFriendList";
	QJsonDocument friendListDoc(replyObj);
	responder.write(friendListDoc);
}
//加载群组列表
void LoginHandle::handle_loadGroupList(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	qDebug() << "--------------------------加载群组列表------------------------";
	auto user_id = paramsObj["id"].toString();
	//查询相关群组信息以及用户角色
	DataBaseQuery query;
	QString queryStr = QString("SELECT g.group_id, g.group_name, g.owner_id,g.groupMemberCount,gm.group_role\
		FROM `group` g\
		JOIN groupmembers gm ON g.group_id = gm.group_id\
		WHERE gm.user_id =?;");
	QVariantList bindvalues;
	bindvalues.append(user_id);
	auto ResultObj = query.executeQuery(queryStr, bindvalues);
	if (ResultObj.contains("error"))
	{
		qDebug() << "加载群组列表 failed";
		return;
	}
	auto groupArray = ResultObj["data"].toArray();
	QJsonArray groupListArray;
	for (auto groupValue : groupArray)
	{
		auto groupObj = groupValue.toObject();
		qDebug() << "群人数:" << groupObj["groupMemberCount"].toInt();
		groupListArray.append(groupObj);
	}
	QJsonObject loadGroupListObj;
	loadGroupListObj["groupList"] = groupListArray;
	QJsonObject allDataObj;
	allDataObj["params"] = loadGroupListObj;
	allDataObj["type"] = "loadGroupList";
	QJsonDocument loadGroupListDoc(allDataObj);
	responder.write(loadGroupListDoc);
}
//加载群成员
void LoginHandle::handle_loadGroupMember(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto group_id = paramsObj["id"].toString();
	//群成员信息
	DataBaseQuery query;
	QString queryStr("select user_id,username,group_role from groupmembers where group_id=?");
	QVariantList bindvalues;
	bindvalues.append(group_id);
	auto queryResult = query.executeQuery(queryStr, bindvalues);
	if (queryResult.contains("error"))
	{
		qDebug() << "queryResult failed";
		return;
	}
	QJsonArray groupMemberArray;
	auto memberArray = queryResult["data"].toArray();
	for (const auto& memberValue : memberArray)
	{
		//获取群成员
		QJsonObject memberObj = memberValue.toObject();
		groupMemberArray.append(memberObj);
		qDebug() << "服务端-----------群臣元--------" << memberObj;
	}
	QJsonObject groupMemberObj;
	groupMemberObj["groupMemberArray"] = groupMemberArray;
	groupMemberObj["group_id"] = group_id;
	QJsonObject allDataObj;
	allDataObj["params"] = groupMemberObj;
	allDataObj["type"] = "loadGroupMember";
	QJsonDocument groupMemberDoc(allDataObj);
	responder.write(groupMemberDoc);
}
//加载群成员头像
void LoginHandle::handle_loadGroupMemberAvatar(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto groupMember_idArray = paramsObj["applicateAvatar_idList"].toArray();
	QByteArray avatarsData;
	for (auto userId_values : groupMember_idArray)
	{
		auto user_id = userId_values.toString();
		auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
		QVariantMap groupMemberAvatarMap;
		groupMemberAvatarMap["user_id"] = user_id;
		groupMemberAvatarMap["size"] = imageData.size();
		auto packet = PacketCreate::binaryPacket("loadGroupMemberAvatar", groupMemberAvatarMap, imageData);
		PacketCreate::addPacket(avatarsData, packet);
	}
	auto allData = PacketCreate::allBinaryPacket(avatarsData);
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
}
//加载群组头像
void LoginHandle::handle_loadGroupAvatars(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto group_IdArray = paramsObj["applicateAvatar_idList"].toArray();
	QByteArray avatarsData;
	for (auto groupId_values : group_IdArray)
	{
		auto group_id = groupId_values.toString();
		auto imageData = ImageUtils::loadImage(group_id, ChatType::Group);
		QVariantMap groupAvatarMap;
		groupAvatarMap["group_id"] = group_id;
		groupAvatarMap["size"] = imageData.size();
		auto packet = PacketCreate::binaryPacket("loadGroupAvatars", groupAvatarMap, imageData);
		PacketCreate::addPacket(avatarsData, packet);
	}
	auto allData = PacketCreate::allBinaryPacket(avatarsData);
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
}
void LoginHandle::handle_loadFriendAvatars(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto friend_IdArray = paramsObj["friend_ids"].toArray();
	QByteArray avatarsData;
	for (auto friendId_values : friend_IdArray)
	{
		auto friend_id = friendId_values.toString();
		auto imageData = ImageUtils::loadImage(friend_id, ChatType::User);
		QVariantMap friendAvatarMap;
		friendAvatarMap["friend_id"] = friend_id;
		friendAvatarMap["size"] = imageData.size();
		auto packet = PacketCreate::binaryPacket("loadFriendAvatars", friendAvatarMap, imageData);
		PacketCreate::addPacket(avatarsData, packet);
	}
	auto allData = PacketCreate::allBinaryPacket(avatarsData);
	QByteArray mimeType = "application/octet-stream";
	responder.write(allData, mimeType);
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

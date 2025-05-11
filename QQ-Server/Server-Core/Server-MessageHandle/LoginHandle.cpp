#include "LoginHandle.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QImage>

#include "DataBaseQuery.h"
#include "ConnectionManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"
#include "LoginDBUtils.h"
#include "UserDBUtils.h"

#include <jwt-cpp/jwt.h>
#include <picojson/picojson.h>


 
//登录认证
void LoginHandle::handle_loginValidation(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	auto user_id = paramsObj["user_id"].toString();
	auto password = paramsObj["password"].toString();
	//数据库查询
	DataBaseQuery query;
	if (!LoginDBUtils::validationPassWord(user_id, password, query))
	{

	}
	QJsonObject userObj;
	userObj["user_id"] = user_id;
	// 登录成功，生成 JWT 令牌
	try {
		// 生成 JWT 令牌
		auto token = jwt::create<jwt::traits::kazuho_picojson>()  // 使用 kazuho-picojson traits
			.set_issuer("auth0")
			.set_type("JWS")
			.set_payload_claim("user_id", jwt::claim(std::string(user_id.toStdString()))) // 将用户名作为 claim
			.sign(jwt::algorithm::hs256{ "xu-server" });

		std::cout << "=== JWT Token ===" << std::endl;
		std::cout << token << std::endl << std::endl;

		userObj["token"] = QString::fromStdString(token);
		// 创建响应数据，包含 token
		QJsonObject allData;
		allData["type"] = "loginValidationSuccess";
		allData["params"] = userObj;
		// 返回响应给客户端
		QJsonDocument responseDoc(allData);
		responder.write(responseDoc);

	}
	catch (const std::exception& e) {
		// 如果生成 token 时发生异常，暂时不处理
	}

}
//登录
void LoginHandle::handle_login(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "--------------------------登录用户数据的加载------------------------";
	auto user_id = paramsObject["user_id"].toString();
	auto password = paramsObject["password"].toString();
	//数据库查询
	DataBaseQuery query;
	auto loginUserObj = UserDBUtils::queryUserDetail(user_id, query);
	qDebug() << "登录用户信息:" << loginUserObj;
	if (loginUserObj.contains("error"))
	{

	}
	auto imageData = ImageUtils::loadImage(user_id, ChatType::User);
	auto packet = PacketCreate::binaryPacket("loginSuccess", loginUserObj.toVariantMap(), imageData);
	QByteArray loginUserData;
	PacketCreate::addPacket(loginUserData, packet);
	auto allData = PacketCreate::allBinaryPacket(loginUserData);
	ConnectionManager::instance()->sendBinaryMessage(user_id, allData);
}
//加载好友列表
void LoginHandle::handle_loadFriendList(const QJsonObject& paramsObj, const QByteArray& data, QHttpServerResponder& responder)
{
	qDebug() << "--------------------------加载好友列表------------------------";
	auto user_id = paramsObj["user_id"].toString();
	//数据库查询
	DataBaseQuery query;
	qDebug() << "handle_loadFriendList" << user_id;
	auto friendListObj = LoginDBUtils::queryFriendList(user_id, query);
	if (friendListObj.contains("error"))
	{

	}
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
	auto groupListObj = LoginDBUtils::queryGroupList(user_id, query);
	if (groupListObj.contains("error"))
	{

	}
	QJsonObject allDataObj;
	allDataObj["params"] = groupListObj;
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
	auto groupMemberObj = LoginDBUtils::queryGroupMemberList(group_id, query);
	if (groupMemberObj.contains("error"))
	{

	}
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
//加载好友头像
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


#include "MessageHandle.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QIODevice>
#include <QWebSocket>

#include "ImageUtil.h"
#include "CreateId.h"
#include "PacketCreate.h"
#include "DataBaseQuery.h"
#include "jwt-cpp/jwt.h"
#include "Server-MessageHandle/LoginHandle.h"
#include "Server-MessageHandle/FriendHandle.h"
#include "Server-MessageHandle/GroupHandle.h"
#include "Server-MessageHandle/RegisterHandle.h"
#include "Server-MessageHandle/UserHandle.h"

MessageHandle::MessageHandle(QObject* parent)
	:QObject(parent)
{
	initWebRequestHash();
	initHttpRequestHash();
}
//映射表
void MessageHandle::initWebRequestHash()
{
	//登录
	webRequestHash["login"] = &LoginHandle::handle_login;
	//好友处理
	webRequestHash["textCommunication"] = &FriendHandle::handle_textCommunication;
	webRequestHash["pictureCommunication"] = &FriendHandle::handle_pictureCommunication;
	webRequestHash["addFriend"] = &FriendHandle::handle_addFriend;
	webRequestHash["friendAddSuccess"] = &FriendHandle::handle_friendAddSuccess;
	webRequestHash["friendAddFail"] = &FriendHandle::handle_friendAddFail;
	//群组处理
	webRequestHash["createGroup"] = &GroupHandle::handle_createGroup;
	webRequestHash["addGroup"] = &GroupHandle::handle_addGroup;
	webRequestHash["groupTextCommunication"] = &GroupHandle::handle_groupTextCommunication;
	webRequestHash["groupPictureCommunication"] = &GroupHandle::handle_groupPictureCommunication;
	webRequestHash["groupInviteSuccess"] = &GroupHandle::handle_groupInviteSuccess;
	webRequestHash["groupAddSuccess"] = &GroupHandle::handle_groupAddSuccess;
	webRequestHash["groupInviteFailed"] = &GroupHandle::handle_groupInviteFailed;
	webRequestHash["groupAddFailed"] = &GroupHandle::handle_groupAddFailed;
	webRequestHash["groupInvite"] = &GroupHandle::handle_groupInvite;
	webRequestHash["groupMemberRemove"] = &GroupHandle::handle_groupMemberRemove;
}
void MessageHandle::initHttpRequestHash()
{
	//登录注册
	httpRequestHash["loginValidation"] = &LoginHandle::handle_loginValidation;
	httpRequestHash["loadFriendList"] = &LoginHandle::handle_loadFriendList;
	httpRequestHash["loadFriendAvatars"] = &LoginHandle::handle_loadFriendAvatars;
	httpRequestHash["loadGroupList"] = &LoginHandle::handle_loadGroupList;
	httpRequestHash["loadGroupAvatars"] = &LoginHandle::handle_loadGroupAvatars;
	httpRequestHash["loadGroupMember"] = &LoginHandle::handle_loadGroupMember;
	httpRequestHash["loadGroupMemberAvatar"] = &LoginHandle::handle_loadGroupMemberAvatar;
	httpRequestHash["register"] = &RegisterHandle::handle_register;
	//搜索
	httpRequestHash["serachUser"] = &UserHandle::handle_searchUser;
	httpRequestHash["searchGroup"] = &UserHandle::handle_searchGroup;
	httpRequestHash["queryUser"] = &UserHandle::handle_queryUser;
	//更新
	httpRequestHash["updateFriendGrouping"] = &FriendHandle::handle_updateFriendGrouping;
	httpRequestHash["updateUserMessage"] = &UserHandle::handle_updateUserMessage;
	httpRequestHash["updateUserAvatar"] = &UserHandle::handle_updateUserAvatar;
	httpRequestHash["deleteFriend"] = &FriendHandle::handle_deleteFriend;
	//群组
	httpRequestHash["groupMemberLoad"] = &GroupHandle::handle_groupMemberLoad;
	httpRequestHash["exitGroup"] = &GroupHandle::handle_exitGroup;
	httpRequestHash["disbandGroup"] = &GroupHandle::handle_disbandGroup;
}

//token验证
bool MessageHandle::tokenRight(const QString& token, const QString& user_id)
{
	try {
		auto decoded = jwt::decode(token.toStdString());

		// 校验签名
		auto verifier = jwt::verify()
			.allow_algorithm(jwt::algorithm::hs256{ "xu-server" })
			.with_issuer("auth0");

		verifier.verify(decoded);

		// 检查 payload 中的 user_id
		std::string payload_user_id = decoded.get_payload_claim("user_id").as_string();
		if (payload_user_id != user_id.toStdString()) {
			qDebug() << "[Token无效] user_id不匹配";
			return false;
		}

		qDebug() << "[Token验证成功]";
		return true;
	}
	catch (const std::exception& e) {
		qDebug() << "[Token验证失败]" << e.what();
		return false;
	}
}

//web消息处理接口
void MessageHandle::handle_message(const QString& message, QWebSocket* socket)
{
	QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
	if (doc.isObject())
	{
		QJsonObject obj = doc.object();
		QString client_id;
		auto type = obj["type"].toString();
		auto paramsObject = obj["params"].toObject();
		client_id = paramsObject["user_id"].toString();
		if (!client_id.isEmpty())
		{
			auto token = paramsObject["token"].toString();
			qDebug() << "token" << token;
			if (!tokenRight(token, client_id))
			{
				qDebug() << "token认证失败";
				return;
			}
			emit this->addClient(client_id, socket);
		}
		qDebug() << "客户端发来消息:" << client_id << "type:" << type;
		//根据类型给处理函数处理
		if (webRequestHash.contains(type))
		{
			auto handle = webRequestHash[type];
			handle(paramsObject, QByteArray());
		}
		else
		{
			qDebug() << "未知的类型:" << type;
		}
	}
}
void MessageHandle::handle_message(const QByteArray& message, QWebSocket* socket)
{
	auto parsePacketList = PacketCreate::parseDataPackets(message);
	for (auto& parsePacket : parsePacketList)
	{
		qDebug() << "客户端发来消息type:" << parsePacket.type;
		//验证token
		auto token = parsePacket.params["token"].toString();
		auto user_id = parsePacket.params["user_id"].toString();
		if (!tokenRight(token, user_id))
		{
			qDebug() << "token认证失败";
			return;
		}
		// 根据类型给处理函数处理
		if (webRequestHash.contains(parsePacket.type)) {
			auto handle = webRequestHash[parsePacket.type];
			handle(parsePacket.params, parsePacket.data);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << parsePacket.type;
		}
	}
}

//http消息处理接口
void MessageHandle::handle_message(const QString& type, const QHttpServerRequest& request, QHttpServerResponder& response)
{
	//token验证
	if (type != "loginValidation")
	{
		QString token;
		QString user_id;
		for (auto& requestHeader : request.headers())
		{
			if (requestHeader.first == "user_id")
			{
				user_id = requestHeader.second;
			}
			if (requestHeader.first == "Authorization")
			{
				token = requestHeader.second;
			}
		}
		// 可能是 Bearer 开头，记得处理
		if (token.startsWith("Bearer "))
			token = token.mid(7);
		if (!tokenRight(token, user_id))
		{
			qDebug() << "token认证失败";
			return;
		}
	}

	auto requestBody = request.body();
	auto paramsObj = QJsonDocument::fromJson(requestBody).object();
	// 根据类型给处理函数处理
	if (httpRequestHash.contains(type)) {
		auto handle = httpRequestHash[type];
		handle(paramsObj, QByteArray(), response);  // 调用对应的处理函数
	}
	else {
		qDebug() << "未知的类型:" << type;
	}
}
void MessageHandle::handle_message(const QHttpServerRequest& request, QHttpServerResponder& responder)
{
	//token验证
	QString token;
	QString user_id;
	for (auto& requestHeader : request.headers())
	{
		if (requestHeader.first == "user_id")
		{
			user_id = requestHeader.second;
		}
		if (requestHeader.first == "Authorization")
		{
			token = requestHeader.second;
		}
	}
	// 可能是 Bearer 开头，记得处理
	if (token.startsWith("Bearer "))
		token = token.mid(7);
	if (!tokenRight(token, user_id))
	{
		qDebug() << "token认证失败";
		return;
	}
	auto message = request.body();
	auto parsePacketList = PacketCreate::parseDataPackets(message);
	for (auto& parsePacket : parsePacketList)
	{
		qDebug() << "客户端发来消息type:" << parsePacket.type;
		// 根据类型给处理函数处理
		if (httpRequestHash.contains(parsePacket.type)) {
			auto handle = httpRequestHash[parsePacket.type];
			handle(parsePacket.params, parsePacket.data, responder);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << parsePacket.type;
		}
	}
}


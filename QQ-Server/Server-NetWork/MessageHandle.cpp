#include "MessageHandle.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QIODevice>
#include <QWebSocket>
#include <QCoreApplication>

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
	initPublicPageType();
}
//映射表
void MessageHandle::initWebRequestHash()
{
	//登录
	m_webRequestHash["login"] = &LoginHandle::handle_login;
	//好友处理
	m_webRequestHash["textCommunication"] = &FriendHandle::handle_textCommunication;
	m_webRequestHash["pictureCommunication"] = &FriendHandle::handle_pictureCommunication;
	m_webRequestHash["addFriend"] = &FriendHandle::handle_addFriend;
	m_webRequestHash["friendAddSuccess"] = &FriendHandle::handle_friendAddSuccess;
	m_webRequestHash["friendAddFail"] = &FriendHandle::handle_friendAddFail;
	//群组处理
	m_webRequestHash["createGroup"] = &GroupHandle::handle_createGroup;
	m_webRequestHash["addGroup"] = &GroupHandle::handle_addGroup;
	m_webRequestHash["groupTextCommunication"] = &GroupHandle::handle_groupTextCommunication;
	m_webRequestHash["groupPictureCommunication"] = &GroupHandle::handle_groupPictureCommunication;
	m_webRequestHash["groupInviteSuccess"] = &GroupHandle::handle_groupInviteSuccess;
	m_webRequestHash["groupAddSuccess"] = &GroupHandle::handle_groupAddSuccess;
	m_webRequestHash["groupInviteFailed"] = &GroupHandle::handle_groupInviteFailed;
	m_webRequestHash["groupAddFailed"] = &GroupHandle::handle_groupAddFailed;
	m_webRequestHash["groupInvite"] = &GroupHandle::handle_groupInvite;
	m_webRequestHash["groupMemberRemove"] = &GroupHandle::handle_groupMemberRemove;
}
void MessageHandle::initHttpRequestHash()
{
	//登录注册
	m_httpRequestHash["/loginValidation"] = &LoginHandle::handle_loginValidation;
	m_httpRequestHash["/loadFriendList"] = &LoginHandle::handle_loadFriendList;
	m_httpRequestHash["/loadFriendAvatars"] = &LoginHandle::handle_loadFriendAvatars;
	m_httpRequestHash["/loadGroupList"] = &LoginHandle::handle_loadGroupList;
	m_httpRequestHash["/loadGroupAvatars"] = &LoginHandle::handle_loadGroupAvatars;
	m_httpRequestHash["/loadGroupMember"] = &LoginHandle::handle_loadGroupMember;
	m_httpRequestHash["/loadGroupMemberAvatar"] = &LoginHandle::handle_loadGroupMemberAvatar;
	m_httpRequestHash["/register"] = &RegisterHandle::handle_register;
	m_httpRequestHash["/passwordChange"] = &UserHandle::handle_passwordChange;
	//搜索		   /
	m_httpRequestHash["/searchUser"] = &UserHandle::handle_searchUser;
	m_httpRequestHash["/searchGroup"] = &UserHandle::handle_searchGroup;
	m_httpRequestHash["/queryUser"] = &UserHandle::handle_queryUser;
	//更新		   /
	m_httpRequestHash["/updateFriendGrouping"] = &FriendHandle::handle_updateFriendGrouping;
	m_httpRequestHash["/updateUserMessage"] = &UserHandle::handle_updateUserMessage;
	m_httpRequestHash["/updateUserAvatar"] = &UserHandle::handle_updateUserAvatar;
	m_httpRequestHash["/deleteFriend"] = &FriendHandle::handle_deleteFriend;
	//群组		   /
	m_httpRequestHash["/groupMemberLoad"] = &GroupHandle::handle_groupMemberLoad;
	m_httpRequestHash["/exitGroup"] = &GroupHandle::handle_exitGroup;
	m_httpRequestHash["/disbandGroup"] = &GroupHandle::handle_disbandGroup;
	m_httpRequestHash["/updateGroupAvatar"] = &GroupHandle::handle_updateGroupAvatar;

}

void MessageHandle::initPublicPageType()
{
	m_publicPageList.append("/loginValidation");
}

//token验证
bool MessageHandle::webTokenVerify(const QString& token, const QString& user_id, const QString& type)
{
	if (m_publicPageList.contains(type))
	{
		return true;
	}

	try {
		auto decoded = jwt::decode(token.toStdString());

		// 校验签名
		auto verifier = jwt::verify()
			.allow_algorithm(jwt::algorithm::hs256{ "xu-server" })
			.with_issuer("auth0");

		verifier.verify(decoded);

		// 检查 payload 中的 user_id
		std::string payload_user_id = decoded.get_payload_claim("user_id").as_string();
		if (payload_user_id != user_id.toStdString())
		{
			qDebug() << "[Token无效] user_id不匹配";
			return false;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		qDebug() << "[Token验证失败]" << e.what();
		return false;
	}
}
bool MessageHandle::httpToeknVerify(const QString& type, const QVariantMap& requestHttpData)
{

	//公共界面访问无需验证
	if (m_publicPageList.contains(type))
	{
		return true;
	}
	QString token;
	QString user_id;
	if (requestHttpData.contains("headers"))
	{
		QVariantMap headers = requestHttpData["headers"].toMap();

		if (headers.contains("user_id"))
		{
			QVariant userIdVar = headers["user_id"];
			user_id = userIdVar.type() == QVariant::ByteArray
				? QString::fromUtf8(userIdVar.toByteArray())
				: userIdVar.toString();
		}

		// 优先从 Authorization 头获取 token
		if (headers.contains("Authorization"))
		{
			QVariant authVar = headers["Authorization"];
			token = authVar.type() == QVariant::ByteArray
				? QString::fromUtf8(authVar.toByteArray())
				: authVar.toString();
		}
		// 回退到 token 字段
		else if (headers.contains("token"))
		{
			QVariant tokenVar = headers["token"];
			token = tokenVar.type() == QVariant::ByteArray
				? QString::fromUtf8(tokenVar.toByteArray())
				: tokenVar.toString();
		}
	}

	if (token.isEmpty()) {
		qDebug() << "[Token验证失败] Token为空";
		return false;
	}
	if (token.startsWith("Bearer "))
	{
		token = token.mid(7);
	}

	//验证
	try {
		auto decoded = jwt::decode(token.toStdString());

		// 校验签名
		auto verifier = jwt::verify()
			.allow_algorithm(jwt::algorithm::hs256{ "xu-server" })
			.with_issuer("auth0");

		verifier.verify(decoded);

		// 检查 payload 中的 user_id
		std::string payload_user_id = decoded.get_payload_claim("user_id").as_string();
		if (payload_user_id != user_id.toStdString())
		{
			qDebug() << "[Token无效] user_id不匹配";
			return false;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		qDebug() << "[Token验证失败]" << e.what();
		return false;
	}
}

//web消息处理
void MessageHandle::webTextHandler(const QString& message, QWebSocket* socket)
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
			if (!webTokenVerify(token, client_id, type))
			{
				qDebug() << "token认证失败";
				return;
			}
			emit this->addClient(client_id, socket);
		}
		qDebug() << "客户端发来消息:" << client_id << "type:" << type;
		//根据类型给处理函数处理
		if (m_webRequestHash.contains(type))
		{
			auto handle = m_webRequestHash[type];
			handle(paramsObject, QByteArray());
		}
		else
		{
			qDebug() << "未知的类型:" << type;
		}
	}
}
void MessageHandle::webBinaryHandler(const QByteArray& message, QWebSocket* socket)
{
	auto parsePacketList = PacketCreate::parseDataPackets(message);
	for (auto& parsePacket : parsePacketList)
	{
		qDebug() << "客户端发来消息type:" << parsePacket.type;
		//验证token
		auto token = parsePacket.params["token"].toString();
		auto user_id = parsePacket.params["user_id"].toString();
		if (!webTokenVerify(token, user_id, parsePacket.type))
		{
			qDebug() << "token认证失败";
			return;
		}
		// 根据类型给处理函数处理
		if (m_webRequestHash.contains(parsePacket.type)) {
			auto handle = m_webRequestHash[parsePacket.type];
			handle(parsePacket.params, parsePacket.data);  // 调用对应的处理函数
		}
		else {
			qDebug() << "未知的类型:" << parsePacket.type;
		}
	}
}

//http消息处理
void MessageHandle::httpGetHandler(const QVariantMap& requestHttpData, QHttpServerResponder& response)
{
	QUrl url = requestHttpData["url"].toUrl();
	QString type = url.path();

	//token
	if (!httpToeknVerify(type, requestHttpData))
	{
		qDebug() << "token验证失败";
		return;
	}

	QUrlQuery query(url);
	QJsonObject paramsObj;
	for (const auto& item : query.queryItems())
	{
		paramsObj[item.first] = item.second;
	}
	//调用对应的处理函数
	if (m_httpRequestHash.contains(type))
	{
		auto handle = m_httpRequestHash[type];
		handle(paramsObj, QByteArray(), response);
	}
	else
	{
		qDebug() << "未知的类型:" << type;
	}

}
void MessageHandle::httpPostTextHandler(const QVariantMap& requestHttpData, QHttpServerResponder& response)
{
	QUrl url = requestHttpData["url"].toUrl();
	QString type = url.path();

	//token
	if (!httpToeknVerify(type, requestHttpData))
	{
		qDebug() << "Token 验证失败";
		return;
	}

	QByteArray requestBody = requestHttpData["body"].toByteArray();
	QJsonObject paramsObj = QJsonDocument::fromJson(requestBody).object();

	//调用对应的处理函数
	if (m_httpRequestHash.contains(type))
	{
		auto handle = m_httpRequestHash[type];
		handle(paramsObj, QByteArray(), response);
	}
	else
	{
		qDebug() << "未知的类型:" << type;
	}


}
void MessageHandle::httpPostBinaryHandler(const QVariantMap& requestHttpData, QHttpServerResponder& response)
{
	QUrl url = requestHttpData["url"].toUrl();
	QString type = url.path();

	//token
	if (!httpToeknVerify(type, requestHttpData))
	{
		qDebug() << "Token 验证失败";
		return;
	}

	QByteArray message = requestHttpData["body"].toByteArray();

	//调用对应的处理函数
	auto parsePacketList = PacketCreate::parseDataPackets(message);
	for (auto& parsePacket : parsePacketList)
	{

		if (m_httpRequestHash.contains(type))
		{
			auto handle = m_httpRequestHash[type];
			handle(parsePacket.params, parsePacket.data, response);
		}
		else
		{
			qDebug() << "未知的类型:" << type;
		}

	}
}

//设置消息来源
void MessageHandle::setServerMessageSrc(MessageQueue* messageQueue)
{
	m_messageQueue = messageQueue;
	startServerConsumerThread();
}

//启动线程
void MessageHandle::startServerConsumerThread()
{
	if (m_running) return;

	m_running = true;
	// 将消费者对象移到新线程
	this->moveToThread(&m_consumerThread);
	// 连接线程启动信号
	connect(&m_consumerThread, &QThread::started, this, &MessageHandle::serverConsumer);
	m_consumerThread.start();
}

//停止线程
void MessageHandle::stopServerConsumerThread()
{
	m_running = false;
	m_consumerThread.quit();
	m_consumerThread.wait();
}

//消息处理
void MessageHandle::serverConsumer()
{
	while (m_running)
	{
		if (!m_messageQueue)
		{
			QThread::msleep(10);
			continue;
		}

		auto msg = m_messageQueue->pop(100);
		if (msg.data.isNull())
		{
			continue;
		}

		switch (msg.type)
		{
		case MessageQueue::WS_Text:
		{
			QString text = msg.data.toString();
			QWebSocket* socket = msg.sender.value<QWebSocket*>();
			QMetaObject::invokeMethod(QCoreApplication::instance(),
				[this, text, socket]()
				{
					webTextHandler(text, socket);
				});
		}
		break;
		case MessageQueue::WS_Binary:
		{
			QByteArray data = msg.data.toByteArray();
			QWebSocket* socket = msg.sender.value<QWebSocket*>();
			QMetaObject::invokeMethod(QCoreApplication::instance(),
				[this, data, socket]()
				{
					webBinaryHandler(data, socket);
				});
		}
		break;
		case MessageQueue::Http_Get:
		{
			QVariantMap httpData = msg.data.toMap();
			auto responder_ptr = msg.sender.value<QSharedPointer<QHttpServerResponder>>();
			QMetaObject::invokeMethod(QCoreApplication::instance(),
				[this, responder_ptr, httpData]()
				{
					httpGetHandler(httpData, *responder_ptr);
				});
		}
		break;
		case MessageQueue::Http_PostText:
		{
			QVariantMap httpData = msg.data.toMap();
			auto responder_ptr = msg.sender.value<QSharedPointer<QHttpServerResponder>>();
			QMetaObject::invokeMethod(QCoreApplication::instance(),
				[this, responder_ptr, httpData]()
				{
					httpPostTextHandler(httpData, *responder_ptr);
				});
		}
		break;
		case MessageQueue::Http_PostBinary:
		{
			QVariantMap httpData = msg.data.toMap();
			auto responder_ptr = msg.sender.value<QSharedPointer<QHttpServerResponder>>();
			QMetaObject::invokeMethod(QCoreApplication::instance(),
				[this, responder_ptr, httpData]()
				{
					httpPostBinaryHandler(httpData, *responder_ptr);
				});
		}
		break;
		default:
			break;
		}
	}
}


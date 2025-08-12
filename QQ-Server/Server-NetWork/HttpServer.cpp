#include "HttpServer.h"
#include <QDebug>
#include <QCoreApplication>

HttpServer::HttpServer(QObject* parent)
	: QObject(parent)
	, m_httpServer(new QHttpServer(this))
{
	setupRoutes();
	m_httpServer->listen(QHostAddress::Any, 8889);
}

void HttpServer::setupRoutes()
{
	registerHttpRoute("/loginValidation");
	registerHttpRoute("/loadFriendList");
	registerHttpRoute("/loadFriendAvatars");
	registerHttpRoute("/loadGroupList");
	registerHttpRoute("/loadGroupMember");
	registerHttpRoute("/groupMemberLoad");
	registerHttpRoute("/loadGroupMemberAvatar");
	registerHttpRoute("/loadGroupAvatars");
	registerHttpRoute("/register");
	registerHttpRoute("/searchUser");
	registerHttpRoute("/searchGroup");
	registerHttpRoute("/updateFriendGrouping");
	registerHttpRoute("/updateUserMessage");
	registerHttpRoute("/updateUserAvatar");
	registerHttpRoute("/updateGroupAvatar");
	registerHttpRoute("/deleteFriend");
	registerHttpRoute("/exitGroup");
	registerHttpRoute("/disbandGroup");
	registerHttpRoute("/queryUser");
	registerHttpRoute("/passwordChange");
}


void HttpServer::registerHttpRoute(const QString& path)
{
	//http请求路由并注册
	m_httpServer->route(path, [this](const QHttpServerRequest& request, QHttpServerResponder&& response)
		{
			auto httpMethod = request.method();
			switch (httpMethod)
			{
			case QHttpServerRequest::Method::Get:
			{
				emit httpGetReceive(request, response);
			}
			break;
			case QHttpServerRequest::Method::Post:
			{
				auto content_type = request.value("Content-Type");
				if (content_type.contains("application/json"))
				{
					emit httpPostTextReceive(request, response);
				}
				else if (content_type.contains("application/octet-stream"))
				{
					emit httpPostBinaryReceive(request, response);
				}
			}
			break;
			default:
				break;
			}

		});
}

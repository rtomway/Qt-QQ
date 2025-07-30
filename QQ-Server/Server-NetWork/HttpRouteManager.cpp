#include "HttpRouteManager.h"
#include <QDebug>

HttpRouteManager::HttpRouteManager(QHttpServer* server, QObject* parent)
	: QObject(parent)
{
	m_httpServer = server;
	setupRoutes();
}

void HttpRouteManager::setupRoutes()
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

void HttpRouteManager::registerHttpRoute(const QString& path)
{
	//http请求路由并注册
	m_httpServer->route(path, [this](const QHttpServerRequest& request, QHttpServerResponder&& response)
		{
			auto httpMethod = request.method();
			switch (httpMethod)
			{
			case QHttpServerRequest::Method::Get:
			{
				m_messageHandle.httpGetHandler(request, response);
			}
			break;
			case QHttpServerRequest::Method::Post:
			{
				auto content_type = request.value("Content-Type");
				if (content_type.contains("application/json"))
				{
					m_messageHandle.httpPostTextHandler(request, response);
				}
				else if (content_type.contains("application/octet-stream"))
				{
					m_messageHandle.httpPostBinaryHandler(request, response);
				}
			}
			break;
			default:
				break;
			}

		});
}

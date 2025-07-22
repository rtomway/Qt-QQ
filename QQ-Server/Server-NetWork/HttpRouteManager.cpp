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
	QString type = path;
	if (type.startsWith('/'))
		type.remove(0, 1);
	//http请求路由并注册
	m_httpServer->route(path, [this, type](const QHttpServerRequest& req, QHttpServerResponder&& resp) {
		qDebug() << "------------------------接收http请求-----------------------";
		qDebug() << "[HTTP] 收到请求：" << type;

		auto content_type = req.value("Content-Type");
		if (content_type.contains("application/json"))
		{
			m_messageHandle.handle_message(type, req, resp);
		}
		else if (content_type.contains("application/octet-stream"))
		{
			m_messageHandle.handle_message(req, resp);
		}
		});
}

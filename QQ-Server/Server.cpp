#include "Server.h"
#include "Server-Core/ConnectionManager.h"
#include <QJsonDocument>

Server::Server(QObject* parent)
	:QObject(parent)
{
	//设定web服务器监听端口
	if (!m_webSocketServer)
	{
		m_webSocketServer = new QWebSocketServer("QQ-Server", QWebSocketServer::SslMode::NonSecureMode, this);
		connect(m_webSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
	}
	m_webSocketServer->listen(QHostAddress::Any, 8888);
	//http服务器
	if (!m_httpServer)
	{
		m_httpServer = new QHttpServer(this);
	}
	m_httpServer->listen(QHostAddress::Any, 8889);
	this->httpRoute();
}

Server::~Server()
{
	m_webSocketServer->close();
}

//通信连接
void Server::onNewConnection()
{
	while (m_webSocketServer->hasPendingConnections())
	{
		//请求连接的客户端
		auto client = m_webSocketServer->nextPendingConnection();
		ConnectionManager::instance()->onNewConnection(client);
	}
}

void Server::httpRoute()
{
	m_httpServer->route("/loginValidation", [this](const QHttpServerRequest& request, QHttpServerResponder&& response) {
		qDebug() << "接收http:loginValidation请求";
		messageHandle.handle_message("loginValidation", request, response);
		});
	m_httpServer->route("/register", [this](const QHttpServerRequest& request, QHttpServerResponder&& response) {
		qDebug() << "接收http:register请求";
		messageHandle.handle_message("register", request, response);
		});
	m_httpServer->route("/serachUser", [this](const QHttpServerRequest& request, QHttpServerResponder&& response) {
		qDebug() << "接收http:serachUser请求";
		messageHandle.handle_message("serachUser", request, response);
		});
	m_httpServer->route("/updateFriendGrouping", [this](const QHttpServerRequest& request, QHttpServerResponder&& response) {
		qDebug() << "接收http:updateFriendGrouping请求";
		messageHandle.handle_message("updateFriendGrouping", request, response);
		});
	m_httpServer->route("/updateUserMessage", [this](const QHttpServerRequest& request, QHttpServerResponder&& response) {
		qDebug() << "接收http:updateUserMessage请求";
		messageHandle.handle_message("updateUserMessage", request, response);
		});
	m_httpServer->route("/updateUserAvatar", [this](const QHttpServerRequest& request, QHttpServerResponder&& response) {
		qDebug() << "接收http:updateUserAvatar请求";
		messageHandle.handle_message(request, response);
		});
	m_httpServer->route("/deleteFriend", [this](const QHttpServerRequest& request, QHttpServerResponder&& response) {
		qDebug() << "接收http:deleteFriend请求";
		messageHandle.handle_message("deleteFriend",request, response);
		});
}




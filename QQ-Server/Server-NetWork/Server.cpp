#include "Server.h"
#include <QJsonDocument>

#include "ConnectionManager.h"

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
	m_httpRouteManager = new HttpRouteManager(m_httpServer);
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





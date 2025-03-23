#include "Server.h"

#include "Server-Core/ConnectionManager.h"

Server::Server(QObject* parent)
	:QObject(parent)
{
	//设定服务器监听端口
	if (!m_server)
	{
		m_server = new QWebSocketServer("QQ-Server", QWebSocketServer::SslMode::NonSecureMode, this);
		connect(m_server, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
	}
	m_server->listen(QHostAddress::Any, 8888);
}

Server::~Server()
{
	m_server->close();
}

//通信连接
void Server::onNewConnection()
{
	while (m_server->hasPendingConnections())
	{
		//请求连接的客户端
		auto client = m_server->nextPendingConnection();
		ConnectionManager::instance()->onNewConnection(client);
	}
}




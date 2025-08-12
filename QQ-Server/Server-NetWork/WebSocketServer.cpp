#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(QObject* parent)
	:QObject(parent)
{
	//设定web服务器监听端口
	if (!m_webSocketServer)
	{
		m_webSocketServer = new QWebSocketServer("QQ-Server", QWebSocketServer::SslMode::NonSecureMode, this);
		connect(m_webSocketServer, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
	}
	m_webSocketServer->listen(QHostAddress::Any, 8888);
}

WebSocketServer::~WebSocketServer()
{

}

void WebSocketServer::onNewConnection()
{
	while (m_webSocketServer->hasPendingConnections())
	{
		//请求连接的客户端
		auto client = m_webSocketServer->nextPendingConnection();

		connect(client, &QWebSocket::disconnected, this, &WebSocketServer::onDisConnection);
		connect(client, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);
		connect(client, &QWebSocket::binaryMessageReceived, this, &WebSocketServer::onBinaryMessageReceived);
	}
}

void WebSocketServer::onTextMessageReceived(const QString& text)
{
	auto socket = dynamic_cast<QWebSocket*>(sender());
	emit textReceive(socket, text);
}

void WebSocketServer::onBinaryMessageReceived(const QByteArray& data)
{
	auto socket = dynamic_cast<QWebSocket*>(sender());
	emit binaryReceive(socket, data);
}

void WebSocketServer::onDisConnection()
{
	auto client = dynamic_cast<QWebSocket*>(sender());
	if (!client)
		return;
	auto user_id = findUserName(client);
	if (user_id.isEmpty())
	{

	}
	m_clients.remove(user_id);
	qDebug() << "客户端" << user_id << "断开";
}

// 客户端id
QString WebSocketServer::findUserName(QWebSocket* client)
{
	for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if (it.value() == client)
		{
			return it.key();
		}
	}
	return QString();
}

//消息的发送
void WebSocketServer::sendTextMessage(const QString& user_id, const QString& response)
{
	if (!m_clients.contains(user_id))
		return;
	m_clients[user_id]->sendTextMessage(response);

}
void WebSocketServer::sendBinaryMessage(const QString& user_id, const QByteArray& response)
{
	if (!m_clients.contains(user_id))
		return;
	m_clients[user_id]->sendBinaryMessage(response);
}

//客户端添加
void WebSocketServer::onAddClient(const QString& user_id, QWebSocket* socket)
{
	if (!m_clients.contains(user_id))
	{
		m_clients[user_id] = socket;
		qDebug() << "客户端" << user_id << "加入连接表";
	}
}

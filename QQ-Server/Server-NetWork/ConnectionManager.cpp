#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(QObject* parent)
	:QObject(parent)
{
	//客户端的连接
	connect(&m_messageHandle, &MessageHandle::addClient, this, &ConnectionManager::addClient);
}

ConnectionManager* ConnectionManager::instance()
{
	static ConnectionManager ins;
	return &ins;
}

//建立连接
void ConnectionManager::onNewConnection(QWebSocket* client)
{
	connect(client, &QWebSocket::disconnected, this, &ConnectionManager::onDisConnection);
	connect(client, &QWebSocket::textMessageReceived, this, &ConnectionManager::onTextMessageReceived);
	connect(client, &QWebSocket::binaryMessageReceived, this, &ConnectionManager::onBinaryMessageReceived);
}

//消息的接收
void ConnectionManager::onTextMessageReceived(const QString& message)
{
	auto socket = dynamic_cast<QWebSocket*>(sender());
	m_messageHandle.handle_message(message, socket);
}
void ConnectionManager::onBinaryMessageReceived(const QByteArray& message)
{
	auto socket = dynamic_cast<QWebSocket*>(sender());
	m_messageHandle.handle_message(message, socket);
}

//客户端断连
void ConnectionManager::onDisConnection()
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

//客户端id
QString ConnectionManager::findUserName(QWebSocket* client)
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
void ConnectionManager::sendTextMessage(const QString& user_id, const QString& response)
{
	if (!m_clients.contains(user_id))
		return;
	m_clients[user_id]->sendTextMessage(response);

}
void ConnectionManager::sendBinaryMessage(const QString& user_id, const QByteArray& response)
{
	if (!m_clients.contains(user_id))
		return;
	m_clients[user_id]->sendBinaryMessage(response);
}

//客户端添加
void ConnectionManager::addClient(const QString& user_id, QWebSocket* socket)
{
	qDebug() << "客户端" << user_id << "加入连接表";
	m_clients[user_id] = socket;
}

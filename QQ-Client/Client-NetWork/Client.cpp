#include "Client.h"
#include "Client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <Qpixmap>

Client::Client(QObject* parent)
	:QObject(parent)
	, m_client(new QWebSocket())
	, m_isConnected(false)
{
	//客户端接受信号
	connect(m_client, &QWebSocket::textMessageReceived, this, &Client::onTextMessageReceived);
	connect(m_client, &QWebSocket::binaryMessageReceived, this, &Client::onBinaryMessageReceived);
	connect(m_client, &QWebSocket::errorOccurred, this, &Client::onErrorOccurred);
	connect(m_client, &QWebSocket::connected, this, &Client::onConnected);
	connect(m_client, &QWebSocket::disconnected, this, &Client::onDisconnected);
}

Client::~Client()
{
	delete m_client;
}
//连接服务端
Client* Client::connectToServer(const QString& url)
{
	if (!m_isConnected)
	{
		m_client->open(url);
		m_isConnected = true;
	}
	return this;
}
//回调
Client* Client::ReciveMessage(std::function<void(const QString&)> callback)
{
	m_messageCallback = callback;
	return this;
}
Client* Client::Error(std::function<void(const QString&)> callback)
{
	m_errorCallback = callback;
	return this;
}
Client* Client::Connected(std::function<void()> callback)
{
	m_connectedCallback = callback;
	return this;
}
Client* Client::DisconnectFromServer(std::function<void()> callback)
{
	m_disconnectedCallback = callback;
	return this;
}
//接受文本信息
void Client::onTextMessageReceived(const QString& message)
{
	m_messageHandle.handle_message(message);
}
//接受二进制数据
void Client::onBinaryMessageReceived(const QByteArray& message)
{
	m_messageHandle.handle_message(message);
}
//回调
void Client::onErrorOccurred(QAbstractSocket::SocketError error)
{
	if (m_errorCallback) {
		m_errorCallback(m_client->errorString());
	}
}
void Client::onConnected()
{
	if (m_connectedCallback)
	{
		m_connectedCallback();
	}
}
void Client::onDisconnected()
{
	m_isConnected = false;
	if (m_disconnectedCallback) {
		m_disconnectedCallback();
	}
}
void Client::disconnect()
{
	if (m_isConnected) {
		m_client->close();
		m_isConnected = false;
	}
}

bool Client::isConnected()const
{
	return m_isConnected;
}

QWebSocket* Client::getClientSocket() const
{
	return m_client;
}

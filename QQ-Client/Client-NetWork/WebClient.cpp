#include "WebClient.h"
#include <QJsonDocument>
#include <QJsonObject>

#include "../Client-ServiceLocator/NetWorkServiceLocator.h"
#include "SConfigFile.h"
#include "PacketCreate.h"

WebClient::WebClient(QObject* parent)
	:QObject(parent)
	, m_webSocket(new QWebSocket())
	, m_isConnected(false)
{
	//客户端接受信号
	connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebClient::onTextMessageReceived);
	connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &WebClient::onBinaryMessageReceived);
	connect(m_webSocket, &QWebSocket::errorOccurred, this, &WebClient::onErrorOccurred);
	connect(m_webSocket, &QWebSocket::connected, this, &WebClient::onConnected);
	connect(m_webSocket, &QWebSocket::disconnected, this, &WebClient::onDisconnected);
}

WebClient::~WebClient()
{
	delete m_webSocket;
}

//连接服务端
WebClient* WebClient::connectToServer(const QString& url)
{
	if (!m_isConnected)
	{
		m_webSocket->open(url);
		m_isConnected = true;
	}
	return this;
}
//回调
WebClient* WebClient::ReciveMessage(std::function<void(const QString&)> callback)
{
	m_messageCallback = callback;
	return this;
}
WebClient* WebClient::Error(std::function<void(const QString&)> callback)
{
	m_errorCallback = callback;
	return this;
}
WebClient* WebClient::Connected(std::function<void()> callback)
{
	m_connectedCallback = callback;
	return this;
}
WebClient* WebClient::DisconnectFromServer(std::function<void()> callback)
{
	m_disconnectedCallback = callback;
	return this;
}

//接受Web文本信息
void WebClient::onTextMessageReceived(const QString& message)
{
	qDebug() << "------------------------接受Web文本信息-----------------------";
	emit textMessage(message);
}

//接受Web二进制数据
void WebClient::onBinaryMessageReceived(const QByteArray& data)
{
	qDebug() << "------------------------接受Web二进制数据-----------------------";
	emit binaryData(data);
}

//回调
void WebClient::onErrorOccurred(QAbstractSocket::SocketError error)
{
	if (m_errorCallback) {
		m_errorCallback(m_webSocket->errorString());
	}
}
void WebClient::onConnected()
{
	if (m_connectedCallback)
	{
		m_connectedCallback();
	}
}
void WebClient::onDisconnected()
{
	m_isConnected = false;
	if (m_disconnectedCallback) {
		m_disconnectedCallback();
	}
}
void WebClient::disconnect()
{
	if (m_isConnected) {
		m_webSocket->close();
		m_isConnected = false;
	}
}

//连接状态
bool WebClient::isConnected()const
{
	return m_isConnected;
}

//文本消息发送
void WebClient::sendTextMessage(const QString& message)
{
	m_webSocket->sendTextMessage(message);
}

//二进制数据发送
void WebClient::sendBinaryData(const QByteArray& data)
{
	m_webSocket->sendBinaryMessage(data);
}


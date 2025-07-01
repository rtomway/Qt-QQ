#include "WebClient.h"
#include <QJsonDocument>
#include <QJsonObject>

#include "SConfigFile.h"
#include "PacketCreate.h"
#include "Network_logger.h"


WebClient::WebClient(QObject* parent)
	:WebClientPort(parent)
	, m_webSocket(new QWebSocket())
	, m_isConnected(false)
{
	//客户端接受信号
	connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebClient::onTextMessageReceived);
	connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &WebClient::onBinaryDataReceived);
	connect(m_webSocket, &QWebSocket::errorOccurred, this, &WebClient::onErrorOccurred);
	connect(m_webSocket, &QWebSocket::connected, this, &WebClient::onConnected);
	connect(m_webSocket, &QWebSocket::disconnected, this, &WebClient::onDisconnected);
}

WebClient::~WebClient()
{
	delete m_webSocket;
}

//连接服务端
void WebClient::connectToServer(const QString& url, std::function<void()>callback)
{
	if (!isConnected())
	{
		m_url = url;
		Network_Logger::info("[WebSocket] Attemp connected to Server at:" + this->url());
		this->Connected([callback]
			{
				if (callback)
					callback();
			});
		m_webSocket->open(url);
	}

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
	Network_Logger::info("[WebSocket] Successfully connected to server at: " + this->url());
	return this;
}
WebClient* WebClient::DisconnectFromServer(std::function<void()> callback)
{
	m_disconnectedCallback = callback;
	Network_Logger::info("[WebSocket] Disconnect from server at:" + this->url());
	return this;
}

//接受Web文本信息
void WebClient::onTextMessageReceived(const QString& message)
{
	Network_Logger::debug("[WebSocket] Accept Web Text message");
	emit textMessage(message);
}

//接受Web二进制数据
void WebClient::onBinaryDataReceived(const QByteArray& data)
{
	Network_Logger::debug("[WebSocket] Accept Web Binary data");
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
		m_isConnected = true;
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
	Network_Logger::debug("[WebSocket] Send web Text message: " + message + QString("to Server at : ") + this->url());
	m_webSocket->sendTextMessage(message);
}

//二进制数据发送
void WebClient::sendBinaryData(const QByteArray& data)
{
	Network_Logger::debug("[WebSocket] Send Web Binary data to Server at:" + this->url());
	m_webSocket->sendBinaryMessage(data);
}

const QString& WebClient::url()
{
	return m_url;
}


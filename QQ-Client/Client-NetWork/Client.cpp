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
	//消息处理信号转发
	connect(&m_messageHandle, &MessageHandle::loginSuccess, this, &Client::loginSuccess);
	connect(&m_messageHandle, &MessageHandle::communication, this, &Client::communication);
	connect(&m_messageHandle, &MessageHandle::addFriend, this, &Client::addFriend);
	connect(&m_messageHandle, &MessageHandle::newFriend, this, &Client::newFriend);
	connect(&m_messageHandle, &MessageHandle::addGroup, this, &Client::addGroup);
	connect(&m_messageHandle, &MessageHandle::rejectAddFriend, this, &Client::rejectAddFriend);
	connect(&m_messageHandle, &MessageHandle::searchUser, this, &Client::searchUser);
	connect(&m_messageHandle, &MessageHandle::updateUserMessage, this, &Client::updateUserMessage);
	connect(&m_messageHandle, &MessageHandle::updateUserAvatar, this, &Client::updateUserAvatar);
}

Client::~Client()
{
	delete m_client;
}
//单例
Client* Client::instance()
{
	static Client ins;
	return &ins;
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
//发送Json数据
Client* Client::sendMessage(const QString& type, const QVariantMap& params)
{
	if (m_isConnected)
	{
		QJsonObject jsonData;
		jsonData["type"] = type;
		//QVariantMap转为QJsonObject
		QJsonObject paramsObject;
		for (auto it = params.begin(); it != params.end(); it++)
		{
			//将不同类型数据转为Json
			paramsObject[it.key()] = QJsonValue::fromVariant(it.value());
		}
		jsonData["params"] = paramsObject;
		//发送Json数据
		QJsonDocument doc(jsonData);
		QString message = QString(doc.toJson(QJsonDocument::Compact));
		m_client->sendTextMessage(message);
	}
	else
	{
		qWarning() << "数据发送失败 连接已断开......";
	}
	return this;
}
//发送二进制数据
Client* Client::sendBinaryData(const QByteArray& data)
{
	m_client->sendBinaryMessage(data);
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
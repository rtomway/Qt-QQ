#include "Client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <Qpixmap>

#include "MessageSender.h"
#include "SConfigFile.h"

Client::Client(QObject* parent)
	:QObject(parent)
	, m_client(new QWebSocket())
	, m_isConnected(false)
	, m_workerThread(new QThread(this))
{
	//后台线程处理消息
	m_messageHandle.moveToThread(m_workerThread);
	m_workerThread->start();
	connect(this, &Client::textMessageToWorkerThread, &m_messageHandle, &MessageHandle::handle_textMessage);
	connect(this, &Client::binaryMessageToWorkerThread, &m_messageHandle, &MessageHandle::handle_binaryMessage);
	//客户端接受信号
	connect(m_client, &QWebSocket::textMessageReceived, this, &Client::onTextMessageReceived);
	connect(m_client, &QWebSocket::binaryMessageReceived, this, &Client::onBinaryMessageReceived);
	connect(m_client, &QWebSocket::errorOccurred, this, &Client::onErrorOccurred);
	connect(m_client, &QWebSocket::connected, this, &Client::onConnected);
	connect(m_client, &QWebSocket::disconnected, this, &Client::onDisconnected);
	//接收http回复
	connect(MessageSender::instance(), &MessageSender::httpTextResponseReceived, this, [=](const QByteArray& data)
		{
			QJsonDocument doc = QJsonDocument::fromJson(data);
			emit textMessageToWorkerThread(doc);
		});
	connect(MessageSender::instance(), &MessageSender::httpDataResponseReceived, this, [=](const QByteArray& data)
		{
			emit binaryMessageToWorkerThread(data);
		});
	//用户身份验证成功，发起连接请求并且获取用户信息
	connect(&m_messageHandle, &MessageHandle::connectToServer, this, [=]
		{
			connectToServer("ws://localhost:8888")
				->Connected([=]
					{
						SConfigFile config("config.ini");
						QVariantMap connectMap;
						connectMap["user_id"] = config.value("user_id");
						MessageSender::instance()->sendMessage("login", connectMap);
					});
		});
}

Client::~Client()
{
	delete m_client;
	// 确保后台线程正确退出
	if (m_workerThread->isRunning()) {
		m_workerThread->quit();
		m_workerThread->wait();
	}
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
//接受Web文本信息
void Client::onTextMessageReceived(const QString& message)
{
	qDebug() << "------------------------接受Web文本信息-----------------------";
	QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
	emit textMessageToWorkerThread(doc);
}
//接受Web二进制数据
void Client::onBinaryMessageReceived(const QByteArray& message)
{
	qDebug() << "------------------------接受Web二进制数据-----------------------";
	emit binaryMessageToWorkerThread(message);
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

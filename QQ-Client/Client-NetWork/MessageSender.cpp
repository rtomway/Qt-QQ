#include "MessageSender.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>

#include "Client.h"
#include "SConfigFile.h"
#include "PacketCreate.h"
#include "TokenManager.h"


MessageSender* MessageSender::instance()
{
	static MessageSender ins;
	return &ins;
}

MessageSender::MessageSender()
	: m_workerThread(new QThread(this))
	, m_httpWorker(new HttpWorker)
{
	//后台线程处理消息
	m_httpWorker->moveToThread(m_workerThread);
	m_workerThread->start();
	//http请求
	connect(this, &MessageSender::sendHttpRequestToThread, m_httpWorker, &HttpWorker::sendRequest);
	//http文本消息响应
	connect(m_httpWorker, &HttpWorker::httpTextResponseReceived, this, &MessageSender::httpTextResponseReceived);
	//http二进制数据响应
	connect(m_httpWorker, &HttpWorker::httpDataResponseReceived, this, &MessageSender::httpDataResponseReceived);
}

//设置当前客户端
void MessageSender::setClient(Client* client)
{
	m_client = client;
}

//关闭连接
void MessageSender::disConnect()
{
	qDebug() << "客户端关闭连接";
	m_client->disconnect();
}

//获取客户端
Client* MessageSender::getClient()
{
	return m_client;
}

//Web发送文本消息
void MessageSender::sendMessage(const QString& message)
{
	if (!m_client->isConnected())
	{
		qWarning() << "数据发送失败 连接已断开......";
		return;
	}
	m_client->getClientSocket()->sendTextMessage(message);
}

//Web发送二进制数据
void MessageSender::sendBinaryData(const QByteArray& data)
{
	if (!m_client->isConnected())
	{
		qWarning() << "数据发送失败 连接已断开......";
		return;
	}
	m_client->getClientSocket()->sendBinaryMessage(data);
}

//发送http请求
void MessageSender::sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack)
{
	emit sendHttpRequestToThread(type, data, Content_type, callBack);
}




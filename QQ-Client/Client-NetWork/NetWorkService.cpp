#include "NetWorkService.h"
#include "NetWorkService.h"


NetWorkService::NetWorkService(QObject* parent)
	: m_webClient(new WebClient(this))
	, m_httpClient(new HttpClient(this))
	, m_messageSender(new MessageSender(this))
	, m_messageHandle(new MessageHandle(this))
{
	init();
}

NetWorkService::~NetWorkService()
{

}

void NetWorkService::connectToServer()
{
	m_webClient->connectToServer("ws://localhost:8888")
		->Connected([&]
			{
				emit connectSuccess();
			});
}

void NetWorkService::disConnect()
{
	qDebug() << "客户端关闭连接";
	m_webClient->disconnect();
}

void NetWorkService::sendWebTextMessage(const QString& message)
{
	m_messageSender->sendWebTextMessage(message);
}

void NetWorkService::sendWebBinaryData(const QByteArray& data)
{
	m_messageSender->sendWebBinaryData(data);
}

void NetWorkService::sendHttpRequest(const QString& type, const QByteArray& data, const QString& content_type, HttpCallback callBack)
{
	m_messageSender->sendHttpRequest(type, data, content_type, callBack);
}

void NetWorkService::init()
{
	//发送消息
	connect(m_messageSender, &MessageSender::sendWebTextMessage_client, m_webClient, &WebClient::sendTextMessage);
	connect(m_messageSender, &MessageSender::sendWebBinaryData_client, m_webClient, &WebClient::sendBinaryData);
	connect(m_messageSender, &MessageSender::sendHttpRequest_client, m_httpClient, &HttpClient::sendRequest);
	//接收消息
	connect(m_webClient, &WebClient::textMessage, m_messageHandle, &MessageHandle::handle_textMessage);
	connect(m_webClient, &WebClient::binaryData, m_messageHandle, &MessageHandle::handle_binaryData);
	connect(m_httpClient, &HttpClient::httpTextResponse, m_messageHandle, &MessageHandle::handle_textMessage);
	connect(m_httpClient, &HttpClient::httpTextResponse, m_messageHandle, &MessageHandle::handle_binaryData);
}

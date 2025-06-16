#include "NetWorkService.h"


NetWorkService::NetWorkService(WebClientPort* webClientPort, HttpClientPort* httpClientPort,QObject* parent)
	: m_webClientPort(webClientPort)
	, m_httpClientPort(httpClientPort)
	, m_messageHandle(new MessageHandle(this))
{
	init();
}

NetWorkService::~NetWorkService()
{

}

void NetWorkService::connectToServer(std::function<void()>callBack)
{
	m_webClientPort->connectToServer("ws://localhost:8888", [callBack]
		{
			if (callBack)
				callBack();
		});
}

void NetWorkService::disConnect()
{
	m_webClientPort->disconnect();
}

void NetWorkService::sendWebTextMessage(const QString& message)
{
	m_webClientPort->sendTextMessage(message);
}

void NetWorkService::sendWebBinaryData(const QByteArray& data)
{
	m_webClientPort->sendBinaryData(data);
}

void NetWorkService::sendHttpRequest(const QString& type, const QByteArray& data, const QString& content_type, HttpCallback callBack)
{
	m_httpClientPort->sendRequest(type, data, content_type, callBack);
}

void NetWorkService::init()
{
	//接收消息
	connect(m_webClientPort, &WebClientPort::textMessage, m_messageHandle, &MessageHandle::handle_textMessage);
	connect(m_webClientPort, &WebClientPort::binaryData, m_messageHandle, &MessageHandle::handle_binaryData);
	connect(m_httpClientPort, &HttpClientPort::httpTextResponse, m_messageHandle, &MessageHandle::handle_textMessage);
	connect(m_httpClientPort, &HttpClientPort::httpDataResponse, m_messageHandle, &MessageHandle::handle_binaryData);
}

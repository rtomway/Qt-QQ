#include "NetWorkService.h"


NetWorkService::NetWorkService(WebClientPort* webClientPort, HttpClientPort* httpClientPort, QObject* parent)
	: m_webClientPort(webClientPort)
	, m_httpClientPort(httpClientPort)
	, m_messageQueue(new MessageQueue(this))
	, m_messageHandle(new MessageHandle)
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


void NetWorkService::sendHttpGetRequest(const QString& path, const QUrlQuery& params, const QMap<QString, QString>& headers, HttpCallback callback)
{
	m_httpClientPort->get(path, params, headers, callback);
}

void NetWorkService::sendHttpPostRequest(const QString& path, const QByteArray& data, const QMap<QString, QString>& headers, HttpCallback callback)
{
	m_httpClientPort->post(path, data, headers, callback);
}

void NetWorkService::sendHttpGetRequest(const QString& path, const QUrlQuery& params, HttpCallback callback)
{
	m_httpClientPort->get(path, params, callback);
}

void NetWorkService::sendHttpPostRequest(const QString& path, const QByteArray& data, HttpCallback callback)
{
	m_httpClientPort->post(path, data, callback);
}


void NetWorkService::init()
{

	m_messageHandle->setMessageSrc(m_messageQueue);

	connect(m_webClientPort, &WebClientPort::textMessage, m_messageQueue, &MessageQueue::pushText);
	connect(m_webClientPort, &WebClientPort::binaryData, m_messageQueue, &MessageQueue::pushBinary);
	connect(m_httpClientPort, &HttpClientPort::httpTextResponse, m_messageQueue, &MessageQueue::pushText);
	connect(m_httpClientPort, &HttpClientPort::httpDataResponse, m_messageQueue, &MessageQueue::pushBinary);
}

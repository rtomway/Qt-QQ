#include "NetWorkService.h"


NetWorkService::NetWorkService(WebSocketServer* wenSocketServer, HttpServer* httpServer, QObject* parent)
	: QObject(parent)
	, m_webSocketServer(wenSocketServer)
	, m_httpServer(httpServer)
	, m_messageQueue(new MessageQueue(this))
	, m_messageHandle(new MessageHandle)
{
	init();
}

NetWorkService::~NetWorkService()
{

}

void NetWorkService::sendTextMessage(const QString& user_id, const QString& response)
{
	m_webSocketServer->sendTextMessage(user_id, response);
}

void NetWorkService::sendBinaryMessage(const QString& user_id, const QByteArray& response)
{
	m_webSocketServer->sendBinaryMessage(user_id, response);
}



void NetWorkService::init()
{
	m_messageHandle->setServerMessageSrc(m_messageQueue);

	connect(m_messageHandle, &MessageHandle::addClient, m_webSocketServer, &WebSocketServer::onAddClient);
	connect(m_webSocketServer, &WebSocketServer::textReceive, this, [=](QWebSocket* webSocket, const QString& text)
		{
			m_messageQueue->push({ MessageQueue::WS_Text,QVariant::fromValue(text),QVariant::fromValue(webSocket) });
		});
	connect(m_webSocketServer, &WebSocketServer::binaryReceive, this, [=](QWebSocket* webSocket, const QByteArray& data)
		{
			m_messageQueue->push({ MessageQueue::WS_Binary,QVariant::fromValue(data),QVariant::fromValue(webSocket) });
		});
	connect(m_httpServer, &HttpServer::httpGetReceive, this, [=](const QHttpServerRequest& request, QHttpServerResponder& response)
		{
			auto httpData = httpDataLoad(request);
			auto response_ptr = QSharedPointer<QHttpServerResponder>::create(std::move(response));
			m_messageQueue->push({ MessageQueue::Http_Get,QVariant::fromValue(httpData),QVariant::fromValue(response_ptr) });
		});
	connect(m_httpServer, &HttpServer::httpPostTextReceive, this, [=](const QHttpServerRequest& request, QHttpServerResponder& response)
		{
			auto httpData = httpDataLoad(request);
			auto response_ptr = QSharedPointer<QHttpServerResponder>::create(std::move(response));
			m_messageQueue->push({ MessageQueue::Http_PostText,QVariant::fromValue(httpData),QVariant::fromValue(response_ptr) });
		});
	connect(m_httpServer, &HttpServer::httpPostBinaryReceive, this, [=](const QHttpServerRequest& request, QHttpServerResponder& response)
		{
			auto httpData = httpDataLoad(request);
			auto response_ptr = QSharedPointer<QHttpServerResponder>::create(std::move(response));
			m_messageQueue->push({ MessageQueue::Http_PostBinary,QVariant::fromValue(httpData),QVariant::fromValue(response_ptr) });
		});
}

QVariantMap NetWorkService::httpDataLoad(const QHttpServerRequest& request)
{
	QVariantMap httpData;

	httpData["url"] = request.url();

	QVariantMap headers;
	for (const auto& header : request.headers())
	{
		headers.insert(header.first, header.second);
	}
	httpData["headers"] = headers;
	httpData["body"] = QByteArray(request.body());

	return httpData;
}

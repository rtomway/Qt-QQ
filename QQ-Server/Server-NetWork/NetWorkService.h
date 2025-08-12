#ifndef NETWORKSERVICE_H_
#define NETWORKSERVICE_H_

#include <QObject>

#include "WebSocketServer.h"
#include "HttpServer.h"
#include "MessageHandle.h"
#include "MessageQueue.h"

class NetWorkService :public QObject
{
	Q_OBJECT
public:
	NetWorkService(WebSocketServer* wenSocketServer, HttpServer* httpServer, QObject* parent = nullptr);
	~NetWorkService();
public:
	//消息发送
	void sendTextMessage(const QString& user_id, const QString& response);
	void sendBinaryMessage(const QString& user_id, const QByteArray& response);
private:
	void init();
	QVariantMap httpDataLoad(const QHttpServerRequest& request);
private:
	WebSocketServer* m_webSocketServer;
	HttpServer* m_httpServer;
	MessageHandle* m_messageHandle;
	MessageQueue* m_messageQueue;
};


#endif // !NETWORKSERVICE_H_

#ifndef HttpServer_H
#define HttpServer_H

#include <QObject>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include "MessageHandle.h"

class HttpServer : public QObject
{
	Q_OBJECT
public:
	explicit HttpServer(QObject* parent = nullptr);
private:
	void setupRoutes();
	void registerHttpRoute(const QString& path);
private:
	QHttpServer* m_httpServer;
signals:
	void httpGetReceive(const QHttpServerRequest& request, QHttpServerResponder& response);
	void httpPostTextReceive(const QHttpServerRequest& request, QHttpServerResponder& response);
	void httpPostBinaryReceive(const QHttpServerRequest& request, QHttpServerResponder& response);
};

#endif // HttpServer_H

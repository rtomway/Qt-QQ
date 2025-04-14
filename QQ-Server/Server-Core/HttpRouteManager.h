#ifndef HTTPROUTEMANAGER_H
#define HTTPROUTEMANAGER_H

#include <QObject>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include "MessageHandle.h"

class HttpRouteManager : public QObject
{
    Q_OBJECT
public:
    explicit HttpRouteManager(QHttpServer* httpServer, QObject* parent = nullptr);
private:
    void setupRoutes();
    void registerHttpRoute(const QString& path);
private:
    QHttpServer* m_httpServer;
    MessageHandle m_messageHandle; 
};

#endif // HTTPROUTEMANAGER_H

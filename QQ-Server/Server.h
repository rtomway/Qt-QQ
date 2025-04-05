#ifndef SERVER_H_
#define  SERVER_H_

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QHttpServer>
#include <MessageHandle.h>

class Server :public QObject
{
	Q_OBJECT
public:
	explicit Server(QObject* parent = nullptr);
	~Server();
protected:
	void onNewConnection();
private:
	QWebSocketServer* m_webSocketServer{};
	QHttpServer* m_httpServer{};
	MessageHandle messageHandle{};
private:
	void httpRoute();
};

#endif // !SERVER_H_

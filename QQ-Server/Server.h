#ifndef SERVER_H_
#define  SERVER_H_

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>

class Server :public QObject
{
	Q_OBJECT
public:
	explicit Server(QObject* parent = nullptr);
	~Server();
protected:
	void onNewConnection();
private:
	QWebSocketServer* m_server{};
};

#endif // !SERVER_H_

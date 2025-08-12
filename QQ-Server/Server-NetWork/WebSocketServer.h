#ifndef  WEBSOCKETSERVER_H_
#define WEBSOCKETSERVER_H_

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>


class WebSocketServer :public QObject
{
	Q_OBJECT
public:
	explicit WebSocketServer(QObject* parent = nullptr);
	~WebSocketServer();
	void sendTextMessage(const QString& user_id, const QString& response);
	void sendBinaryMessage(const QString& user_id, const QByteArray& response);
	void onAddClient(const QString& user_id, QWebSocket* socket);
protected:
	void onNewConnection();
	void onTextMessageReceived(const QString& text);
	void onBinaryMessageReceived(const QByteArray& data);
	void onDisConnection();
private:
	QString findUserName(QWebSocket* client);
private:
	QWebSocketServer* m_webSocketServer = nullptr;
	//客户端
	QHash<QString, QWebSocket*>m_clients;
signals:
	void textReceive(QWebSocket* webSocket, const QString& text);
	void binaryReceive(QWebSocket* webSocket, const QByteArray& data);
};


#endif // ! WEBSOCKETSERVER_H_

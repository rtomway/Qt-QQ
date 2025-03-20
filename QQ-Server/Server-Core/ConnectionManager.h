#ifndef CONNECTIONMANAGER
#define CONNECTIONMANAGER

#include <QObject>
#include <QWebSocket>

#include "MessageHandle.h"

class ConnectionManager :public QObject
{
	Q_OBJECT
public:
	static ConnectionManager* instance();
	void onNewConnection(QWebSocket* client);
	void onTextMessageReceived(const QString& data);
	void onBinaryMessageReceived(const QByteArray& data);
	void onDisConnection();
	void sendTextMessage(const QString& user_id, const QString& response);
	void sendBinaryMessage(const QString& user_id, const QByteArray& response);
private:
	ConnectionManager(QObject* parent = nullptr);
private:
	QString findUserName(QWebSocket* client);
	
private:
	//客户端
	QHash<QString, QWebSocket*>m_clients{};
	MessageHandle m_messageHandle;
private:
	void addClient(const QString&user_id,QWebSocket*socket);
};

#endif // !CONNECTIONMANAGER

#ifndef CONNECTIONMANAGER
#define CONNECTIONMANAGER

#include <QObject>
#include <QWebSocket>

#include "MessageHandle.h"

class ConnectionManager :public QObject
{
	Q_OBJECT
public:
	// 获取单例实例的静态方法
	static ConnectionManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	ConnectionManager(const ConnectionManager&) = delete;
	ConnectionManager& operator=(const ConnectionManager&) = delete;
private:
	ConnectionManager(QObject* parent = nullptr);
public:
	void onNewConnection(QWebSocket* client);
	void onTextMessageReceived(const QString& data);
	void onBinaryMessageReceived(const QByteArray& data);
	void onDisConnection();
	void sendTextMessage(const QString& user_id, const QString& response);
	void sendBinaryMessage(const QString& user_id, const QByteArray& response);
private:
	QString findUserName(QWebSocket* client);
private:
	//客户端
	QHash<QString, QWebSocket*>m_clients{};
	MessageHandle m_messageHandle;
private:
	void addClient(const QString& user_id, QWebSocket* socket);
};

#endif // !CONNECTIONMANAGER

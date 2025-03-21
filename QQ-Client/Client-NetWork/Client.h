#ifndef CLIENT_H_
#define CLIENT_H_

#include <QWebSocket>
#include <QObject>
#include <functional>
#include <QUrl>

#include "MessageHandle.h"

class Client :public QObject
{
	Q_OBJECT
public:
	~Client();
	Client(QObject* parent = nullptr);
public:
	//链式调用
	Client* connectToServer(const QString& url);
	//事件回调
	Client* ReciveMessage(std::function<void(const QString&)>callback);
	Client* Error(std::function<void(const QString&)>callback);
	Client* Connected(std::function<void()>callback);
	Client* DisconnectFromServer(std::function<void()>callback);
	void disconnect();
	bool isConnected()const;
	QWebSocket* getClientSocket()const;
private:
	QWebSocket* m_client;
	bool m_isConnected;
	MessageHandle m_messageHandle;
	//回调函数
	std::function<void(const QString&)> m_messageCallback;
	std::function<void(const QString&)> m_errorCallback;
	std::function<void()> m_connectedCallback;
	std::function<void()> m_disconnectedCallback;
private slots:
	void onTextMessageReceived(const QString& message);
	void onBinaryMessageReceived(const QByteArray& data);
	void onErrorOccurred(QAbstractSocket::SocketError error);
	void onConnected();
	void onDisconnected();
};


#endif // !CLIENT_H_

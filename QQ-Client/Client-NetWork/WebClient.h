#ifndef WEBCLIENT_H_
#define WEBCLIENT_H_

#include <QWebSocket>
#include <QObject>
#include <functional>
#include <QUrl>
#include <QThread>

class WebClient :public QObject
{
	Q_OBJECT
public:
	WebClient(QObject* parent = nullptr);
	~WebClient();
public:
	//链式调用
	WebClient* connectToServer(const QString& url);
	//事件回调
	WebClient* ReciveMessage(std::function<void(const QString&)>callback);
	WebClient* Error(std::function<void(const QString&)>callback);
	WebClient* Connected(std::function<void()>callback);
	WebClient* DisconnectFromServer(std::function<void()>callback);
	void disconnect();
	bool isConnected()const;
	void sendTextMessage(const QString& message);
	void sendBinaryData(const QByteArray& data);
private:
	QWebSocket* m_webSocket{};
	bool m_isConnected{};
	//回调函数
	std::function<void(const QString&)> m_messageCallback{};
	std::function<void(const QString&)> m_errorCallback{};
	std::function<void()> m_connectedCallback{};
	std::function<void()> m_disconnectedCallback{};
private slots:
	void onTextMessageReceived(const QString& message);
	void onBinaryMessageReceived(const QByteArray& data);
	void onErrorOccurred(QAbstractSocket::SocketError error);
	void onConnected();
	void onDisconnected();
signals:
	void textMessage(const QString& message);
	void binaryData(const QByteArray& data);
};


#endif // !WEBCLIENT_H_

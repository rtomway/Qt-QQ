#ifndef WEBCLIENT_H_
#define WEBCLIENT_H_

#include <QWebSocket>
#include <QObject>
#include <functional>
#include <QUrl>
#include <QThread>

#include "WebClientPort.h"

class WebClient :public WebClientPort
{
	Q_OBJECT
public:
	WebClient(QObject* parent = nullptr);
	~WebClient();
public:
	void connectToServer(const QString& url, std::function<void()>callback = nullptr);
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
	const QString& url();
private:
	QWebSocket* m_webSocket{};
	bool m_isConnected{};
	QString m_url{};
	//回调函数
	std::function<void(const QString&)> m_messageCallback{};
	std::function<void(const QString&)> m_errorCallback{};
	std::function<void()> m_connectedCallback{};
	std::function<void()> m_disconnectedCallback{};
private slots:
	void onTextMessageReceived(const QString& message);
	void onBinaryDataReceived(const QByteArray& data);
	void onErrorOccurred(QAbstractSocket::SocketError error);
	void onConnected();
	void onDisconnected();

};


#endif // !WEBCLIENT_H_

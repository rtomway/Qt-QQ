#ifndef MESSAGESENDER_H_
#define MESSAGESENDER_H_

#include <QObject>
#include <QNetWorkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <QUrl>
#include <QThread>
#include "HttpWorker.h"

class Client;

class MessageSender :public QObject
{
	Q_OBJECT
public:
	// 获取单例实例的静态方法
	static MessageSender* instance();
	// 禁止拷贝构造函数和赋值操作符
	MessageSender(const MessageSender&) = delete;
	MessageSender& operator=(const MessageSender&) = delete;
public:
	void setClient(Client* client);
	void disConnect();
	void sendMessage(const QString& type, const QVariantMap& params = {});
	void sendBinaryData(const QByteArray& data);
private:
	MessageSender();
private:
	Client* m_client{};
	QThread* m_workerThread;
	HttpWorker* m_httpWorker;
	QNetworkAccessManager* m_networkManager;
signals:
	void sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type);
	void httpTextResponseReceived(const QByteArray& data);
	void httpDataResponseReceived(const QByteArray& data);
};

#endif // !MESSAGESENDER_H_

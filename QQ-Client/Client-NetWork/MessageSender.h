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
public:
	void setClient(Client* client);
	void disConnect();
	Client* getClient();
	void sendMessage(const QString& message);
	void sendBinaryData(const QByteArray& data);
	// 使用 std::function 定义回调类型
	using HttpCallback = std::function<void(const QJsonObject&, const QByteArray&)>;
	void sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack = nullptr);
private:
	MessageSender();
	// 禁止拷贝构造函数和赋值操作符
	MessageSender(const MessageSender&) = delete;
	MessageSender& operator=(const MessageSender&) = delete;
private:
	Client* m_client{};
	QThread* m_workerThread{};
	HttpWorker* m_httpWorker{};
signals:
	void connectToServer(const QString& user_id);

	void sendHttpRequestToThread(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack);
	void httpTextResponseReceived(const QByteArray& data);
	void httpDataResponseReceived(const QByteArray& data);
};

#endif // !MESSAGESENDER_H_

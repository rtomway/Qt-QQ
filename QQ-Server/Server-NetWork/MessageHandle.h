#ifndef MESSAGEHANDLE_H_
#define MESSAGEHANDLE_H_

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QWebSocket>
#include <QHttpServer>
#include "MessageQueue.h"

class MessageHandle :public QObject
{
	Q_OBJECT
public:
	MessageHandle(QObject* parent = nullptr);
	//消息处理接口
	void webTextHandler(const QString& message, QWebSocket* socket);
	void webBinaryHandler(const QByteArray& message, QWebSocket* socket);

	void httpGetHandler(const QVariantMap& requestHttpData, QHttpServerResponder& response);
	void httpPostBinaryHandler(const QVariantMap& requestHttpData, QHttpServerResponder& response);
	void httpPostTextHandler(const QVariantMap& requestHttpData, QHttpServerResponder& response);
private:
	//消息处理函数表
	QHash<QString, void(*)(const QJsonObject&, const QByteArray&)>m_webRequestHash{};
	QHash<QString, void(*)(const QJsonObject&, const QByteArray&, QHttpServerResponder&)> m_httpRequestHash{};
	//公开界面操作列表(无需token认证)
	QStringList m_publicPageList;
private:
	void initWebRequestHash();
	void initHttpRequestHash();
	void initPublicPageType();
	bool webTokenVerify(const QString& token, const QString& user_id, const QString& type);
	bool httpToeknVerify(const QString& type, const QVariantMap& requestHttpData);
signals:
	void addClient(const QString& user_id, QWebSocket* client);
public:
	void setServerMessageSrc(MessageQueue* messageQueue);
	void startServerConsumerThread();
	void stopServerConsumerThread();
private slots:
	void serverConsumer();

private:
	QThread m_consumerThread;
	std::atomic<bool> m_running{ false };
	MessageQueue* m_messageQueue;
};

#endif // !MESSAGEHANDLE_H_

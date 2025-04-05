#ifndef MESSAGEHANDLE_H_
#define MESSAGEHANDLE_H_

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QWebSocket>
#include <QHttpServer>

class MessageHandle :public QObject
{
	Q_OBJECT
public:
	MessageHandle(QObject* parent = nullptr);
	//消息处理接口
	void handle_message(const QString& message, QWebSocket* socket);
	void handle_message(const QByteArray& message, QWebSocket* socket);
	void handle_message(const QString& type, const QHttpServerRequest& request, QHttpServerResponder& response);
private:
	//消息处理函数表
	QHash<QString, void(*)(const QJsonObject&, const QByteArray&)>webRequestHash{};
	QHash<QString, void(*)(const QByteArray&, QHttpServerResponder&)> httpRequestHash{};

private:
	void initWebRequestHash();
	void initHttpRequestHash();
signals:
	void addClient(const QString& user_id, QWebSocket* client);
};

#endif // !MESSAGEHANDLE_H_

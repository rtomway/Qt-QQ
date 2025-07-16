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
	void handle_message(const QHttpServerRequest& request, QHttpServerResponder& response);
private:
	//消息处理函数表
	QHash<QString, void(*)(const QJsonObject&, const QByteArray&)>webRequestHash{};
	QHash<QString, void(*)(const QJsonObject&,const QByteArray&, QHttpServerResponder&)> httpRequestHash{};
	//公开界面操作列表(无需token认证)
	QStringList m_publicPage_list;
private:
	void initWebRequestHash();
	void initHttpRequestHash();
	void initPublicPageType();
	bool tokenRight(const QString&token,const QString&user_id,const QString&type);
signals:
	void addClient(const QString& user_id, QWebSocket* client);
};

#endif // !MESSAGEHANDLE_H_

#ifndef MESSAGESENDER_H_
#define MESSAGESENDER_H_

#include <QObject>
#include <QNetWorkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <QUrl>


class MessageSender :public QObject
{
	Q_OBJECT
public:
	MessageSender(QObject*parent=nullptr);
public:
	void sendWebTextMessage(const QString& message);
	void sendWebBinaryData(const QByteArray& data);
	// 使用 std::function 定义回调类型
	using HttpCallback = std::function<void(const QJsonObject&, const QByteArray&)>;
	void sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack = nullptr);
signals:
	void sendWebTextMessage_client(const QString& message);
	void sendWebBinaryData_client(const QByteArray& data);
	void sendHttpRequest_client(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack);
};

#endif // !MESSAGESENDER_H_

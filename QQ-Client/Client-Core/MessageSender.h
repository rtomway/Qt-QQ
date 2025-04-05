#ifndef MESSAGESENDER_H_
#define MESSAGESENDER_H_

#include <QObject>
#include <QNetWorkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <QUrl>

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
	void sendMessage(const QString& type, const QVariantMap& params = {});
	void sendBinaryData(const QByteArray& data);
	void sendHttpRequest(const QString& type, const QJsonObject& paramsObj);
	void sendFile(const QString& path, const QString& filePath, const QVariantMap& params = {});
	void sendFile(const QString& path, const QByteArray& fileData, const QString& fileName, const QVariantMap& params = {});
private:
	MessageSender();
	Client* m_client{};
	QNetworkAccessManager* m_networkManager = nullptr;
	QString m_baseUrl = "http://127.0.0.1:8889/";
signals:
	void httpTextResponseReceived(const QByteArray& data);
	void httpDataResponseReceived(const QByteArray& data);
};

#endif // !MESSAGESENDER_H_

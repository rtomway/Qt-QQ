#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetWorkAccessManager>
#include <QNetworkReply>

class HttpClient : public QObject
{
	Q_OBJECT
public:
	explicit HttpClient(QObject* parent = nullptr);
	// 使用 std::function 定义回调类型
	using HttpCallback = std::function<void(const QJsonObject&, const QByteArray&)>;
	// 发送 HTTP 请求，接收回调函数
	void sendRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callback = nullptr);
private:
	void replyErrorHandle(QNetworkReply::NetworkError error);
	void replyDataHandle(QNetworkReply* reply, HttpCallback callBack);
signals:
	void httpTextResponse(const QByteArray& data);
	void httpDataResponse(const QByteArray& data);
private:
	QNetworkAccessManager* m_networkManager{};
	QString m_baseUrl = "http://127.0.0.1:8889/";
};

#endif // HTTPWORKER_H

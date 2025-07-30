#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetWorkAccessManager>
#include <QNetworkReply>

#include "HttpClientPort.h"

class HttpClient : public HttpClientPort
{
	Q_OBJECT
public:
	explicit HttpClient(QObject* parent = nullptr);
	// 使用 std::function 定义回调类型
	using HttpCallback = std::function<void(const QJsonObject&, const QByteArray&)>;
	// 发送 HTTP 请求，接收回调函数
	void get(const QString& type, const QUrlQuery& params, const QMap<QString, QString>& headers, HttpCallback callback = nullptr)override;
	void post(const QString& type, const QByteArray& data, const QMap<QString, QString>& headers, HttpCallback callback = nullptr)override;
	void get(const QString& type, const QUrlQuery& params, HttpCallback callback = nullptr)override;
	void post(const QString& type, const QByteArray& data, HttpCallback callback = nullptr)override;
private:
	void setHeaders(QNetworkRequest& request, const QMap<QString, QString>& headers = QMap<QString, QString>());
	void handleReply(QNetworkReply* reply, HttpCallback callback);
	void replyErrorHandle(QNetworkReply::NetworkError error);
	void replyDataHandle(QNetworkReply* reply, HttpCallback callBack);
private:
	QNetworkAccessManager* m_networkManager{};
	QString m_baseUrl = "http://127.0.0.1:8889/";

};

#endif // HTTPWORKER_H

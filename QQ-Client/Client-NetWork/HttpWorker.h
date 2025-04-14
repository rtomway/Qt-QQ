#ifndef HTTPWORKER_H
#define HTTPWORKER_H

#include <QObject>
#include <QNetWorkAccessManager>
#include <QNetworkReply>

class HttpWorker : public QObject
{
	Q_OBJECT
public:
	explicit HttpWorker(QObject* parent = nullptr);
	void sendRequest(const QString& url, const QByteArray& data, const QString& contentType);
signals:
	void httpTextResponseReceived(const QByteArray& data);
	void httpDataResponseReceived(const QByteArray& data);
private:
	QNetworkAccessManager* m_networkManager;
	QString m_baseUrl = "http://127.0.0.1:8889/";
};

#endif // HTTPWORKER_H

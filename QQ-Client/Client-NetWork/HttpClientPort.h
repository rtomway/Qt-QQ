#ifndef HTTPCLIENTPORT_H
#define HTTPCLIENTPORT_H

#include <QObject>

class HttpClientPort : public QObject
{
	Q_OBJECT
public:
	explicit HttpClientPort(QObject* parent = nullptr)
		: QObject(parent) {}
	~HttpClientPort() {};

	using HttpCallback = std::function<void(const QJsonObject&, const QByteArray&)>;
	virtual void sendRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callback = nullptr) = 0;
signals:
	void httpTextResponse(const QByteArray& data);
	void httpDataResponse(const QByteArray& data);
};

#endif // HTTPCLIENTPORT_H

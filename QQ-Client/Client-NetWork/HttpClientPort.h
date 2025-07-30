#ifndef HTTPCLIENTPORT_H
#define HTTPCLIENTPORT_H

#include <QObject>
#include <QMap>
#include <QString>
#include <functional>
#include <QByteArray>
#include <QUrlQuery>

class HttpClientPort : public QObject
{
	Q_OBJECT
public:
	explicit HttpClientPort(QObject* parent = nullptr)
		: QObject(parent) {}
	~HttpClientPort() = default;

	using HttpCallback = std::function<void(const QJsonObject&, const QByteArray&)>;
	virtual void get(const QString& path, const QUrlQuery& params, const QMap<QString, QString>& headers, HttpCallback callback = nullptr) = 0;
	virtual void post(const QString& path, const QByteArray& data, const QMap<QString, QString>& headers, HttpCallback callback = nullptr) = 0;
	virtual void get(const QString& path, const QUrlQuery& params, HttpCallback callback = nullptr) = 0;
	virtual void post(const QString& path, const QByteArray& data, HttpCallback callback = nullptr) = 0;
signals:
	void httpTextResponse(const QByteArray& data);
	void httpDataResponse(const QByteArray& data);
};

#endif // HTTPCLIENTPORT_H

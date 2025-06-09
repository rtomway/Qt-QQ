#ifndef NETWORKSERVICE_H_
#define NETWORKSERVICE_H_

#include <QObject>

#include "WebClient.h"
#include "HttpClient.h"
#include "MessageSender.h"
#include "MessageHandle.h"

class NetWorkService :public QObject
{
	Q_OBJECT
public:
	NetWorkService(QObject* parent = nullptr);
	~NetWorkService();
public:
	//连接建立
	void connectToServer();
	void disConnect();
	//消息发送
	void sendWebTextMessage(const QString& message);
	void sendWebBinaryData(const QByteArray& data);
	// 使用 std::function 定义回调类型
	using HttpCallback = std::function<void(const QJsonObject&, const QByteArray&)>;
	void sendHttpRequest(const QString& type, const QByteArray& data, const QString& Content_type, HttpCallback callBack = nullptr);
private:
	void init();
private:
	WebClient* m_webClient;
	HttpClient* m_httpClient;
	MessageSender* m_messageSender;
	MessageHandle* m_messageHandle;
signals:
	void connectSuccess();
};


#endif // !NETWORKSERVICE_H_

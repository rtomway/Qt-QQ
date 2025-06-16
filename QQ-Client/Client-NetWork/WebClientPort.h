#ifndef WEBCLIENTPORT_H_
#define WEBCLIENTPORT_H_

#include <QObject>

class WebClientPort :public QObject
{
	Q_OBJECT
public:
	explicit WebClientPort(QObject* parent = nullptr)
		: QObject(parent) {}
	~WebClientPort() {};
public:
	virtual void connectToServer(const QString& url, std::function<void()>callback = nullptr)=0;
	virtual void disconnect()=0;
	virtual void sendTextMessage(const QString& message)=0;
	virtual void sendBinaryData(const QByteArray& data)=0;
signals:
	void textMessage(const QString& message);
	void binaryData(const QByteArray& data);

};


#endif // !WEBCLIENTPORT_H_

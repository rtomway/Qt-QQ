#ifndef MESSAGEHANDLE_H_
#define MESSAGEHANDLE_H_

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QWebSocket>

class MessageHandle :public QObject
{
	Q_OBJECT
public:
	MessageHandle(QObject* parent = nullptr);
	//消息处理接口
	void handle_message(const QString& message,QWebSocket* socket);
	void handle_message(const QByteArray& message, QWebSocket* socket);
private:
	//消息处理函数表
	QHash<QString, void(*)(const QJsonObject&, const QByteArray&)>requestHash{};
private:
	void initRequestHash();
private:
	QString getRandomID(int length);
	QString generateUserID();
signals:
	void addClient(const QString& user_id, QWebSocket* client);
};

#endif // !MESSAGEHANDLE_H_

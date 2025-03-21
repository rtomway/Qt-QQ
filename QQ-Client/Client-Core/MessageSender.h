#ifndef MESSAGESENDER_H_
#define MESSAGESENDER_H_

#include <QObject>
#include <Client.h>

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
private:
	MessageSender() {};
	Client* m_client{};
};

#endif // !MESSAGESENDER_H_

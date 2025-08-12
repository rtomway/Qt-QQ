#ifndef MESSAGEHANDLE_H_
#define MESSAGEHANDLE_H_

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QThread>

#include "Client_MessageHandle/Client_LoginHandle.h"
#include "Client_MessageHandle/Client_FriendHandle.h"
#include "Client_MessageHandle/Client_GroupHandle.h"
#include "Client_MessageHandle/Client_UserHandle.h"

#include "MessageQueue.h"


class MessageHandle :public QObject
{
	Q_OBJECT
public:
	MessageHandle();
	//消息处理接口
	void handle_textMessage(const QString& message);
	void handle_binaryData(const QByteArray& data);
	//消息注册
	template <typename T>
	void registerHandle(const QString& key, T& instance, void (T::* handler)(const QJsonObject&, const QByteArray&));
	void setMessageSrc(MessageQueue* messageQueue);
	void startConsumerThread();
	void stopConsumerThread();
private:
	void initRequestHash();
private slots:
	void consumerLoop();

private:
	QThread m_consumerThread;
	std::atomic<bool> m_running{ false };
private:
	Client_LoginHandle m_loginHandle{};
	Client_FriendHandle m_friendHandle{};
	Client_GroupHandle m_groupHandle{};
	Client_UserHandle m_userHandle{};
	//消息处理函数表
	QHash<QString, std::function<void(const QJsonObject&, const QByteArray&)>> requestHash{};

	MessageQueue* m_messageQueue = nullptr;
};

#endif // !MESSAGEHANDLE_H_

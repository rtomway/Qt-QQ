#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QVariant>

class MessageQueue : public QObject {
	Q_OBJECT
public:
	enum MessageType
	{
		WS_Text,
		WS_Binary,
		Http_Get,
		Http_PostText,
		Http_PostBinary
	};

	struct Message
	{
		MessageType type;
		QVariant data;
		QVariant sender;
	};

	explicit MessageQueue(QObject* parent = nullptr);

	void push(Message&& message);


	// 消息出队（阻塞式）
	Message pop(int timeoutMs = -1);

	// 安全关闭
	void shutdown();

signals:
	void newMessageAvailable();

private:
	QQueue<Message> m_queue;
	QMutex m_mutex;
	QWaitCondition m_condition;
	bool m_shutdownRequested = false;
};

#endif // !MESSAGEQUEUE_H_

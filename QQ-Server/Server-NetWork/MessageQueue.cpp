#include "MessageQueue.h"
#include <QDebug>

MessageQueue::MessageQueue(QObject* parent)
	:QObject(parent)
{

}

//text消息
void MessageQueue::push(Message&& message)
{
	QMutexLocker lock(&m_mutex);
	if (m_shutdownRequested)
		return;
	m_queue.enqueue(message);
	m_condition.wakeOne();
}



//消息出队
MessageQueue::Message MessageQueue::pop(int timeoutMs)
{
	QMutexLocker lock(&m_mutex);
	QDeadlineTimer deadline(timeoutMs);

	while (m_queue.isEmpty() && !m_shutdownRequested)
	{
		if (!m_condition.wait(&m_mutex, deadline))
		{
			return { WS_Text, QByteArray(),QVariant() };
		}
	}

	if (m_shutdownRequested && m_queue.isEmpty())
	{
		return { WS_Text, QByteArray(),QVariant() };
	}

	return m_queue.dequeue();
}

//关闭消息队列
void MessageQueue::shutdown()
{
	QMutexLocker lock(&m_mutex);
	m_shutdownRequested = true;
	m_condition.wakeAll();
}

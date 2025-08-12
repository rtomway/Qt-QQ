#include "MessageQueue.h"
#include <QDebug>

MessageQueue::MessageQueue(QObject* parent)
	:QObject(parent)
{

}

//text消息
void MessageQueue::pushText(const QString& text)
{
	QMutexLocker lock(&m_mutex);
	if (m_shutdownRequested)
		return;
	m_queue.enqueue({ Text, text.toUtf8() });
	m_condition.wakeOne();
}

//二进制消息
void MessageQueue::pushBinary(const QByteArray& data)
{
	QMutexLocker lock(&m_mutex);
	if (m_shutdownRequested)
		return;
	m_queue.enqueue({ Binary, data });
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
			return { Text, QByteArray() };
		}
	}

	if (m_shutdownRequested && m_queue.isEmpty())
	{
		return { Text, QByteArray() };
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

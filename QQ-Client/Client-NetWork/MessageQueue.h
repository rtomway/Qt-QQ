#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

class MessageQueue : public QObject {
    Q_OBJECT
public:
    enum MessageType { Text, Binary };

    struct Message {
        MessageType type;
        QByteArray data;
    };

    explicit MessageQueue(QObject* parent = nullptr);

    // 文本消息入队
    void pushText(const QString& text);

    // 二进制消息入队
    void pushBinary(const QByteArray& data);

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

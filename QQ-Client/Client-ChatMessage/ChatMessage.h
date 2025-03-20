#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QString>
#include <QList>
#include <memory> // 为了使用 std::shared_ptr
#include "Message.h"

class ChatMessage {
public:
	// 构造函数
	ChatMessage(QString user_id, QString friend_id);

	// 添加消息
	void addMessage(std::shared_ptr<Message> msg);

	// 获取所有消息
	const QList<std::shared_ptr<Message>>& getMessages() const;

private:
	QString m_user_id;      // 用户ID
	QString m_friend_id;    // 好友ID
	QList<std::shared_ptr<Message>> m_messages; // 消息列表，使用 shared_ptr
};

#endif // CHATMESSAGE_H

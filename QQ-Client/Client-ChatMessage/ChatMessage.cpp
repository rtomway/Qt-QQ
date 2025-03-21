#include "ChatMessage.h"

ChatMessage::ChatMessage(QString user_id, QString friend_id)
	: m_user_id(user_id), m_friend_id(friend_id) {}

// 添加消息
void ChatMessage::addMessage(std::shared_ptr<Message> msg) {
	m_messages.append(msg); 
}

// 获取所有消息
const QList<std::shared_ptr<Message>>& ChatMessage::getMessages() const {
	return m_messages; 
}

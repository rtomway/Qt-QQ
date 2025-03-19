#include "ChatMessage.h"

// 带有用户和好友ID的构造函数
ChatMessage::ChatMessage(QString user_id, QString friend_id)
	: m_user_id(user_id), m_friend_id(friend_id) {}

// 添加消息
void ChatMessage::addMessage(std::shared_ptr<Message> msg) {
	m_messages.append(msg);  // 不需要 std::move，因为 shared_ptr 会管理引用计数
}

// 获取所有消息
const QList<std::shared_ptr<Message>>& ChatMessage::getMessages() const {
	return m_messages;  // 返回共享所有权的消息列表
}

#include "ChatManager.h"

ChatManager* ChatManager::instance() {
	static ChatManager ins;
	return &ins;
}

// 获取指定用户的聊天记录，如果没有则返回空指针
std::shared_ptr<ChatMessage> ChatManager::getChat(const QString& userId) {
	return m_chats.value(userId, nullptr);
}

// 添加新的聊天记录
void ChatManager::addChat(const QString& userId, std::shared_ptr<ChatMessage> chatMessage) {
	if (!m_chats.contains(userId)) {
		m_chats[userId] = chatMessage;
	}
}

// 获取并更新指定好友的聊天记录
ChatMessage* ChatManager::updateChat(const QString& userId) {
	if (m_chats.contains(userId)) {
		//m_chats[userId] = std::make_shared<ChatMessage>(userId);  // 如果聊天记录不存在，创建新的
		return m_chats[userId].get();  // 返回指向ChatMessage的原始指针
	}

}

// 清除指定用户的聊天记录
void ChatManager::clearChat(const QString& userId) {
	m_chats.remove(userId);
}

// 清空所有聊天记录
void ChatManager::clearAllChats() {
	m_chats.clear();
}

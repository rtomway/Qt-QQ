#include "ChatRecordManager.h"

ChatRecordManager* ChatRecordManager::instance() {
	static ChatRecordManager ins;
	return &ins;
}

// 获取指定用户的聊天记录，如果没有则返回空指针
std::shared_ptr<ChatRecordMessage> ChatRecordManager::getChat(const QString& userId) {
	return m_friendChats.value(userId, nullptr);
}
// 添加新的聊天记录
void ChatRecordManager::addChat(const QString& userId, std::shared_ptr<ChatRecordMessage> chatMessage) {
	if (!m_friendChats.contains(userId)) {
		m_friendChats[userId] = chatMessage;
	}
}
// 获取并更新指定好友的聊天记录
ChatRecordMessage* ChatRecordManager::updateChat(const QString& userId) {
	if (m_friendChats.contains(userId)) {
		return m_friendChats[userId].get();  // 返回指向ChatMessage的原始指针
	}
}
// 清除指定用户的聊天记录
void ChatRecordManager::clearChat(const QString& userId) {
	m_friendChats.remove(userId);
}
std::shared_ptr<ChatRecordMessage> ChatRecordManager::getGroupChat(const QString& groupId)
{
	return m_groupChats.value(groupId, nullptr);
}
void ChatRecordManager::addGroupChat(const QString& groupId, std::shared_ptr<ChatRecordMessage> chatMessage)
{
	if (!m_groupChats.contains(groupId)) {
		m_groupChats[groupId] = chatMessage;
	}
}
ChatRecordMessage* ChatRecordManager::updateGroupChat(const QString& groupId)
{
	if (m_groupChats.contains(groupId)) {
		return m_groupChats[groupId].get();  // 返回指向ChatMessage的原始指针
	}
}
void ChatRecordManager::clearGroupChat(const QString& groupId)
{
	m_groupChats.remove(groupId);
}
// 清空所有聊天记录
void ChatRecordManager::clearAllChats() {
	m_friendChats.clear();
	m_groupChats.clear();
}

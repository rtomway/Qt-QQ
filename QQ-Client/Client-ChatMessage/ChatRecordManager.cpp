#include "ChatRecordManager.h"

ChatRecordManager* ChatRecordManager::instance() {
	static ChatRecordManager ins;
	return &ins;
}
// 获取指定用户的聊天记录，如果没有则返回空指针
std::shared_ptr<ChatRecordMessage> ChatRecordManager::getUserChat(const QString& userId) 
{
	return m_friendChats.value(userId, nullptr);
}
// 添加新的聊天记录
void ChatRecordManager::addUserChat(const QString& userId, std::shared_ptr<ChatRecordMessage> chatMessage) 
{
	if (!m_friendChats.contains(userId)) {
		m_friendChats[userId] = chatMessage;
	}
}
//为聊天记录添加消息
void ChatRecordManager::addMessageToUserChat(const QString& user_id, const std::shared_ptr<MessageRecord>& message)
{
	auto chat = getUserChat(user_id);
	if(chat)
	chat->addMessage(message);
}
// 清除指定用户的聊天记录
void ChatRecordManager::clearUserChat(const QString& userId)
{
	m_friendChats.remove(userId);
}
//获取群聊聊天记录
std::shared_ptr<ChatRecordMessage> ChatRecordManager::getGroupChat(const QString& groupId)
{
	return m_groupChats.value(groupId, nullptr);
}
//添加群聊聊天记录
void ChatRecordManager::addGroupChat(const QString& groupId, std::shared_ptr<ChatRecordMessage> chatMessage)
{
	if (!m_groupChats.contains(groupId)) 
	{
		m_groupChats[groupId] = chatMessage;
	}
}
//为聊天记录添加消息
void ChatRecordManager::addMessageToGroupChat(const QString& group_id, const std::shared_ptr<MessageRecord>& message)
{
	auto chat = getGroupChat(group_id);
	if (chat)
		chat->addMessage(message);
}
//清空聊天记录
void ChatRecordManager::clearGroupChat(const QString& groupId)
{
	m_groupChats.remove(groupId);
}
// 清空所有聊天记录
void ChatRecordManager::clearAllChats() {
	m_friendChats.clear();
	m_groupChats.clear();
}

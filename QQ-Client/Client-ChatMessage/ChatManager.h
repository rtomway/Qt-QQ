#ifndef CHATMANAGER_H_
#define CHATMANAGER_H_

#include <QHash>
#include <memory>
#include "ChatMessage.h"

class ChatManager {
public:
	// 获取单例实例的静态方法
	static ChatManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	ChatManager(const ChatManager&) = delete;
	ChatManager& operator=(const ChatManager&) = delete;

	// 获取指定用户的聊天记录
	std::shared_ptr<ChatMessage> getChat(const QString& userId);

	// 添加新的聊天记录
	void addChat(const QString& userId, std::shared_ptr<ChatMessage> chatMessage);

	// 获取并更新指定好友的聊天记录，返回ChatMessage对象
	ChatMessage* updateChat(const QString& userId);

	// 清除指定用户的聊天记录
	void clearChat(const QString& userId);

	// 清空所有聊天记录
	void clearAllChats();

private:
	ChatManager() {};
	QHash<QString, std::shared_ptr<ChatMessage>> m_chats;  // 用于存储聊天记录的哈希表
};

#endif // CHATMANAGER_H_

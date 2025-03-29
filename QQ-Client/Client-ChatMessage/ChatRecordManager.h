#ifndef CHATRECORDMANAGER_H_
#define CHATRECORDMANAGER_H_

#include <QHash>
#include <memory>
#include "ChatRecordMessage.h"

class ChatRecordManager {
public:
	// 获取单例实例的静态方法
	static ChatRecordManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	ChatRecordManager(const ChatRecordManager&) = delete;
	ChatRecordManager& operator=(const ChatRecordManager&) = delete;

	// 获取指定用户的聊天记录
	std::shared_ptr<ChatRecordMessage> getChat(const QString& userId);
	// 添加新的聊天记录
	void addChat(const QString& userId, std::shared_ptr<ChatRecordMessage> chatMessage);
	// 获取并更新指定好友的聊天记录，返回ChatMessage对象
	ChatRecordMessage* updateChat(const QString& userId);
	// 清除指定用户的聊天记录
	void clearChat(const QString& userId);

	// 获取指定群组的聊天记录
	std::shared_ptr<ChatRecordMessage> getGroupChat(const QString& groupId);
	// 添加新群组聊天记录
	void addGroupChat(const QString& groupId, std::shared_ptr<ChatRecordMessage> chatMessage);
	// 获取并更新指定群组的聊天记录，返回ChatMessage对象
	ChatRecordMessage* updateGroupChat(const QString& groupId);
	// 清除指定群组的聊天记录
	void clearGroupChat(const QString& groupId);

	// 清空所有聊天记录
	void clearAllChats();

private:
	ChatRecordManager() {};
	QHash<QString, std::shared_ptr<ChatRecordMessage>> m_friendChats;
	QHash<QString, std::shared_ptr<ChatRecordMessage>> m_groupChats;
};

#endif // CHATRECORDMANAGER_H_

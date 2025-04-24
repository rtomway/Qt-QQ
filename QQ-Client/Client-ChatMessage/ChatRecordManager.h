#ifndef CHATRECORDMANAGER_H_
#define CHATRECORDMANAGER_H_

#include <QHash>
#include <memory>
#include "ChatRecordMessage.h"
#include "GlobalTypes.h"

class ChatRecordManager {
public:
	// 获取单例实例的静态方法
	static ChatRecordManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	ChatRecordManager(const ChatRecordManager&) = delete;
	ChatRecordManager& operator=(const ChatRecordManager&) = delete;
	//聊天记录操作
	void addMessageToChat(const ChatMessage& chatMessage);
	std::shared_ptr<ChatRecordMessage> getChatRecord(const QString& userId, ChatType type);
	//用户
	void addUserChat(const QString& userId, std::shared_ptr<ChatRecordMessage> chatMessage);
	void clearUserChat(const QString& userId);
	//群组
	void addGroupChat(const QString& groupId, std::shared_ptr<ChatRecordMessage> chatMessage);
	void clearGroupChat(const QString& groupId);
	// 清空所有聊天记录
	void clearAllChats();
private:
	ChatRecordManager() {};
	QHash<QString, std::shared_ptr<ChatRecordMessage>> m_friendChats;
	QHash<QString, std::shared_ptr<ChatRecordMessage>> m_groupChats;
};

#endif // CHATRECORDMANAGER_H_

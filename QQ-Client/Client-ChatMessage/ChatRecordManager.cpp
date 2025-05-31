#include "ChatRecordManager.h"
#include "LoginUserManager.h"


ChatRecordManager* ChatRecordManager::instance() 
{
	static ChatRecordManager ins;
	return &ins;
}

// 添加新的聊天记录
void ChatRecordManager::addUserChat(const QString& userId, std::shared_ptr<ChatRecordMessage> chatMessage)
{
	if (!m_friendChats.contains(userId)) 
	{
		m_friendChats[userId] = chatMessage;
	}
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
void ChatRecordManager::addMessageToChat(const ChatMessage& chatMessage)
{
	// 创建消息指针
	std::shared_ptr<MessageRecord> message;
	switch (chatMessage.messageType)
	{
	case MessageType::Text:
	{
		// 创建文本消息
		QString text = chatMessage.data.toString();
		message = std::make_shared<TextMessage>(chatMessage.sendId, chatMessage.receiveId, chatMessage.time, text);
		break;
	}
	case MessageType::Image:
	{
		// 创建图片消息
		QPixmap pixmap = chatMessage.data.value<QPixmap>();
		message = std::make_shared<ImageMessage>(chatMessage.sendId, chatMessage.receiveId, chatMessage.time, pixmap);
		break;
	}
	case MessageType::System:
	{
		// 创建系统消息
		QString text = chatMessage.data.toString();
		message = std::make_shared<SystemMessage>(chatMessage.sendId, chatMessage.receiveId, chatMessage.time, text);
		break;
	}
	default:
		break;
	}

	//找到相应聊天记录存储
	QString getChatId;
	auto& loginUserId = LoginUserManager::instance()->getLoginUserID();
	if (chatMessage.chatType == ChatType::User && chatMessage.receiveId == loginUserId)
	{
		getChatId = chatMessage.sendId;
	}
	else
	{
		getChatId = chatMessage.receiveId;
	}
	auto chatRecord = getChatRecord(getChatId, chatMessage.chatType);
	if (chatRecord)
	{
		chatRecord->addMessage(message);
	}
	else
	{
		qDebug() << "聊天记录不存在:" << getChatId;
	}

}

//获取聊天记录
std::shared_ptr<ChatRecordMessage> ChatRecordManager::getChatRecord(const QString& id, ChatType type)
{
	switch (type)
	{
	case ChatType::User:
		return m_friendChats.value(id, nullptr);
		break;
	case ChatType::Group:
		return m_groupChats.value(id, nullptr);
		break;
	default:
		break;
	}
	return std::shared_ptr<ChatRecordMessage>();
}

// 清除指定用户的聊天记录
void ChatRecordManager::clearUserChat(const QString& userId)
{
	m_friendChats.remove(userId);
}

//清空指定群组聊天记录
void ChatRecordManager::clearGroupChat(const QString& groupId)
{
	m_groupChats.remove(groupId);
}

// 清空所有聊天记录
void ChatRecordManager::clearAllChats() 
{
	m_friendChats.clear();
	m_groupChats.clear();
}

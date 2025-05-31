#include "ChatRecordMessage.h"

ChatRecordMessage::ChatRecordMessage(QString senderId, QString receiveId, ChatType type)
	: m_senderId(senderId), m_receiveId(receiveId), m_type(type)
{

}

// 添加消息
void ChatRecordMessage::addMessage(const std::shared_ptr<MessageRecord>& message) 
{
	m_messages.append(message);
}

// 获取所有消息
const QList<std::shared_ptr<MessageRecord>>& ChatRecordMessage::getMessages() const
{
	return m_messages;
}

//获取聊天记录类型
ChatType ChatRecordMessage::type() const
{
	return m_type;
}

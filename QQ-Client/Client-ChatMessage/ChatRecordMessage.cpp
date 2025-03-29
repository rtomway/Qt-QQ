#include "ChatRecordMessage.h"

ChatRecordMessage::ChatRecordMessage(QString senderId, QString receiveId, ChatType type)
	: m_senderId(senderId), m_receiveId(receiveId), m_type(type)
{

}
// 添加消息
void ChatRecordMessage::addMessage(std::shared_ptr<MessageRecord> msg) {
	m_messages.append(msg);
}
// 获取所有消息
const QList<std::shared_ptr<MessageRecord>>& ChatRecordMessage::getMessages() const {
	return m_messages;
}

ChatType ChatRecordMessage::type() const
{
	return m_type;
}

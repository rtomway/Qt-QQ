#include "ChatMessage.h"

ChatMessage::ChatMessage()
{

}

ChatMessage::ChatMessage(QString user_id, QString friend_id)
	:m_user_id(user_id)
	,m_friend_id(friend_id)
{

}
void ChatMessage::addMessage(const Message& msg)
{
	m_messages.append(msg);
}

QList<Message> ChatMessage::getMessage()const
{
	return m_messages;
}

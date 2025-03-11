#pragma once
#include "Message.h"

Message::Message(QString sender, QString receiver, QString message, QDateTime time)
	:m_sender_id(sender)
	,m_receiver_id(receiver)
	,m_message(message)
	,m_time(time)
	,m_isRead(false)
{

}
//消息已读
void Message::readMessage()
{
	m_isRead = true;
}

QString Message::getSenderId()
{
	return m_sender_id;
}

QString Message::getMessage()
{
	return m_message;
}

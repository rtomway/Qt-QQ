#include "MessageRecord.h"

MessageRecord::MessageRecord(QString sender, QString receiver, QDateTime time)
	: m_sender_id(sender)
	, m_receiver_id(receiver)
	, m_time(time)
	, m_isRead(false)
{

}

//消息已读
void MessageRecord::readMessage()
{
	m_isRead = true;
}

// 获取发送者ID
QString MessageRecord::getSenderId()
{
	return m_sender_id;
}

//获取消息时间
QString MessageRecord::getTime() const
{
	return m_time.toString("MM-dd hh:mm");
}

//文字
TextMessage::TextMessage(QString sender, QString receiver, QDateTime time, QString text)
	:MessageRecord(sender, receiver, time)
	, m_textMessage(text)
{

}
const QString& TextMessage::getTextMessage() const
{
	return m_textMessage;
}
MessageType TextMessage::getType() const
{
	return MessageType::Text;
}

//图片
ImageMessage::ImageMessage(QString sender, QString receiver, QDateTime time, QPixmap image)
	:MessageRecord(sender, receiver, time)
	, m_imageMessage(image)
{

}
const QPixmap& ImageMessage::getImageMessage() const
{
	return m_imageMessage;
}
MessageType ImageMessage::getType() const
{
	return MessageType::Image;
}

//系统
SystemMessage::SystemMessage(QString sender, QString receiver, QDateTime time, QString systemMessage)
	:MessageRecord(sender, receiver, time)
	, m_systemMessage(systemMessage)
{

}
const QString& SystemMessage::getSystemMessage() const
{
	return m_systemMessage;
}
MessageType SystemMessage::getType() const
{
	return MessageType::System;
}

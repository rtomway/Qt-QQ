#pragma once
#include "Message.h"

Message::Message(QString sender, QString receiver, QDateTime time)
	:m_sender_id(sender)
	, m_receiver_id(receiver)
	, m_time(time)
	, m_isRead(false)
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
QString Message::getContent() const
{
	return QString();
}
QPixmap Message::getImageMessage() const
{
	return QPixmap();
}
QString Message::getType() const
{
	return QString();
}
//文字
TextMessage::TextMessage(QString sender, QString receiver, QDateTime time, QString text)
	:Message(sender, receiver, time), m_textMessage(text)  // 调用基类构造函数
{

}

QString TextMessage::getTextMessage() const
{
	return m_textMessage;
}
//图片
ImageMessage::ImageMessage(QString sender, QString receiver, QDateTime time, QPixmap image)
	:Message(sender, receiver, time), m_imageMessage(image)  // 调用基类构造函数
{

}

QPixmap ImageMessage::getImageMessage() const
{
	return m_imageMessage;
}

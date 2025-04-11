#pragma once
#include "MessageRecord.h"

MessageRecord::MessageRecord(QString sender, QString receiver, QDateTime time)
	:m_sender_id(sender)
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
// 获取消息类型
QString MessageRecord::getType() const
{
	return QString();
}
//文字
TextMessage::TextMessage(QString sender, QString receiver, QDateTime time, QString text)
	:MessageRecord(sender, receiver, time), m_textMessage(text)  // 调用基类构造函数
{

}

QString TextMessage::getTextMessage() const
{
	return m_textMessage;
}
//图片
ImageMessage::ImageMessage(QString sender, QString receiver, QDateTime time, QPixmap image)
	:MessageRecord(sender, receiver, time), m_imageMessage(image)  // 调用基类构造函数
{

}

QPixmap ImageMessage::getImageMessage() const
{
	return m_imageMessage;
}

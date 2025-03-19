#ifndef MESSAGE_H_
#define MESSAGE_H_
#pragma once
#include <QString>
#include <QDateTime>
#include <QPixmap>

class Message {
public:
	Message(QString sender, QString receiver, QDateTime time);
	virtual ~Message() = default;
	// 消息已读
	void readMessage();
	// 获取发送者ID
	QString getSenderId();
	// 获取文本消息
	virtual QString getContent() const;
	// 获取图片消息
	virtual QPixmap getImageMessage() const;
	// 获取消息类型
	virtual QString getType() const;
protected:
	QString m_sender_id;
	QString m_receiver_id;
	QDateTime m_time;
	bool m_isRead;
	QString m_textMessage;
	QPixmap m_imageMessage;
};
// 文本消息类
class TextMessage : public Message {
public:
	TextMessage(QString sender, QString receiver, QDateTime time, QString text);
	QString getTextMessage() const;
private:
	QString m_textMessage;
};

// 图片消息类
class ImageMessage : public Message {
public:
	ImageMessage(QString sender, QString receiver, QDateTime time, QPixmap image);
	QPixmap getImageMessage() const;
private:
	QPixmap m_imageMessage;
};

#endif // !MESSAGE_H_


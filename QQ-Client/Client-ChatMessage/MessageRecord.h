#ifndef MESSAGERECORD_H_
#define MESSAGERECORD_H_
#pragma once
#include <QString>
#include <QDateTime>
#include <QPixmap>

class MessageRecord {
public:
	MessageRecord(QString sender, QString receiver, QDateTime time);
	virtual ~MessageRecord() = default;
	void readMessage();
	QString getSenderId();
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
class TextMessage : public MessageRecord {
public:
	TextMessage(QString sender, QString receiver, QDateTime time, QString text);
	QString getTextMessage() const;
private:
	QString m_textMessage;
};
// 图片消息类
class ImageMessage : public MessageRecord {
public:
	ImageMessage(QString sender, QString receiver, QDateTime time, QPixmap image);
	QPixmap getImageMessage() const;
private:
	QPixmap m_imageMessage;
};

#endif // !MESSAGERECORD_H_


#ifndef MESSAGERECORD_H_
#define MESSAGERECORD_H_
#include <QString>
#include <QDateTime>
#include <QPixmap>
#include "GlobalTypes.h"

enum class MessageType
{
	Text,
	Image,
	System
};
struct ChatMessage
{
	QString sendId;
	QString receiveId;
	QDateTime time;
	QVariant data;
	MessageType messageType;
	ChatType chatType;
};

class MessageRecord
{
public:
	MessageRecord(QString sender, QString receiver, QDateTime time);
	virtual ~MessageRecord() = default;
	void readMessage();
	QString getSenderId();
	// 获取消息类型
	virtual MessageType getType() const = 0;
	//获取消息时间
	QString getTime()const;
protected:
	QString m_sender_id;
	QString m_receiver_id;
	QDateTime m_time;
	bool m_isRead;
};
// 文本消息类
class TextMessage : public MessageRecord
{
public:
	TextMessage(QString sender, QString receiver, QDateTime time, QString textMessage);
	const QString& getTextMessage() const;
	virtual MessageType getType() const override;
private:
	QString m_textMessage;
};
// 图片消息类
class ImageMessage : public MessageRecord
{
public:
	ImageMessage(QString sender, QString receiver, QDateTime time, QPixmap imageMessage);
	const QPixmap& getImageMessage() const;
	virtual MessageType getType() const override;
private:
	QPixmap m_imageMessage;
};
//系统消息
class SystemMessage :public MessageRecord
{
public:
	SystemMessage(QString sender, QString receiver, QDateTime time, QString systemMessage);
	const QString& getSystemMessage() const;
	virtual MessageType getType() const override;
private:
	QString m_systemMessage;

};

#endif // !MESSAGERECORD_H_


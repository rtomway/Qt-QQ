#ifndef CHATRECORDMESSAGE_H
#define CHATRECORDMESSAGE_H

#include <QString>
#include <QList>
#include <memory> 

#include "MessageRecord.h"
#include "GlobalTypes.h"

class ChatRecordMessage {
public:
	ChatRecordMessage(QString user_id, QString friend_id, ChatType type);

	// 添加消息
	void addMessage(std::shared_ptr<MessageRecord> msg);
	// 获取所有消息
	const QList<std::shared_ptr<MessageRecord>>& getMessages() const;
	ChatType type()const;

private:
	QString m_senderId;
	QString m_receiveId;
	ChatType m_type;
	QList<std::shared_ptr<MessageRecord>> m_messages;
};

#endif // CHATRECORDMESSAGE_H

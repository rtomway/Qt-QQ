#ifndef CHATRECORDMESSAGE_H
#define CHATRECORDMESSAGE_H

#include <QString>
#include <QList>
#include <memory> 

#include "MessageRecord.h"
#include "GlobalTypes.h"

class ChatRecordMessage 
{
public:
	ChatRecordMessage(QString user_id, QString friend_id, ChatType type);
	void addMessage(const std::shared_ptr<MessageRecord>& message);
	const QList<std::shared_ptr<MessageRecord>>& getMessages() const;
	ChatType type()const;
private:
	QString m_senderId;
	QString m_receiveId;
	ChatType m_type;
	QList<std::shared_ptr<MessageRecord>> m_messages;
};

#endif // CHATRECORDMESSAGE_H

#ifndef CHATMESSAGE_H_
#define CHATMESSAGE_H_

#include "Message.h"

class ChatMessage
{
public:
	ChatMessage();
	ChatMessage(QString user_id,QString friend_id);
	void addMessage(const Message&msg);
	QList<Message> getMessage()const;
private:
	QString m_user_id;
	QString m_friend_id;
	QList<Message> m_messages;

};

#endif // !CHATMESSAGE_H_

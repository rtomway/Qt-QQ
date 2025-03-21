#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QString>
#include <QList>
#include <memory> 
#include "Message.h"

class ChatMessage {
public:
	ChatMessage(QString user_id, QString friend_id);

	// 添加消息
	void addMessage(std::shared_ptr<Message> msg);

	// 获取所有消息
	const QList<std::shared_ptr<Message>>& getMessages() const;

private:
	QString m_user_id;     
	QString m_friend_id;   
	QList<std::shared_ptr<Message>> m_messages; 
};

#endif // CHATMESSAGE_H

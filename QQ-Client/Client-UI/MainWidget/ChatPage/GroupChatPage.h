#ifndef GROUPCHATPAGE_H_
#define GROUPCHATPAGE_H_

#include "ChatPage.h"
#include "Group.h"
#include <QWidget>
#include <QMenu>
#include <QPixmap>
#include <memory>
#include <QPropertyAnimation>

#include "ChatRecordMessage.h"
#include "FriendSetWidget.h"
#include "MessageBubble.h"

class GroupChatPage :public ChatPage
{
	Q_OBJECT
public:
	GroupChatPage(QWidget* parent = nullptr);
	~GroupChatPage();
public:
	//重写
	void setChatWidget(const QString& id);
	void storeMessage(const QString& sender_id, const QString& receiver_id, const std::shared_ptr<MessageRecord>& message);
	bool isCurrentChat(const QString& id, ChatType type) const;
	void createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id = QString());
	void createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id = QString());
private:
	void init();
private:
	QSharedPointer<Group>m_group{};
	std::shared_ptr<ChatRecordMessage>m_chat;
private:
	QStringList m_imageMessagePath{};
	QString m_textMessage{};
	bool m_isImageInTextEdit{ false };
};

#endif // !GROUPCHATPAGE_H_

#ifndef CHATPAGE
#define CHATPAGE

#include <QWidget>
#include <QMenu>
#include <QPixmap>
#include <memory>
#include <QPropertyAnimation>

#include "ChatRecordMessage.h"
#include "Friend.h"
#include "Group.h"
#include "FriendSetWidget.h"
#include "MessageBubble.h"

namespace Ui { class ChatPage; }

class ChatPage :public QWidget
{
public:
	ChatPage(QWidget* parent = nullptr);
	~ChatPage();
public:
	virtual bool isCurrentChat(const QString& id, ChatType type) const = 0;
	void updateReciveMessage(const QString& user_id, const QString& message);
	void updateReciveMessage(const QString& user_id, const QPixmap& pixmap);
	void clearMessageWidget();
	void updateChatMessage(const QString& sender_id, const QString& receiver_id, const QVariant& msg);
private:
	void init();
protected:
	Ui::ChatPage* ui{};
	virtual void setChatWidget(const QString&id)=0;
	virtual void storeMessage(const QString& sender_id, const QString& receiver_id, const std::shared_ptr<MessageRecord>& message) = 0;
	virtual void createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id=QString()) = 0;
	virtual void createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id=QString()) = 0;
protected:
	void loadChatMessage(const ChatRecordMessage& chatMessage);
	void installEventFilterForChildren(QWidget* parent);
protected:
	QSharedPointer<Friend>m_oneself{};
};

#endif // !CHATPAGE
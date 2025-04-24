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
#include "MessageRecord.h"

namespace Ui { class ChatPage; }

class ChatPage :public QWidget
{
	Q_OBJECT
public:
	ChatPage(QWidget* parent = nullptr);
	~ChatPage();
public:
	virtual bool isCurrentChat(const QString& id) const = 0;
	void updateReceiveMessage(const QString& send_id, const QVariant& message, MessageType type);
	void clearMessageWidget();
private:
	void init();
protected:
	Ui::ChatPage* ui{};
	virtual void setChatWidget(const QString& id) = 0;
	virtual void refreshChatWidget() = 0;
	virtual void createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id = QString()) = 0;
	virtual void createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id = QString()) = 0;
	virtual void insertTipMessage(const QString& text) = 0;
protected:
	void loadChatMessage(const ChatRecordMessage& chatMessage);
protected:
	QSharedPointer<Friend>m_loginUser{};
	QString m_title{};
	bool m_currentChat{ false };
};

#endif // !CHATPAGE
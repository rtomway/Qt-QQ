#ifndef FRIENDCHATPAGE_H_
#define FRIENDCHATPAGE_H_

#include "ChatPage.h"
#include <QWidget>
#include <QMenu>
#include <QPixmap>
#include <memory>
#include <QPropertyAnimation>

#include "Friend.h"
#include "ChatRecordMessage.h"
#include "MessageBubble.h"
#include "FriendSetPannelWidget.h"

class FriendChatPage :public ChatPage
{
	Q_OBJECT
public:
	FriendChatPage(QWidget* parent = nullptr);
	~FriendChatPage();
public:
	//重写
	void setChatWidget(const QString& id);
	void refreshChatWidget()override;
	bool isCurrentChat(const QString& id) const;
	void createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id = QString());
	void createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id = QString());
	void insertTipMessage(const QString& text);
	void clearChatPage();
private:
	void init();
	void sendImageMessageToServer(const QString& user_id, const QPixmap& pixmap);
	void sendTextMessageToServer(const QString& user_id, const QPixmap& pixmap);
private:
	QSharedPointer<Friend>m_friend{};
	std::shared_ptr<ChatRecordMessage>m_chat{};
private:
	QString m_textMessage{};
	FriendSetPannelWidget* m_friendPannel{};
};

#endif // !FRIENDCHATPAGE_H_

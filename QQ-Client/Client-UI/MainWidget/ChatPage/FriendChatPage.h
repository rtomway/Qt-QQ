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
#include "FriendSetWidget.h"
#include "MessageBubble.h"

class FriendChatPage :public ChatPage
{
	Q_OBJECT
public:
	FriendChatPage(QWidget* parent = nullptr);
	~FriendChatPage();
public:
	//重写
	void setChatWidget(const QString& id);
	void storeMessage(const QString& sender_id, const QString& receiver_id, const std::shared_ptr<MessageRecord>& message);
	bool isCurrentChat(const QString& id, ChatType type) const;
	void createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id = QString());
	void createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id = QString());
private:
	void init();
	void installEventFilterForChildren(QWidget* parent);
private:
	QSharedPointer<Friend>m_friend{};
	std::shared_ptr<ChatRecordMessage>m_chat;
	FriendSetWidget* m_setWidget{};
	QPropertyAnimation* m_animation{};
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* ev)override;
private:
	QStringList m_imageMessagePath{};
	QString m_textMessage{};
	bool m_isImageInTextEdit{ false };
};

#endif // !FRIENDCHATPAGE_H_

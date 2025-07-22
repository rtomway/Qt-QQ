#ifndef GROUPCHATPAGE_H_
#define GROUPCHATPAGE_H_

#include "ChatPage.h"
#include "Group.h"
#include <QWidget>
#include <QMenu>
#include <QPixmap>
#include <memory>
#include <QPropertyAnimation>
#include <QWindow>

#include "ChatRecordMessage.h"
#include "MessageBubble.h"
#include "GroupSetPannelWidget.h"


class GroupChatPage :public ChatPage
{
	Q_OBJECT
public:
	GroupChatPage(QWidget* parent = nullptr);
	~GroupChatPage();
private:
	void init();
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
	void sendImageMessageToServer(const QString& user_id, const QPixmap& headPix);
	void sendTextMessageToServer(const QString& user_id, const QPixmap& headPix);
private:
	QSharedPointer<Group>m_group{};
	std::shared_ptr<ChatRecordMessage>m_chat{};
private:
	QString m_textMessage{};
	GroupSetPannelWidget* m_groupPannel{};
};

#endif // !GROUPCHATPAGE_H_

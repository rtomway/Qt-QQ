#ifndef MESSAGEPAGE_H_
#define MESSAGEPAGE_H_

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

namespace Ui { class MessagePage; }

class MessagePage :public QWidget
{
	Q_OBJECT
public:
	MessagePage(QWidget* parent = nullptr);
	~MessagePage();
public:
	void loadMessagePage(const QString& id, ChatType type);
	void initFriedMessaePage();
	void initGroupMessaePage();
	void loadFriendChatMessage(const ChatRecordMessage& chatMessage);
	void loadGroupChatMessage(const ChatRecordMessage& chatMessage);
	bool isCurrentChat(const QString& id, ChatType type) const;

	void updateReciveMessage(const QString& message);
	void updateReciveMessage(const QPixmap& pixmap);
	void updateChatMessage(ChatType type, const QString& sender_id, const QString& receiver_id, const QVariant& msg);
	void clearMessageWidget();
private:
	void init();
	void loadChatMessage(const ChatRecordMessage& chatMessage);
	void createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& memberName = QString(), const QString& memberRole = QString());
	void createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& memberName = QString(), const QString& memberRole = QString());
	void installEventFilterForChildren(QWidget* parent);
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* ev)override;
private:
	Ui::MessagePage* ui{};
private:
	QSharedPointer<Friend>m_friend{};
	QSharedPointer<Friend>m_oneself{};
	QSharedPointer<Group>m_group{};
	std::shared_ptr<ChatRecordMessage>m_chat;
	FriendSetWidget* m_setWidget{};
	QPropertyAnimation* m_animation{};
private:
	QStringList m_imageMessagePath{};
	QString m_textMessage{};
	bool m_isImageInTextEdit{ false };
};





#endif // !MESSAGEPAGE_H_

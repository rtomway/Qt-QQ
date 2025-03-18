#ifndef MESSAGEPAGE_H_
#define MESSAGEPAGE_H_

#include <QWidget>
#include <QMenu>
#include <QPixmap>
#include <memory>
#include <QDockWidget>
#include <QPropertyAnimation>

#include "ChatMessage.h"
#include "Friend.h"
#include "FriendSetWidget.h"
#include "MessageBubble.h"

namespace Ui { class MessagePage; }

class MessagePage :public QWidget
{
	Q_OBJECT
public:
	MessagePage(QWidget* parent = nullptr);
	~MessagePage();
	void init();
	void setCurrentUser(const QJsonObject& obj);
	//void setUser(const QJsonObject& obj);
	QString getCurrentID();
	void updateReciveMessage(const QString& message);
	void updateChatMessage(const QString& sender_id, const QString& receiver_id, const QVariant& msg);
	void clearMessageWidget();
private:
	void updateMessageWidget();
	void loadChatMessage(const ChatMessage& chatMessage);
	void createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType);
	void createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType);
	void installEventFilterForChildren(QWidget* parent);
private:
	QString m_friend_username{};
	QPixmap m_friend_headPix{};
	QString m_friend_id{};
	QString m_currentID{};
	QSharedPointer<Friend>m_oneself{};
	FriendSetWidget* m_setWidget{};
	QPropertyAnimation* m_animation{};
	QPropertyAnimation* m_opacityAnimation{};
private:
	QString m_imageMessagePath{};
	QString m_textMessage{};
private:
	Ui::MessagePage* ui{};
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* ev)override;
};





#endif // !MESSAGEPAGE_H_

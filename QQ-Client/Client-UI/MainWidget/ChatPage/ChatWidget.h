#ifndef CHATWIDGET_H_
#define CHATWIDGET_H_

#include <QWidget>
#include <QStackedWidget>

#include "FriendChatPage.h"
#include "GroupChatPage.h"
#include "ChatPage.h"
#include "MessageRecord.h"

class ChatWidget :public QWidget
{
	Q_OBJECT
public:
	ChatWidget(QWidget* parent = nullptr);
public:
	void loadChatPage(ChatType type, const QString& id);
	bool isCurrentChat(ChatType type, const QString& id);
	bool isStackedCurrentChat(ChatType type, const QString& id);
	void updateReceiveMessage(const ChatMessage& chatMessage);
	void clearChatWidget();
private:
	void init();
	void initStackedWidget();
private:
	QStackedWidget* m_stackedChatWidget{};
	FriendChatPage* m_friendChatPage{};
	GroupChatPage* m_groupChatPage{};
	ChatPage* m_currentPage{};
};

#endif // !CHATWIDGET_H_

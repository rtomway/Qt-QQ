#ifndef CHATWIDGET_H_
#define CHATWIDGET_H_

#include <QWidget>
#include <QStackedWidget>

#include "FriendChatPage.h"
#include "GroupChatPage.h"
#include "ChatPage.h"

class ChatWidget :public QWidget
{
	Q_OBJECT
public:
	ChatWidget(QWidget* parent = nullptr);
public:
	void loadChatPage(ChatType type, const QString& id);
	bool isCurrentChat(ChatType type, const QString& id);
	void updateReceiveMessage(const QString& user_id, const QString& message);
	void updateReceiveMessage(const QString& user_id, const QPixmap& pixmap);
	void clearChatWidget();
	void updateChatMessage(ChatType type, const QString& sender_id, const QString& receiver_id, const QVariant& msg);
private:
	void init();
	void initStackedWidget();
private:
	QStackedWidget* m_stackedChatWidget{};
	FriendChatPage* m_friendChatPage{};
	GroupChatPage* m_groupChatPage{};
	ChatPage* m_currentPage = nullptr;
};

#endif // !CHATWIDGET_H_

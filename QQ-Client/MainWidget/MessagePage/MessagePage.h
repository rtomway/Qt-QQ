#ifndef MESSAGEPAGE_H_
#define MESSAGEPAGE_H_

#include <QWidget>
#include <QMenu>
#include <QPixmap>
#include "ChatMessage.h"
namespace Ui { class MessagePage; }

class MessagePage :public QWidget
{
	Q_OBJECT
public:
	MessagePage(QWidget* parent = nullptr);
	~MessagePage();
	void init();             
	void setCurrentUser(const QJsonObject& obj);
	void setUser(const QJsonObject& obj);
	QString getCurrentID();
	void updateReciveMessage(const QString&message);
	void updateChatMessage(const QString& sender_id, const QString& receiver_id, const QString& msg);
	void clearMessageWidget();
private:
	void updateMessageWidget();
	void loadChatMessage(const ChatMessage& chatMessage);
private:
	QString m_friend_username{};
	QPixmap m_friend_headPix{};
	QString m_friend_id{};
	QString m_currentID{};
	QHash<QString, ChatMessage> m_chats;
private:
	Ui::MessagePage* ui{};
};





#endif // !MESSAGEPAGE_H_

﻿#ifndef MESSAGELISTITEM_H_
#define MESSAGELISTITEM_H_

#include <QWidget>
#include <QJsonObject>


namespace Ui { class MessageListItem; }

class MessageListItem :public QWidget
{
	Q_OBJECT
public:
	MessageListItem(QWidget* parent = nullptr);
private:
	void init();
	void updateItemWidget();
	QString covertToChinese(const QString& date);
public:
	void setUser(const QJsonObject& obj);
	QJsonObject getUser();
	QString getId();
	void clearUnreadMessage();
private:
	QWidget* m_leftWidget{};
	QWidget* m_messageAttention{};
	Ui::MessageListItem* ui{};
private:
	QString m_username;
	QString m_user_id;
	QStringList m_unReadMessage;
};


#endif // !MESSAGELISTITEM_H_

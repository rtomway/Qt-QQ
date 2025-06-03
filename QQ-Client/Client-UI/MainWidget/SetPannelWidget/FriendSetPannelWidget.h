#ifndef FRIENDSETPANNELWIDGET_H_
#define FRIENDSETPANNELWIDGET_H_

#include <QWidget>
#include <QPushButton>
#include "SetPannelWidget.h"

class FriendSetPannelWidget :public QWidget
{
	Q_OBJECT
public:
	FriendSetPannelWidget(QWidget* parent = nullptr);
	~FriendSetPannelWidget();
public:
	void loadFriendPannel(const QString& friend_id);
private:
	void init();
	void deleteFriend();
private:
	SetPannelWidget* m_pannelContains{};
	QString m_friend_id{};
	QPushButton* m_deleteBtn;
};

#endif // !FRIENDSETPANNELWIDGET_H_

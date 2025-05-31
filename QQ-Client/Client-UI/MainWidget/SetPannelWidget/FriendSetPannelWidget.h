#ifndef FRIENDSETPANNELWIDGET_H_
#define FRIENDSETPANNELWIDGET_H_

#include <QWidget>
#include "SetPannelWidget.h"

class FriendSetPannelWidget :public QWidget
{
	Q_OBJECT
public:
	FriendSetPannelWidget(QWidget* parent = nullptr);
	~FriendSetPannelWidget();
public:
	void loadFriendPannel(const QString&friend_id);
private:
	void init();
private:
	SetPannelWidget* m_pannelContains{};
	QString m_friend_id{};
};

#endif // !FRIENDSETPANNELWIDGET_H_

#ifndef FRIENDLISTITEMWIDGET_H_
#define FRIENDLISTITEMWIDGET_H_

#include "ItemWidget.h"
#include "Friend.h"
class FriendListItemWidget :public ItemWidget
{
	Q_OBJECT
public:
	FriendListItemWidget(QWidget* parent = nullptr);
	virtual void setItemWidget(const QString& user_id)override;
	void setGrouping(const QString& grouping);
private:
	void init();
	void refershItemWidget();
private:
	QSharedPointer<Friend>m_friend = nullptr;
	QString m_friendId{};
	QJsonObject m_friendJson{};
};



#endif // !FRIENDLISTITEMWIDGET_H_

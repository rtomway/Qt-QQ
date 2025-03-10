#ifndef FRIENDNOTICEITEMWIDGET_H_
#define FRIENDNOTICEITEMWIDGET_H_

#include <QWidget>
#include "AngleRoundedWidget.h"
#include "AddWidget.h"

namespace Ui { class FriendNoticeItemWidget; }

class FriendNoticeItemWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	FriendNoticeItemWidget(QWidget* parent = nullptr);
	void init();
	~FriendNoticeItemWidget();
	void setUser(const QJsonObject& obj);
	void setPixmap(const QPixmap& pixmap);
private:
	Ui::FriendNoticeItemWidget* ui{};
	QString m_userName{};
	QString m_user_id{};
	QPixmap m_userHead{};
	std::unique_ptr<AddWidget> m_addWidget;
signals:
	void newlyFriend(const QJsonObject& obj);
};

#endif // !FRIENDNOTICEITEMWIDGET_H_

#ifndef FRIENDNOTICEITEMWIDGET_H_
#define FRIENDNOTICEITEMWIDGET_H_

#include <QWidget>
#include "AngleRoundedWidget.h"

namespace Ui { class FriendNoticeItemWidget;}

class FriendNoticeItemWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	FriendNoticeItemWidget(QWidget* parent = nullptr);
	void init();
	~FriendNoticeItemWidget();
	void setUser(const QJsonObject&obj);
private:
	Ui::FriendNoticeItemWidget* ui{};
	QString m_userName{};
	QString m_user_id{};
	QPixmap m_userHead{};

};

#endif // !FRIENDNOTICEITEMWIDGET_H_

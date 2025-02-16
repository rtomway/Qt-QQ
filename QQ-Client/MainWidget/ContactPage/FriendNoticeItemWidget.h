#ifndef FRIENDNOTICEITEMWIDGET_H_
#define FRIENDNOTICEITEMWIDGET_H_

#include <QWidget>

namespace Ui { class FriendNoticeItemWidget; }

class FriendNoticeItemWidget :public QWidget
{
	Q_OBJECT
public:
	FriendNoticeItemWidget(QWidget* parent = nullptr);
	void init();
	~FriendNoticeItemWidget();
private:
	Ui::FriendNoticeItemWidget* ui{};
};

#endif // !FRIENDNOTICEITEMWIDGET_H_

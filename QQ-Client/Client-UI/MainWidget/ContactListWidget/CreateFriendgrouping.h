#ifndef CREATEFRIENDGROUPING
#define CREATEFRIENDGROUPING

#include <QWidget>
#include "AngleRoundedWidget.h"

namespace Ui { class CreateFriendgrouping; }

class CreateFriendgrouping :public AngleRoundedWidget
{
	Q_OBJECT
public:
	CreateFriendgrouping(QWidget* parent = nullptr);
private:
	void init();
private:
	Ui::CreateFriendgrouping* ui{};
signals:
	void createGrouping(const QString&grouping);
};

#endif // !CREATEFRIENDGROUPING

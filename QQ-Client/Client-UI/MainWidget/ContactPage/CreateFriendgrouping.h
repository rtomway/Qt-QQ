#ifndef CREATEFRIENDGROUPING
#define CREATEFRIENDGROUPING

#include <QWidget>

namespace Ui { class CreateFriendgrouping; }
class CreateFriendgrouping :public QWidget
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

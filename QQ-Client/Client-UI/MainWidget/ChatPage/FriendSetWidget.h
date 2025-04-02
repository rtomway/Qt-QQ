#ifndef FRIENDSETWIDGET_H_
#define FRIENDSETWIDGET_H_

namespace Ui { class FriendSetWidget; }

#include <QWidget>
class FriendSetWidget :public QWidget
{
	Q_OBJECT
public:
	FriendSetWidget(QWidget* parent = nullptr);
private:
	void init();
public:
	void setId(const QString& user_id);
private:
	Ui::FriendSetWidget* ui{};
	QString m_userId{};
};


#endif // !FRIENDSETWIDGET_H_

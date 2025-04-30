#ifndef ADDFRIENDWIDGET_H_
#define ADDFRIENDWIDGET_H_

#include <QWidget>
#include <QListWidget>


enum search_type{
	User,
	Grouop
};

namespace Ui { class AddFriendWidget; }

class AddFriendWidget : public QWidget
{
	Q_OBJECT
public:
	AddFriendWidget(QWidget* parent = nullptr);
	~AddFriendWidget();
	void init();

private:
	search_type m_seatchType{search_type::User};
	QListWidget* m_userList{};
	QListWidget* m_groupList{};
	QString m_lastUserId{};
	QString m_lastGroupId{};
private:
	void addListWidgetItem(QListWidget*list,const QJsonObject&obj, const QPixmap& pixmap);
private:
	Ui::AddFriendWidget* ui{};
	
};

#endif // !ADDFRIENDWIDGET_H_

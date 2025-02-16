#ifndef CONTACTLIST_H_
#define CONTACTLIST_H_

#include <QWidget>
#include <QTreeWidget>
#include <QStringList>
#include <QButtonGroup>
#include "TopItemWidget.h"

namespace Ui { class ContactList; }
class ContactList :public QWidget
{
	Q_OBJECT
public:
	~ContactList();
	static ContactList* instance();
	// 防止复制和赋值
	ContactList(const ContactList&) = delete;
	ContactList& operator=(const ContactList&) = delete;

	void init();
private:
	ContactList(QWidget* parent = nullptr);
private:
	Ui::ContactList* ui{};
private:
	QTreeWidget* m_friendList{};
	QTreeWidget* m_groupList{};
	static QStringList m_fNamelist;
	static QStringList m_gNamelist;
	QButtonGroup m_buttonGroup{};
public:
	static QStringList getfGrouping();
	static QStringList getgGrouping();
public:
	TopItemWidget* addFriendListItem(QString friendName);
	void addFriendItem(QTreeWidgetItem*firendList,QString friendName);
	QTreeWidgetItem* getFriendTopItem(QString friendName);

	TopItemWidget* addGroupListItem(QString groupName);
	void addGroupItem(QTreeWidgetItem* groupList, QString groupName);
	QTreeWidgetItem* getGroupTopItem(QString groupName);
protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
signals:
	void clickedFriend();
	void friendNotice();
	void groupNotice();
};

#endif // !CONTACTLIST_H_

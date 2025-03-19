#ifndef CONTACTLIST_H_
#define CONTACTLIST_H_

#include <QWidget>
#include <QTreeWidget>
#include <QStringList>
#include <QButtonGroup>
#include <memory>

#include "TopItemWidget.h"
#include "CreateFriendgrouping.h"


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
	std::unique_ptr<CreateFriendgrouping>m_createFriendgrouping{};
private:
	int m_fNoticeUnreadCount = 0;
	int m_gNoticeUnreadCount = 0;
public:
	static QStringList getfGrouping();
	static QStringList getgGrouping();
public:
	TopItemWidget* addFriendListItem(QString friendName);
	void addFriendItem(QTreeWidgetItem* firendList, const QString& user_id);
	QTreeWidgetItem* getFriendTopItem(QString friendName);
	QTreeWidgetItem* findItemByIdInGroup(QTreeWidgetItem* group, const QString& userId);

	TopItemWidget* addGroupListItem(QString groupName);
	void addGroupItem(QTreeWidgetItem* groupList, QString groupName);
	QTreeWidgetItem* getGroupTopItem(QString groupName);

	void updateFriendNoticeCount();

	void clearContactList();
protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
signals:
	void clickedFriend(const QString& user_id);
	void friendNotice();
	void groupNotice();
	void updateFriendgrouping();
};

#endif // !CONTACTLIST_H_

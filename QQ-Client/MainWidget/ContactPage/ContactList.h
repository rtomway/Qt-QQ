#ifndef CONTACTLIST_H_
#define CONTACTLIST_H_

#include <QWidget>
#include <QTreeWidget>
#include <QStringList>
#include <QButtonGroup>
#include "TopItemWidget.h"

class NoticeWidget;
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
private:
	int m_fNoticeUnreadCount = 0;
	int m_gNoticeUnreadCount = 0;
public:
	static QStringList getfGrouping();
	static QStringList getgGrouping();
public:
	TopItemWidget* addFriendListItem(QString friendName);
	void addFriendItem(QTreeWidgetItem*firendList,const QJsonObject&obj);
	QTreeWidgetItem* getFriendTopItem(QString friendName);

	TopItemWidget* addGroupListItem(QString groupName);
	void addGroupItem(QTreeWidgetItem* groupList, QString groupName);
	QTreeWidgetItem* getGroupTopItem(QString groupName);
	void newlyFriendItem(const QJsonObject& obj);
protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
signals:
	void clickedFriend(const QJsonObject&obj);
	void friendNotice();
	void groupNotice();
	void agreeAddFriend(const QJsonObject& obj);
};

#endif // !CONTACTLIST_H_

#ifndef CONTACTLISTWIDGET_H_
#define CONTACTLISTWIDGET_H_

#include <QWidget>
#include <QTreeWidget>
#include <QStringList>
#include <QButtonGroup>
#include <memory>

#include "TopItemWidget.h"
#include "CreateFriendgrouping.h"

class GroupInviteWidget;

namespace Ui { class ContactListWidget; }
class ContactListWidget :public QWidget
{
	friend class GroupInviteWidget; // 允许 AnotherClass 访问私有成员
	Q_OBJECT
public:
	ContactListWidget(QWidget* parent = nullptr);
	~ContactListWidget();
private:
	void init();
public:
	//分组获取
	static QStringList getfGrouping();
	static QStringList getgGrouping();
	//好友列表
	TopItemWidget* addFriendListItem(QString friendName);
	void addFriendItem(QTreeWidgetItem* firendList, const QString& user_id);
	QTreeWidgetItem* getFriendTopItem(QString friendName);
	QTreeWidgetItem* findItemByIdInGroup(QTreeWidgetItem* group, const QString& userId);
	//群组列表
	TopItemWidget* addGroupListItem(QString groupName);
	void addGroupItem(QTreeWidgetItem* groupList, QString groupName);
	QTreeWidgetItem* getGroupTopItem(QString groupName);
	//通知
	void updateFriendNoticeCount();
	//清空
	void clearContactList();
protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
private:
	Ui::ContactListWidget* ui{};
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
signals:
	void clickedFriend(const QString& user_id);
	void friendNotice();
	void groupNotice();
	void updateFriendgrouping();
};

#endif // !CONTACTLISTWIDGET_H_

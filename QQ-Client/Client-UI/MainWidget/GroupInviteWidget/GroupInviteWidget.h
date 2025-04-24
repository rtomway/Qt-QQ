#ifndef GROUPINVITEWIDGET_H_
#define GROUPINVITEWIDGET_H_

#include <QWidget>
#include <QTreeWidget>
#include <QListWidgetItem>
#include <QListWidget>

#include "AngleRoundedWidget.h"
#include "ContactListWidget.h"

namespace Ui { class GroupInviteWidget; }

class GroupInviteWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	enum GroupMode
	{
		CreateGroup,
		InviteMembers,
		DeleteMembers
	};
public:
	GroupInviteWidget(QWidget* parent = nullptr);
	~GroupInviteWidget();
	void setGroupWidgetMode(GroupMode mode);
	void cloneFriendTree(ContactListWidget& contactListWidget);
	void cloneGroupMember(const QString& group_id);
	void setGroupid(const QString& group_id);
private:
	void init();
	void addSelectedFriendItem(QListWidget* listWidget, const QString& user_id);
	void removeFriendListItem(const QString& user_id);
	QListWidgetItem* findListItem(QListWidget* listWidget, const QString& user_id);
	QTreeWidgetItem* findTreeItem(const QString& user_id);

	void clearFriendTree();
	void clearSearchList();
private:
	Ui::GroupInviteWidget* ui{};
	QTreeWidget* m_friendTree{};
	QListWidget* m_searchList{};
	QString m_groupId{};
	QStringList m_selectedList{};
};

#endif // !GROUPINVITEWIDGET_H_

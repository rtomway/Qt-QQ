#ifndef GROUPMEMBEROPERATORWIDGET_H_
#define GROUPMEMBEROPERATORWIDGET_H_

#include <QWidget>
#include <QTreeWidget>
#include <QListWidgetItem>
#include <QListWidget>

#include "AngleRoundedWidget.h"
#include "ContactListWidget.h"

namespace Ui { class GroupMemberOperatorWidget; }

class GroupMemberOperatorWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	GroupMemberOperatorWidget(QWidget* parent = nullptr);
	~GroupMemberOperatorWidget();
	virtual void loadData(const QString& id = QString()) = 0;

protected:
	virtual bool shouldFilterUser(const QString& user_id) = 0;
	virtual void initUi() = 0;
protected:
	void loadFriendsList();
	void loadGroupMembers(const QString& group_id);
protected:
	void init();
	void addSelectedItem(QListWidget* listWidget, const QString& user_id, const QString& userName);
	void removeSelectedItem(const QString& user_id);
	QListWidgetItem* findListItem(QListWidget* listWidget, const QString& user_id);
	QTreeWidgetItem* findTreeItem(const QString& user_id);
	void clearFriendTree();
	void clearSearchList();
protected:
	Ui::GroupMemberOperatorWidget* ui{};
	QTreeWidget* m_selectTree{};
	QListWidget* m_searchList{};
	QStringList m_selectedList{};
	QStringList m_userIdList{};
};

#endif // !GROUPMEMBEROPERATORWIDGET_H_

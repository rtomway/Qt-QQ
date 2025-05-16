#ifndef GROUPMEMBERGRIDWIDGET_H_
#define GROUPMEMBERGRIDWIDGET_H_

#include <QWidget>
#include <QLabel>

#include "Group.h"
#include "GroupMemberAvatarWidget.h"


class GroupMemberGridWidget :public QWidget
{
	Q_OBJECT
public:
	GroupMemberGridWidget(QWidget* parent = nullptr);
	~GroupMemberGridWidget();
	void setGroupMembersGrid(const QString& group_id);
	void clearGroupMemberGrid();
private:
	void init();
private:
	QSharedPointer<Group>m_group{};
	QList<GroupMemberAvatarWidget*>m_avatarList;
};

#endif // !GROUPMEMBERGRIDWIDGET_H_

#ifndef GROUPMEMBERGRIDWIDGET_H_
#define GROUPMEMBERGRIDWIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

#include "Group.h"
#include "GroupMemberAvatarWidget.h"
#include "SMaskWidget.h"
#include "FriendProfileEditWidget.h"
#include "GroupMemberGridWidget.h"


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
	void loadGridMember();
signals:
	void heightChanged(int height);
private:
	QSharedPointer<Group>m_group{};
	QList<GroupMemberAvatarWidget*>m_avatarList;
	QVBoxLayout* m_layout{};
	QGridLayout* m_gridLayout{};
	GroupMemberOperatorWidget* m_groupInviteWidget{};
	int m_lastHeight = 0;
};

#endif // !GROUPMEMBERGRIDWIDGET_H_

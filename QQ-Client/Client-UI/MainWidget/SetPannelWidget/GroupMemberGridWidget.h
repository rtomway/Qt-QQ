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
private:
	QSharedPointer<Group>m_group{};
	QList<GroupMemberAvatarWidget*>m_avatarList;
	QVBoxLayout* m_layout{};
	QGridLayout* m_gridLayout{};
	GroupMemberOperatorWidget* m_groupInviteWidget{};
	GroupMemberOperatorWidget* m_groupRemoveWidget{};
	int m_lastHeight = 0;
signals:
	void heightChanged(int height);
	void queryMoreGroupMember();
};

#endif // !GROUPMEMBERGRIDWIDGET_H_

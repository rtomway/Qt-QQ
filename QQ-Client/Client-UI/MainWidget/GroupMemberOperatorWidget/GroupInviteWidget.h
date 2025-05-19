#ifndef GROUPINVITEWIDGET_H_
#define GROUPINVITEWIDGET_H_

#include "GroupMemberOperatorWidget.h"

class GroupInviteWidget :public GroupMemberOperatorWidget
{
	Q_OBJECT
public:
	GroupInviteWidget(const QString& group_id, QWidget* parent = nullptr);
	~GroupInviteWidget();
	void loadData()override;
	bool shouldFilterUser(const QString& user_id)override;
private:
	void initUi()override;
private:
	QString m_group_id;
	QString m_group_name;
	QStringList m_groupMemberIdList;
};

#endif // !GROUPINVITEWIDGET_H_

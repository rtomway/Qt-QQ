#ifndef GROUPDELETEMEMBERWIDGET_H_
#define GROUPDELETEMEMBERWIDGET_H_

#include "GroupMemberOperatorWidget.h"

class GroupDeleteMemberWidget :public GroupMemberOperatorWidget
{
	Q_OBJECT
public:
	GroupDeleteMemberWidget(const QString&group_id,QWidget* parent = nullptr);
	~GroupDeleteMemberWidget();
	void loadData()override;
	bool shouldFilterUser(const QString& user_id);
	void initUi();
private:
	QString m_group_id;
};

#endif // !GROUPDELETEMEMBERWIDGET_H_

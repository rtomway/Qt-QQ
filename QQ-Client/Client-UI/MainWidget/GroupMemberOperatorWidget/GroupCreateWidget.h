#ifndef GROUPCREATEWIDGET_H_
#define GROUPCREATEWIDGET_H_

#include "GroupMemberOperatorWidget.h"

class GroupCreateWidget :public GroupMemberOperatorWidget
{
	Q_OBJECT
public:
	GroupCreateWidget(QWidget* parent = nullptr);
	~GroupCreateWidget();
	void loadData()override;
	void initUi()override;
	bool shouldFilterUser(const QString& user_id)override;
};

#endif // !GROUPCREATEWIDGET_H_

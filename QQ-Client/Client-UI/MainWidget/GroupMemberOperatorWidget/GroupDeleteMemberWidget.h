#ifndef GROUPDELETEMEMBERWIDGET_H_
#define GROUPDELETEMEMBERWIDGET_H_

#include "GroupMemberOperatorWidget.h"

class GroupDeleteMemberWidget :public GroupMemberOperatorWidget
{
	Q_OBJECT
public:
	GroupDeleteMemberWidget(const QString& group_id, QWidget* parent = nullptr);
	~GroupDeleteMemberWidget();
	void loadData(const QString& id = QString())override;
	bool shouldFilterUser(const QString& user_id);
	void initUi();
private:
	QString m_group_id;
	QString m_group_name;
};

#endif // !GROUPDELETEMEMBERWIDGET_H_

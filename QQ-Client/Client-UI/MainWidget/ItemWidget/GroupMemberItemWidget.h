#ifndef GROUPMEMBERITEMWIDGET_H_
#define GROUPMEMBERITEMWIDGET_H_

#include "ItemWidget.h"
#include <QLabel>
#include "Group.h"

class GroupMemberItemWidget :public ItemWidget
{
	Q_OBJECT
public:
	GroupMemberItemWidget(const QString&group_id,QWidget* parent = nullptr);
	~GroupMemberItemWidget();
public:
	void setItemWidget(const QString& member_id)override;
	void refershWidget();
private:
	void init();
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
private:
	QString m_group_id;
	QString m_member_id;
	Member m_member;
	QLabel* m_groupRoleLab;
};

#endif // !GROUPMEMBERITEMWIDGET_H_

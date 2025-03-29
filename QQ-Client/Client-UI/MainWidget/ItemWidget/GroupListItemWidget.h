#ifndef GROUPLISTITEMWIDGET_H_
#define GROUPLISTITEMWIDGET_H_

#include "ItemWidget.h"
#include "Group.h"

class GroupListItemWidget :public ItemWidget
{
	Q_OBJECT
public:
	GroupListItemWidget(QWidget* parent = nullptr);
	virtual void setItemWidget(const QString& group_id)override;
	void setGrouping(const QString& grouping);
private:
	void init();
private:
	QSharedPointer<Group>m_group = nullptr;
	QString m_groupId{};
};


#endif // !GROUPLISTITEMWIDGET_H_
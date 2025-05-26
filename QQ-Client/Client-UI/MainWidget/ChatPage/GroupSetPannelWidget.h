#ifndef GROUPSETPANNELWIDGET_H_
#define GROUPSETPANNELWIDGET_H_

#include <QWidget>
#include <QPushButton>
#include "SetPannelWidget.h"
#include "GroupListItemWidget.h"
#include "GroupMemberGridWidget.h"

class GroupSetPannelWidget :public QWidget
{
	Q_OBJECT
public:
	GroupSetPannelWidget(QWidget* parent = nullptr);
	~GroupSetPannelWidget();
public:
	void loadGroupPannel(const QString& group_id);
private:
	void init();
	void refreshWidget();
	void exitGroup();
	void disbandGroup();
private:
	SetPannelWidget* m_pannelContains{};
	QString m_group_id{};
	bool m_isGroupOwner{ false };
	GroupListItemWidget* m_groupListItemWidget{};
	GroupMemberGridWidget* m_groupMemberGrid{};
	QPushButton* m_exitGroupBtn{};

};

#endif // !GROUPSETPANNELWIDGET_H_

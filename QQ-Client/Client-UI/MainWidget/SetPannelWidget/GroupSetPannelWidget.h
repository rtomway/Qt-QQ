#ifndef GROUPSETPANNELWIDGET_H_
#define GROUPSETPANNELWIDGET_H_

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>

#include "SetPannelWidget.h"
#include "GroupMemberQueryWidget.h"
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
	void initPannel();
	void refreshWidget();
	void exitGroup();
	void disbandGroup();
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
private:
	QStackedWidget* m_stackedWidget{};
	SetPannelWidget* m_pannelContains{};
	GroupMemberQueryWidget* m_groupMemberQueryWidget{};
	QString m_group_id{};
	bool m_isGroupOwner{ false };
	GroupListItemWidget* m_groupListItemWidget{};
	GroupMemberGridWidget* m_groupMemberGrid{};
	QPushButton* m_exitGroupBtn{};
signals:
	void inviteNewMember();

};

#endif // !GROUPSETPANNELWIDGET_H_

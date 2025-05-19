#include "GroupDeleteMemberWidget.h"
#include "ui_GroupMemberOperatorWidget.h"

#include "GroupManager.h"

GroupDeleteMemberWidget::GroupDeleteMemberWidget(const QString& group_id, QWidget* parent)
	:GroupMemberOperatorWidget(parent)
	,m_group_id(group_id)
{

}

GroupDeleteMemberWidget::~GroupDeleteMemberWidget()
{

}

void GroupDeleteMemberWidget::loadData()
{
	loadGroupMembers(m_group_id);
}

bool GroupDeleteMemberWidget::shouldFilterUser(const QString& user_id)
{
	return false;
}

void GroupDeleteMemberWidget::initUi()
{
	ui->groupNameEdit->setVisible(false);
	ui->GroupNameLab->setText(GroupManager::instance()->findGroup(m_group_id)->getGroupName());
	ui->groupModeLab->setText("邀请新成员");
}

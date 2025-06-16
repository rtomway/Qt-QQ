#include "GroupDeleteMemberWidget.h"
#include "ui_GroupMemberOperatorWidget.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocuMent>

#include "GroupManager.h"
#include "LoginUserManager.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include "PacketCreate.h"
#include "GroupManager.h"
#include "Group.h"
#include "EventBus.h"

GroupDeleteMemberWidget::GroupDeleteMemberWidget(const QString& group_id, QWidget* parent)
	:GroupMemberOperatorWidget(parent)
	, m_group_id(group_id)
{
	initUi();
	m_group_name = GroupManager::instance()->findGroup(m_group_id)->getGroupName();
	//移除群成员
	connect(ui->okBtn, &QPushButton::clicked, this, [=]
		{
			if (ui->selectedFriendList->count() == 0)
			{
				QMessageBox::warning(nullptr, "警告", "请选择移除成员");
				return;
			}
			//向服务端发送
			QJsonObject groupMemberRemoveObj;
			auto& user_id = LoginUserManager::instance()->getLoginUserID();
			groupMemberRemoveObj["user_id"] = user_id;
			groupMemberRemoveObj["username"] = LoginUserManager::instance()->getLoginUserName();
			groupMemberRemoveObj["group_id"] = m_group_id;
			groupMemberRemoveObj["group_name"] = m_group_name;
			groupMemberRemoveObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
			QJsonArray inviteMembersArray;
			for (const QString& member_id : m_selectedList) 
			{
				inviteMembersArray.append(member_id);
			}
			groupMemberRemoveObj["removeMembers"] = inviteMembersArray;
			auto message = PacketCreate::textPacket("groupMemberRemove", groupMemberRemoveObj);
			NetWorkServiceLocator::instance()->sendWebTextMessage(message);
			//本地刷新
			auto group = GroupManager::instance()->findGroup(m_group_id);
			for (const QString& member_id : m_selectedList)
			{
				group->removeMember(member_id);
			}
			EventBus::instance()->emit updateChatWidget(ChatType::Group,m_group_id);
			//清除
			clearFriendTree();
			clearSearchList();
			this->hide();
		});
	//取消
	connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
		{
			//删除旧friendtree并且释放资源
			clearFriendTree();
			clearSearchList();
			this->hide();
		});
}

GroupDeleteMemberWidget::~GroupDeleteMemberWidget()
{

}

void GroupDeleteMemberWidget::loadData(const QString& id)
{
	if (!id.isEmpty())
		m_group_id = id;
	loadGroupMembers(m_group_id);
}

bool GroupDeleteMemberWidget::shouldFilterUser(const QString& user_id)
{
	if (user_id == LoginUserManager::instance()->getLoginUserID())
		return true;
	return false;
}

void GroupDeleteMemberWidget::initUi()
{
	ui->groupNameEdit->setVisible(false);
	ui->GroupNameLab->setText(GroupManager::instance()->findGroup(m_group_id)->getGroupName());
	ui->groupModeLab->setText("移除群成员");
}

#include "GroupInviteWidget.h"
#include "ui_GroupMemberOperatorWidget.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>

#include "GroupManager.h"
#include "LoginUserManager.h"
#include "FriendManager.h"
#include "PacketCreate.h"
#include "MessageSender.h"

GroupInviteWidget::GroupInviteWidget(const QString& group_id, QWidget* parent)
	:GroupMemberOperatorWidget(parent)
	, m_group_id(group_id)
{
	initUi();
	//邀请好友进群
	connect(ui->okBtn, &QPushButton::clicked, this, [=]
		{
			if (ui->selectedFriendList->count() == 0)
			{
				QMessageBox::warning(nullptr, "警告", "请选择群聊邀请人数");
				return;
			}
			QJsonObject groupInviteObj;
			auto& user_id = LoginUserManager::instance()->getLoginUserID();
			groupInviteObj["user_id"] = user_id;
			groupInviteObj["username"] = LoginUserManager::instance()->getLoginUserName();
			groupInviteObj["group_id"] = m_group_id;
			groupInviteObj["group_name"] = m_group_name;
			// 将 QStringList 转换为 QJsonArray
			QJsonArray inviteMembersArray;
			for (const QString& member : m_selectedList) {
				inviteMembersArray.append(member);
			}
			groupInviteObj["inviteMembers"] = inviteMembersArray;
			auto message = PacketCreate::textPacket("groupInvite", groupInviteObj);
			MessageSender::instance()->sendMessage(message);
			qDebug() << "群聊创建发送";
			clearFriendTree();
			clearSearchList();
			this->hide();
		});
	//取消邀请
	connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
		{
			//删除旧friendtree并且释放资源
			clearFriendTree();
			clearSearchList();
			this->hide();
		});
}

GroupInviteWidget::~GroupInviteWidget()
{
	qDebug() << "GroupInviteWidget has destoryed" << m_group_id;
}

void GroupInviteWidget::loadData(const QString& id)
{
	qDebug() << m_group_id;
	m_group_id = id;
	m_groupMemberIdList = GroupManager::instance()->findGroup(id)->getGroupMembersIdList();
	loadFriendsList();
}

bool GroupInviteWidget::shouldFilterUser(const QString& user_id)
{
	if (m_groupMemberIdList.contains(user_id))
	{
		return true;
	}
	return false;
}

void GroupInviteWidget::initUi()
{
	m_group_name = GroupManager::instance()->findGroup(m_group_id)->getGroupName();
	ui->groupNameEdit->setVisible(false);
	ui->GroupNameLab->setText(m_group_name);
	ui->groupModeLab->setText("邀请新成员");
}

#include "GroupCreateWidget.h"
#include "ui_GroupMemberOperatorWidget.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QPushButton>

#include "LoginUserManager.h"
#include "FriendManager.h"
#include "PacketCreate.h"
#include "MessageSender.h"


GroupCreateWidget::GroupCreateWidget(QWidget* parent)
	:GroupMemberOperatorWidget(parent)
{
	//创建群聊
	connect(ui->okBtn, &QPushButton::clicked, this, [=]
		{
			if (ui->selectedFriendList->count() == 0)
			{
				QMessageBox::warning(nullptr, "警告", "请选择群聊邀请人数");
				return;
			}
			if (ui->groupNameEdit->text().isEmpty())
			{
				QMessageBox::warning(nullptr, "警告", "群名不能为空");
				return;
			}
			QJsonObject createGroupObj;
			auto& user_id = LoginUserManager::instance()->getLoginUserID();
			createGroupObj["user_id"] = user_id;
			createGroupObj["username"] = FriendManager::instance()->findFriend(user_id)->getFriendName();
			createGroupObj["group_name"] = ui->groupNameEdit->text();
			// 将 QStringList 转换为 QJsonArray
			QJsonArray inviteMembersArray;
			for (const QString& member : m_selectedList) {
				inviteMembersArray.append(member);
			}
			createGroupObj["inviteMembers"] = inviteMembersArray;
			auto message = PacketCreate::textPacket("createGroup", createGroupObj);
			MessageSender::instance()->sendMessage(message);
			qDebug() << "群聊创建发送";
			clearFriendTree();
			clearSearchList();
			this->hide();
		});
	//取消创建
	connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
		{
			//删除旧friendtree并且释放资源
			clearFriendTree();
			clearSearchList();
			this->hide();
		});
}

GroupCreateWidget::~GroupCreateWidget()
{
	initUi();
}

void GroupCreateWidget::loadData(const QString& id)
{
	loadFriendsList();
}

void GroupCreateWidget::initUi()
{
}

//过滤出不可选择对象
bool GroupCreateWidget::shouldFilterUser(const QString& user_id)
{
	if (user_id == LoginUserManager::instance()->getLoginUserID())
	{
		return true;
	}
	return false;
}


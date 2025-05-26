#include "GroupSetPannelWidget.h"
#include "GroupSetPannelWidget.h"
#include "GroupSetPannelWidget.h"

#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocuMent>
#include <QJsonArray>
#include "EventBus.h"
#include "MessageSender.h"
#include "LoginUserManager.h"
#include "GroupManager.h"

GroupSetPannelWidget::GroupSetPannelWidget(QWidget* parent)
	:QWidget(parent)
	,m_pannelContains(new SetPannelWidget(this))
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setFixedWidth(250);
	this->setStyleSheet(R"(
		QWidget{ border: none;background-color:red;}
		)");
	auto mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_pannelContains);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	this->setLayout(mainLayout);
}

GroupSetPannelWidget::~GroupSetPannelWidget()
{

}

void GroupSetPannelWidget::loadGroupPannel(const QString& group_id)
{
	//绑定群组
	m_group_id = group_id;
	//判断登录用户role
	auto group = GroupManager::instance()->findGroup(m_group_id);
	if (group->getGroupOwerId() == LoginUserManager::instance()->getLoginUserID())
		m_isGroupOwner = true;
	//界面加载一次
	if (!m_groupListItemWidget)
	{
		init();
	}
	//界面数据刷新
	refreshWidget();
}

void GroupSetPannelWidget::init()
{
	m_pannelContains->clearListWidget();

	//群信息
	m_groupListItemWidget = new GroupListItemWidget(m_pannelContains);
	m_groupListItemWidget->setStyleSheet(R"(
		QWidget{ border: none;border-radius:10px;background-color:white;}
		)");

	//群成员
	m_groupMemberGrid = new GroupMemberGridWidget(this);
	m_groupMemberGrid->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//监测群成员框高度是否变化
	connect(m_groupMemberGrid, &GroupMemberGridWidget::heightChanged, this, [=](int height)
		{
			auto item = m_pannelContains->getItemByWidget(m_groupMemberGrid);
			if (item)
			{
				item->setSizeHint(QSize(m_groupMemberGrid->width(), height));
			}
		});
	
	//退群
	m_exitGroupBtn = new QPushButton(m_pannelContains);
	QString exitText;
	if (m_isGroupOwner)
		exitText = "解散群组";
	else
		exitText = "退出群聊";
	m_exitGroupBtn->setText(exitText);
	m_exitGroupBtn->setStyleSheet(R"(
		QPushButton{background-color:white;border:1px solid white;height:25px;border-radius:5px;color:red}
		QPushButton:hover{background-color:rgb(240,240,240);}
		)");
	connect(m_exitGroupBtn, &QPushButton::clicked, this, [=]
		{
			auto deleteResult = QMessageBox::question(nullptr, exitText, "请确认");
			if (deleteResult == QMessageBox::No)
				return;
			m_pannelContains->hide();
			//通知控件和服务端
			if (m_isGroupOwner)
				disbandGroup();
			else
				exitGroup();
		});

	m_pannelContains->addItemWidget(m_groupListItemWidget, 80);
	m_pannelContains->addItemWidget(m_groupMemberGrid, m_groupMemberGrid->sizeHint().height());
	m_pannelContains->addItemWidget(m_exitGroupBtn, 30);
}

void GroupSetPannelWidget::refreshWidget()
{
	m_groupListItemWidget->setItemWidget(m_group_id);
	m_groupListItemWidget->showGroupId();
	m_groupMemberGrid->setGroupMembersGrid(m_group_id);
}

void GroupSetPannelWidget::exitGroup()
{
	auto& loginUserId = LoginUserManager::instance()->getLoginUserID();
	EventBus::instance()->emit exitGroup(m_group_id, loginUserId);
	QJsonObject deleteObj;
	deleteObj["user_id"] = loginUserId;
	deleteObj["username"] = LoginUserManager::instance()->getLoginUserName();
	deleteObj["group_id"] = m_group_id;
	QJsonDocument doc(deleteObj);
	QByteArray data = doc.toJson(QJsonDocument::Compact);
	MessageSender::instance()->sendHttpRequest("exitGroup", data, "application/json");
}

void GroupSetPannelWidget::disbandGroup()
{
	//发送服务端
	auto& loginUserId = LoginUserManager::instance()->getLoginUserID();
	auto group = GroupManager::instance()->findGroup(m_group_id);
	QJsonObject deleteObj;
	deleteObj["user_id"] = loginUserId;
	deleteObj["username"] = LoginUserManager::instance()->getLoginUserName();
	deleteObj["group_id"] = m_group_id;
	deleteObj["group_name"] = group->getGroupName();
	deleteObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	QJsonDocument doc(deleteObj);
	QByteArray data = doc.toJson(QJsonDocument::Compact);
	MessageSender::instance()->sendHttpRequest("disbandGroup", data, "application/json");
	//退出群组
	EventBus::instance()->emit exitGroup(m_group_id, loginUserId);
}

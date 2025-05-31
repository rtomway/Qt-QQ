#include "FriendSetPannelWidget.h"

#include <QMessageBox>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonDocuMent>
#include <QJsonArray>
#include "EventBus.h"
#include "MessageSender.h"
#include "LoginUserManager.h"

FriendSetPannelWidget::FriendSetPannelWidget(QWidget* parent)
	:QWidget(parent)
	,m_pannelContains(new SetPannelWidget(this))
{
	init();
}

FriendSetPannelWidget::~FriendSetPannelWidget()
{

}

void FriendSetPannelWidget::init()
{
	auto deleteBtn = new QPushButton(m_pannelContains);
	deleteBtn->setText("删除好友");
	deleteBtn->setStyleSheet(R"(
	QPushButton{background-color:white;border:1px solid white;height:25px;border-radius:5px;color:red}
	QPushButton:hover{background-color:rgb(240,240,240);}
	)");
	connect(deleteBtn, &QPushButton::clicked, this, [=]
		{
			auto deleteResult = QMessageBox::question(nullptr, "删除好友", "请确认是否删除");
			if (deleteResult == QMessageBox::No)
				return;
			m_pannelContains->hide();
			//数据更新
			EventBus::instance()->emit deleteFriend(m_friend_id);
			QJsonObject deleteObj;
			deleteObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
			deleteObj["username"] = LoginUserManager::instance()->getLoginUserName();
			deleteObj["friend_id"] = m_friend_id;
			QJsonDocument doc(deleteObj);
			QByteArray data = doc.toJson(QJsonDocument::Compact);
			MessageSender::instance()->sendHttpRequest("deleteFriend", data, "application/json");
		});
	//控件填入面板
	m_pannelContains->addItemWidget(deleteBtn, 30);
}

//加载好友面板
void FriendSetPannelWidget::loadFriendPannel(const QString& friend_id)
{
	m_friend_id = friend_id;
}



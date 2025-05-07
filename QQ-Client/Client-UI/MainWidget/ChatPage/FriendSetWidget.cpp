#include "FriendSetWidget.h"
#include "ui_FriendSetWidget.h"
#include <QMessageBox>

#include "Friend.h"
#include "FriendManager.h"
#include "MessageSender.h"
#include "EventBus.h"
#include "LoginUserManager.h"
#include "PacketCreate.h"


FriendSetWidget::FriendSetWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::FriendSetWidget)
{
	ui->setupUi(this);
	init();
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

}
//设置当前好友Id
void FriendSetWidget::setId(const QString& user_id)
{
	m_userId = user_id;
	if (m_userId == LoginUserManager::instance()->getLoginUserID())
	{
		ui->deleteBtn->setVisible(false);
	}
	else
	{
		ui->deleteBtn->setVisible(true);
	}
}

void FriendSetWidget::init()
{
	this->setStyleSheet(R"(
		QWidget{ border: none;background-color:white;}
		QPushButton{border:1px solid rgb(220,220,220);height:25px;border-radius:5px;color:red}
		QPushButton:hover{background-color:rgb(220,220,220);}"
		)");
	ui->deleteBtn->setFixedWidth(200);
	connect(ui->deleteBtn, &QPushButton::clicked, this, [=]
		{
			auto deleteResult = QMessageBox::question(nullptr, "删除好友", "请确认是否删除");
			if (deleteResult == QMessageBox::No)
			{
				return;
			}
			this->hide();
			//数据更新
			EventBus::instance()->emit deleteFriend(m_userId);
			QJsonObject deleteObj;
			deleteObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
			deleteObj["friend_id"] = m_userId;
			auto message = PacketCreate::textPacket("deleteFriend", deleteObj);
			MessageSender::instance()->sendMessage(message);
		});
}






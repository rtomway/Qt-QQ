#pragma once
#include "FriendNoticeItemWidget.h"
#include "ui_FriendNoticeItemWidget.h"
#include <QJsonObject>
#include "User.h"
#include "Client.h"
#include <memory>
#include "ImageUtil.h"
#include "Friend.h"
#include "FriendManager.h"


FriendNoticeItemWidget::FriendNoticeItemWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::FriendNoticeItemWidget)
{
	ui->setupUi(this);
	init();


}

void FriendNoticeItemWidget::init()
{
	//好友申请是否同意
	connect(ui->okBtn, &QPushButton::clicked, [=]
		{
			//回复对方同意申请
			QVariantMap paramsObject;
			paramsObject["user_id"] = FriendManager::instance()->getOneselfID();
			//paramsObject["username"] = FriendManager::instance()->get;
			paramsObject["to"] = m_user_id;
			//paramsObject["addFriend"] = "加为好友";
			paramsObject["result"] = true;
			Client::instance()->sendMessage("resultOfAddFriend", paramsObject);
			//更新申请界面
			ui->cancelBtn->setVisible(false);
			ui->okBtn->setText("已同意");
			ui->okBtn->setEnabled(false);
			//发送新增好友信号
			QJsonObject friendObj;
			friendObj["username"] = m_userName;
			friendObj["user_id"] = m_user_id;
			friendObj["isSend"] = false;
			m_addWidget = std::make_unique<AddWidget>(); // 使用智能指针
			m_addWidget->setUser(friendObj, ui->headLab->pixmap());
			m_addWidget->show();

			//emit newlyFriend(friendObj);

		});
	connect(ui->cancelBtn, &QPushButton::clicked, [=]
		{
			QVariantMap paramsObject;
			paramsObject["user_id"] = User::instance()->getUserId();
			paramsObject["username"] = User::instance()->getUserName();
			paramsObject["to"] = m_user_id;
			//paramsObject["addFriend"] = "拒绝加为好友";
			paramsObject["result"] = false;
			Client::instance()->sendMessage("resultOfAddFriend", paramsObject);

			ui->okBtn->setVisible(false);
			ui->cancelBtn->setText("已拒绝");
			ui->cancelBtn->setEnabled(false);

		});
}

FriendNoticeItemWidget::~FriendNoticeItemWidget()
{

}

void FriendNoticeItemWidget::setUser(const QJsonObject& obj)
{
	m_user_id = obj["user_id"].toString();
	m_userName = obj["username"].toString();
	ui->nameLab->setText(m_userName);
	ui->timeLab->setText(obj["time"].toString());
	ui->requestLab->setText(obj["addFriend"].toString());
	if (obj["addFriend"].toString() != "请求加为好友")
	{
		ui->leaveLab->setVisible(false);
		ui->okBtn->setVisible(false);
		ui->cancelBtn->setVisible(false);
		ui->messageLab->setVisible(false);
	}
	else
	{
		ui->messageLab->setText(obj["message"].toString());
	}

}

void FriendNoticeItemWidget::setPixmap(const QPixmap& pixmap)
{
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(60, 60));
	ui->headLab->setPixmap(headPix);
}

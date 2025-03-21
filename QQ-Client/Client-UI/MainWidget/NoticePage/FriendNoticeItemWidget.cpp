#pragma once
#include "FriendNoticeItemWidget.h"
#include "ui_FriendNoticeItemWidget.h"
#include <QJsonObject>
#include <memory>

#include "MessageSender.h"
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
			//同意对方申请
			//更新申请界面
			ui->cancelBtn->setVisible(false);
			ui->okBtn->setText("已同意");
			ui->okBtn->setEnabled(false);
			//发送新增好友信号
			QJsonObject friendObj;
			friendObj["username"] = m_userName;
			friendObj["user_id"] = m_user_id;
			qDebug() << "新增好友id:" << m_user_id;
			friendObj["isSend"] = false;
			m_addWidget = std::make_unique<AddWidget>(); // 使用智能指针
			m_addWidget->setUser(friendObj, ui->headLab->pixmap());
			m_addWidget->show();

		});
	//拒绝
	connect(ui->cancelBtn, &QPushButton::clicked, [=]
		{
			//信息发送
			auto oneselfID = FriendManager::instance()->getOneselfID();
			auto oneself = FriendManager::instance()->findFriend(oneselfID);
			auto paramsObject = oneself->getFriend().toVariantMap();
			paramsObject["to"] = m_user_id;
			paramsObject["result"] = false;
			MessageSender::instance()->sendMessage("resultOfAddFriend", paramsObject);
			//控件更新
			ui->okBtn->setVisible(false);
			ui->cancelBtn->setText("已拒绝");
			ui->cancelBtn->setEnabled(false);

		});
}

FriendNoticeItemWidget::~FriendNoticeItemWidget()
{
	delete ui;
}
//用户设置
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
//头像设置
void FriendNoticeItemWidget::setPixmap(const QPixmap& pixmap)
{
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(60, 60));
	ui->headLab->setPixmap(headPix);
}

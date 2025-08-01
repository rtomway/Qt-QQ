﻿#include "FNoticeItemWidget.h"
#include "ui_ItemWidget.h"
#include "TempManager.h"
#include "AvatarManager.h"
#include "ImageUtil.h"
#include "LoginUserManager.h"
#include "PacketCreate.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include <QBoxLayout>

FNoticeItemWidget::FNoticeItemWidget(QWidget* parent)
	:ItemWidget(parent)
	, m_noticeMessageLab(new QLabel(this))
	, m_timeLab(new QLabel(this))
	, m_okBtn(new QPushButton("同意", this))
	, m_cancelBtn(new QPushButton("拒绝", this))
{
	init();
	//同意好友添加
	connect(m_okBtn, &QPushButton::clicked, this, [=]
		{
			m_okBtn->setText("已同意");
			m_okBtn->setEnabled(false);
			m_cancelBtn->setVisible(false);
			QJsonObject replyObj = m_json;
			replyObj["isSend"] = false;
			m_addWidget = std::make_unique<AddWidget>(); // 使用智能指针
			m_addWidget->setUser(replyObj, ui->headLab->pixmap());
			m_addWidget->show();
		});
	//拒绝好友添加
	connect(m_cancelBtn, &QPushButton::clicked, this, [=]
		{
			m_cancelBtn->setText("已拒绝");
			m_cancelBtn->setEnabled(false);
			m_okBtn->setVisible(false);
			QJsonObject replyObj;
			replyObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
			replyObj["username"] = LoginUserManager::instance()->getLoginUserName();
			replyObj["noticeMessage"] = "拒绝了你的好友申请";
			replyObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
			replyObj["to"] = m_json["user_id"].toString();
			auto message = PacketCreate::textPacket("friendAddFail", replyObj);
			NetWorkServiceLocator::instance()->sendWebTextMessage(message);
		});
}

void FNoticeItemWidget::init()
{
	auto nameLayout = ui->nameWidget->layout();
	nameLayout->addWidget(m_noticeMessageLab);
	nameLayout->addWidget(m_timeLab);
	ui->rightWidget->setLayout(new QHBoxLayout(ui->rightWidget));
	auto rightLayout = ui->rightWidget->layout();
	rightLayout->addWidget(m_okBtn);
	rightLayout->addWidget(m_cancelBtn);
	m_okBtn->setFixedWidth(90);
	m_cancelBtn->setFixedWidth(90);
	ui->preMessageLab->setText("留言：");
}

//设置item窗口
void FNoticeItemWidget::setItemWidget(const QString& user_id)
{
	if (m_isReply)
	{
		auto noticeData = TempManager::instance()->getFriendReplyInfo(user_id);
		m_json = noticeData.replyData;
		m_headPix = noticeData.avatar;
		m_okBtn->setVisible(false);
		m_cancelBtn->setVisible(false);
	}
	else
	{
		auto noticeData = TempManager::instance()->getFriendRequestInfo(user_id);
		m_json = noticeData.requestData;
		m_headPix = noticeData.avatar;
	}
	refershItemWidget();
}

void FNoticeItemWidget::refershItemWidget()
{
	auto headPix = ImageUtils::roundedPixmap(m_headPix, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
	ui->nameLab->setText(m_json["username"].toString());
	m_noticeMessageLab->setText(m_json["noticeMessage"].toString());
	m_timeLab->setText(m_json["time"].toString());
}

//设置通知类型
void FNoticeItemWidget::setMode(bool isReply)
{
	m_isReply = isReply;
}



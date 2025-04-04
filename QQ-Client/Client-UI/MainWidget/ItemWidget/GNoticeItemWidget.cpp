﻿#include "GNoticeItemWidget.h"
#include "ui_ItemWidget.h"
#include "TempManager.h"
#include "AvatarManager.h"
#include "FriendManager.h"
#include "ImageUtil.h"
#include "MessageSender.h"
#include <QBoxLayout>

GNoticeItemWidget::GNoticeItemWidget(QWidget* parent)
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
			m_cancelBtn->setVisible(false);
			m_okBtn->setEnabled(false);
			if (m_type == GroupNoticeType::GroupInvite)
			{
				QVariantMap groupInviteMap;
				auto loginUser = FriendManager::instance()->findFriend(FriendManager::instance()->getOneselfID());
				groupInviteMap["user_id"] = loginUser->getFriendId();
				groupInviteMap["username"] = loginUser->getFriendName();
				groupInviteMap["group_id"] = m_json["group_id"].toString();
				groupInviteMap["groupOwerId"] = m_json["user_id"].toString();
				groupInviteMap["group_name"] = m_json["group_name"].toString();
				MessageSender::instance()->sendMessage("groupInviteSuccess", groupInviteMap);
			}
			else if (m_type == GroupNoticeType::GroupRequestAdd)
			{

			}
		});
}
void GNoticeItemWidget::init()
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
	//ui->preMessageLab->setText("留言：");
	ui->preMessageLab->setVisible(false);
}


void GNoticeItemWidget::setItemWidget(const QString& group_id)
{
	if (m_isReply)
	{
		auto noticeData = TempManager::instance()->getGroupReplyInfo(group_id);
		m_json = noticeData.replyData;
		m_headPix = noticeData.avatar;
		m_okBtn->setVisible(false);
		m_cancelBtn->setVisible(false);
	}
	else
	{
		auto noticeData = TempManager::instance()->getGroupRequestInfo(group_id);
		m_json = noticeData.requestData;
		m_headPix = noticeData.avatar;
		m_type = static_cast<GroupNoticeType>(m_json["groupType"].toInt());
	}
	auto headPix = ImageUtils::roundedPixmap(m_headPix, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
	ui->nameLab->setText(m_json["username"].toString());
	m_noticeMessageLab->setText(m_json["noticeMessage"].toString());
	m_timeLab->setText(m_json["time"].toString());
}

void GNoticeItemWidget::setMode(bool isReply)
{
	m_isReply = isReply;
}


#include "GNoticeItemWidget.h"
#include "ui_ItemWidget.h"
#include <QBoxLayout>

#include "TempManager.h"
#include "AvatarManager.h"
#include "FriendManager.h"
#include "ImageUtil.h"
#include "LoginUserManager.h"
#include "../Client-ServiceLocator/NetWorkServiceLocator.h"
#include "PacketCreate.h"

GNoticeItemWidget::GNoticeItemWidget(QWidget* parent)
	:ItemWidget(parent)
	, m_noticeMessageLab(new QLabel(this))
	, m_timeLab(new QLabel(this))
	, m_okBtn(new QPushButton("同意", this))
	, m_cancelBtn(new QPushButton("拒绝", this))
{
	init();
	//同意加入群组
	connect(m_okBtn, &QPushButton::clicked, this, [=]
		{
			m_okBtn->setText("已同意");
			m_cancelBtn->setVisible(false);
			m_okBtn->setEnabled(false);
			if (m_type == GroupNoticeType::GroupInvite)
			{
				QJsonObject groupInviteObj;
				auto& loginUser = LoginUserManager::instance()->getLoginUser();
				groupInviteObj["user_id"] = loginUser->getFriendId();
				groupInviteObj["username"] = loginUser->getFriendName();
				groupInviteObj["group_id"] = m_json["group_id"].toString();
				groupInviteObj["groupOwerId"] = m_json["user_id"].toString();
				groupInviteObj["group_name"] = m_json["group_name"].toString();
				groupInviteObj["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
				auto message = PacketCreate::textPacket("groupInviteSuccess", groupInviteObj);
				NetWorkServiceLocator::instance()->sendWebTextMessage(message);
			}
			else if (m_type == GroupNoticeType::GroupRequestAdd)
			{
				QJsonObject groupAddObj;
				groupAddObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
				groupAddObj["applicant_id"] = m_json["user_id"].toString();
				groupAddObj["applicant_name"] = m_json["username"].toString();
				groupAddObj["group_id"] = m_json["group_id"].toString();
				groupAddObj["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
				auto message = PacketCreate::textPacket("groupAddSuccess", groupAddObj);
				NetWorkServiceLocator::instance()->sendWebTextMessage(message);
			}
		});
	//拒绝加入群组
	connect(m_cancelBtn, &QPushButton::clicked, this, [=]
		{
			m_cancelBtn->setText("已拒绝");
			m_cancelBtn->setEnabled(false);
			m_okBtn->setVisible(false);
			if (m_type == GroupNoticeType::GroupInvite)
			{
				QJsonObject groupInviteReplyObj;
				groupInviteReplyObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
				groupInviteReplyObj["username"] = LoginUserManager::instance()->getLoginUserName();
				groupInviteReplyObj["groupOwnerId"] = m_json["user_id"].toString();
				groupInviteReplyObj["group_id"] = m_json["group_id"].toString();
				groupInviteReplyObj["noticeMessage"] = "拒绝了你的群组邀请";
				groupInviteReplyObj["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
				auto message = PacketCreate::textPacket("groupInviteFailed", groupInviteReplyObj);
				NetWorkServiceLocator::instance()->sendWebTextMessage(message);
			}
			else if (m_type == GroupNoticeType::GroupRequestAdd)
			{
				QJsonObject groupAddReplyObj;
				groupAddReplyObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
				groupAddReplyObj["username"] = LoginUserManager::instance()->getLoginUserName();
				groupAddReplyObj["to"] = m_json["user_id"].toString();
				groupAddReplyObj["group_id"] = m_json["group_id"].toString();
				groupAddReplyObj["group_name"] = m_json["group_name"].toString();
				groupAddReplyObj["noticeMessage"] = "拒绝了你加入群组" + m_json["group_name"].toString();
				groupAddReplyObj["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
				auto message = PacketCreate::textPacket("groupAddFailed", groupAddReplyObj);
				NetWorkServiceLocator::instance()->sendWebTextMessage(message);
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
	ui->preMessageLab->setVisible(false);
}

//设置item窗口
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
	if (m_json.contains("message") && !m_json["message"].toString().isEmpty())
	{
		ui->preMessageLab->setVisible(true);
		ui->preMessageLab->setText("留言:");
		ui->afterMessageLab->setText(m_json["message"].toString());
	}
	refershItemWidget();
}

void GNoticeItemWidget::refershItemWidget()
{
	auto headPix = ImageUtils::roundedPixmap(m_headPix, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
	ui->nameLab->setText(m_json["username"].toString());
	m_noticeMessageLab->setText(m_json["noticeMessage"].toString());
	m_timeLab->setText(m_json["time"].toString());
}

//设置通知类型
void GNoticeItemWidget::setMode(bool isReply)
{
	m_isReply = isReply;
}


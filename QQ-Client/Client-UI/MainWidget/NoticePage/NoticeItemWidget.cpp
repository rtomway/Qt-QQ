#include "NoticeItemWidget.h"
#include "ui_NoticeItemWidget.h"

#include "ImageUtil.h"
#include "MessageSender.h"
#include "FriendManager.h"

NoticeItemWidget::NoticeItemWidget(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::NoticeItemWidget)
{
	ui->setupUi(this);
	init();
	this->setAttribute(Qt::WA_StyledBackground, true);
}

NoticeItemWidget::~NoticeItemWidget()
{
	delete ui;
}

void NoticeItemWidget::init()
{
	connect(ui->okBtn, &QPushButton::clicked,this,[=]
		{
			ui->okBtn->setText("已同意");
			ui->cancelBtn->setVisible(false);
			ui->okBtn->setEnabled(false);
			QJsonObject replyObj;
			replyObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
			switch (m_requestType)
			{
			case NoticeItemWidget::RequestType::FriendAddRequest:
				replyObj["username"] = m_userName;
				replyObj["user_id"] = m_userId;
				replyObj["isSend"] = false;
				m_addWidget = std::make_unique<AddWidget>(); // 使用智能指针
				m_addWidget->setUser(replyObj, ui->headLab->pixmap());
				m_addWidget->show();
				break;
			case NoticeItemWidget::RequestType::GroupInviteRequest:
				replyObj["group_id"] = m_groupId;
				replyObj["group_name"] = m_groupName;
				replyObj["user_id"] = FriendManager::instance()->getOneselfID();
				replyObj["to"] = m_userId;
				MessageSender::instance()->sendMessage("acceptGroupInvite", replyObj.toVariantMap());
				break;
			case NoticeItemWidget::RequestType::UserAddGroupRequest:
				replyObj["group_id"] = m_groupId;
				replyObj["user_id"] = m_userId;
				MessageSender::instance()->sendMessage("groupAddSuccesss", replyObj.toVariantMap());
				break;
			default:
				break;
			}
		});
	connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
		{
			ui->cancelBtn->setText("已拒绝");
			ui->okBtn->setVisible(false);
			ui->cancelBtn->setEnabled(false);
			QJsonObject replyObj;
			replyObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
			auto oneselfId = FriendManager::instance()->getOneselfID();
			auto oneself= FriendManager::instance()->findFriend(oneselfId)->getFriend();
			auto username = oneself["username"].toString();
			switch (m_requestType)
			{
			case NoticeItemWidget::RequestType::FriendAddRequest:
				replyObj["replyMessage"] = "拒绝了你的好友申请";
				replyObj["user_id"] = oneselfId;
				replyObj["username"] = username;
				replyObj["to"] = m_userId;
				MessageSender::instance()->sendMessage("friendAddFail", replyObj.toVariantMap());
				break;
			case NoticeItemWidget::RequestType::GroupInviteRequest:
				replyObj["replyMessage"] = "拒绝了你的群聊邀请";
				replyObj["user_id"] = oneselfId;
				replyObj["username"] = username;
				replyObj["to"] = m_userId;
				MessageSender::instance()->sendMessage("rejectGroupInvite", replyObj.toVariantMap());
				break;
			case NoticeItemWidget::RequestType::UserAddGroupRequest:
				replyObj["replyMessage"] = "拒绝了你的群聊申请";
				replyObj["group_id"] = m_groupId;
				replyObj["groupname"] = m_groupName;
				replyObj["to"] = m_userId;
				MessageSender::instance()->sendMessage("groupAddFail", replyObj.toVariantMap());
				break;
			default:
				break;
			}
		});
}

void NoticeItemWidget::setUser(const QJsonObject& obj, const QPixmap& pixmap,NoticeItemWidget::NoticeType type)
{
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(60, 60));
	ui->headLab->setPixmap(headPix);
	ui->timeLab->setText(obj["time"].toString());
	ui->messageLab->setText(obj["message"].toString());

	m_userId = obj["user_id"].toString();
	m_userName = obj["username"].toString();
	//回复通知
	if (type == NoticeType::ReplyNotice)
	{
		if (obj.contains("group_name"))
		{
			m_groupId = obj["group_id"].toString();
			m_groupName = obj["group_name"].toString();
			ui->nameLab->setText(m_groupName);
		}
		else
		{
			ui->nameLab->setText(m_userName);

		}
		ui->requestLab->setText(obj["replyMessage"].toString());
		ui->okBtn->setVisible(false);
		ui->cancelBtn->setVisible(false);
		ui->leaveLab->setVisible(false);
		ui->messageLab->setVisible(false);
		return;
	}
	//请求通知
	ui->requestLab->setText(obj["requestMessage"].toString());
	auto requestType = static_cast<RequestType>(obj["requestType"].toInt());
	m_requestType = requestType;
	switch (requestType)
	{
	case NoticeItemWidget::RequestType::FriendAddRequest:
		ui->nameLab->setText(m_userName);
		break;
	case NoticeItemWidget::RequestType::GroupInviteRequest:
		m_groupId = obj["group_id"].toString();
		m_groupName = obj["group_name"].toString();
		ui->nameLab->setText(m_groupName);
		break;
	case NoticeItemWidget::RequestType::UserAddGroupRequest:
		m_groupId = obj["group_id"].toString();
		m_groupName = obj["group_name"].toString();
		ui->nameLab->setText(m_userName);
		break;
	default:
		break;
	}
}


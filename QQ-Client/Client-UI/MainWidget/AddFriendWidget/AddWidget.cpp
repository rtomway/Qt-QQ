#include "AddWidget.h"
#include "ui_AddWidget.h"
#include <QBoxLayout>
#include <QJsonObject>

#include "MessageSender.h"
#include "Friend.h"
#include "FriendManager.h"
#include "ImageUtil.h"
#include "ContactListWidget.h"
#include "LoginUserManager.h"
#include "PacketCreate.h"


AddWidget::AddWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::AddWidget)
	, m_grouping(new LineEditwithButton)
{
	ui->setupUi(this);
	this->setFixedSize(350, 500);
	init();
	this->setStyleSheet(R"(
			QLineEdit{
				border:none;
				border-radius:5px;
				background-color:white;
				height:30px;
			}
			QPushButton{
				border:none;
				border-radius:5px;
				background-color:#0096FF;
				color:white;
				height:30px;
				width:50px;
			}
			QPushButton:hover{
				font-size:14px;
			}
			)");
}

AddWidget::~AddWidget()
{
	delete ui;
}

void AddWidget::init()
{
	m_grouping->setParent(ui->groupWidget);
	m_grouping->setFixedHeight(50);
	m_grouping->setFixedWidth(this->width() - 10);
	ui->messageEdit->setFixedWidth(this->width() - 20);
	ui->nicknameEdit->setFixedWidth(this->width() - 20);
	m_grouping->setComboBox();
	m_grouping->setText("我的好友");
	//获取分组列表
	auto groupingList = ContactListWidget::getfGrouping();
	for (const auto& name : groupingList)
	{
		m_grouping->addMenuItem(name);
	}
	ui->groupWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//好友添加界面
	//发送
	connect(ui->sendBtn, &QPushButton::clicked, [=]
		{
			if (m_isSend)//申请
			{
				auto& applicateFriendObj = LoginUserManager::instance()->getLoginUser()->getFriend();
				applicateFriendObj["to"] = m_friendId;
				applicateFriendObj["message"] = ui->messageEdit->text();
				applicateFriendObj["noticeMessage"] = "请求加为好友";
				applicateFriendObj["grouping"] = m_grouping->getLineEditText();

				auto message = PacketCreate::textPacket("addFriend", applicateFriendObj);
				MessageSender::instance()->sendMessage(message);
			}
			else  //添加
			{
				//用户信息
				QJsonObject friendObj;
				friendObj["to"] = m_friendId;
				friendObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
				friendObj["grouping"] = m_grouping->getLineEditText();
				friendObj["replyMessage"] = "同意了你的好友申请";
				auto message = PacketCreate::textPacket("friendAddSuccess", friendObj);
				MessageSender::instance()->sendMessage(message);
			}
			this->close();
		});
	//取消
	connect(ui->cancelBtn, &QPushButton::clicked, [=]
		{
			this->close();
		});

}
//用户信息
void AddWidget::setUser(const QJsonObject& obj, const QPixmap& pixmap)
{
	m_friendName = obj["username"].toString();
	m_friendId = obj["user_id"].toString();
	ui->nameLab->setText(m_friendName);
	ui->idLab->setText(m_friendId);
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(60, 60));
	ui->headLab->setPixmap(headPix);
	if (!obj["isSend"].toBool())
	{
		m_isSend = false;
		ui->sendBtn->setText("确认");
		ui->messageEdit->setVisible(false);
		ui->messageLab->setVisible(false);
		ui->cancelBtn->setVisible(false);
	}
	else
	{
		m_isSend = true;
		m_grouping->getLineEditText();
	}
}

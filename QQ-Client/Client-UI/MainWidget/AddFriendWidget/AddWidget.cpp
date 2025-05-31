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
	init();
	this->setStyleSheet(R"(
			QLineEdit{
				border:none;border-radius:5px;
				background-color:white;height:30px;
			}
			QPushButton{
				border:none;border-radius:5px;background-color:#0096FF;
				color:white;height:30px;width:50px;
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
	this->setFixedSize(350, 500);
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

	//发送
	connect(ui->sendBtn, &QPushButton::clicked, [=]
		{
			//群组
			if (m_type == ChatType::Group)
			{
				QJsonObject applicateGroupObj;
				applicateGroupObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
				applicateGroupObj["username"] = LoginUserManager::instance()->getLoginUserName();
				applicateGroupObj["to"] = m_addId;
				qDebug() << "applicateGroupObj[to] :" << m_addId;
				applicateGroupObj["message"] = ui->messageEdit->text();
				applicateGroupObj["noticeMessage"] = "请求加入群组" + m_addName;

				auto message = PacketCreate::textPacket("addGroup", applicateGroupObj);
				MessageSender::instance()->sendMessage(message);
				this->close();
				return;
			}
			//好友
			if (m_isSend)//申请
			{
				QJsonObject applicateFriendObj;
				applicateFriendObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
				applicateFriendObj["username"] = LoginUserManager::instance()->getLoginUserName();
				applicateFriendObj["to"] = m_addId;
				applicateFriendObj["message"] = ui->messageEdit->text();
				applicateFriendObj["noticeMessage"] = "请求加为好友";
				applicateFriendObj["Fgrouping"] = m_grouping->getLineEditText();
				qDebug() << applicateFriendObj << m_addId;
				auto message = PacketCreate::textPacket("addFriend", applicateFriendObj);
				MessageSender::instance()->sendMessage(message);
			}
			else  //添加
			{
				//用户信息
				QJsonObject friendObj;
				friendObj["to"] = m_addId;
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
	m_type = ChatType::User;
	m_addName = obj["username"].toString();
	m_addId = obj["user_id"].toString();
	ui->nameLab->setText(m_addName);
	ui->idLab->setText(m_addId);
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
		ui->messageLab->setText("好友申请信息");
	}
}

//群组信息
void AddWidget::setGroup(const QJsonObject& obj, const QPixmap& pixmap)
{
	m_type = ChatType::Group;
	m_addName = obj["group_name"].toString();
	m_addId = obj["group_id"].toString();
	ui->nameLab->setText(m_addName);
	ui->idLab->setText(m_addId);
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(60, 60));
	ui->headLab->setPixmap(headPix);
	if (obj["isSend"].toBool())
	{
		m_isSend = true;
		m_grouping->setVisible(false);
		ui->groupIngLab->setVisible(false);
		ui->messageLab->setText("群组申请信息");
	}
}

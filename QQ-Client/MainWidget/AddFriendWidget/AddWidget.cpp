#include "AddWidget.h"
#include "ui_AddWidget.h"
#include <QBoxLayout>
#include "ContactList.h"
#include "User.h"
#include <QJsonObject>
#include "Client.h"
#include "Friend.h"
#include "FriendManager.h"


AddWidget::AddWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::AddWidget)
	,m_grouping(new LineEditwithButton)
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

}

void AddWidget::init()
{
	m_grouping->setParent(ui->groupWidget);
	m_grouping->setFixedHeight(50);
	m_grouping->setFixedWidth(this->width()-10);
	ui->messageEdit->setFixedWidth(this->width() - 20);
	ui->nicknameEdit->setFixedWidth(this->width() - 20);
	m_grouping->setComboBox();
	m_grouping->setText("我的好友");
	auto groupingList = ContactList::getfGrouping();
	for (const auto&name : groupingList)
	{
		m_grouping->addMenuItem(name);
	}
	ui->groupWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//好友添加界面
	connect(ui->sendBtn, &QPushButton::clicked, [=]
		{
			if (m_isSend)//申请
			{
				auto oneselfID = FriendManager::instance()->getOneselfID();
				auto oneself = FriendManager::instance()->findFriend(oneselfID);
				auto paramsObject = oneself->getFriend().toVariantMap();
				paramsObject["to"] = m_user_id;
				paramsObject["message"] = ui->messageEdit->text();
				paramsObject["addFriend"] = "请求加为好友";
				Client::instance()->sendMessage("addFriend", paramsObject);
				qDebug() << "申请的json:" << paramsObject;
			}
			else  //添加
			{
				//用户信息
				QJsonObject friendObject;
				friendObject["username"] = m_userName;
				friendObject["user_id"] = m_user_id;
				friendObject["grouping"] = m_grouping->getLineEditText();
				// 好友列表添加
				ContactList::instance()->newlyFriendItem(friendObject);
				
			}
			this->close();
		});
	connect(ui->cancelBtn, &QPushButton::clicked, [=]
		{
			this->close();
		});

}

void AddWidget::setUser(const QJsonObject& obj)
{
	m_userName = obj["username"].toString();
	m_user_id = obj["user_id"].toString();
	ui->nameLab->setText(m_userName);
	ui->idLab->setText(m_user_id);
	if (!obj["isSend"].toBool())
	{
		m_isSend = false;
		ui->sendBtn->setText("确认");
		ui->messageEdit->setVisible(false);
		ui->messageLab->setVisible(false);
	}
	else
	{
		m_isSend = true; m_grouping->getLineEditText();
	}
}

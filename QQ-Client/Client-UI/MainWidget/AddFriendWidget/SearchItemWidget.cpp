#include "SearchItemWidget.h"
#include "ui_SearchItemWidget.h"
#include <QJsonObject>
#include <QPointer>

#include "AddWidget.h"
#include "ImageUtil.h"
#include "FriendManager.h"


SearchItemWidget::SearchItemWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::SearchItemWidget)
{
	ui->setupUi(this);
	init();
}

SearchItemWidget::~SearchItemWidget()
{
	delete ui;
}

void SearchItemWidget::init()
{
	//添加好友
	connect(ui->addBtn, &QPushButton::clicked, [=]
		{
			if (m_isFriend)
			{
				FriendManager::instance()->emit chatWithFriend(m_user_id);
				return;
			}
			QPointer<AddWidget>addWidget = new AddWidget();
			addWidget->setUser(this->getUser(), m_userHead);
			addWidget->show();
		});
}
//设置用户信息
void SearchItemWidget::setUser(const QJsonObject& obj)
{
	m_userName = obj["username"].toString();
	m_user_id = obj["user_id"].toString();
	ui->nameLab->setText(m_userName);
	ui->idLab->setText(m_user_id);
	if (obj.contains("isFriend"))
	{
		m_isFriend = obj["isFriend"].toBool();
		qDebug() << "isFriend";
	}
	qDebug() << m_isFriend;
	if (m_isFriend)
	{
		ui->addBtn->setText("发消息");
	}
	else
	{
		ui->addBtn->setText("添加");
	}

}
//用户信息获取
QJsonObject SearchItemWidget::getUser()
{
	QJsonObject obj;
	obj["user_id"] = m_user_id;
	obj["username"] = m_userName;
	obj["isSend"] = true;
	return obj;
}
//设置头像
void SearchItemWidget::setPixmap(const QPixmap& pixmap)
{
	m_userHead = pixmap;
	auto headPix = ImageUtils::roundedPixmap(m_userHead, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
}
//设置分组
void SearchItemWidget::setGroup(const QJsonObject& obj)
{
	m_group_id = obj["group_id"].toString();
	m_groupName = obj["group_name"].toString();
	ui->idLab->setText(m_group_id);
	ui->nameLab->setText(m_groupName);
	ui->addBtn->setText("加入");
}

#include "ItemWidget.h"
#include "ui_ItemWidget.h"
#include <QRegion>
#include <QPainter>
#include <QPainterPath>

#include "ImageUtil.h"
#include "Friend.h"
#include "FriendManager.h"

ItemWidget::ItemWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::ItemWidget)
{
	ui->setupUi(this);
	init();
}

ItemWidget::~ItemWidget()
{
	delete ui;
}

void ItemWidget::init()
{
	ui->nameLab->setStyleSheet(QString("QLabel{font-size:14px}"));
	QPixmap pixmap = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(40, 40));
	ui->headLab->setPixmap(pixmap);
}
//用户设置
void ItemWidget::setUser(const QString& user_id)
{
	if (!m_oneself || m_oneself->getFriendId() != user_id)
	{
		m_oneself = FriendManager::instance()->findFriend(user_id);
	}
	m_json = m_oneself->getFriend();
	ui->nameLab->setText(m_json["username"].toString());
	m_userId = user_id;
	auto pixmap = ImageUtils::roundedPixmap(m_oneself->getAvatar(), QSize(40, 40));
	ui->headLab->setPixmap(pixmap);
	ui->signatureLab->setText(m_json["signature"].toString());
}
//分组设置
void ItemWidget::setGrouping(const QString& grouping)
{
	m_json["grouping"] = grouping;
	m_grouping = grouping;
}
const QJsonObject& ItemWidget::getUser()
{
	return m_json;
}

const QString& ItemWidget::getGrouping()
{
	return m_grouping;
}

const QString& ItemWidget::getUserId()
{
	return m_userId;
}







#include "ItemWidget.h"
#include "ui_ItemWidget.h"
#include <QRegion>
#include <QPainter>
#include <QPainterPath>
#include <ImageUtil.h>
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

void ItemWidget::setUser(const QJsonObject& obj)
{
	m_json = obj;
	qDebug() << "好友列表的json信息:" << m_json;
	ui->nameLab->setText(m_json["username"].toString());
	QSharedPointer<Friend> myfriend = FriendManager::instance()->findFriend(m_json["user_id"].toString());
	myfriend->loadAvatar();
	auto pixmap = ImageUtils::roundedPixmap(myfriend->getAvatar(), QSize(40, 40));
	ui->headLab->setPixmap(pixmap);
	ui->signatureLab->setText(m_json["signature"].toString());
}

void ItemWidget::setGrouping(const QString& grouping)
{
	m_json["grouping"] = grouping;
}

const QJsonObject& ItemWidget::getUser()
{
	return m_json;
}







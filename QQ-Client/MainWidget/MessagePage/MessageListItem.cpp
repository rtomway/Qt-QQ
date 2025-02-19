#pragma once
#include "MessageListItem.h"
#include "ui_MessageListItem.h"
#include <QBoxLayout>
#include <QLabel>
#include "ImageUtil.h"
#include <QJsonArray>

MessageListItem::MessageListItem(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::MessageListItem)
	,m_leftWidget(new QWidget)
	,m_messageAttention(new QWidget)
{
	ui->setupUi(this);
	init();
	
}

void MessageListItem::init()
{
	QPixmap pixmap= ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(40, 40));
	//ui->headLab->setPixmap(QPixmap(":/picture/Resource/Picture/qq.png"));
	//pixmap.setDevicePixelRatio(this->devicePixelRatioF());
	//ui->headLab->setPixmap(pixmap.scaled(size() * devicePixelRatioF(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	ui->headLab->setFixedSize(40, 40);
	ui->headLab->setPixmap(pixmap);
	ui->headLab->setScaledContents(true);
	//ui->headLab->setMask(QRegion(ui->headLab->rect(), QRegion::RegionType::Ellipse));
	
}

void MessageListItem::setUser(const QJsonObject& obj)
{
	m_username = obj["username"].toString();
	m_user_id = obj["user_id"].toString();
	m_unReadMessage.append(obj["message"].toString());
	qDebug() << "name" << m_username << m_user_id;
	//信息更新到界面
	updateItemWidget();
}

QJsonObject MessageListItem::getUser()
{
	QJsonObject userData;
	userData["username"] = m_username;
	userData["user_id"] = m_user_id;

	QJsonArray messagesArray;
	for (const QString& message : m_unReadMessage) {
		messagesArray.append(message);
	}
	userData["message"] = messagesArray;

	m_unReadMessage.clear();

	return userData;
}

QString MessageListItem::getId()
{
	return m_user_id;
}

void MessageListItem::updateItemWidget()
{
	ui->usernameLab->setText(m_username);
	auto newMessage = m_unReadMessage.last();
	ui->messageLab->setText(newMessage);
	ui->countLab->setText(QString::number(m_unReadMessage.count()));
}


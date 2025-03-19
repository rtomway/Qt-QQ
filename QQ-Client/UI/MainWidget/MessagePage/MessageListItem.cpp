#pragma once
#include "MessageListItem.h"
#include "ui_MessageListItem.h"
#include <QBoxLayout>
#include <QLabel>
#include <QJsonArray>

#include "Friend.h"
#include "FriendManager.h"
#include "ImageUtil.h"

MessageListItem::MessageListItem(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::MessageListItem)
	, m_leftWidget(new QWidget)
	, m_messageAttention(new QWidget)
{
	ui->setupUi(this);
	init();
	ui->countLab->setFixedSize(18, 18);
	ui->countLab->setStyleSheet(R"(
				border:none;
				border-radius:9px;
				color:white;
				background-color:red;
			)");
	ui->countLab->setAlignment(Qt::AlignCenter);
}
void MessageListItem::init()
{
	QPixmap pixmap = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(50, 50));
	ui->headLab->setFixedSize(50, 50);
	ui->headLab->setPixmap(pixmap);
	ui->headLab->setScaledContents(true);
}
//设置用户信息
void MessageListItem::setUser(const QJsonObject& obj)
{
	m_user_id = obj["user_id"].toString();
	auto user = FriendManager::instance()->findFriend(m_user_id);
	m_username = user->getFriendName();
	if (obj.contains("message") && !obj["message"].isNull() && obj["message"].isString())
	{
		m_unReadMessage.append(obj["message"].toString());
	}
	//头像
	auto pixmap = ImageUtils::roundedPixmap(user->getAvatar(), QSize(50, 50));
	ui->headLab->setPixmap(pixmap);
	//信息更新到界面
	updateItemWidget();
}
//获取用户信息
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
//获取id
QString MessageListItem::getId()
{
	return m_user_id;
}
//更新界面
void MessageListItem::updateItemWidget()
{
	ui->usernameLab->setText(m_username);
	if (m_unReadMessage.count() != 0)
	{
		ui->countLab->setVisible(true);
		ui->countLab->setText(QString::number(m_unReadMessage.count()));
		ui->countLab->adjustSize();
		auto newMessage = m_unReadMessage.last();
		ui->messageLab->setText(newMessage);
		ui->timeLab->setText(covertToChinese(QDateTime::currentDateTime().toString()));
	}
	else
	{
		ui->countLab->setVisible(false);
	}
}
//英文日期字符串转中文
QString MessageListItem::covertToChinese(const QString& date)
{
	// 英文日期字符串转换为 QDateTime 对象
	QDateTime dateTime = QDateTime::fromString(date, "ddd MMM dd hh:mm:ss yyyy");
	if (dateTime.isValid())
	{
		qDebug() << dateTime;
	}
	// 设置本地化为中文
	QLocale locale(QLocale::Chinese);
	qDebug() << "时间" << locale.toString(dateTime, "yyyy年MM月dd日 hh:mm");
	// 转换为中文格式，并控制到分钟
	return locale.toString(dateTime, "M月dd日 hh:mm");
}
//清空未读
void MessageListItem::clearUnreadMessage()
{
	m_unReadMessage.clear();
	ui->countLab->setVisible(false);
}


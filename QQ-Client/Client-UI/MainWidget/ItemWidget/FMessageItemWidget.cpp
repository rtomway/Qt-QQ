#include "FMessageItemWidget.h"
#include "ui_ItemWidget.h"
#include <QBoxLayout>

#include "ImageUtil.h"
#include "AvatarManager.h"
#include "FriendManager.h"

FMessageItemWidget::FMessageItemWidget(QWidget* parent)
	:ItemWidget(parent)
	, m_timeLab(new QLabel(this))
	, m_countLab(new QLabel("dfasdf", this))
{
	init();
}

void FMessageItemWidget::init()
{
	ui->rightWidget->setLayout(new QVBoxLayout(ui->rightWidget));
	auto rightLayout = ui->rightWidget->layout();
	rightLayout->addWidget(m_timeLab);
	rightLayout->addWidget(m_countLab);
	m_countLab->setFixedSize(18, 18);
	m_countLab->setStyleSheet(R"(
				border:none;
				border-radius:9px;
				color:white;
				background-color:red;
			)");
	m_countLab->setAlignment(Qt::AlignCenter);
	ui->preMessageLab->setVisible(false);
}

void FMessageItemWidget::setItemWidget(const QString& user_id)
{
	//数据绑定
	m_friendId = user_id;
	if (!m_friend || m_friend->getFriendId() != user_id)
	{
		m_friend = FriendManager::instance()->findFriend(user_id);
	}
	//json信息
	m_friendJson = m_friend->getFriend();
	//页面显示
	auto pixmap = AvatarManager::instance()->getAvatar(m_friendId, ChatType::User);
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
	ui->nameLab->setText(m_friendJson["username"].toString());
	//最新消息
	if (!m_unReadMesssage.isEmpty())
		ui->afterMessageLab->setText(m_unReadMesssage.last());
	qDebug() << m_friendId << QString::number(m_unReadMesssage.count());
	m_countLab->setText(QString::number(m_unReadMesssage.count()));
	m_timeLab->setText(m_lastTime);
}

void FMessageItemWidget::clearUnRead()
{
	m_countLab->setVisible(false);
	m_unReadMesssage.clear();
}

void FMessageItemWidget::updateUnReadMessage(const QString& message, const QString& time)
{
	if (message == "picture")
	{
		m_unReadMesssage.append("文件[图片]");
	}
	else
	{
		m_unReadMesssage.append(message);
	}
	m_countLab->setVisible(true);
	m_lastTime = time;
}



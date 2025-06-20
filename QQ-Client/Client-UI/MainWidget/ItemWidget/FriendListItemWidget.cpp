﻿#include "FriendListItemWidget.h"
#include "ui_ItemWidget.h"
#include "AvatarManager.h"
#include "FriendManager.h"
#include "ImageUtil.h"
#include "TopItemWidget.h"

FriendListItemWidget::FriendListItemWidget(QWidget* parent)
{
	init();
}

void FriendListItemWidget::init()
{
	ui->preMessageLab->setVisible(false);
}

//设置item窗口
void FriendListItemWidget::setItemWidget(const QString& user_id)
{
	//数据绑定
	m_friendId = user_id;
	if (!m_friend || m_friend->getFriendId() != user_id)
	{
		m_friend = FriendManager::instance()->findFriend(user_id);
	}
	//json信息
	m_friendJson = m_friend->getFriend();
	refershItemWidget();
}

//页面显示
void FriendListItemWidget::refershItemWidget()
{
	AvatarManager::instance()->getAvatar(m_friendId, ChatType::User, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
			ui->headLab->setPixmap(headPix);
		});
	ui->nameLab->setText(m_friendJson["username"].toString());
	ui->afterMessageLab->setText(m_friendJson["signature"].toString());
}

//设置分组
void FriendListItemWidget::setGrouping(const QString& grouping)
{
	if (m_friend)
		m_friend->setGrouping(grouping);
}



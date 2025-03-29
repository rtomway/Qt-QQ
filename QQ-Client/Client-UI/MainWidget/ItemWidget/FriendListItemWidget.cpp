#include "FriendListItemWidget.h"
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
	qDebug() << "FriendListItemWidget信息：" << m_friendJson;
	//页面显示
	auto pixmap = AvatarManager::instance()->getAvatar(m_friendId, ChatType::User);
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
	ui->nameLab->setText(m_friendJson["username"].toString());
	ui->afterMessageLab->setText(m_friendJson["signature"].toString());
}

void FriendListItemWidget::setGrouping(const QString& grouping)
{
	if (m_friend)
		m_friend->setGrouping(grouping);
}



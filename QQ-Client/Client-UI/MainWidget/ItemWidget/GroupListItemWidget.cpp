#include "GroupListItemWidget.h"
#include "ui_ItemWidget.h"
#include "AvatarManager.h"
#include "GroupManager.h"
#include "ImageUtil.h"
#include "TopItemWidget.h"

GroupListItemWidget::GroupListItemWidget(QWidget* parent)
{
	init();
}
void GroupListItemWidget::init()
{
	ui->messageWidget->setVisible(false);
}

void GroupListItemWidget::setItemWidget(const QString& group_id)
{
	//数据绑定
	m_groupId = group_id;
	if (!m_group || m_group->getGroupId() != group_id)
	{
		m_group = GroupManager::instance()->findGroup(group_id);
	}
	qDebug() << "GroupListItemWidget信息：" << m_group->getGroupId() << m_group->getGrouping();
	//页面显示
	auto pixmap = AvatarManager::instance()->getAvatar(m_groupId, ChatType::Group);
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
	ui->headLab->setPixmap(headPix);

	ui->nameLab->setText(m_group->getGroupName());

}

void GroupListItemWidget::setGrouping(const QString& grouping)
{
	m_group->setGrouping(grouping);
}



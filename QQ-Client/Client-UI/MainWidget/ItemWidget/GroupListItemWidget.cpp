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

//设置item窗口数据
void GroupListItemWidget::setItemWidget(const QString& group_id)
{
	//数据绑定
	m_groupId = group_id;
	if (!m_group || m_group->getGroupId() != group_id)
	{
		m_group = GroupManager::instance()->findGroup(group_id);
	}
	refershItemWidget();

}

//刷新界面
void GroupListItemWidget::refershItemWidget()
{
	//页面显示
	AvatarManager::instance()->getAvatar(m_groupId, ChatType::Group, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
			ui->headLab->setPixmap(headPix);
		});

	ui->nameLab->setText(m_group->getGroupName());
}

//设置分组
void GroupListItemWidget::setGrouping(const QString& grouping)
{
	m_group->setGrouping(grouping);
}

//是否显示id
void GroupListItemWidget::showGroupId()
{
	ui->messageWidget->setVisible(true);
	ui->preMessageLab->setText(m_groupId);
}



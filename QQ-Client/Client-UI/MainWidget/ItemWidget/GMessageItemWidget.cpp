#include "GMessageItemWidget.h"
#include "ui_ItemWidget.h"

#include "ImageUtil.h"
#include "AvatarManager.h"
#include "GroupManager.h"

GMessageItemWidget::GMessageItemWidget(QWidget* parent)
	:ItemWidget(parent)
	,m_timeLab(new QLabel(this))
	,m_countLab(new QLabel(this))
{
	init();
}
void GMessageItemWidget::init()
{
	ui->rightWidget->setLayout(new QHBoxLayout(ui->rightWidget));
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
}

void GMessageItemWidget::setItemWidget(const QString& group_id)
{
	//数据绑定
	m_groupId = group_id;
	if (!m_group || m_group->getGroupId() != group_id)
	{
		m_group = GroupManager::instance()->findGroup(group_id);
	}
	//页面显示
	auto pixmap = AvatarManager::instance()->getAvatar(m_groupId, ChatType::Group);
	auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
	ui->nameLab->setText(m_group->getGroupName());
	//最新消息
}

void GMessageItemWidget::clearUnRead()
{
	m_countLab->setVisible(false);
}




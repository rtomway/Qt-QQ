#include "GroupMemberItemWidget.h"
#include "ui_ItemWidget.h"
#include <QBoxLayout>

#include "GroupManager.h"
#include "AvatarManager.h"
#include "ImageUtil.h"
#include "UserProfileDispatcher.h"


GroupMemberItemWidget::GroupMemberItemWidget(const QString& group_id,QWidget* parent)
	:ItemWidget(parent)
	, m_group_id(group_id)
	, m_groupRoleLab(new QLabel(this))
{
	init();
}

GroupMemberItemWidget::~GroupMemberItemWidget()
{
	
}

void GroupMemberItemWidget::init()
{
	this->installEventFilter(this);
	ui->rightWidget->setLayout(new QHBoxLayout(ui->rightWidget));
	auto rightLayout = ui->rightWidget->layout();
	rightLayout->addWidget(m_groupRoleLab);
	ui->messageWidget->setVisible(false);
	m_groupRoleLab->setStyleSheet(QString("background-color:rgb(183, 212, 250)"));
	m_groupRoleLab->setContentsMargins(6, 6, 6, 6);
}


void GroupMemberItemWidget::setItemWidget(const QString& member_id)
{
	m_member_id = member_id;
	auto group = GroupManager::instance()->findGroup(m_group_id);
	m_member = group->getMember(member_id);
	refershWidget();
}

void GroupMemberItemWidget::refershWidget()
{
	ui->nameLab->setText(m_member.member_name);
	m_groupRoleLab->setText(m_member.member_role);
	AvatarManager::instance()->getAvatar(m_member_id, ChatType::User, [=](const QPixmap& pixmap)
		{
			if (!pixmap)
			{
				return;
			}
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
			ui->headLab->setPixmap(headPix);
		});

}


bool GroupMemberItemWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		auto point = this->mapToGlobal(QPoint(0, 0));
		UserProfileDispatcher::instance()->showUserProfile(m_member_id, point, PopUpPosition::Left);
		return true;
	}
	return false;
}


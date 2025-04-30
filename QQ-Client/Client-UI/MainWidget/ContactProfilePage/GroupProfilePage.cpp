#include "GroupProfilePage.h"
#include "ui_GroupProfilePage.h"
#include <QFile>

#include "GroupManager.h"
#include "AvatarManager.h"
#include "ImageUtil.h"

GroupProfilePage::GroupProfilePage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::GroupProfilePage)
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/FriendProfilePage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
	this->setWindowFlag(Qt::FramelessWindowHint);
}

void GroupProfilePage::init()
{
	//发消息
	connect(ui->sendmessageBtn, &QPushButton::clicked, this, [=]
		{
			GroupManager::instance()->emit chatWithGroup(m_groupId);
		});
}

void GroupProfilePage::setGroupProfile(const QString& group_id)
{
	//m_oneself为空或id不等,存入新的Friend
	if (!m_group || m_group->getGroupId() != group_id)
	{
		m_group = GroupManager::instance()->findGroup(group_id);
	}
	//获取friend信息
	m_groupId = group_id;
	m_groupJson = m_group->getGroupProfile();

	AvatarManager::instance()->getAvatar(m_groupId, ChatType::Group, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100));
			ui->headLab->setPixmap(headPix);
		});
	ui->nameLab->setText(m_groupJson["group_name"].toString() + "(" + QString::number(m_groupJson["groupMemberCount"].toInt()) + "人)");
	ui->idPreLab->setText("群号:");
	ui->idLab->setText(m_groupId);
}

void GroupProfilePage::clearWidget()
{
	m_group = nullptr;
}

#include "GroupProfilePage.h"
#include "ui_GroupProfilePage.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

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

}

void GroupProfilePage::init()
{
	this->setWindowFlag(Qt::FramelessWindowHint);
	ui->headLab->installEventFilter(this);

	//发消息
	connect(ui->sendmessageBtn, &QPushButton::clicked, this, [=]
		{
			GroupManager::instance()->emit chatWithGroup(m_groupId);
		});

	//群信息更新
	connect(GroupManager::instance(), &GroupManager::updateGroupProfile, this, [=](const QString& group_id)
		{
			if (m_group && m_groupId == group_id)
				setGroupProfile(m_groupId);
		});
}

//群信息设置
void GroupProfilePage::setGroupProfile(const QString& group_id)
{
	if (!m_group || m_group->getGroupId() != group_id)
	{
		m_group = GroupManager::instance()->findGroup(group_id);
	}
	//获取group信息
	m_groupId = group_id;
	m_groupJson = m_group->getGroupProfile();

	refresh();
}

//界面更新
void GroupProfilePage::refresh()
{
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

bool GroupProfilePage::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui->headLab && event->type() == QEvent::MouseButtonPress)
	{
		qDebug() << "-------------------------";
		m_avatarNewPath = QFileDialog::getOpenFileName(this, "选择头像", "",
			"Images(*.jpg *.png *.jpeg *.bnp)");
		if (!m_avatarNewPath.isEmpty())
		{
			m_avatarIsChanged = false;
			//头像是否更改
			if (m_avatarNewPath == m_avatarOldPath)
			{
				return false;
			}
			QPixmap avatar(m_avatarNewPath);
			if (avatar.isNull())
			{
				// 头像加载失败，给出提示
				QMessageBox::warning(this, "错误", "无法加载图片，选择的文件不是有效的头像图片。");
				return false;  // 如果头像无效，返回 false
			}
			ui->headLab->setPixmap(ImageUtils::roundedPixmap(avatar, QSize(80, 80)));
			m_avatarOldPath = m_avatarNewPath;
			m_avatarIsChanged = true;
			ImageUtils::saveAvatarToLocal(m_avatarNewPath, m_groupId, ChatType::Group);

		}
	}
	return false;
}

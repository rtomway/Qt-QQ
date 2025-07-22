#include "GroupProfilePage.h"
#include "ui_GroupProfilePage.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QBoxLayout>
#include <QtConcurrent/QtConcurrent>

#include "GroupManager.h"
#include "AvatarManager.h"
#include "ImageUtil.h"
#include "PacketCreate.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"

constexpr int LAOD_MEMBERAVATAR_COUNT = 6;

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

	ui->GroupMemberBtn->setEnabled(false);
	ui->GroupMemberBtn->setIcon(QIcon(":/icon/Resource/Icon/groupMember.png"));
	ui->GroupMemberBtn->setText("群成员");

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

	//获取群主和部分成员id(显示头像)
	m_loadAvatar_memberIdList.clear();
	m_loadAvatar_memberIdList.append(m_group->getGroupOwerId());
	auto member_idList = m_group->getGroupMembersIdList();
	for (int i = 0; i < LAOD_MEMBERAVATAR_COUNT && i < m_group->count(); i++)
	{
		if (member_idList.at(i) != m_group->getGroupOwerId())
			m_loadAvatar_memberIdList.append(member_idList.at(i));
	}

	refresh();
}

//界面更新
void GroupProfilePage::refresh()
{
	//清除旧avatarLayout
	if (ui->memberAvatarWidget->layout() != nullptr)
		clearAvatarLayout();

	AvatarManager::instance()->getAvatar(m_groupId, ChatType::Group, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100));
			ui->headLab->setPixmap(headPix);
		});
	ui->nameLab->setText(m_groupJson["group_name"].toString() + "(" + QString::number(m_groupJson["groupMemberCount"].toInt()) + "人)");
	ui->idPreLab->setText("群号:");
	ui->idLab->setText(m_groupId);
	addMemberAvatar();
}

void GroupProfilePage::clearWidget()
{
	m_group = nullptr;
}

//添加群成员头像
void GroupProfilePage::addMemberAvatar()
{
	m_avatarLayout = new QHBoxLayout(ui->memberAvatarWidget);
	ui->memberAvatarWidget->setLayout(m_avatarLayout);

	for (auto& member_id : m_loadAvatar_memberIdList)
	{
		auto avatar_lab = new QLabel(this);
		m_avatarContains.append(avatar_lab);
		m_avatarLayout->addWidget(avatar_lab);
		AvatarManager::instance()->getAvatar(member_id, ChatType::User, [=](const QPixmap& pixmap)
			{
				auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
				avatar_lab->setPixmap(headPix);
			});
	}
	m_avatarLayout->addStretch();
}

void GroupProfilePage::clearAvatarLayout()
{
	delete m_avatarLayout;
	m_avatarLayout = nullptr;
	qDeleteAll(m_avatarContains);
	m_avatarContains.clear();
}

//更改群组头像
void GroupProfilePage::updateGroupAvatar()
{
	//本地保存
	ImageUtils::saveAvatarToLocal(m_avatarNewPath, m_groupId, ChatType::Group);
	//向服务器发送
	AvatarManager::instance()->emit UpdateGroupAvatar(m_groupId);
	//通知服务端
	QtConcurrent::run([=]() 
	{
		auto pixmap = QPixmap(m_avatarNewPath);
		qDebug() << "pximap:" << m_avatarNewPath;
		QByteArray byteArray;
		QBuffer buffer(&byteArray);
		buffer.open(QIODevice::WriteOnly);
		if (!pixmap.save(&buffer, "PNG"))
		{
			qDebug() << "Failed to convert avatar to PNG format.";
			return;
		}
		
		QVariantMap params;
		params["group_id"] = m_groupId;
		params["size"] = byteArray.size();

		auto packet = PacketCreate::binaryPacket("updateGroupAvatar", params, byteArray);
		QByteArray groupData;
		PacketCreate::addPacket(groupData, packet);
		auto allData = PacketCreate::allBinaryPacket(groupData);

		// 发到主线程发信号
		QMetaObject::invokeMethod(NetWorkServiceLocator::instance(), [=]() 
		{
			NetWorkServiceLocator::instance()->sendHttpRequest("updateGroupAvatar", allData, "application/octet-stream");
		});
	});

}

bool GroupProfilePage::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui->headLab && event->type() == QEvent::MouseButtonPress)
	{
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

			this->updateGroupAvatar();
		}
	}
	return false;
}

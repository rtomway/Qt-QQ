#include "FriendProfilePage.h"
#include "ui_FriendProfilePage.h"
#include <QFile>
#include <QJsonDocument>
#include <QToolButton>

#include "ImageUtil.h"
#include "SMaskWidget.h"
#include "ContactListWidget.h"
#include "FriendManager.h"
#include "MessageSender.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"


FriendProfilePage::FriendProfilePage(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::FriendProfilePage)
	, m_detailEditWidget(new FriendProfileEditWidget)
	, m_friend(nullptr)
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
FriendProfilePage::~FriendProfilePage()
{
	delete ui;
}
void FriendProfilePage::init()
{
	this->setObjectName("FriendProfilePage");

	ui->headLab->setPixmap(ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/h2.jpg"), QSize(100, 100)));
	ui->nameLab->setText("哈哈");
	ui->onlineBtn->setIcon(QIcon(":/icon/Resource/Icon/online.png"));
	ui->onlineBtn->setText("在线");
	ui->likeBtn->setIcon(QIcon(":/icon/Resource/Icon/like.png"));
	ui->likeBtn->setText(QString::number(m_likeCount));
	connect(ui->likeBtn, &QToolButton::clicked, [=]()
		{
			ui->likeBtn->setText(QString::number(++m_likeCount));
		});
	ui->genderBtn->setIcon(QIcon(":/icon/Resource/Icon/man.png"));
	ui->genderBtn->setText("男");
	ui->ageLab->setText(QString("25") + "岁");
	ui->friendGroupBtn->setIcon(QIcon(":/icon/Resource/Icon/friendgroup.png"));
	ui->signaltureBtn->setIcon(QIcon(":/icon/Resource/Icon/signalture.png"));
	//获取好友分组
	auto groupingName = ContactListWidget::getfGrouping();
	for (auto& name : groupingName)
	{
		ui->groupcomBox->addItem(name);
	}
	//信息编辑
	connect(ui->editdetailBtn, &QPushButton::clicked, [=]()
		{
			//小窗口编辑关闭
			if (!this->parent())
			{
				this->hide();
			}
			m_detailEditWidget->setUser(m_json);
			//蒙层
			SMaskWidget::instance()->popUp(m_detailEditWidget);
			auto mainWidgetSize = SMaskWidget::instance()->getMainWidgetSize();
			int x = (mainWidgetSize.width() - m_detailEditWidget->width()) / 2;
			int y = (mainWidgetSize.height() - m_detailEditWidget->height()) / 2;
			SMaskWidget::instance()->setPopGeometry(QRect(x, y, m_detailEditWidget->width(), m_detailEditWidget->height()));
		});
	//更新用户信息
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			qDebug() << "------------user_id:" << user_id;
			this->setFriendProfile(user_id);
		});
	//更新用户头像
	connect(AvatarManager::instance(), &AvatarManager::UpdateUserAvatar, this, [=](const QString& user_id)
		{
			AvatarManager::instance()->getAvatar(user_id, ChatType::User, [=](const QPixmap& pixmap)
				{
					auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100));
					ui->headLab->setPixmap(headPix);
				});
		});
	//发消息
	connect(ui->sendmessageBtn, &QPushButton::clicked, this, [=]
		{
			//小窗口编辑关闭
			if (!this->parent())
			{
				this->hide();
			}
			FriendManager::instance()->emit chatWithFriend(m_friendId);
		});
	//分组改变
	connect(ui->groupcomBox, &QComboBox::currentIndexChanged, [=](int index)
		{
			if (m_isBlockedComboBox)
				return;
			auto grouping = ui->groupcomBox->itemText(index);
			if (m_friend && !grouping.isEmpty())
			{
				auto oldGrouping = m_friend->getGrouping();
				qDebug() << "oldGrouping" << oldGrouping;
				m_friend->setGrouping(grouping);

				FriendManager::instance()->emit updateFriendGrouping(m_friendId, oldGrouping);
				//发送给服务端
				auto& user_id = LoginUserManager::instance()->getLoginUserID();
				QJsonObject groupingObj;
				groupingObj["user_id"] = user_id;
				groupingObj["friend_id"] = m_friendId;
				groupingObj["grouping"] = grouping;
				QJsonDocument doc(groupingObj);
				auto data = doc.toJson(QJsonDocument::Compact);
				MessageSender::instance()->sendHttpRequest("updateFriendGrouping", data, "application/json");
			}
		});
}
//更新好友分组
void FriendProfilePage::updateFriendgrouping()
{
	ui->groupcomBox->clear();
	auto groupingName = ContactListWidget::getfGrouping();
	for (auto name : groupingName)
	{
		ui->groupcomBox->addItem(name);
	}
}
//用户设置
void FriendProfilePage::setFriendProfile(const QString& user_id)
{
	//m_oneself为空或id不等,存入新的Friend
	if (!m_friend || m_friend->getFriendId() != user_id)
	{
		m_friend = FriendManager::instance()->findFriend(user_id);
	}
	//获取friend信息
	m_friendId = user_id;
	m_json = m_friend->getFriend();
	//未设置信息隐藏
	if (LoginUserManager::instance()->getLoginUserID() != m_friendId)
	{
		ui->editdetailBtn->setVisible(false);
	}
	else {
		ui->editdetailBtn->setVisible(true);
	}
	//年龄
	if (m_json["age"].toInt() == 0)
	{
		ui->line_2->setVisible(false);
		ui->ageLab->setVisible(false);
	}
	else {
		ui->line_2->setVisible(true);
		ui->ageLab->setVisible(true);
		ui->ageLab->setText(QString::number(m_json["age"].toInt()) + "岁");
	}
	//生日
	if (m_json["birthday"].toString().isEmpty())
	{
		ui->line_3->setVisible(false);
		ui->birthdayLab->setVisible(false);
	}
	else {
		ui->line_3->setVisible(true);
		ui->birthdayLab->setVisible(true);
		ui->birthdayLab->setText(m_json["birthday"].toString());
	}
	//居住地
	if (m_json["resident"].toString().isEmpty())
	{
		ui->line_4->setVisible(false);
		ui->label_2->setVisible(false);
		ui->residentLab->setVisible(false);
	}
	else
	{
		ui->line_4->setVisible(true);
		ui->label_2->setVisible(true);
		ui->residentLab->setVisible(true);
	}
	//控件信息更新
	ui->nameLab->setText(m_json["username"].toString());
	ui->idLab->setText(m_friendId);
	ui->genderBtn->setText(m_json["gender"].toInt() == 1 ? "男" : (m_json["gender"].toInt() == 2 ? "女" : "未知"));
	//性别图标
	if (m_json["gender"].toInt() == 1)
	{
		ui->genderBtn->setIcon(QIcon(":/icon/Resource/Icon/man.png"));
	}
	else if (m_json["gender"].toInt() == 2)
	{
		ui->genderBtn->setIcon(QIcon(":/icon/Resource/Icon/woman.png"));
	}
	else
	{
		ui->genderBtn->setIcon(QIcon(":/icon/Resource/Icon/nogender.png"));
	}
	ui->groupcomBox->setCurrentText(m_json["grouping"].toString());
	AvatarManager::instance()->getAvatar(m_friendId, ChatType::User, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100));
			ui->headLab->setPixmap(headPix);
		});
	ui->signaltureLab->setText(m_json["signature"].toString());

	m_isBlockedComboBox = false;
}
//清空
void FriendProfilePage::clearWidget()
{
	m_friend = nullptr;
	//退出后禁止信号
	m_isBlockedComboBox = true;
}

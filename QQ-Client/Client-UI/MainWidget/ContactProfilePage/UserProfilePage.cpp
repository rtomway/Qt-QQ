#include "UserProfilePage.h"
#include "ui_UserProfilePage.h"
#include <QFile>

#include "AvatarManager.h"
#include "ImageUtil.h"
#include "FriendManager.h"
#include "LoginUserManager.h"

UserProfilePage::UserProfilePage(QWidget* parent)
	:AngleRoundedWidget(parent)
	,ui(new Ui::UserProfilePage)
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/UserProfilePage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
}
UserProfilePage::~UserProfilePage()
{
	qDebug() << "UserProfilePage destoryed:"<<m_user_id;
}

void UserProfilePage::init()
{
	this->setObjectName("UserProfilePage");
	this->setWindowFlag(Qt::FramelessWindowHint);
	//设置弹窗属性，点击窗口外部，窗口自动关闭
	this->setWindowFlag(Qt::Popup);
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->setFixedWidth(350);
	//发消息
	connect(ui->sendmessageBtn, &QPushButton::clicked, this, [=]
		{
			this->hide();
			FriendManager::instance()->emit chatWithFriend(m_user_id);
		});
	//编辑按钮||好友添加
	connect(ui->editdetailBtn, &QPushButton::clicked, this, [=]
		{
			this->hide();
			if (m_userRelation == UserRelation::LoginUser)
			{
				editUserProfile();
			}
			else if (m_userRelation == UserRelation::Stranger)
			{
				addFriendRequest();
			}
		});
}
//设置用户信息
void UserProfilePage::setUserProfilePage(const QJsonObject& obj, UserRelation userRelation)
{
	m_json = obj;
	m_user_id = obj["user_id"].toString();
	m_userRelation = userRelation;
	refreshProfilePage();
}
//更新用户信息
void UserProfilePage::refreshProfilePage()
{
	clearDynamicWidgets();
	ui->nameLab->setText(m_json["username"].toString());
	ui->idLab->setText(m_user_id);
	//头像
	AvatarManager::instance()->getAvatar(m_user_id, ChatType::User, [this](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100));
			ui->headLab->setPixmap(headPix);
		});
	//编辑按钮设置
	switch (m_userRelation)
	{
	case UserRelation::LoginUser:
		ui->editdetailBtn->setText("编辑资料");
		break;
	case UserRelation::Friend:
		ui->editdetailBtn->setVisible(false);
		break;
	case UserRelation::Stranger:
		ui->editdetailBtn->setText("加为好友");
		break;
	default:
		break;
	}
	//性别
	auto genderLab = new QLabel(m_json["gender"].toInt() == 1 ? "男" : (m_json["gender"].toInt() == 2 ? "女" : "未知"));
	ui->formLayout->addRow("性别:", genderLab);
	m_dynamicLabels["gender"] = genderLab;
	//签名
	auto signatureLab = new QLabel(m_json["signature"].toString());
	ui->formLayout->addRow("签名:", signatureLab);
	m_dynamicLabels["signature"] = signatureLab;

	// 强制更新布局
	ui->formLayout->update();
	ui->formLayout->parentWidget()->updateGeometry();
}
//编辑资料
void UserProfilePage::editUserProfile()
{
	m_profileEditWidget = std::make_unique<FriendProfileEditWidget>();
	m_profileEditWidget->setUser(m_json);
	//蒙层
	SMaskWidget::instance()->popUp(m_profileEditWidget.get());
	auto mainWidgetSize = SMaskWidget::instance()->getMainWidgetSize();
	int x = (mainWidgetSize.width() - m_profileEditWidget->width()) / 2;
	int y = (mainWidgetSize.height() - m_profileEditWidget->height()) / 2;
	SMaskWidget::instance()->setPopGeometry(QRect(x, y, m_profileEditWidget->width(), m_profileEditWidget->height()));
}
//好友添加
void UserProfilePage::addFriendRequest()
{

}
//清除动态更新的控件
void UserProfilePage::clearDynamicWidgets()
{
	// 先删除所有动态标签
	for (auto it = m_dynamicLabels.begin(); it != m_dynamicLabels.end(); ) {
		QLabel* lab = it.value();
		ui->formLayout->removeWidget(lab);  // 先从布局中移除
		delete lab;                         // 然后删除对象
		it = m_dynamicLabels.erase(it);     // 从map中移除
	}

	// 确保所有行都被移除
	while (ui->formLayout->rowCount() > 0) {
		// 获取第一行的部件
		QLayoutItem* itemLabel = ui->formLayout->itemAt(0, QFormLayout::LabelRole);
		QLayoutItem* itemField = ui->formLayout->itemAt(0, QFormLayout::FieldRole);

		// 从布局中移除部件但不删除
		if (itemLabel) {
			QWidget* widget = itemLabel->widget();
			if (widget) {
				widget->setParent(nullptr);  // 解除父子关系
				ui->formLayout->removeWidget(widget);
			}
		}

		if (itemField) {
			QWidget* widget = itemField->widget();
			if (widget) {
				widget->setParent(nullptr);  // 解除父子关系
				ui->formLayout->removeWidget(widget);
			}
		}

		// 移除行
		ui->formLayout->removeRow(0);
	}
}


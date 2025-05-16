#include "UserProfilePage.h"
#include "ui_UserProfilePage.h"
#include <QFile>

#include "AvatarManager.h"
#include "ImageUtil.h"

UserProfilePage::UserProfilePage(QWidget* parent)
	:AngleRoundedWidget(parent)
	,ui(new Ui::UserProfilePage)
{
	ui->setupUi(this);
	this->setObjectName("UserProfilePage");
	QFile file(":/stylesheet/Resource/StyleSheet/UserProfilePage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
	this->setWindowFlag(Qt::FramelessWindowHint);
	//设置弹窗属性，点击窗口外部，窗口自动关闭
	this->setWindowFlag(Qt::Popup);
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

UserProfilePage::~UserProfilePage()
{

}

void UserProfilePage::init()
{
	ui->nameLab->setText(m_json["username"].toString());
	ui->idLab->setText(m_user_id);
	AvatarManager::instance()->getAvatar(m_user_id, ChatType::User, [this](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100));
			ui->headLab->setPixmap(headPix);
		});
	ui->formLayout->addRow("性别:",
		new QLabel(m_json["gender"].toInt() == 1 ? "男" : (m_json["gender"].toInt() == 2 ? "女" : "未知")));
	ui->formLayout->addRow("签名:",new QLabel(m_json["signature"].toString()));

}

void UserProfilePage::setUserProfilePage(const QJsonObject& obj, bool isFriend)
{
	m_isFriend = isFriend;
	m_json = obj;
	qDebug() << "setUserProfilePage:" << m_json;
	m_user_id = obj["user_id"].toString();
	init();
}


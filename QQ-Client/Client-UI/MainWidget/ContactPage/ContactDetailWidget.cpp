#include "ContactDetailWidget.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QDateEdit>
#include <QMessageBox>
#include <QBuffer>
#include <QPixmap>

#include "ImageUtil.h"
#include "Friend.h"
#include "FriendManager.h"
#include "MessageSender.h"
#include "PacketCreate.h"
#include "AvatarManager.h"



ContactDetailWidget::ContactDetailWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	, m_editDetail(new QLabel(this))
	, m_exitBtn(new QPushButton(this))
	, m_headLab(new QLabel(this))
	, m_nickNameEdit(new LineEditwithButton(this))
	, m_signaltureEdit(new LineEditwithButton(this))
	, m_genderEdit(new LineEditwithButton(this))
	, m_ageEdit(new LineEditwithButton(this))
	, m_birthdayEdit(new LineEditwithButton(this))
	, m_countryEdit(new LineEditwithButton(this))
	, m_provinceEdit(new LineEditwithButton(this))
	, m_areaEdit(new LineEditwithButton(this))
{
	this->setStyleSheet(R"(QWidget{background-color:rgb(240,240,240);border-radius: 10px;})");
	m_headLab->installEventFilter(this);
	init();
	setAttribute(Qt::WA_StyledBackground);
	setAttribute(Qt::WA_StyleSheet);
}

void ContactDetailWidget::init()
{
	//窗口初始化
	auto mlayout = new QVBoxLayout(this);
	mlayout->setSpacing(0);
	mlayout->setContentsMargins(20, 0, 0, 0);

	m_editDetail->setText("编辑资料");
	m_nickNameEdit->setPreLab("昵称");
	m_signaltureEdit->setPreLab("个签");
	m_genderEdit->setPreLab("性别");
	m_ageEdit->setPreLab("年龄");
	m_birthdayEdit->setPreLab("生日");
	m_countryEdit->setPreLab("国家");
	m_provinceEdit->setPreLab("省份");
	m_areaEdit->setPreLab("地区");

	m_genderEdit->addMenuItem("男");
	m_genderEdit->addMenuItem("女");


	m_genderEdit->setEditEnable(false);
	m_birthdayEdit->setEditEnable(false);
	m_countryEdit->setEditEnable(false);
	m_provinceEdit->setEditEnable(false);
	m_areaEdit->setEditEnable(false);

	m_nickNameEdit->setFixedWidth(this->width() - 150);
	m_signaltureEdit->setFixedWidth(this->width() - 150);
	m_genderEdit->setFixedWidth(this->width() - 150);
	m_ageEdit->setFixedWidth(this->width() - 150);
	m_birthdayEdit->setFixedWidth(this->width() - 150);
	m_countryEdit->setFixedWidth(this->width() - 150);
	m_nickNameEdit->setFixedHeight(55);
	m_signaltureEdit->setFixedHeight(55);
	m_genderEdit->setFixedHeight(55);
	m_ageEdit->setFixedHeight(55);
	m_birthdayEdit->setFixedHeight(55);
	m_countryEdit->setFixedHeight(55);

	//top
	m_exitBtn->setFixedSize(32, 32);
	m_exitBtn->setStyleSheet("border:none");

	m_exitBtn->setIcon(QIcon(":/icon/Resource/Icon/x.png"));
	auto toplayout = new QHBoxLayout;
	toplayout->addStretch();
	toplayout->addWidget(m_editDetail);
	toplayout->addStretch();
	toplayout->addWidget(m_exitBtn);
	mlayout->addLayout(toplayout);

	m_headPix = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/h1.jpg"), QSize(80, 80));
	m_headLab->setPixmap(m_headPix);
	m_headLab->setFixedHeight(90);

	QWidget* headWidget = new QWidget(this);
	headWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	headWidget->setMinimumHeight(100);
	auto headlayout = new QHBoxLayout(headWidget);
	headlayout->addStretch();
	headlayout->addWidget(m_headLab);
	headlayout->addStretch();

	mlayout->addWidget(headWidget);

	QWidget* editWidget = new QWidget(this);
	auto editLayout = new QVBoxLayout(editWidget);
	editLayout->setContentsMargins(0, 0, 0, 0);

	editLayout->addWidget(m_nickNameEdit);
	editLayout->addWidget(m_signaltureEdit);
	editLayout->addWidget(m_genderEdit);
	editLayout->addWidget(m_ageEdit);
	editLayout->addWidget(m_birthdayEdit);
	editLayout->addWidget(m_countryEdit);

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(m_provinceEdit);
	hlayout->addWidget(m_areaEdit);
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);

	mlayout->addWidget(editWidget);
	mlayout->addLayout(hlayout);

	auto saveLayout = new QHBoxLayout;
	saveLayout->setSpacing(10);
	auto okBtn = new QPushButton("保存", this);
	okBtn->setStyleSheet(
		QString("\
            QPushButton{\
            background-color: #0096FF;\
            color:white;\
            height:40px;\
            border-radius:5px;\
            width:75px;\
            }\
           QPushButton:hover{\
             background-color: #048eee;\
            }\
            "));
	auto cancelBtn = new QPushButton("取消", this);
	cancelBtn->setStyleSheet(
		QString("\
            QPushButton{  \
            border:1px solid gray;\
            background-color:rgb(240,240,240);\
            color:black;\
            height:40px;\
            border-radius:5px;\
            width:75px;}\
            QPushButton:hover{\
              background-color: #c1c1c1  \
            }\
            "\
		));
	saveLayout->addStretch();
	saveLayout->addWidget(okBtn);
	saveLayout->addWidget(cancelBtn);

	mlayout->addStretch();
	mlayout->addLayout(saveLayout);
	mlayout->addStretch();

	m_genderEdit->setComboBox();
	m_countryEdit->setComboBox();
	m_provinceEdit->setComboBox();
	m_areaEdit->setComboBox();

	connect(m_exitBtn, &QPushButton::clicked, [=]
		{
			this->hide();
		});
	connect(m_birthdayEdit, &LineEditwithButton::clicked, this, [=]
		{
			qDebug() << "选择生日";
			auto menu = m_birthdayEdit->getMenu();
			if (!m_calendarAction)
			{
				m_calendarAction = std::make_unique<QWidgetAction>(menu);
			}
			// 创建 QCalendarWidget 并放入菜单
			if (!m_calendarWidget)
			{
				m_calendarWidget = std::make_unique<QCalendarWidget>();
				m_calendarWidget->setGridVisible(true);
				m_calendarWidget->setNavigationBarVisible(true);
				// 让 QCalendarWidget 宽度和 m_birthdayEdit 相同
				m_calendarWidget->setFixedWidth(m_birthdayEdit->width());
			}
			m_calendarWidget->setStyleSheet(R"(
            QCalendarWidget QWidget {
                background-color: #f0f0f0;
                border: none;
            }
            QCalendarWidget QToolButton {
                border: none;
                color: black;
                font-size: 16px;
                padding: 5px;
            }
            QCalendarWidget QToolButton
          {
                color: black;
                font-size: 16px;
                font-weight: bold;
            }
        )");
			m_calendarAction->setDefaultWidget(m_calendarWidget.get());
			menu->addAction(m_calendarAction.get());
			menu->popup(m_birthdayEdit->mapToGlobal(QPoint(0, m_birthdayEdit->height() - 10)));
			connect(m_calendarWidget.get(), &QCalendarWidget::clicked, this, [=](const QDate& date) {
				m_birthdayEdit->setText(date.toString("MM-dd"));
				menu->close();  // 关闭菜单
				});
		});
	connect(cancelBtn, &QPushButton::clicked, [=]
		{
			this->hide();
		});
	connect(okBtn, &QPushButton::clicked, [=]
		{
			//信息更新
			qDebug() << "m_avatarIsChange" << m_avatarIsChange;
			//头像更新
			if (m_avatarIsChange)
			{
				updateAvatar();
			}
			m_json["avatar_path"] = m_userId + ".png";
			m_json["username"] = m_nickNameEdit->getLineEditText();
			m_json["signature"] = m_signaltureEdit->getLineEditText();
			if (!m_genderEdit->getLineEditText().isEmpty())
				m_json["gender"] = m_genderEdit->getLineEditText() == "男" ? 1 : 2;
			m_json["birthday"] = m_birthdayEdit->getLineEditText();
			m_json["age"] = m_ageEdit->getLineEditText().toInt();
			auto user = FriendManager::instance()->findFriend(m_userId);
			user->setFriend(m_json);
			//向客户端其他控件更新信号
			FriendManager::instance()->emit UpdateFriendMessage(m_userId);
			//向服务端发送更新信息
			QVariantMap friendObj = user->getFriend().toVariantMap();
			//客户端独立信息删除
			friendObj.remove("grouping");
			friendObj.remove("avatar_path");
			MessageSender::instance()->sendMessage("updateUserMessage", friendObj);
			this->hide();
		});
}
void ContactDetailWidget::updateAvatar()
{
	//更新头像文件和缓存
	ImageUtils::saveAvatarToLocal(m_avatarNewPath, m_userId);
	AvatarManager::instance()->updateAvatar(m_userId);
	qDebug() << "ContactDetailWidget" << AvatarManager::instance()->getAvatar(m_userId);
	//通知内部客户端
	AvatarManager::instance()->emit UpdateUserAvatar(m_userId);
	//通知服务端
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	// 将 QPixmap 转换为 PNG 并存入 QByteArray
	if (!m_headPix.save(&buffer, "PNG")) {
		qDebug() << "Failed to convert avatar to PNG format.";
		return;
	}
	QVariantMap params;
	params["user_id"] = m_userId;
	params["size"] = byteArray.size();
	//二进制数据的发送
	auto packet = PacketCreate::binaryPacket("updateUserAvatar", params, byteArray);
	QByteArray userData;
	PacketCreate::addPacket(userData, packet);
	auto allData = PacketCreate::allBinaryPacket(userData);
	MessageSender::instance()->sendBinaryData(allData);
}

void ContactDetailWidget::setUser(const QJsonObject& obj)
{
	m_json = obj;
	m_userId = obj["user_id"].toString();
	QSharedPointer<Friend> myfriend = FriendManager::instance()->findFriend(m_userId);
	auto pixmap = ImageUtils::roundedPixmap(AvatarManager::instance()->getAvatar(m_userId), QSize(80, 80));
	m_headLab->setPixmap(pixmap);
	m_nickNameEdit->setText(m_json["username"].toString());
	m_genderEdit->setText(m_json["gender"].toInt() == 1 ? "男" : "女");
	m_ageEdit->setText(QString::number(m_json["age"].toInt()));
	m_birthdayEdit->setText(m_json["birthday"].toString());
	m_signaltureEdit->setText(m_json["signature"].toString());
}

const QJsonObject& ContactDetailWidget::getUser() const
{
	return m_json;
}

bool ContactDetailWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == m_headLab && event->type() == QEvent::MouseButtonPress)
	{
		m_avatarNewPath = QFileDialog::getOpenFileName(this, "选择头像", "",
			"Images(*.jpg *.png *.jpeg *.bnp)");
		if (!m_avatarNewPath.isEmpty())
		{
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
			m_headPix = avatar;
			m_headLab->setPixmap(ImageUtils::roundedPixmap(avatar, QSize(80, 80)));
			m_avatarOldPath = m_avatarNewPath;
			m_avatarIsChange = true;
		}
	}
	return false;
}

#include "LoginWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QFile>
#include <QGraphicsDropShadowEffect>
#include <QPalette>
#include <QSpacerItem>
#include <QMenu>
#include "RegisterPage.h"
#include "ImageUtil.h"
#include "RoundLabel.h"
#include <QPainter>
#include<QPixmap>
#include<QBitmap>
#include <QJsonDocument>
#include <QJsonObject>

#include "Client.h"
#include "SConfigFile.h"
#include "User.h"
#include <memory>
#include <QPointer>

LoginWidget::LoginWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	,m_account(new LineEditwithButton(this))
	,m_password(new LineEditwithButton(this))
	,m_loginBtn(new QPushButton("登录",this))
	,m_setBtn(new QPushButton(this))
	,m_exitBtn(new QPushButton(this))
	,m_scanBtn(new QPushButton("扫码登录", this))
	,m_moreBtn(new QPushButton("更多选项", this))
	,m_attentionRBtn(new QRadioButton("已阅读并同意协议", this))
{
	this->setObjectName("login");
	init();
	this->setWindowFlag(Qt::FramelessWindowHint);
	setFixedSize(320, 450);
	QFile file(":/stylesheet/Resource/StyleSheet/LoginWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else
	{
			qDebug() << file.fileName() << "打开失败";
	}
	this->installEventFilter(this);
	qDebug() << "loginWidget:" << size();
}

void LoginWidget::init()
{
	//布局
	auto mlayout = new QVBoxLayout(this);

	auto toplayout = new QHBoxLayout;
	m_setBtn->setIcon(QIcon(":/icon/Resource/Icon/set.png"));
	m_setBtn->setFixedWidth(15);
	m_exitBtn->setIcon(QIcon(":/icon/Resource/Icon/x.png"));
	m_exitBtn->setFixedWidth(15);
	m_exitBtn->setObjectName("top");
	m_setBtn->setObjectName("top");
	toplayout->addStretch();
	toplayout->addWidget(m_setBtn);
	toplayout->addWidget(m_exitBtn);

	mlayout->addLayout(toplayout);

	auto headerlayout = new QHBoxLayout;
	headerlayout->addStretch();
	auto header = new QLabel(this);
	header->setFixedSize(80, 80);
	
	QPixmap pixmap = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(80, 80));
	header->setPixmap(pixmap);
	headerlayout->addWidget(header);
	headerlayout->addStretch();

	mlayout->addLayout(headerlayout);

	m_account->setPlaceholderText("输入QQ账号");
	m_account->setEditPosition(Qt::AlignCenter);
	m_account->setclearBtn();
	m_password->setPlaceholderText("输入QQ密码");
	m_password->setEchoMode();
	m_password->setEditPosition(Qt::AlignCenter);
	m_password->setclearBtn();
	
	auto a_layout = new QHBoxLayout;
	a_layout->addSpacerItem(new QSpacerItem(30, m_account->height()));
	a_layout->addWidget(m_account);
	a_layout->addSpacerItem(new QSpacerItem(30, m_account->height()));
	auto p_layout = new QHBoxLayout;
	p_layout->addSpacerItem(new QSpacerItem(30, m_password->height()));
	p_layout->addWidget(m_password);
	p_layout->addSpacerItem(new QSpacerItem(30, m_password->height()));

	mlayout->addLayout(a_layout);
	mlayout->addLayout(p_layout);

	auto RBtn_layout = new QHBoxLayout;
	RBtn_layout->addStretch();
	RBtn_layout->addWidget(m_attentionRBtn);
	RBtn_layout->addStretch();

	auto login_layout = new QHBoxLayout;
	login_layout->addSpacerItem(new QSpacerItem(30, m_password->height()));
	login_layout->addWidget(m_loginBtn);
	login_layout->addSpacerItem(new QSpacerItem(30, m_password->height()));

	mlayout->addLayout(RBtn_layout);
	mlayout->addSpacerItem(new QSpacerItem(this->width(), 30));
	mlayout->addLayout(login_layout);

	auto lastLayout = new QHBoxLayout;
	lastLayout->addStretch();
	lastLayout->addWidget(m_scanBtn);
	m_scanBtn->setObjectName("end");
	m_moreBtn->setObjectName("end");
	auto lab = new QLabel("|");
	lastLayout->addWidget(lab);
	lastLayout->addWidget(m_moreBtn);
	lastLayout->addStretch();

	mlayout->addStretch();
	mlayout->addLayout(lastLayout);
	

	//更多的菜单
	auto moreMenu = new QMenu(this);
	moreMenu->addAction("注册账号", this, [=]
		{
			//auto registerPage = new RegisterPage;
			QPointer<RegisterPage>registerPage = new RegisterPage;
			registerPage->show();
			registerPage->close();
			connect(registerPage, &RegisterPage::destroyed, [registerPage]() {
				qDebug() << "窗口已销毁";
				});
			connect(registerPage, &QObject::destroyed, []() {
				qDebug() << "对象被销毁";  // 确认对象是否销毁
				});
		});
	
	moreMenu->addAction("忘记密码", this, [=]
		{
			
		});

	connect(m_exitBtn, &QPushButton::clicked, [=]
		{
			hide();
		});

	//点击别处使lineedit失去焦点
	connect(this, &LoginWidget::editfinish, m_account, &LineEditwithButton::editfinished);
	connect(this, &LoginWidget::editfinish, m_password, &LineEditwithButton::editfinished);
	
	auto config = new SConfigFile("config.ini");
	QFile configFile("config.ini");
	qDebug() << configFile.exists();
	if (configFile.exists())
	{
		qDebug() << "aaa";
		m_account->setText(config->value("user_id").toString());
		m_password->setText(config->value("password").toString());
	}
	//登录
	connect(m_loginBtn, &QPushButton::clicked, [=]
		{
			auto user_id = m_account->getLineEdit();
			auto password = m_password->getLineEdit();
			QVariantMap loginParams;
			loginParams["user_id"] = user_id;
			loginParams["password"] = password;
			Client::instance()->sendMessage("login", loginParams)
				->ReciveMessage([=](const QString&message)
					{
						QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
						if (doc.isObject())
						{
							QJsonObject obj = doc.object();
							QJsonObject data = obj["data"].toObject();
							auto user_name = data["username"].toString();
							qDebug() << obj["code"]<< data["username"].toString();
							if (obj["code"].toInt() == 0)
							{
								//登录成功 用户信息写入配置文件
								auto config = new SConfigFile("config.ini");
								config->setValue("user_id", user_id);
								config->setValue("password", password);

								//当前登录用户信息
								User::instance()->setUserId(user_id);
								User::instance()->setUserName(user_name);
								emit Loginsuccess();
							}
							else
							{
								qDebug() << obj["message"].toString();
							}

						}
					});
			
		});

	//点击更多弹出菜单
	connect(m_moreBtn, &QPushButton::clicked, [=]
		{
			moreMenu->popup(mapToGlobal(QPoint(m_moreBtn->geometry().x(), m_moreBtn->geometry().y()-70)));
		});
	
}


bool LoginWidget::eventFilter(QObject* watched, QEvent* event)
{
	//可拖动
	QMouseEvent* mev = dynamic_cast<QMouseEvent*>(event);
	if (event->type() == QEvent::MouseButtonPress)
	{
		emit editfinish();
	}
	
	return false;
}

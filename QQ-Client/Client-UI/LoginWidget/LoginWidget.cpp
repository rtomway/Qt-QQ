﻿#include "LoginWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>


#include "ImageUtil.h"
#include "RoundLabel.h"
#include "SConfigFile.h"
#include "RegisterPage.h"
#include "FriendManager.h"
#include "Friend.h"
#include "EventBus.h"
#include "LoginUserManager.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"


LoginWidget::LoginWidget(QWidget* parent)
	: AngleRoundedWidget(parent)
	, m_account(new LineEditwithButton(this))
	, m_password(new LineEditwithButton(this))
	, m_loginBtn(new QPushButton("登录", this))
	, m_setBtn(new QPushButton(this))
	, m_exitBtn(new QPushButton(this))
	, m_scanBtn(new QPushButton("扫码登录", this))
	, m_moreBtn(new QPushButton("更多选项", this))
	, m_attentionRBtn(new QRadioButton("已阅读并同意协议", this))
{
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/LoginWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
}

void LoginWidget::init()
{
	this->setObjectName("login");
	this->setWindowFlag(Qt::FramelessWindowHint);
	setFixedSize(320, 450);
	this->installEventFilter(this);

	initLayout();

	//更多的菜单
	auto moreMenu = new QMenu(this);
	moreMenu->addAction("注册账号", this, [=]
		{
			m_registerPage = std::make_unique<RegisterPage>();
			m_registerPage->show();
		});
	connect(EventBus::instance(), &EventBus::registerSuccess, this, [=](const QJsonObject& obj)
		{
			m_account->setText(obj["user_id"].toString());
			m_password->setText(obj["password"].toString());
		});

	//密码更新
	moreMenu->addAction("忘记密码", this, [=]
		{
			m_passwordChangePage = std::make_unique<PassWordChangePage>();
			m_passwordChangePage->show();
		});
	connect(EventBus::instance(), &EventBus::passwordChangeSuccess, this, [this]()
		{
			m_password->setText(QString());
		});

	//窗口关闭
	connect(m_exitBtn, &QPushButton::clicked, this, &LoginWidget::hide);

	//点击别处使lineedit失去焦点
	connect(this, &LoginWidget::editfinish, m_account, &LineEditwithButton::editfinished);
	connect(this, &LoginWidget::editfinish, m_password, &LineEditwithButton::editfinished);

	//个人信息配置
	SConfigFile config("config.ini");
	QFile configFile("config.ini");
	m_account->setText(config.value("user_id").toString());
	m_password->setText(config.value("password").toString());

	//发送登录信号
	connect(m_loginBtn, &QPushButton::clicked, this, &LoginWidget::onLoginRequest);

	//登录成功
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, [=]
		{
			emit Loginsuccess();
			auto user_id = m_account->getLineEditText();
			auto password = m_password->getLineEditText();
			SConfigFile config("config.ini");
			config.setValue("user_id", user_id);
			config.setValue("password", password);
		});

	//点击更多弹出菜单
	connect(m_moreBtn, &QPushButton::clicked, [=]
		{
			moreMenu->popup(mapToGlobal(QPoint(m_moreBtn->geometry().x(), m_moreBtn->geometry().y() - 70)));
		});
}

//布局
void LoginWidget::initLayout()
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
}

//发送登录请求
void LoginWidget::onLoginRequest()
{
	auto user_id = m_account->getLineEditText();
	auto password = m_password->getLineEditText();
	if (user_id.isEmpty() || password.isEmpty())
	{
		QMessageBox::warning(nullptr, "警告", "账号或密码不能为空");
		return;
	}
	QJsonObject loginObj;
	loginObj["user_id"] = user_id;
	loginObj["password"] = password;
	QJsonDocument doc(loginObj);
	auto data = doc.toJson(QJsonDocument::Compact);


	NetWorkServiceLocator::instance()->sendHttpPostRequest("loginValidation", data);

	SConfigFile config("config.ini");
	config.setValue("user_id", user_id);
	config.setValue("password", password);
}

//event--退出编辑
bool LoginWidget::eventFilter(QObject* watched, QEvent* event)
{
	QMouseEvent* mev = dynamic_cast<QMouseEvent*>(event);
	if (event->type() == QEvent::MouseButtonPress)
	{
		emit editfinish();
	}

	return false;
}

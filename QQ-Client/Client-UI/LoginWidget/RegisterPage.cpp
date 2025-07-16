#include "RegisterPage.h"
#include "ui_RegisterPage.h"
#include <QFile>
#include <QMouseEvent>
#include <QMessageBox>
#include <QJsonDocument>

#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include "EventBus.h"

RegisterPage::RegisterPage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::RegisterPage)
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/RegisterPage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
}

RegisterPage::~RegisterPage()
{
	delete ui;
}

void RegisterPage::init()
{
	this->installEventFilter(this);
	ui->passwordEdit->installEventFilter(this);
	//窗口初始化
	this->setFocus();
	ui->titleLab->setObjectName("title");
	ui->headerLab->setObjectName("header");
	this->setObjectName("this");
	ui->verifyEdit->setObjectName("verifyEdit");
	ui->veriftBtn->setObjectName("verifyBtn");

	ui->nickNameEdit->setPlaceholderText("输入昵称");
	ui->passwordEdit->setPlaceholderText("输入密码");
	ui->confidential->setPlaceholderText("输入个人密保");
	ui->phoneEdit->setPlaceholderText("输入手机号码");
	ui->verifyEdit->setPlaceholderText("输入短信验证码");

	ui->nickNameEdit->setAlignment(Qt::AlignCenter);
	ui->passwordEdit->setAlignment(Qt::AlignCenter);
	ui->phoneEdit->setAlignment(Qt::AlignCenter);
	ui->verifyEdit->setAlignment(Qt::AlignCenter);
	ui->passwordLab->setVisible(false);
	//注册
	connect(ui->registerBtn, &QPushButton::clicked, this, &RegisterPage::onRegisterUser);
	connect(EventBus::instance(), &EventBus::registerSuccess, this, &RegisterPage::hide);
}

//用户注册
void RegisterPage::onRegisterUser()
{
	if (ui->nickNameEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "警告", "昵称不能为空");
		return;
	}
	if (ui->passwordEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "警告", "密码不能为空");
		return;
	}
	//向服务器发送注册信息
	QJsonObject registerObj;
	registerObj["username"] = ui->nickNameEdit->text();
	registerObj["password"] = ui->passwordEdit->text();
	registerObj["confidential"] = ui->confidential->text();
	QJsonDocument doc(registerObj);
	auto data = doc.toJson(QJsonDocument::Compact);
	NetWorkServiceLocator::instance()->sendHttpRequest("register", data, "application/json");
}

//控件事件重写
bool RegisterPage::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui->passwordEdit && event->type() == QEvent::FocusIn)
	{
		ui->passwordLab->setVisible(true);
	}
	else if (watched == ui->passwordEdit && event->type() == QEvent::FocusOut)
	{
		ui->passwordLab->setVisible(false);
	}
	//点击控件外地方时清除控件焦点
	if (watched == this && event->type() == QEvent::MouseButtonPress)
	{
		this->setFocus();
	}
	return false;
}

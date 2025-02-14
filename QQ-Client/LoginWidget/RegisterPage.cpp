#include "RegisterPage.h"
#include "ui_RegisterPage.h"
#include <QFile>

RegisterPage::RegisterPage(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::RegisterPage)
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/RegisterPage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}else
	{
		qDebug() << file.fileName() << "打开失败";
	}
	connect(this, &RegisterPage::destroyed, [=]
		{
			qDebug() << "kkkkk";
		});
}

RegisterPage::~RegisterPage()
{
	qDebug() << "kkkkk";
	delete ui;
}

void RegisterPage::init()
{
	ui->titleLab->setObjectName("title");
	ui->headerLab->setObjectName("header");
	this->setObjectName("this");
	ui->nickNameEdit->setPlaceholderText("输入昵称");
	ui->passwordEdit->setPlaceholderText("输入密码");
	ui->phoneEdit->setPlaceholderText("输入手机号码");
	ui->verifyEdit->setPlaceholderText("输入短信验证码");
	ui->nickNameEdit->setAlignment(Qt::AlignCenter);
	ui->passwordEdit->setAlignment(Qt::AlignCenter);
	ui->phoneEdit->setAlignment(Qt::AlignCenter);
	ui->verifyEdit->setAlignment(Qt::AlignCenter);
	ui->verifyEdit->setObjectName("verifyEdit");
	ui->veriftBtn->setObjectName("verifyBtn");
	
	
}

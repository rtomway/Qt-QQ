#include "PassWordChangePage.h"
#include "ui_PassWordChangePage.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include "EventBus.h"

PassWordChangePage::PassWordChangePage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::PassWordChangePage)
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/PasswordChangePage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}

}

PassWordChangePage::~PassWordChangePage()
{

}

void PassWordChangePage::init()
{
	this->setFixedSize(700, 400);
	this->setObjectName("PassWordChangePage");
	this->setWindowIconText("密码修改");

	ui->accountEdit->setPlaceholderText("账号输入");
	ui->confidentialEdit->setPlaceholderText("密保输入");
	ui->passwordEdit->setPlaceholderText("输入新密码");
	ui->passwordEdit->setEchoMode(QLineEdit::Password);
	ui->okBtn->setText("确认");
	ui->cancelBtn->setText("取消");
	ui->accountEdit->setFixedWidth(200);
	ui->confidentialEdit->setFixedWidth(200);
	ui->passwordEdit->setFixedWidth(200);

	//密码可视
	connect(ui->visableBtn, &QPushButton::clicked, this, &PassWordChangePage::onPasswordVisable);
	//取消返回
	connect(ui->cancelBtn, &QPushButton::clicked, this, &PassWordChangePage::hide);
	//修改密码
	connect(ui->okBtn, &QPushButton::clicked, this, &PassWordChangePage::onChangePassword);

}

//密码可视
void PassWordChangePage::onPasswordVisable()
{
	m_passwordIsVisable = !m_passwordIsVisable;
	if (m_passwordIsVisable)
	{
		ui->passwordEdit->setEchoMode(QLineEdit::Normal);
		ui->visableBtn->setStyleSheet(QString("border-image: url(:/icon/Resource/Icon/showPassword.png);"));
	}
	else
	{
		ui->passwordEdit->setEchoMode(QLineEdit::Password);
		ui->visableBtn->setStyleSheet(QString("border-image: url(:/icon/Resource/Icon/hidePassword.png);"));
	}
}

//修改密码
void PassWordChangePage::onChangePassword()
{
	if (ui->accountEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "警告", "账号不能为空");
		return;
	}
	if (ui->passwordEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "警告", "个人密保不能为空");
		return;
	}
	if (ui->passwordEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "警告", "密码不能为空");
		return;
	}

	//向服务器发送注册信息
	QJsonObject passwordChangeObj;
	passwordChangeObj["user_id"] = ui->accountEdit->text();
	passwordChangeObj["password"] = ui->passwordEdit->text();
	passwordChangeObj["confidential"] = ui->confidentialEdit->text();
	QJsonDocument doc(passwordChangeObj);
	auto data = doc.toJson(QJsonDocument::Compact);

	NetWorkServiceLocator::instance()->sendHttpPostRequest("passwordChange", data,
		[this](const QJsonObject& obj, const QByteArray& data)
		{
			auto changeObj = obj["params"].toObject();
			auto error = changeObj["error"].toBool();
			qDebug() << "error:" << error;
			if (error)
			{
				QMessageBox::warning(this, "错误", "账号或个人密保错误");
				return;
			}
			else
			{
				QMessageBox::information(this, "成功", "密码修改成功");
				this->hide();
				EventBus::instance()->emit passwordChangeSuccess();
			}

		});
	/*NetWorkServiceLocator::instance()->sendHttpRequest("passwordChange", data, "application/json",
		[this](const QJsonObject& obj, const QByteArray& data)
		{
			auto changeObj = obj["params"].toObject();
			auto error = changeObj["error"].toBool();
			qDebug() << "error:" << error;
			if (error)
			{
				QMessageBox::warning(this, "错误", "账号或个人密保错误");
				return;
			}
			else
			{
				QMessageBox::information(this, "成功", "密码修改成功");
				this->hide();
				EventBus::instance()->emit passwordChangeSuccess();
			}

		});*/
}

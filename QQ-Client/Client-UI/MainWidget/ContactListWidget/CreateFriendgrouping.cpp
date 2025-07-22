#include "CreateFriendgrouping.h"
#include "ui_CreateFriendgrouping.h"
#include <QBoxLayout>

CreateFriendgrouping::CreateFriendgrouping(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::CreateFriendgrouping)
{
	ui->setupUi(this);
	init();
}

void CreateFriendgrouping::init()
{
	this->setFixedSize(300, 170);
	this->setStyleSheet(R"(
			QWidget{ border: none;border-radius:10px;background-color:white;}
			QLabel{	font-size:14px;color:gray}
			QLineEdit{border:1px solid rgb(240,240,240);border-radius:4px;height:35px;}
			QPushButton{border-radius:4px;height:35px;font-size:14px;}
		)");
	ui->cancelBtn->setStyleSheet(R"(
			QPushButton{background-color:white;color:black;border:1px solid rgb(240,240,240);}
			QPushButton:hover{background-color:rgb(240,240,240);}
			)");
	ui->createBtn->setStyleSheet(R"(
			QPushButton{background-color:#c7e3f8;color:white;}
			)");
	setAttribute(Qt::WA_StyledBackground);
	setAttribute(Qt::WA_StyleSheet);

	ui->groupingEdit->setPlaceholderText("填写新分组");
	//取消
	connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
		{
			ui->groupingEdit->clear();
			this->close();
		});
	ui->createBtn->setEnabled(false);
	connect(ui->groupingEdit, &QLineEdit::textEdited, this, [=](const QString& text)
		{
			ui->createBtn->setEnabled(false);
			ui->createBtn->setStyleSheet("background-color:#c7e3f8;color:white;");
			if (!text.isEmpty())
			{
				ui->createBtn->setEnabled(true);
				ui->createBtn->setStyleSheet(R"(
				QPushButton{background-color:#5facf1;color:white;}
				QPushButton:hover{background-color:#276691;color:white;}
			)");
			}
		});
	//创建分组
	connect(ui->createBtn, &QPushButton::clicked, this, [=]
		{
			emit createGrouping(ui->groupingEdit->text());
			ui->groupingEdit->clear();
			this->close();
		});
}

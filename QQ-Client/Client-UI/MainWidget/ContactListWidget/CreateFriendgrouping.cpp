#include "CreateFriendgrouping.h"
#include "ui_CreateFriendgrouping.h"
#include <QBoxLayout>

CreateFriendgrouping::CreateFriendgrouping(QWidget* parent)
	:AngleRoundedWidget(parent)
	,ui(new Ui::CreateFriendgrouping)
{
	ui->setupUi(this);
	init();
	this->setFixedSize(300, 170);
	this->setStyleSheet(R"(
			QWidget{ border: none;border-radius:10px;background-color:white;}
			QLabel{	font-size:14px;color:gray}
			QLineEdit{
				 border:1px solid rgb(240,240,240);
				 border-radius:4px;
				height:35px;
			}
			QPushButton{
				border: none;
				background-color:#0096FF;
				border-radius: 4px;
				color: white;
				height: 35px;
				font-size: 14px;
			}
			QPushButton:hover{
				font-size: 16px;
			}
		)");
	setAttribute(Qt::WA_StyledBackground);
	setAttribute(Qt::WA_StyleSheet);
}

void CreateFriendgrouping::init()
{
	ui->groupingEdit->setPlaceholderText("填写新分组");
	//取消
	connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
		{
			this->close();
		});
	//创建分组
	connect(ui->createBtn, &QPushButton::clicked, this, [=]
		{
			if (ui->groupingEdit->text().isEmpty())
			{
				return;
			}
			emit createGrouping(ui->groupingEdit->text());
			this->close();
		});
}

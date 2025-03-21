#include "CreateFriendgrouping.h"
#include "ui_CreateFriendgrouping.h"
#include <QBoxLayout>

CreateFriendgrouping::CreateFriendgrouping(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::CreateFriendgrouping)
{
	ui->setupUi(this);
	init();
	this->setStyleSheet(R"(
			QLabel{	font-size:18px;}
			QLineEdit{
				 border: 0px;
				padding: 0px;
				 margin: 0px;
				height:40px;
				font-size:18px;
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
				font-size: 18px;
			}
		)");
}

void CreateFriendgrouping::init()
{
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

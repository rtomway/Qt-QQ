#pragma once
#include "FriendNoticeItemWidget.h"
#include "ui_FriendNoticeItemWidget.h"

FriendNoticeItemWidget::FriendNoticeItemWidget(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::FriendNoticeItemWidget)
{
	ui->setupUi(this);
	init();
	/*this->setStyleSheet(R"(
		QLabel{
		background-color:red;
			color:green;
		}
		QWidget{
		background-color:blue;
		}	
		)");*/

}

void FriendNoticeItemWidget::init()
{

}

FriendNoticeItemWidget::~FriendNoticeItemWidget()
{

}

#include "FriendSetWidget.h"
#include "ui_FriendSetWidget.h"
#include "Friend.h"
#include "FriendManager.h"

#include <QMessageBox>

FriendSetWidget::FriendSetWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::FriendSetWidget)
{
	ui->setupUi(this);
	init();
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

}

void FriendSetWidget::setId(const QString& user_id)
{
	m_userId = user_id;
}

void FriendSetWidget::init()
{
	this->setStyleSheet(R"(
		QWidget{ border: none;background-color:white;}
		QPushButton{border:1px solid rgb(220,220,220);height:25px;border-radius:5px;}
		QPushButton:hover{background-color:rgb(220,220,220);}"
		)");
	ui->deleteBtn->setFixedWidth(200);
	connect(ui->deleteBtn, &QPushButton::clicked, this, [=]
		{
			auto deleteResult = QMessageBox::question(nullptr, "删除好友", "请确认是否删除");
			if (deleteResult == QMessageBox::No)
			{
				return;
			}

			this->hide();
			FriendManager::instance()->emit deleteFriend(m_userId);
			FriendManager::instance()->removeFriend(m_userId);
		});
}






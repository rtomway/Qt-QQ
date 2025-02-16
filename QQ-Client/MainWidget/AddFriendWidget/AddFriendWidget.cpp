#include "AddFriendWidget.h"
#include "ui_AddFriendWidget.h"
#include <QMouseEvent>
#include <QFile>
#include "SearchItemWidget.h"
#include "Client.h"
#include "User.h"
#include <QJSonDocument>
#include <QJSonObject>
#include <QJSonArray>

AddFriendWidget::AddFriendWidget(QWidget*parent)
	:QWidget(parent)
	,ui(new Ui::AddFriendWidget)
	,m_userList(new QListWidget(this))
	,m_groupList(new QListWidget(this))
{
	//this->setWindowFlag(Qt::FramelessWindowHint);
	resize(500, 500);
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/AddFriendWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
	/*m_userList->setStyleSheet(R"(
	QListWidget::item {
               background-color: transparent; 
     }
    QListWidget::item:hover {
      background-color: rgb(240,240,240); 
    }
    QWidget {
     background-color: transparent;  
    }
			)");*/
}
AddFriendWidget::~AddFriendWidget()
{
	delete ui;
}
void AddFriendWidget::init()
{
	ui->stackedWidget->addWidget(m_userList);
	ui->stackedWidget->addWidget(m_groupList);
	ui->stackedWidget->setCurrentWidget(m_userList);
	ui->searchLine->setPlaceholderText("用户搜索");
	//选择搜索
	connect(ui->userBtn, &QPushButton::clicked, [=]
		{
			m_groupList->clear();
			ui->stackedWidget->setCurrentWidget(m_userList);
			ui->searchLine->setPlaceholderText("用户搜索");
			type = search_type::User;
			ui->userBtn->setStyleSheet("QPushButton{background-color:rgb(240,240,240)}\
				QPushButton:hover{background-color:rgb(240,240,240)}");
			ui->groupBtn->setStyleSheet("QPushButton{background-color:white}");
		});
	connect(ui->groupBtn, &QPushButton::clicked, [=]
		{
			m_userList->clear();
			ui->stackedWidget->setCurrentWidget(m_groupList);
			ui->searchLine->setPlaceholderText("群聊搜索");
			type = search_type::Grouop;
			ui->groupBtn->setStyleSheet("QPushButton{background-color:rgb(240,240,240)}\
				QPushButton:hover{background-color:rgb(240,240,240)}");
			ui->userBtn->setStyleSheet("QPushButton{background-color:white}");
		});


	this->setWindowIcon(QIcon(":/icon/Resource/Icon/search.png"));
	ui->searchBtn->setCheckable(false);
	ui->searchBtn->setIcon(QIcon(":/icon/Resource/Icon/search.png"));
	
	//搜索栏
	connect(ui->searchidBtn, &QPushButton::clicked, this, [=]()
		{
			qDebug() << type;
			if (type == search_type::Grouop)
				return;

			m_userList->clear();
			auto search_id = ui->searchLine->text();
			qDebug() << "text:" << search_id;
			if (!search_id.isEmpty())
			{
				QVariantMap serach;
				serach["search_id"] = search_id;
				serach["user_id"] = User::instance()->getUserId();
				Client::instance()->sendMessage("searchUser", serach)
					->ReciveMessage([=](const QString& message)
						{
							QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
							QJsonObject obj = doc.object();
							if (obj["code"] == 0)
							{
								QJsonObject data = obj["data"].toObject();
								QJsonArray alldata= data["search_data"].toArray();
								for (auto object : alldata)
								{
									auto user = object.toObject();
									addListWidgetItem(m_userList, user);
								}
							}
							else
							{
								qDebug() << obj["message"].toString();
							}
						});
			}

		});
	connect(ui->searchidBtn, &QPushButton::clicked, this, [=]()
		{
			qDebug() << type;
			if (type == search_type::User)
				return;

			m_groupList->clear();
			auto search_id = ui->searchLine->text();
			qDebug() << "text:" << search_id;
			if (!search_id.isEmpty())
			{
				QVariantMap serach;
				serach["search_id"] = search_id;
				serach["user_id"] = User::instance()->getUserId();
				Client::instance()->sendMessage("searchGroup", serach)
					->ReciveMessage([=](const QString& message)
						{
							QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
							QJsonObject obj = doc.object();
							if (obj["code"] == 0)
							{
								QJsonObject data = obj["data"].toObject();
								QJsonArray alldata = data["search_data"].toArray();
								for (auto object : alldata)
								{
									auto group = object.toObject();
									addListWidgetItem(m_groupList, group);
								}
							}
							else
							{
								qDebug() << obj["message"].toString();
							}
						});
			}

		});
}

//查询结果返回添加到对应list上
void AddFriendWidget::addListWidgetItem(QListWidget* list,const QJsonObject& obj)
{
	//为item设置用户id
	auto item = new QListWidgetItem(list);
	item->setSizeHint(QSize(list->width(), 70));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new SearchItemWidget(list);

	if(list==m_userList)
	itemWidget->setUser(obj);
	else
	itemWidget->setGroup(obj);

	//关联item和widget
	list->setItemWidget(item, itemWidget);

}

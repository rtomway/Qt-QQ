#include "AddFriendWidget.h"
#include "ui_AddFriendWidget.h"
#include <QMouseEvent>
#include <QFile>
#include <QJSonDocument>
#include <QJSonObject>
#include <QJSonArray>

#include "SearchItemWidget.h"
#include "FriendManager.h"
#include "Friend.h"
#include "EventBus.h"
#include "MessageSender.h"
#include "LoginUserManager.h"


AddFriendWidget::AddFriendWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::AddFriendWidget)
	, m_userList(new QListWidget(this))
	, m_groupList(new QListWidget(this))
{
	resize(700, 500);
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/AddFriendWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
	this->setWindowIcon(QIcon(":/icon/Resource/Icon/search.png"));
}
AddFriendWidget::~AddFriendWidget()
{
	delete ui;
}
void AddFriendWidget::init()
{
	ui->searchLine->setPlaceholderText("用户搜索");
	ui->stackedWidget->addWidget(m_userList);
	ui->stackedWidget->addWidget(m_groupList);
	ui->stackedWidget->setCurrentWidget(m_userList);
	m_userList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_groupList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//选择搜索
	ui->userBtn->setStyleSheet(
		"QPushButton{background-color:rgb(240,240,240)}"
	);
	//用户
	connect(ui->userBtn, &QPushButton::clicked, [=]
		{
			m_groupList->clear();
			ui->stackedWidget->setCurrentWidget(m_userList);
			ui->searchLine->setPlaceholderText("用户搜索");
			type = search_type::User;
			ui->userBtn->setStyleSheet(
				"QPushButton{background-color:rgb(240,240,240)}"
			);
			ui->groupBtn->setStyleSheet(
				"QPushButton{background-color:white}"
				"QPushButton:hover{background-color:rgb(240,240,240)}"
			);
		});
	//群组
	connect(ui->groupBtn, &QPushButton::clicked, [=]
		{
			m_userList->clear();
			ui->stackedWidget->setCurrentWidget(m_groupList);
			ui->searchLine->setPlaceholderText("群聊搜索");
			type = search_type::Grouop;
			ui->groupBtn->setStyleSheet(
				"QPushButton{background-color:rgb(240,240,240)}"
			);
			ui->userBtn->setStyleSheet(
				"QPushButton{background-color:white}"
				"QPushButton:hover{background-color:rgb(240,240,240)}"
			);
		});
	//搜索图标
	ui->searchBtn->setCheckable(false);
	ui->searchBtn->setIcon(QIcon(":/icon/Resource/Icon/search.png"));
	//搜索栏
	connect(ui->searchidBtn, &QPushButton::clicked, this, [=]()
		{
			if (type == search_type::Grouop)
				return;
			m_userList->clear();
			auto search_id = ui->searchLine->text();
			if (!search_id.isEmpty())
			{
				QJsonObject serachObj;
				serachObj["search_id"] = search_id;
				serachObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
				QJsonDocument doc(serachObj);
				auto data = doc.toJson(QJsonDocument::Compact);
				MessageSender::instance()->sendHttpRequest("serachUser", data, "application/json");
			}
		});
	//搜索结果
	connect(EventBus::instance(), &EventBus::searchUser, this, [=](const QJsonObject& paramsObject, const QPixmap& pixmap)
		{
			addListWidgetItem(m_userList, paramsObject, pixmap);
		});
}
//查询结果返回添加到对应list上
void AddFriendWidget::addListWidgetItem(QListWidget* list, const QJsonObject& obj, const QPixmap& pixmap)
{
	//为item设置用户id
	auto item = new QListWidgetItem(list);
	item->setSizeHint(QSize(list->width(), 70));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new SearchItemWidget(list);
	if (list == m_userList)
	{
		itemWidget->setUser(obj);
		itemWidget->setPixmap(pixmap);
	}
	else
	{
		itemWidget->setGroup(obj);
	}
	//关联item和widget
	list->setItemWidget(item, itemWidget);
}

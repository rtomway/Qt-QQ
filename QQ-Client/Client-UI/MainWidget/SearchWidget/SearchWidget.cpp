#include "SearchWidget.h"
#include "ui_SearchWidget.h"
#include <QMouseEvent>
#include <QFile>
#include <QJSonDocument>
#include <QJSonObject>
#include <QJSonArray>
#include <QScrollBar>

#include "SearchItemWidget.h"
#include "FriendManager.h"
#include "Friend.h"
#include "GroupManager.h"
#include "EventBus.h"
#include "../Client-ServiceLocator/NetWorkServiceLocator.h"
#include "LoginUserManager.h"

constexpr int MaxPageSize = 20; //每次加载量
constexpr int MinScrollBottom = 10; //滚动条底部更新距离

SearchWidget::SearchWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::SearchWidget)
	, m_userList(new QListWidget(this))
	, m_groupList(new QListWidget(this))
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/AddFriendWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
}

SearchWidget::~SearchWidget()
{
	delete ui;
}

void SearchWidget::init()
{
	resize(700, 500);
	this->setWindowIcon(QIcon(":/icon/Resource/Icon/search.png"));

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
			m_searchType = search_type::User;
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
			m_searchType = search_type::Grouop;
			ui->groupBtn->setStyleSheet(
				"QPushButton{background-color:rgb(240,240,240)}"
			);
			ui->userBtn->setStyleSheet(
				"QPushButton{background-color:white}"
				"QPushButton:hover{background-color:rgb(240,240,240)}"
			);
		});

	//会话界面窗口关闭
	connect(FriendManager::instance(), &FriendManager::chatWithFriend, this, &SearchWidget::hide);
	connect(GroupManager::instance(), &GroupManager::chatWithGroup, this, &SearchWidget::hide);

	//搜索图标
	ui->searchBtn->setCheckable(false);
	ui->searchBtn->setIcon(QIcon(":/icon/Resource/Icon/search.png"));

	//搜索栏
	connect(ui->searchidBtn, &QPushButton::clicked, this, [=]()
		{
			m_searchText = ui->searchLine->text();
			if (m_searchText.isEmpty())
				return;
			//重置页码
			m_currentPage = 1;

			if (m_searchType == search_type::User)
			{
				m_userList->clear();
			}
			else
			{
				m_groupList->clear();
			}
			sendSearchRequest();
		});

	//搜索结果 
	connect(EventBus::instance(), &EventBus::searchUser, this, [=](const QJsonObject& paramsObject, const QPixmap& pixmap)
		{
			addListWidgetItem(m_userList, paramsObject, pixmap);
		});
	connect(EventBus::instance(), &EventBus::searchGroup, this, [=](const QJsonObject& paramsObject, const QPixmap& pixmap)
		{
			addListWidgetItem(m_groupList, paramsObject, pixmap);
		});

	//滑动到底部加载
	connect(m_userList->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value)
		{
			onListScroll(m_userList->verticalScrollBar(), value);
		});
	connect(m_groupList->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value)
		{
			onListScroll(m_groupList->verticalScrollBar(), value);
		});
}

//查询结果返回添加到对应list上
void SearchWidget::addListWidgetItem(QListWidget* list, const QJsonObject& obj, const QPixmap& pixmap)
{
	//为item设置用户id
	auto item = new QListWidgetItem(list);
	item->setSizeHint(QSize(list->width(), 70));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new SearchItemWidget(list);
	if (list == m_userList)
	{
		itemWidget->setUser(obj);
	}
	else
	{
		itemWidget->setGroup(obj);
	}
	itemWidget->setPixmap(pixmap);
	//关联item和widget
	list->setItemWidget(item, itemWidget);

	//一次性加载MaxPageSize
	m_loadCount++;
	if (m_loadCount == MaxPageSize)
	{
		m_isLoading = false;
		m_loadCount = 0;
		update();
	}
}

//发送搜索请求
void SearchWidget::sendSearchRequest()
{
	QJsonObject searchObj;
	searchObj["search_id"] = m_searchText;
	searchObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
	searchObj["page"] = m_currentPage;
	searchObj["pageSize"] = MaxPageSize;
	QJsonDocument doc(searchObj);
	auto data = doc.toJson(QJsonDocument::Compact);

	if (m_searchType == search_type::User)
	{
		NetWorkServiceLocator::instance()->sendHttpPostRequest("searchUser", data);
	}
	else
	{
		NetWorkServiceLocator::instance()->sendHttpPostRequest("searchGroup", data);
	}

}

//监听滚动条
void SearchWidget::onListScroll(QScrollBar* scrollBar, int scrollValue)
{
	if (m_isLoading)
		return;
	if (scrollBar->maximum() - scrollValue < MinScrollBottom)
	{
		m_isLoading = true;
		m_currentPage++;
		sendSearchRequest();
	}
}

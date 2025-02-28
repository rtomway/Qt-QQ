#pragma once
#include "ContactList.h"
#include "ui_ContactList.h"
#include <QFile>
#include <QLabel>
#include <QBoxLayout>
#include <QMutex>
#include <QMutexLocker>
#include <QJsonObject>

#include "ItemWidget.h"
#include "Client.h"
#include "FriendNoticeItemWidget.h"
#include "User.h"


QStringList ContactList::m_fNamelist{};
QStringList ContactList::m_gNamelist{};

ContactList::ContactList(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::ContactList)
	,m_friendList(new QTreeWidget(this))
	,m_groupList(new QTreeWidget(this))
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/ContactList.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else {
		qDebug() <<file.fileName()<< "样式表打开失败";
	}
	ui->fInformWidget->installEventFilter(this);
	ui->gInformWidget->installEventFilter(this);
}

ContactList::~ContactList()
{
	delete ui;
}

ContactList* ContactList::instance()
{
	//保证互斥量唯一
	/*static QMutex mutex;
	QMutexLocker locker(&mutex);*/
	static ContactList *ins=new ContactList;
	return ins;
}

void ContactList::init()
{
	m_friendList->setHeaderHidden(true);
	m_friendList->setColumnWidth(0, m_friendList->width());

	//QTreeWidget里的根和子项相同宽度
	m_friendList->setIndentation(0);
	m_groupList->setIndentation(0);

	m_groupList->setHeaderHidden(true);
	ui->stackedWidget->addWidget(m_friendList);
	ui->stackedWidget->addWidget(m_groupList);
	ui->stackedWidget->setCurrentWidget(m_friendList);

	addFriendListItem(QString("特别关心"));
	addFriendListItem(QString("我的好友"));
	addFriendListItem(QString("家人"));

	addGroupListItem(QString("置顶群聊"));
	addGroupListItem(QString("我创建的群聊"));
	addGroupListItem(QString("我管理的群聊"));
	addGroupListItem(QString("我加入的群聊"));
	
	m_buttonGroup.addButton(ui->friendBtn);
	m_buttonGroup.addButton(ui->groupBtn);
	m_buttonGroup.setExclusive(true);
	m_buttonGroup.button(-2)->setChecked(true);

	ui->friendNoticeCountLab->setFixedSize(18, 18);
	ui->groupNoticeCountLab->setFixedSize(18, 18);
	ui->friendNoticeCountLab->setVisible(false);
	ui->groupNoticeCountLab->setVisible(false);
	ui->friendNoticeCountLab->setAlignment(Qt::AlignCenter);
	ui->groupNoticeCountLab->setAlignment(Qt::AlignCenter);

	//个人信息
	connect(User::instance(), &User::setUserSuccess,this, [=]
		{
			addFriendItem(getFriendTopItem("我的好友"), User::instance()->getUser());
		});
	

	//列表切换
	connect(&m_buttonGroup,&QButtonGroup::idClicked,this,[=](int id)
	{
			if (id == -2)
				ui->stackedWidget->setCurrentWidget(m_friendList);
			else if(id==-3)
				ui->stackedWidget->setCurrentWidget(m_groupList);
	});
	//topitemWidget展开
	connect(m_friendList, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item)
		{
			//点击分组展开
			if (item->parent() == nullptr)
			{
				item->setExpanded(!item->isExpanded());
				//通过itemwidget找到自定义的小部件topItemWidget
				auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendList->itemWidget(item, 0));
				topItemWidget->setAgale();
			}
			else     //点击用户 弹出用户信息
			{
				auto itemWidget= qobject_cast<ItemWidget*>(m_friendList->itemWidget(item, 0));
				auto obj = itemWidget->getUser();
				qDebug() << "clikedobj" << obj["grouping"].toString();
				emit clickedFriend(obj);
			}

		});
	//好友添加通知未读数量更新
	connect(Client::instance(), &Client::addFriend, this, [=]
		{
			m_fNoticeUnreadCount++;
			ui->friendNoticeCountLab->setVisible(true);
			ui->friendNoticeCountLab->setText(QString::number(m_fNoticeUnreadCount));
		});
	//申请添加好友成功 更新好友列表
	connect(Client::instance(), &Client::agreeAddFriend, this, [=](const QJsonObject&obj)
		{
			addFriendItem(getFriendTopItem(QString("我的好友")),obj);
		});
	
	
}
//获取分组
QStringList ContactList::getfGrouping()
{
	return m_fNamelist;
}
QStringList ContactList::getgGrouping()
{
	return m_gNamelist;
}

//添加分组item
TopItemWidget* ContactList::addFriendListItem(QString friendName)
{
	auto friendListItem = new QTreeWidgetItem(m_friendList);
	friendListItem->setData(0, Qt::UserRole, m_friendList->topLevelItemCount());
	m_fNamelist.append(friendName);

	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_friendList);
	topItemWidget->setName(friendName);
	topItemWidget->setItem(friendListItem);
	m_friendList->setItemWidget(friendListItem,0,topItemWidget);
	return topItemWidget;
}
//添加子item
void ContactList::addFriendItem(QTreeWidgetItem* firendList, const QJsonObject& obj)
{
	auto friendItem = new QTreeWidgetItem(firendList);
	friendItem->setSizeHint(0,QSize(m_friendList->width(),60));
	qDebug() << "username:" << obj["username"].toString();
	//自定义Item
	ItemWidget* itemWidget =new ItemWidget(this);
	itemWidget->setUser(obj);
	m_friendList->setItemWidget(friendItem, 0, itemWidget);

	//通过itemwidget找到自定义的小部件
	auto topItemWidget= qobject_cast<TopItemWidget*>(m_friendList->itemWidget(firendList, 0));
	topItemWidget->setCount(firendList->childCount());

	itemWidget->setGrouping(topItemWidget->getName());
	qDebug() << topItemWidget->getName();
	
}
//获取分组item
QTreeWidgetItem* ContactList::getFriendTopItem(QString friendName)
{
	int id = 0;
	for (int i = 0; i < m_fNamelist.count(); i++)
	{
		if (friendName == m_fNamelist.at(i))
		{
			id = i + 1;
		}
	}
	QTreeWidgetItem* root = m_friendList->invisibleRootItem();
	for (int i = 0; i < root->childCount(); ++i) {
		QTreeWidgetItem* item = root->child(i);
		if (item->data(0, Qt::UserRole).toInt() == id) {
			return item;
		}
	}
}

TopItemWidget* ContactList::addGroupListItem(QString groupName)
{
	auto groupListItem = new QTreeWidgetItem(m_groupList);
	groupListItem->setData(0, Qt::UserRole, m_groupList->topLevelItemCount());
	m_gNamelist.append(groupName);

	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_groupList);
	topItemWidget->setName(groupName);
	topItemWidget->setItem(groupListItem);
	m_groupList->setItemWidget(groupListItem, 0, topItemWidget);
	return topItemWidget;
}

void ContactList::addGroupItem(QTreeWidgetItem* groupListItem, QString groupName)
{
	//auto groupItem = new QTreeWidgetItem(groupListItem, groupName);
}

QTreeWidgetItem* ContactList::getGroupTopItem(QString groupName)
{
	return nullptr;
}
//新增好友
void ContactList::newlyFriendItem(const QJsonObject& obj)
{
	auto grouping= getFriendTopItem(obj["grouping"].toString());
	addFriendItem(grouping, obj);
	//消息项添加
	emit agreeAddFriend(obj);
}
//事件重写
bool ContactList::eventFilter(QObject* obj, QEvent* event)
{
	// 监听子窗口的鼠标点击事件
	if (obj == ui->fInformWidget && event->type() == QEvent::MouseButtonPress) {
		qDebug() << "子窗口被点击!";
		//点击好友申请通知 清空未读
		emit friendNotice();
		m_fNoticeUnreadCount = 0;
		ui->friendNoticeCountLab->clear();
		ui->friendNoticeCountLab->setVisible(false);
		return true; // 事件已处理，不再继续传递
	}
	 if (obj == ui->gInformWidget && event->type() == QEvent::MouseButtonPress)
	{
		qDebug() << "子窗口被点击!";
		//点击群聊申请通知 清空未读
		emit groupNotice();
		m_gNoticeUnreadCount = 0;
		ui->groupNoticeCountLab->clear();
		ui->groupNoticeCountLab->setVisible(false);
		return true; // 事件已处理，不再继续传递
	}
	return false; // 传递给基类处理其他事件
}


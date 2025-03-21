#include "ContactListWidget.h"
#include "ui_ContactListWidget.h"
#include <QFile>
#include <QLabel>
#include <QBoxLayout>
#include <QMutex>
#include <QMutexLocker>
#include <QJsonObject>
#include <QSharedPointer>

#include "ItemWidget.h"
#include "FriendNoticeItemWidget.h"
#include "FriendManager.h"
#include "Friend.h"
#include "EventBus.h"

QStringList ContactListWidget::m_fNamelist{};
QStringList ContactListWidget::m_gNamelist{};

ContactListWidget::ContactListWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::ContactListWidget)
	, m_friendList(new QTreeWidget(this))
	, m_groupList(new QTreeWidget(this))
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/ContactList.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else {
		qDebug() << file.fileName() << "样式表打开失败";
	}
	ui->fInformWidget->installEventFilter(this);
	ui->gInformWidget->installEventFilter(this);
}

ContactListWidget::~ContactListWidget()
{
	delete ui;
}

void ContactListWidget::init()
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

	//好友分组添加
	connect(ui->friendmanage, &QPushButton::clicked, this, [=]
		{
			m_createFriendgrouping = std::make_unique<CreateFriendgrouping>();
			m_createFriendgrouping->show();
			connect(m_createFriendgrouping.get(), &CreateFriendgrouping::createGrouping, this, [=](const QString& grouping)
				{
					addFriendListItem(grouping);
				});
		});
	//登录时加载信息
	connect(FriendManager::instance(), &FriendManager::FriendManagerLoadSuccess, this, [=]()
		{
			auto friends = FriendManager::instance()->getFriends();
			for (auto it = friends.begin(); it != friends.end(); ++it)
			{
				const QSharedPointer<Friend>& myfriend = it.value();
				auto grouping = myfriend->getGrouping();
				//判断该分组是否已存在
				if (!m_fNamelist.contains(grouping))
					addFriendListItem(grouping);
				//添加
				addFriendItem(getFriendTopItem(grouping), myfriend->getFriendId());
			}
		});
	//列表切换
	connect(&m_buttonGroup, &QButtonGroup::idClicked, this, [=](int id)
		{
			if (id == -2)
				ui->stackedWidget->setCurrentWidget(m_friendList);
			else if (id == -3)
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
				auto itemWidget = qobject_cast<ItemWidget*>(m_friendList->itemWidget(item, 0));
				/*auto obj = itemWidget->getUser();
				qDebug() << "clikedobj" << obj["grouping"].toString();*/
				auto user_id = itemWidget->getUserId();
				emit clickedFriend(user_id);
			}

		});
	//新增好友
	connect(FriendManager::instance(), &FriendManager::NewFriend, this, [=](const QString& user_id, const QString& grouping)
		{
			addFriendItem(getFriendTopItem(grouping), user_id);
		});
	//信息更新
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			qDebug() << "用户信息更新99999999999999999999999999";
			auto user = FriendManager::instance()->findFriend(user_id);
			qDebug() << user->getFriend();
			auto groupingItem = getFriendTopItem(user->getGrouping());
			if(!groupingItem)
			qDebug() << "groupingItem";
			auto item = findItemByIdInGroup(groupingItem, user_id);
			if (item)
			{
				auto itemWidget = qobject_cast<ItemWidget*>(m_friendList->itemWidget(item, 0));
				itemWidget->setUser(user_id);
			}
		});
	//好友分组更改
	connect(FriendManager::instance(), &FriendManager::updateFriendGrouping, this, [=](const QString& user_id, const QString& oldGrouping)
		{
			//从旧分组中移除 
			auto oldTopItem = getFriendTopItem(oldGrouping);
			if (oldTopItem)
			{
				auto item = findItemByIdInGroup(oldTopItem, user_id);
				oldTopItem->removeChild(item);
				auto oldTopItemWidget = qobject_cast<TopItemWidget*>(m_friendList->itemWidget(oldTopItem, 0));
				if (!oldTopItemWidget)
				{
					qDebug() << "oldTopItemWidget为空";
					return;
				}
				oldTopItemWidget->setCount(oldTopItem->childCount());
			}
			//添加到新分组
			auto user = FriendManager::instance()->findFriend(user_id);
			auto grouping = user->getGrouping();
			addFriendItem(getFriendTopItem(grouping), user_id);
		});
	//删除好友
	connect(EventBus::instance(), &EventBus::deleteFriend, this, [=](const QString& user_id)
		{
			auto user = FriendManager::instance()->findFriend(user_id);
			auto grouping = user->getGrouping();
			auto topItem = getFriendTopItem(grouping);
			auto item = findItemByIdInGroup(topItem, user_id);
			topItem->removeChild(item);
			auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendList->itemWidget(topItem, 0));
			topItemWidget->setCount(topItem->childCount());
		});
}
//获取分组
QStringList ContactListWidget::getfGrouping()
{
	return m_fNamelist;
}
QStringList ContactListWidget::getgGrouping()
{
	return m_gNamelist;
}
//添加分组item
TopItemWidget* ContactListWidget::addFriendListItem(QString friendName)
{
	auto friendListItem = new QTreeWidgetItem(m_friendList);
	friendListItem->setData(0, Qt::UserRole, m_friendList->topLevelItemCount());
	m_fNamelist.append(friendName);
	emit updateFriendgrouping();
	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_friendList);
	topItemWidget->setName(friendName);
	m_friendList->setItemWidget(friendListItem, 0, topItemWidget);
	return topItemWidget;
}
//添加子item
void ContactListWidget::addFriendItem(QTreeWidgetItem* firendList, const QString& user_id)
{
	auto friendItem = findItemByIdInGroup(firendList, user_id);
	if (friendItem != nullptr)
	{
		qDebug() << "该好友已存在";
		return;
	}
	friendItem = new QTreeWidgetItem(firendList);
	//把自己置顶
	if (user_id == FriendManager::instance()->getOneselfID())
	{
		firendList->removeChild(friendItem);
		firendList->insertChild(0, friendItem);
	}
	friendItem->setData(0, Qt::UserRole, user_id);
	friendItem->setSizeHint(0, QSize(m_friendList->width(), 60));
	//自定义Item
	ItemWidget* itemWidget = new ItemWidget(this);
	itemWidget->setUser(user_id);
	m_friendList->setItemWidget(friendItem, 0, itemWidget);
	//通过itemwidget找到自定义的小部件
	auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendList->itemWidget(firendList, 0));
	topItemWidget->setCount(firendList->childCount());

	itemWidget->setGrouping(topItemWidget->getName());
}
//获取分组item
QTreeWidgetItem* ContactListWidget::getFriendTopItem(QString friendName)
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
//获取子item
QTreeWidgetItem* ContactListWidget::findItemByIdInGroup(QTreeWidgetItem* group, const QString& userId) {
	if (!group) return nullptr;  // 避免空指针

	for (int i = 0; i < group->childCount(); ++i) {
		QTreeWidgetItem* friendItem = group->child(i);
		if (friendItem->data(0, Qt::UserRole).toString() == userId) {
			return friendItem;  // 找到匹配 ID，返回 item
		}
	}
	return nullptr;  // 没找到
}
//添加分组item
TopItemWidget* ContactListWidget::addGroupListItem(QString groupName)
{
	auto groupListItem = new QTreeWidgetItem(m_groupList);
	groupListItem->setData(0, Qt::UserRole, m_groupList->topLevelItemCount());
	m_gNamelist.append(groupName);

	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_groupList);
	topItemWidget->setName(groupName);
	//topItemWidget->setItem(groupListItem);
	m_groupList->setItemWidget(groupListItem, 0, topItemWidget);
	return topItemWidget;
}
//添加子item
void ContactListWidget::addGroupItem(QTreeWidgetItem* groupListItem, QString groupName)
{
}
//获取分组item
QTreeWidgetItem* ContactListWidget::getGroupTopItem(QString groupName)
{
	return nullptr;
}
//通知数量更新
void ContactListWidget::updateFriendNoticeCount()
{
	m_fNoticeUnreadCount++;
	ui->friendNoticeCountLab->setVisible(true);
	ui->friendNoticeCountLab->setText(QString::number(m_fNoticeUnreadCount));
}
//退出清除前一个账户信息
void ContactListWidget::clearContactList()
{
	m_friendList->clear();
	m_groupList->clear();
	m_fNamelist.clear();
	m_gNamelist.clear();
}
//事件重写
bool ContactListWidget::eventFilter(QObject* obj, QEvent* event)
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


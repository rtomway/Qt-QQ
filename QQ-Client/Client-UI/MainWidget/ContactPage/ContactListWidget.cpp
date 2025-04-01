#include "ContactListWidget.h"
#include "ui_ContactListWidget.h"
#include <QFile>
#include <QLabel>
#include <QBoxLayout>
#include <QMutex>
#include <QMutexLocker>
#include <QJsonObject>
#include <QSharedPointer>

#include "FriendManager.h"
#include "Friend.h"
#include "EventBus.h"
#include "AvatarManager.h"
#include "ImageUtil.h"
#include "Group.h"
#include "GroupManager.h"
#include "ItemWidget.h"
#include "FriendListItemWidget.h"
#include "GroupListItemWidget.h"

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

	addGroupTopItem(QString("置顶群聊"));
	addGroupTopItem(QString("我创建的群聊"));
	addGroupTopItem(QString("我管理的群聊"));
	addGroupTopItem(QString("我加入的群聊"));

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
					addFriendTopItem(grouping);
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
					addFriendTopItem(grouping);
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
				auto user_id = item->data(0, Qt::UserRole).toString();
				emit clickedFriend(user_id);
			}
		});
	connect(m_groupList, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item)
		{
			//点击分组展开
			if (item->parent() == nullptr)
			{
				item->setExpanded(!item->isExpanded());
				//通过itemwidget找到自定义的小部件topItemWidget
				auto topItemWidget = qobject_cast<TopItemWidget*>(m_groupList->itemWidget(item, 0));
				topItemWidget->setAgale();
			}
			else     //点击用户 弹出用户信息
			{

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
			auto user = FriendManager::instance()->findFriend(user_id);
			qDebug() << user->getFriend();
			auto groupingItem = getFriendTopItem(user->getGrouping());
			auto item = findItemByIdInGroup(groupingItem, user_id);
			if (item)
			{
				auto itemWidget = qobject_cast<ItemWidget*>(m_friendList->itemWidget(item, 0));
				itemWidget->setItemWidget(user_id);
			}
		});
	//头像更新
	connect(AvatarManager::instance(), &AvatarManager::UpdateUserAvatar, this, [=](const QString& user_id)
		{
			auto user = FriendManager::instance()->findFriend(user_id);
			qDebug() << user->getFriend();
			auto groupingItem = getFriendTopItem(user->getGrouping());
			auto item = findItemByIdInGroup(groupingItem, user_id);
			if (item)
			{
				auto itemWidget = qobject_cast<ItemWidget*>(m_friendList->itemWidget(item, 0));
				//itemWidget->updatePixmap();
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
	//新增群组
	connect(GroupManager::instance(), &GroupManager::newGroup, this, [=](const QString& group_id)
		{
			addGroupItem(getGroupTopItem("我创建的群聊"), group_id);
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
TopItemWidget* ContactListWidget::addFriendTopItem(QString friendName)
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
//添加好友item
void ContactListWidget::addFriendItem(QTreeWidgetItem* firendTopItem, const QString& user_id)
{
	auto friendItem = findItemByIdInGroup(firendTopItem, user_id);
	if (friendItem != nullptr)
	{
		qDebug() << "该好友已存在";
		return;
	}
	friendItem = new QTreeWidgetItem(firendTopItem);
	//把自己置顶
	if (user_id == FriendManager::instance()->getOneselfID())
	{
		firendTopItem->removeChild(friendItem);
		firendTopItem->insertChild(0, friendItem);
	}
	friendItem->setData(0, Qt::UserRole, user_id);
	friendItem->setSizeHint(0, QSize(m_friendList->width(), 60));
	//自定义Item
	ItemWidget* itemWidget = new FriendListItemWidget(this);
	itemWidget->setItemWidget(user_id);
	m_friendList->setItemWidget(friendItem, 0, itemWidget);
	//通过firendTopItem找到自定义的小部件
	auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendList->itemWidget(firendTopItem, 0));
	topItemWidget->setCount(firendTopItem->childCount());

	auto friendItemWidget = qobject_cast<FriendListItemWidget*>(itemWidget);
	friendItemWidget->setGrouping(topItemWidget->getName());
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
	if (!group)
		return nullptr;  // 避免空指针
	for (int i = 0; i < group->childCount(); ++i) {
		QTreeWidgetItem* item = group->child(i);
		if (item->data(0, Qt::UserRole).toString() == userId) {
			return item;  // 找到匹配 ID，返回 item
		}
	}
	return nullptr;  // 没找到
}
//添加分组item
TopItemWidget* ContactListWidget::addGroupTopItem(QString groupName)
{
	auto groupListItem = new QTreeWidgetItem(m_groupList);
	groupListItem->setData(0, Qt::UserRole, m_groupList->topLevelItemCount());
	m_gNamelist.append(groupName);

	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_groupList);
	topItemWidget->setName(groupName);
	m_groupList->setItemWidget(groupListItem, 0, topItemWidget);
	return topItemWidget;
}
//添加群组item
void ContactListWidget::addGroupItem(QTreeWidgetItem* groupTopItem, const QString& group_id)
{
	auto groupItem = findItemByIdInGroup(groupTopItem, group_id);
	if (groupItem != nullptr)
	{
		qDebug() << "该群组已存在";
		return;
	}
	groupItem = new QTreeWidgetItem(groupTopItem);
	groupItem->setData(0, Qt::UserRole, group_id);
	groupItem->setSizeHint(0, QSize(m_groupList->width(), 60));
	//自定义Item
	ItemWidget* itemWidget = new GroupListItemWidget(this);
	itemWidget->setItemWidget(group_id);
	m_groupList->setItemWidget(groupItem, 0, itemWidget);
	//通过groupTopItem找到自定义的小部件
	auto topItemWidget = qobject_cast<TopItemWidget*>(m_groupList->itemWidget(groupTopItem, 0));
	topItemWidget->setCount(groupTopItem->childCount());

	auto groupItemWidget = qobject_cast<GroupListItemWidget*>(itemWidget);
	groupItemWidget->setGrouping(topItemWidget->getName());

}
//获取分组item
QTreeWidgetItem* ContactListWidget::getGroupTopItem(QString groupName)
{
	int id = 0;
	for (int i = 0; i < m_gNamelist.count(); i++)
	{
		if (groupName == m_gNamelist.at(i))
		{
			id = i + 1;
		}
	}
	QTreeWidgetItem* root = m_groupList->invisibleRootItem();
	for (int i = 0; i < root->childCount(); ++i) {
		QTreeWidgetItem* item = root->child(i);
		if (item->data(0, Qt::UserRole).toInt() == id) {
			return item;
		}
	}
}
//通知数量更新
void ContactListWidget::updateFriendNoticeCount()
{
	m_fNoticeUnreadCount++;
	ui->friendNoticeCountLab->setVisible(true);
	ui->friendNoticeCountLab->setText(QString::number(m_fNoticeUnreadCount));
}
void ContactListWidget::updateGroupNoticeCount()
{
	m_gNoticeUnreadCount++;
	ui->groupNoticeCountLab->setVisible(true);
	ui->groupNoticeCountLab->setText(QString::number(m_gNoticeUnreadCount));
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


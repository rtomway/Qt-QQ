﻿#include "ContactListWidget.h"
#include "ContactListWidget.h"
#include "ui_ContactListWidget.h"
#include <QFile>
#include <QLabel>
#include <QBoxLayout>
#include <QJsonObject>
#include <QSharedPointer>

#include "LoginUserManager.h"
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
#include "SMaskWidget.h"

QStringList ContactListWidget::m_fNamelist{};
QStringList ContactListWidget::m_gNamelist{};

ContactListWidget::ContactListWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::ContactListWidget)
	, m_friendList(new QTreeWidget(this))
	, m_groupList(new QTreeWidget(this))
	, m_createFriendgrouping(new CreateFriendgrouping)
{
	ui->setupUi(this);
	init();
	externalSignals();
	QFile file(":/stylesheet/Resource/StyleSheet/ContactListWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else {
		qDebug() << file.fileName() << "样式表打开失败";
	}
}

ContactListWidget::~ContactListWidget()
{
	delete ui;
}

void ContactListWidget::init()
{
	ui->fInformWidget->installEventFilter(this);
	ui->gInformWidget->installEventFilter(this);
	//隐藏tree根
	m_friendList->setHeaderHidden(true);
	m_groupList->setHeaderHidden(true);
	//QTreeWidget里的根和子项相同宽度
	m_friendList->setIndentation(0);
	m_groupList->setIndentation(0);
	// 禁用所有双击展开
	m_friendList->setExpandsOnDoubleClick(false);
	m_groupList->setExpandsOnDoubleClick(false);
	//treeList
	ui->stackedWidget->addWidget(m_friendList);
	ui->stackedWidget->addWidget(m_groupList);
	ui->stackedWidget->setCurrentWidget(m_friendList);
	//buttonGroup
	m_buttonGroup.addButton(ui->friendBtn);
	m_buttonGroup.addButton(ui->groupBtn);
	m_buttonGroup.setExclusive(true);
	m_buttonGroup.button(-2)->setChecked(true);
	//noticeCount
	ui->friendNoticeCountLab->setFixedSize(18, 18);
	ui->groupNoticeCountLab->setFixedSize(18, 18);
	ui->friendNoticeCountLab->setVisible(false);
	ui->groupNoticeCountLab->setVisible(false);
	ui->friendNoticeCountLab->setAlignment(Qt::AlignCenter);
	ui->groupNoticeCountLab->setAlignment(Qt::AlignCenter);
	//列表切换
	connect(&m_buttonGroup, &QButtonGroup::idClicked, this, [=](int id)
		{
			if (id == -2)  //好友列表
			{
				ui->stackedWidget->setCurrentWidget(m_friendList);
			}
			else if (id == -3)  //群组列表
			{
				ui->stackedWidget->setCurrentWidget(m_groupList);
			}
		});
	//topitemWidget展开
	connect(m_friendList, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item)
		{
			//点击分组展开
			if (item->parent() == nullptr)
			{
				item->setExpanded(!item->isExpanded());
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
				auto topItemWidget = qobject_cast<TopItemWidget*>(m_groupList->itemWidget(item, 0));
				topItemWidget->setAgale();
			}
			else     //点击用户 弹出用户信息
			{
				auto group_id = item->data(0, Qt::UserRole).toString();
				emit clickedGroup(group_id);
			}
		});
	//好友分组添加
	connect(ui->createFriendGrouping, &QPushButton::clicked, this, [=]
		{
			//蒙层
			SMaskWidget::instance()->popUp(m_createFriendgrouping);
			auto mainWidgetSize = SMaskWidget::instance()->getMainWidgetSize();
			int x = (mainWidgetSize.width() - m_createFriendgrouping->width()) / 2;
			int y = (mainWidgetSize.height() - m_createFriendgrouping->height()) / 2;
			SMaskWidget::instance()->setPopGeometry(QRect(x, y, m_createFriendgrouping->width(), m_createFriendgrouping->height()));
		});
	//创建分组
	connect(m_createFriendgrouping, &CreateFriendgrouping::createGrouping, this, &ContactListWidget::addFriendTopItem);
}

//外部信号
void ContactListWidget::externalSignals()
{
	//加载登录用户信息
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, this, [=]
		{
			//默认分组
			addFriendTopItem("我的好友");
			addGroupTopItem(QString("我创建的群聊"));
			addGroupTopItem(QString("我管理的群聊"));
			addGroupTopItem(QString("我加入的群聊"));

			auto& loginUser = LoginUserManager::instance()->getLoginUser();
			auto grouping = loginUser->getGrouping();
			//判断该分组是否已存在
			if (!m_fNamelist.contains(grouping))
				addFriendTopItem(grouping);
			//添加
			addFriendItem(getFriendTopItem(grouping), loginUser->getFriendId());

		});
	//本地已有的头像可直接加载
	connect(FriendManager::instance(), &FriendManager::loadLocalAvatarFriend, this, [=](const QStringList& friend_idList)
		{
			for (auto& friend_id : friend_idList)
			{
				auto myfriend = FriendManager::instance()->findFriend(friend_id);
				auto grouping = myfriend->getGrouping();
				//判断该分组是否已存在
				if (!m_fNamelist.contains(grouping))
					addFriendTopItem(grouping);
				//添加
				addFriendItem(getFriendTopItem(grouping), myfriend->getFriendId());
			}
		});
	connect(GroupManager::instance(), &GroupManager::loadLocalAvatarGroup, this, [=](const QStringList& group_idList)
		{
			for (auto& group_id : group_idList)
			{
				auto group = GroupManager::instance()->findGroup(group_id);
				if (group->getGroupOwerId() != LoginUserManager::instance()->getLoginUserID())
				{
					addGroupItem(getGroupTopItem("我加入的群聊"), group_id);
				}
				else
				{
					addGroupItem(getGroupTopItem("我创建的群聊"), group_id);
				}
			}
		});
	//需要向服务器申请头像的
	connect(AvatarManager::instance(), &AvatarManager::loadGroupAvatarSuccess, this, [=](const QString& group_id)
		{
			auto group = GroupManager::instance()->findGroup(group_id);
			if (group->getGroupOwerId() != LoginUserManager::instance()->getLoginUserID())
			{
				addGroupItem(getGroupTopItem("我加入的群聊"), group_id);
			}
			else
			{
				addGroupItem(getGroupTopItem("我创建的群聊"), group_id);
			}
		});
	connect(AvatarManager::instance(), &AvatarManager::loadFriendAvatarSuccess, this, [=](const QString& user_id)
		{
			auto myfriend = FriendManager::instance()->findFriend(user_id);
			auto grouping = myfriend->getGrouping();
			//判断该分组是否已存在
			if (!m_fNamelist.contains(grouping))
				addFriendTopItem(grouping);
			//添加
			addFriendItem(getFriendTopItem(grouping), myfriend->getFriendId());
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
			auto groupingItem = getFriendTopItem(user->getGrouping());
			auto item = findItemByIdInGroup(groupingItem, user_id);
			if (item)
			{
				auto itemWidget = qobject_cast<ItemWidget*>(m_friendList->itemWidget(item, 0));
				itemWidget->setItemWidget(user_id);
			}
		});
	connect(AvatarManager::instance(), &AvatarManager::UpdateGroupAvatar, this, [=](const QString& group_id)
		{
			auto item = findItemByIdInAll(m_groupList, group_id);
			if (item)
			{
				auto itemWidget = qobject_cast<ItemWidget*>(m_groupList->itemWidget(item, 0));
				itemWidget->setItemWidget(group_id);
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
	connect(FriendManager::instance(), &FriendManager::deleteFriend, this, [=](const QString& user_id)
		{
			auto user = FriendManager::instance()->findFriend(user_id);
			auto grouping = user->getGrouping();
			auto topItem = getFriendTopItem(grouping);
			auto item = findItemByIdInGroup(topItem, user_id);
			topItem->removeChild(item);
			auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendList->itemWidget(topItem, 0));
			topItemWidget->setCount(topItem->childCount());
			FriendManager::instance()->removeFriend(user_id);
		});
	//退出分组
	connect(GroupManager::instance(), &GroupManager::exitGroup, this, [=](const QString& group_id, const QString& user_id)
		{
			auto list = GroupManager::instance()->getGroups().keys();
			auto exitGroup = GroupManager::instance()->findGroup(group_id);
			auto& grouping = exitGroup->getGrouping();
			auto topItem = getGroupTopItem(grouping);
			auto item = findItemByIdInGroup(topItem, group_id);
			topItem->removeChild(item);
			auto topItemWidget = qobject_cast<TopItemWidget*>(m_groupList->itemWidget(topItem, 0));
			topItemWidget->setCount(topItem->childCount());
			GroupManager::instance()->removeGroup(group_id);
		});
	//新增群组
	connect(GroupManager::instance(), &GroupManager::createGroupSuccess, this, [=](const QString& group_id)
		{
			addGroupItem(getGroupTopItem("我创建的群聊"), group_id);
		});
	connect(GroupManager::instance(), &GroupManager::newGroup, this, [=](const QString& group_id)
		{
			addGroupItem(getGroupTopItem("我加入的群聊"), group_id);
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
	if (user_id == LoginUserManager::instance()->getLoginUserID())
	{
		firendTopItem->removeChild(friendItem);
		firendTopItem->insertChild(0, friendItem);
	}
	friendItem->setData(0, Qt::UserRole, user_id);
	friendItem->setSizeHint(0, QSize(m_friendList->width(), 60));
	//自定义ItemWidget
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
	return nullptr;
}

//在单个分组中获取子item
QTreeWidgetItem* ContactListWidget::findItemByIdInGroup(QTreeWidgetItem* group, const QString& userId) {
	if (!group)
		return nullptr;  // 避免空指针
	for (int i = 0; i < group->childCount(); ++i) 
	{
		QTreeWidgetItem* item = group->child(i);
		if (item->data(0, Qt::UserRole).toString() == userId) 
		{
			return item;  // 找到匹配 ID，返回 item
		}
	}
	return nullptr;  // 没找到
}
//在整个列表中搜索
QTreeWidgetItem* ContactListWidget::findItemByIdInAll(QTreeWidget* treeWidget, const QString& id)
{
	for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
	{
		for (int j = 0; j < treeWidget->topLevelItem(i)->childCount(); ++j)
		{
			QTreeWidgetItem* item = treeWidget->topLevelItem(i)->child(j);
			if (!item)
			{
				continue;
			}
			if (item->data(0, Qt::UserRole).toString() == id) 
			{
				return item;  // 找到匹配 ID，返回 item
			}
		}
	}

	return nullptr;  // 没有找到，返回nullptr
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
	return nullptr;
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
	ui->stackedWidget->setCurrentWidget(m_friendList);
	m_buttonGroup.button(-2)->setChecked(true);
	m_friendList->clear();
	m_groupList->clear();
	m_fNamelist.clear();
	m_gNamelist.clear();
}

//事件重写
bool ContactListWidget::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == ui->fInformWidget && event->type() == QEvent::MouseButtonPress) 
	{
		//点击好友申请通知 清空未读
		emit friendNotice();
		m_fNoticeUnreadCount = 0;
		ui->friendNoticeCountLab->clear();
		ui->friendNoticeCountLab->setVisible(false);
		return true; 
	}

	if (obj == ui->gInformWidget && event->type() == QEvent::MouseButtonPress)
	{
		//点击群聊申请通知 清空未读
		emit groupNotice();
		m_gNoticeUnreadCount = 0;
		ui->groupNoticeCountLab->clear();
		ui->groupNoticeCountLab->setVisible(false);
		return true; 
	}
	return false;
}


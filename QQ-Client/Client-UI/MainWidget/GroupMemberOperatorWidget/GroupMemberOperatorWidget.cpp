#include "GroupMemberOperatorWidget.h"
#include "ui_GroupMemberOperatorWidget.h"
#include <QFile>
#include <QStack>
#include <QMessageBox>
#include <QJSonArray>

#include "TopItemWidget.h"
#include "ItemWidget.h"
#include "SelectedItemWidget.h"
#include "FriendManager.h"
#include "EventBus.h"
#include "MessageSender.h"
#include "AvatarManager.h"
#include "GroupManager.h"
#include "Group.h"
#include "LoginUserManager.h"
#include "PacketCreate.h"
#include "FriendListItemWidget.h"

GroupMemberOperatorWidget::GroupMemberOperatorWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::GroupMemberOperatorWidget)
	, m_selectTree(new QTreeWidget(this))
	, m_searchList(new QListWidget(this))
{
	ui->setupUi(this);
	init();
	this->setFixedSize(530, 580);
	m_searchList->setObjectName("searchListWidget");
	ui->selectedFriendList->setObjectName("selectedFriendList");
	QFile file(":/stylesheet/Resource/StyleSheet/GroupMemberOperatorWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else {
		qDebug() << file.fileName() << "样式表打开失败";
	}
}

GroupMemberOperatorWidget::~GroupMemberOperatorWidget()
{

}

void GroupMemberOperatorWidget::init()
{
	ui->friendStackedList->addWidget(m_selectTree);
	ui->friendStackedList->addWidget(m_searchList);
	ui->friendStackedList->setCurrentWidget(m_selectTree);
	//好友搜索框
	ui->searchEdit->setPlaceholderText("好友搜索");
	//好友搜索
	connect(ui->searchEdit, &QLineEdit::textChanged, this, [=](const QString& text)
		{
			clearSearchList();
			if (text.isEmpty())
			{
				ui->friendStackedList->setCurrentWidget(m_selectTree);
				return;
			}
			ui->friendStackedList->setCurrentWidget(m_searchList);
			auto searchResultHash = FriendManager::instance()->findFriends(text);
			if (searchResultHash.isEmpty()) {
				qWarning() << "No friends found for text: " << text;
			}
			for (auto it = searchResultHash.begin(); it != searchResultHash.end(); ++it) {
				if (!it.value()) {
					qWarning() << "Invalid QSharedPointer!";
					continue;  // 跳过当前无效项
				}
				auto& obj = it.value()->getFriend();
				AvatarManager::instance()->getAvatar(it.value()->getFriendId(), ChatType::User, [=](const QPixmap& pixmap)
					{
						addSelectedItem(m_searchList, it.value()->getFriendId(), it.value()->getFriendName());
					});
			}
		});
	m_selectTree->setHeaderHidden(true);
	m_selectTree->setColumnWidth(0, m_selectTree->width());
	//QTreeWidget里的根和子项相同宽度
	m_selectTree->setIndentation(0);

	//好友列表展开
	connect(m_selectTree, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item)
		{
			//点击分组展开
			if (item->parent() == nullptr)
			{
				item->setExpanded(!item->isExpanded());
				//通过itemwidget找到自定义的小部件topItemWidget
				auto topItemWidget = qobject_cast<TopItemWidget*>(m_selectTree->itemWidget(item, 0));
				topItemWidget->setAgale();
			}
			else     //点击用户
			{
				////选择好友
				auto itemWidget = qobject_cast<SelectedItemWidget*>(m_selectTree->itemWidget(item, 0));
				auto user_id = itemWidget->getUserId();
				//群主或已加入的成员不可操作
				if (shouldFilterUser(user_id))
					return;
				itemWidget->setChecked(!itemWidget->isChecked());
			}
		});
	//搜索列表
	connect(m_searchList, &QListWidget::itemClicked, [=](QListWidgetItem* item)
		{
			auto itemWidget = qobject_cast<SelectedItemWidget*>(m_searchList->itemWidget(item));
			auto user_id = itemWidget->getUserId();
			//群主或已加入的成员不可操作
			if (shouldFilterUser(user_id))
				return;

			itemWidget->setChecked(!itemWidget->isChecked());
		});
}

//选中好友添加至选中列表
void GroupMemberOperatorWidget::addSelectedItem(QListWidget* listWidget, const QString& user_id, const QString& userName)
{
	auto item = findListItem(listWidget, user_id);
	if (item)
	{
		return;
	}
	item = new QListWidgetItem(listWidget);
	item->setData(Qt::UserRole, user_id);
	auto itemWidget = new SelectedItemWidget(user_id, userName, listWidget);
	item->setSizeHint(QSize(listWidget->width(), 70));

	auto treeItem = findTreeItem(user_id);
	auto treeItemWidget = dynamic_cast<SelectedItemWidget*>(m_selectTree->itemWidget(treeItem, 0));
	auto checked = treeItemWidget->isChecked();
	itemWidget->setChecked(checked);

	if (listWidget == m_searchList && shouldFilterUser(user_id))
	{
		itemWidget->setEnabled(false);
	}

	listWidget->setItemWidget(item, itemWidget);
	connect(itemWidget, &SelectedItemWidget::checked, this, [=](bool isChecked)
		{
			if (isChecked)
			{
				//搜索列表确认（tree确认并添加）
				if (listWidget == m_searchList)
				{
					treeItemWidget->setChecked(true);
					addSelectedItem(ui->selectedFriendList, user_id, userName);
				}
			}
			else
			{
				removeSelectedItem(user_id);
			}
		});
}

//删除选中好友item
void GroupMemberOperatorWidget::removeSelectedItem(const QString& user_id)
{
	m_selectedList.removeOne(user_id);
	auto listItem = findListItem(ui->selectedFriendList, user_id);
	if (listItem)
	{
		auto row = ui->selectedFriendList->row(listItem);
		delete ui->selectedFriendList->takeItem(row);;
	}
	auto treeItem = findTreeItem(user_id);
	auto treeItemWidget = dynamic_cast<SelectedItemWidget*>(m_selectTree->itemWidget(treeItem, 0));
	treeItemWidget->setChecked(false);
	auto searchItem = findListItem(m_searchList, user_id);
	if (searchItem)
	{
		auto searchItemWidget = dynamic_cast<SelectedItemWidget*>(m_searchList->itemWidget(searchItem));
		searchItemWidget->setChecked(false);
	}

}

//通过用户id找到用户消息项item
QListWidgetItem* GroupMemberOperatorWidget::findListItem(QListWidget* listWidget, const QString& user_id)
{
	for (auto i = 0; i < listWidget->count(); i++)
	{
		auto item = listWidget->item(i);
		if (user_id == item->data(Qt::UserRole).toString())
		{
			return item;
			break;
		}
	}
	return nullptr;
}

//通过用户id找到用户消息项item
QTreeWidgetItem* GroupMemberOperatorWidget::findTreeItem(const QString& user_id)
{
	// 遍历顶层节点
	for (int i = 0; i < m_selectTree->topLevelItemCount(); ++i) {
		QTreeWidgetItem* item = m_selectTree->topLevelItem(i);

		// 递归查找
		QStack<QTreeWidgetItem*> stack;
		stack.push(item);

		while (!stack.isEmpty()) {
			QTreeWidgetItem* current = stack.pop();

			// 检查当前节点
			if (current->data(0, Qt::UserRole).toString() == user_id) {
				return current;
			}

			// 添加子节点到栈中
			for (int j = 0; j < current->childCount(); ++j) {
				stack.push(current->child(j));
			}
		}
	}

	return nullptr; // 没找到
}

//加载好友列表
void GroupMemberOperatorWidget::loadFriendsList()
{
	clearFriendTree();
	//获取好友数据
	auto friendsList = FriendManager::instance()->getAllFriends();
	m_userIdList = FriendManager::instance()->getAllFriendIdList();
	//获取好友分组
	auto friendGroupingsList = FriendManager::instance()->getAllFriendGroupings();
	//创建分组项并缓存分组关系
	QHash<QString, QTreeWidgetItem*>groupingNameToItemHash;
	for (const auto& friendGrouping : friendGroupingsList)
	{
		QTreeWidgetItem* topItem = new QTreeWidgetItem(m_selectTree);
		TopItemWidget* topItemWidget = new TopItemWidget(m_selectTree);
		topItemWidget->setName(friendGrouping);
		m_selectTree->setItemWidget(topItem, 0, topItemWidget);
		groupingNameToItemHash[friendGrouping] = topItem;
	}
	//添加好友item
	for (const auto& friendData : friendsList)
	{
		auto& friend_id = friendData->getFriendId();
		auto& friend_name = friendData->getFriendName();
		auto grouping = friendData->getGrouping();
		auto topItem = groupingNameToItemHash[grouping];
		auto friendItem = new QTreeWidgetItem(topItem);
		friendItem->setSizeHint(0, QSize(m_selectTree->width(), 60));
		SelectedItemWidget* friendItemWidget = new SelectedItemWidget(friend_id, friend_name, m_selectTree);
		friendItem->setData(0, Qt::UserRole, friend_id);
		//创建者或已加入无法选择
		if (shouldFilterUser(friend_id))
		{
			friendItemWidget->setEnabled(false);
		}
		//关联
		m_selectTree->setItemWidget(friendItem, 0, friendItemWidget);
		connect(friendItemWidget, &SelectedItemWidget::checked, this, [=](bool isChecked)
			{
				if (isChecked)
				{
					addSelectedItem(ui->selectedFriendList, friend_id, friend_name);
					//选中成员列表
					m_selectedList.append(friend_id);
				}
				else
				{
					removeSelectedItem(friend_id);
				}
			});
	}
	//分组子item数量
	for (auto it = groupingNameToItemHash.cbegin(); it != groupingNameToItemHash.cend(); it++)
	{
		QTreeWidgetItem* topItem = it.value();
		auto topItemWidget = static_cast<TopItemWidget*>(m_selectTree->itemWidget(topItem, 0));
		topItemWidget->setCount(topItem->childCount());
	}
}

//加载群组列表
void GroupMemberOperatorWidget::loadGroupMembers(const QString& group_id)
{
	clearFriendTree();
	auto currentGroup = GroupManager::instance()->findGroup(group_id);
	if (!currentGroup) {
		qDebug() << "群组未找到：" << group_id;
		return;
	}
	auto& groupMembers = currentGroup->getMembers();
	m_userIdList = groupMembers.keys();
	//当前群成员topitem
	auto groupMemberTopItem = new QTreeWidgetItem(m_selectTree);
	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_selectTree);
	topItemWidget->setName("当前群成员");
	m_selectTree->setItemWidget(groupMemberTopItem, 0, topItemWidget);
	for (auto it = groupMembers.begin(); it != groupMembers.end(); ++it)
	{
		const QString& member_id = it.key();
		const Member& member = it.value();

		auto groupMemberItem = new QTreeWidgetItem(groupMemberTopItem);
		//把自己置顶
		if (member_id == LoginUserManager::instance()->getLoginUserID())
		{
			groupMemberItem->removeChild(groupMemberItem);
			groupMemberItem->insertChild(0, groupMemberItem);
		}
		groupMemberItem->setData(0, Qt::UserRole, member_id);
		groupMemberItem->setSizeHint(0, QSize(m_selectTree->width(), 60));
		//自定义Item
		SelectedItemWidget* itemWidget = new SelectedItemWidget(member_id, member.member_name, this);
		if (shouldFilterUser(member_id))
		{
			itemWidget->setEnabled(false);
		}
		//关联
		m_selectTree->setItemWidget(groupMemberItem, 0, itemWidget);
		connect(itemWidget, &SelectedItemWidget::checked, this, [=](bool isChecked)
			{
				if (isChecked)
				{
					addSelectedItem(ui->selectedFriendList, member_id, member.member_name);
					//选中成员列表
					m_selectedList.append(member_id);
				}
				else
				{
					removeSelectedItem(member_id);
				}
			});
		//通过itemwidget找到自定义的小部件
		auto topItemWidget = qobject_cast<TopItemWidget*>(m_selectTree->itemWidget(groupMemberTopItem, 0));
		topItemWidget->setCount(groupMemberTopItem->childCount());
	}
}

//清空好友列表
void GroupMemberOperatorWidget::clearFriendTree()
{
	// 清空 QTreeWidget 中的项
	m_selectTree->clear();
	// 清空 selectedFriendList 中的项
	ui->selectedFriendList->clear();

	ui->searchEdit->clear();
	m_selectedList.clear();
	ui->groupNameEdit->clear();
}

//清空搜索列表
void GroupMemberOperatorWidget::clearSearchList()
{
	// 清空 m_searchList 中的项
	m_searchList->clear();
}

#include "GroupInviteWidget.h"
#include "ui_GroupInviteWidget.h"
#include <QFile>
#include <QStack>
#include <QMessageBox>

#include "TopItemWidget.h"
#include "ItemWidget.h"
#include "FriendSelectedWidget.h"
#include "FriendManager.h"
#include "EventBus.h"
#include "MessageSender.h"

GroupInviteWidget::GroupInviteWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::GroupInviteWidget)
	, m_friendTree(new QTreeWidget(this))
	, m_searchList(new QListWidget(this))
{
	ui->setupUi(this);
	init();
	this->setObjectName("groupInviteWidget");
	m_searchList->setObjectName("searchListWidget");
	ui->selectedFriendList->setObjectName("selectedFriendList");
	QFile file(":/stylesheet/Resource/StyleSheet/GroupInviteWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else {
		qDebug() << file.fileName() << "样式表打开失败";
	}
}
GroupInviteWidget::~GroupInviteWidget()
{

}

void GroupInviteWidget::init()
{
	ui->friendStackedList->addWidget(m_friendTree);
	ui->friendStackedList->addWidget(m_searchList);
	ui->friendStackedList->setCurrentWidget(m_friendTree);
	//好友搜索框
	ui->searchEdit->setPlaceholderText("好友搜索");
	//好友搜索
	connect(ui->searchEdit, &QLineEdit::textChanged, this, [=](const QString& text)
		{
			clearSearchList();
			if (text.isEmpty())
			{
				ui->friendStackedList->setCurrentWidget(m_friendTree);
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
				auto obj = it.value()->getFriend();
				auto pixmap = it.value()->getAvatar();
				if (obj.isEmpty() || pixmap.isNull()) {
					qWarning() << "Invalid object or pixmap!";
					return;
				}
				addSelectedFriendItem(m_searchList, it.value()->getFriendId());
			}
		});

	m_friendTree->setHeaderHidden(true);
	m_friendTree->setColumnWidth(0, m_friendTree->width());
	//QTreeWidget里的根和子项相同宽度
	m_friendTree->setIndentation(0);
	//好友列表展开
	connect(m_friendTree, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item)
		{
			//点击分组展开
			if (item->parent() == nullptr)
			{
				item->setExpanded(!item->isExpanded());
				//通过itemwidget找到自定义的小部件topItemWidget
				auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendTree->itemWidget(item, 0));
				topItemWidget->setAgale();
			}
			else     //点击用户
			{
				////选择好友
				auto itemWidget = qobject_cast<FriendSelectedWidget*>(m_friendTree->itemWidget(item, 0));
				itemWidget->setChecked(!itemWidget->isChecked());
				auto user_id = itemWidget->getUserId();
				if (itemWidget->isChecked())
				{
					addSelectedFriendItem(ui->selectedFriendList, user_id);
				}
				else
				{
					removeFriendListItem(user_id);
				}
			}
		});
	//搜索列表
	connect(m_searchList, &QListWidget::itemClicked, [=](QListWidgetItem* item)
		{
			auto itemWidget = qobject_cast<FriendSelectedWidget*>(m_searchList->itemWidget(item));
			itemWidget->setChecked(!itemWidget->isChecked());
			auto user_id = itemWidget->getUserId();
			if (itemWidget->isChecked())
			{
				addSelectedFriendItem(ui->selectedFriendList, user_id);
			}
			else
			{
				removeFriendListItem(user_id);
			}
		});
	//创建
	connect(ui->okBtn, &QPushButton::clicked, this, [=]
		{
			if (ui->selectedFriendList->count() == 0)
			{
				QMessageBox::warning(nullptr, "警告", "请选择群聊邀请人数");
				return;
			}
			if (ui->groupNameEdit->text().isEmpty())
			{
				QMessageBox::warning(nullptr, "警告", "群名不能为空");
				return;
			}
			EventBus::instance()->emit createGroup(ui->groupNameEdit->text());
			QVariantMap createGroupMap;
			createGroupMap["user_id"] = FriendManager::instance()->getOneselfID();
			createGroupMap["group_name"] = ui->groupNameEdit->text();
			MessageSender::instance()->sendMessage("createGroup", createGroupMap);
			this->hide();
		});
	//取消创建
	connect(ui->cancelBtn, &QPushButton::clicked, this, [=]
		{
			//删除旧friendtree并且释放资源
			clearFriendTree();
			clearSearchList();
			this->hide();
		});
}
//选中好友添加至选中列表
void GroupInviteWidget::addSelectedFriendItem(QListWidget* listWidget, const QString& user_id)
{
	auto item = findListItem(listWidget, user_id);
	if (item)
	{
		return;
	}
	item = new QListWidgetItem(listWidget);
	item->setData(Qt::UserRole, user_id);
	auto itemWidget = new FriendSelectedWidget(listWidget);
	item->setSizeHint(QSize(listWidget->width(), 70));
	itemWidget->setUser(user_id);

	auto treeItem = findTreeItem(user_id);
	auto treeItemWidget = dynamic_cast<FriendSelectedWidget*>(m_friendTree->itemWidget(treeItem, 0));
	auto checked = treeItemWidget->isChecked();
	itemWidget->setChecked(checked);

	listWidget->setItemWidget(item, itemWidget);
	connect(itemWidget, &FriendSelectedWidget::checkFriend, this, [=](bool isChecked)
		{
			if (isChecked)
			{
				if (listWidget == m_searchList)
				{
					treeItemWidget->setChecked(true);
					addSelectedFriendItem(ui->selectedFriendList, user_id);
				}
			}
			else
			{
				removeFriendListItem(user_id);
			}
		});
}
//删除选中好友item
void GroupInviteWidget::removeFriendListItem(const QString& user_id)
{
	auto listItem = findListItem(ui->selectedFriendList, user_id);
	if (listItem)
	{
		auto itemWidget = dynamic_cast<FriendSelectedWidget*>(ui->selectedFriendList->itemWidget(listItem));
		delete itemWidget;
		auto row = ui->selectedFriendList->row(listItem);
		delete ui->selectedFriendList->takeItem(row);;
	}
	auto treeItem = findTreeItem(user_id);
	auto treeItemWidget = dynamic_cast<FriendSelectedWidget*>(m_friendTree->itemWidget(treeItem, 0));
	treeItemWidget->setChecked(false);
	auto searchItem = findListItem(m_searchList, user_id);
	if (searchItem)
	{
		auto searchItemWidget = dynamic_cast<FriendSelectedWidget*>(m_searchList->itemWidget(searchItem));
		searchItemWidget->setChecked(false);
	}

}
//通过用户id找到用户消息项item
QListWidgetItem* GroupInviteWidget::findListItem(QListWidget* listWidget, const QString& user_id)
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
QTreeWidgetItem* GroupInviteWidget::findTreeItem(const QString& user_id)
{
	// 遍历顶层节点
	for (int i = 0; i < m_friendTree->topLevelItemCount(); ++i) {
		QTreeWidgetItem* item = m_friendTree->topLevelItem(i);

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
//克隆好友列表
void GroupInviteWidget::cloneFriendTree(ContactListWidget& contactListWidget)
{
	// 遍历顶级项
	for (int i = 0; i < contactListWidget.m_friendList->topLevelItemCount(); ++i) {
		// 获取old顶级项
		QTreeWidgetItem* topItem = contactListWidget.m_friendList->topLevelItem(i);
		TopItemWidget* topItemWidget = dynamic_cast<TopItemWidget*>(contactListWidget.m_friendList->itemWidget(topItem, 0));
		//new顶级项
		QTreeWidgetItem* newTopItem = new QTreeWidgetItem(m_friendTree);
		TopItemWidget* newTopItemWidget = new TopItemWidget(m_friendTree);
		newTopItem->setData(0, Qt::UserRole, m_friendTree->topLevelItemCount());
		//复制topitemwidget属性
		newTopItemWidget->setName(topItemWidget->getName());
		newTopItemWidget->setCount(topItem->childCount());
		//关联
		m_friendTree->setItemWidget(newTopItem, 0, newTopItemWidget);
		for (int j = 0; j < topItem->childCount(); ++j)
		{
			auto item = topItem->child(j);
			auto itemWidget = dynamic_cast<ItemWidget*>(contactListWidget.m_friendList->itemWidget(item, 0));
			auto user_id = itemWidget->getUserId();
			//new子项
			QTreeWidgetItem* newItem = new QTreeWidgetItem(newTopItem);
			FriendSelectedWidget* newItemWidget = new FriendSelectedWidget(this);
			//复制itemwidget属性
			newItem->setData(0, Qt::UserRole, user_id);
			newItemWidget->setUser(user_id);
			//关联
			m_friendTree->setItemWidget(newItem, 0, newItemWidget);
			connect(newItemWidget, &FriendSelectedWidget::checkFriend, this, [=](bool isChecked)
				{
					if (isChecked)
					{
						addSelectedFriendItem(ui->selectedFriendList, user_id);
					}
					else
					{
						removeFriendListItem(user_id);
					}
				});
		}
	}
}
//清空好友列表
void GroupInviteWidget::clearFriendTree()
{
	// 清空 QTreeWidget 中的项
	m_friendTree->clear();

	// 如果有需要，可以通过手动删除 `setItemWidget` 的小部件来避免内存泄漏
	for (int i = 0; i < m_friendTree->topLevelItemCount(); ++i) {
		QTreeWidgetItem* topItem = m_friendTree->topLevelItem(i);
		TopItemWidget* topItemWidget = dynamic_cast<TopItemWidget*>(m_friendTree->itemWidget(topItem, 0));
		delete topItem;
		if (topItemWidget) {
			delete topItemWidget;  // 显式删除小部件
		}
	}

	// 如果是 QListWidget
	for (int i = 0; i < ui->selectedFriendList->count(); ++i) {
		QListWidgetItem* listItem = ui->selectedFriendList->item(i);
		// 获取关联的小部件
		QWidget* listItemWidget = ui->selectedFriendList->itemWidget(listItem);
		if (listItemWidget) {
			delete listItemWidget;  // 删除小部件
		}
		delete listItem;
	}
	// 清空 selectedFriendList 中的项
	ui->selectedFriendList->clear();

	ui->searchEdit->clear();
	ui->groupNameEdit->clear();
}
//清空搜索列表
void GroupInviteWidget::clearSearchList()
{
	// 如果是 QListWidget
	for (int i = 0; i < m_searchList->count(); ++i) {
		QListWidgetItem* listItem = m_searchList->item(i);
		// 获取关联的小部件
		QWidget* itemWidget = m_searchList->itemWidget(listItem);
		if (itemWidget) {
			delete itemWidget;  // 删除小部件
		}
		delete listItem;
	}
	// 清空 m_searchList 中的项
	m_searchList->clear();
}

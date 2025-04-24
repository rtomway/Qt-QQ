#include "GroupInviteWidget.h"
#include "ui_GroupInviteWidget.h"
#include <QFile>
#include <QStack>
#include <QMessageBox>
#include <QJSonArray>

#include "TopItemWidget.h"
#include "ItemWidget.h"
#include "FriendSelectedWidget.h"
#include "FriendManager.h"
#include "EventBus.h"
#include "MessageSender.h"
#include "AvatarManager.h"
#include "GroupManager.h"
#include "Group.h"
#include "LoginUserManager.h"
#include "PacketCreate.h"

GroupInviteWidget::GroupInviteWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::GroupInviteWidget)
	, m_friendTree(new QTreeWidget(this))
	, m_searchList(new QListWidget(this))
{
	ui->setupUi(this);
	init();
	this->setFixedSize(530, 580);
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
				AvatarManager::instance()->getAvatar(it.value()->getFriendId(), ChatType::User, [=](const QPixmap& pixmap)
					{
						addSelectedFriendItem(m_searchList, it.value()->getFriendId());
					});
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
				auto user_id = itemWidget->getUserId();
				//群主或已加入的成员不可操作
				if (user_id == LoginUserManager::instance()->getLoginUserID())
					return;
				itemWidget->setChecked(!itemWidget->isChecked());
			}
		});
	//搜索列表
	connect(m_searchList, &QListWidget::itemClicked, [=](QListWidgetItem* item)
		{
			auto itemWidget = qobject_cast<FriendSelectedWidget*>(m_searchList->itemWidget(item));
			auto user_id = itemWidget->getUserId();
			//群主或已加入的成员不可操作
			if (user_id == LoginUserManager::instance()->getLoginUserID())
				return;

			itemWidget->setChecked(!itemWidget->isChecked());
		});
	//创建群聊
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
			QJsonObject createGroupObj;
			auto& user_id = LoginUserManager::instance()->getLoginUserID();
			createGroupObj["user_id"] = user_id;
			createGroupObj["username"] = FriendManager::instance()->findFriend(user_id)->getFriendName();
			createGroupObj["group_name"] = ui->groupNameEdit->text();
			// 将 QStringList 转换为 QJsonArray
			QJsonArray inviteMembersArray;
			for (const QString& member : m_selectedList) {
				inviteMembersArray.append(member);
			}
			createGroupObj["inviteMembers"] = inviteMembersArray;
			auto message = PacketCreate::textPacket("createGroup", createGroupObj);
			MessageSender::instance()->sendMessage(message);
			qDebug() << "群聊创建发送";
			clearFriendTree();
			clearSearchList();
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
//设置模式
void GroupInviteWidget::setGroupWidgetMode(GroupMode mode)
{
	switch (mode)
	{
	case GroupInviteWidget::CreateGroup:
		ui->groupModeLab->setText("群聊创建");
		ui->GroupNameLab->setText("群聊名称");
		break;
	case GroupInviteWidget::InviteMembers:
		ui->groupModeLab->setText("邀请新成员");
		ui->groupNameEdit->setVisible(false);
		ui->GroupNameLab->setText(GroupManager::instance()->findGroup(m_groupId)->getGroupName());
		break;
	case GroupInviteWidget::DeleteMembers:
		ui->groupModeLab->setText("移除成员");
		ui->groupNameEdit->setVisible(false);
		ui->GroupNameLab->setText(GroupManager::instance()->findGroup(m_groupId)->getGroupName());
		break;
	default:
		break;
	}
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
				//搜索列表确认（tree确认并添加）
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
	m_selectedList.removeOne(user_id);
	auto listItem = findListItem(ui->selectedFriendList, user_id);
	if (listItem)
	{
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
			auto user_id = item->data(0, Qt::UserRole).toString();
			//new子项
			QTreeWidgetItem* newItem = new QTreeWidgetItem(newTopItem);
			FriendSelectedWidget* newItemWidget = new FriendSelectedWidget(this);
			//复制itemwidget属性
			newItem->setData(0, Qt::UserRole, user_id);
			newItemWidget->setUser(user_id);
			// 取消可用状态
			if (user_id == LoginUserManager::instance()->getLoginUserID())
			{
				newItem->setFlags(newItem->flags() & ~Qt::ItemIsEnabled);
				newItemWidget->setEnabled(false);
			}
				
			//关联
			m_friendTree->setItemWidget(newItem, 0, newItemWidget);
			connect(newItemWidget, &FriendSelectedWidget::checkFriend, this, [=](bool isChecked)
				{
					if (isChecked)
					{
						addSelectedFriendItem(ui->selectedFriendList, user_id);
						//选中成员列表
						m_selectedList.append(user_id);
					}
					else
					{
						removeFriendListItem(user_id);
					}
				});
		}
	}
}
//删除时 当前群成员
void GroupInviteWidget::cloneGroupMember(const QString& group_id)
{
	auto currentGroup = GroupManager::instance()->findGroup(group_id);
	if (!currentGroup) {
		qDebug() << "群组未找到：" << group_id;
		return;
	}
	auto& groupMembers = currentGroup->getMembers();
	//当前群成员topitem
	auto groupMemberTopItem = new QTreeWidgetItem(m_friendTree);
	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_friendTree);
	topItemWidget->setName("当前群成员");
	m_friendTree->setItemWidget(groupMemberTopItem, 0, topItemWidget);
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
		groupMemberItem->setSizeHint(0, QSize(m_friendTree->width(), 60));
		//自定义Item
		FriendSelectedWidget* itemWidget = new FriendSelectedWidget(this);
		itemWidget->setGroupMember(member_id, member.member_name);
		m_friendTree->setItemWidget(groupMemberItem, 0, itemWidget);
		//通过itemwidget找到自定义的小部件
		auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendTree->itemWidget(groupMemberTopItem, 0));
		topItemWidget->setCount(groupMemberTopItem->childCount());
	}
}
void GroupInviteWidget::setGroupid(const QString& group_id)
{
	m_groupId = group_id;
}
//清空好友列表
void GroupInviteWidget::clearFriendTree()
{
	// 清空 QTreeWidget 中的项
	m_friendTree->clear();
	// 清空 selectedFriendList 中的项
	ui->selectedFriendList->clear();

	ui->searchEdit->clear();
	m_selectedList.clear();
	ui->groupNameEdit->clear();
}
//清空搜索列表
void GroupInviteWidget::clearSearchList()
{
	// 清空 m_searchList 中的项
	m_searchList->clear();
}

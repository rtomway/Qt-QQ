#include "ContactList.h"
#include "ui_ContactList.h"
#include <QFile>
#include <QLabel>
#include <QBoxLayout>
#include <QMutex>
#include <QMutexLocker>
#include <QJsonObject>
#include <QSharedPointer>

#include "ItemWidget.h"
#include "Client.h"
#include "FriendNoticeItemWidget.h"
#include "User.h"
#include "FriendManager.h"
#include "Friend.h"

QStringList ContactList::m_fNamelist{};
QStringList ContactList::m_gNamelist{};

ContactList::ContactList(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::ContactList)
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

ContactList::~ContactList()
{
	delete ui;
}

ContactList* ContactList::instance()
{
	static ContactList instance;
	return &instance;
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
	connect(FriendManager::instance(), &FriendManager::UserAvatarLoaded, this, [=](const QPixmap& avatar)
		{
			auto friends = FriendManager::instance()->getFriends();
			for (auto it = friends.begin(); it != friends.end(); ++it) {
				const QSharedPointer<Friend>& friendInfo = it.value();
				auto grouping = friendInfo->getGrouping();
				//判断该分组是否已存在
				if(!m_fNamelist.contains(grouping))
				addFriendListItem(grouping);
				qDebug() << "分组" << grouping;
				addFriendItem(getFriendTopItem(grouping), friendInfo->getFriend());
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
				auto obj = itemWidget->getUser();
				qDebug() << "clikedobj" << obj["grouping"].toString();
				emit clickedFriend(obj);
			}

		});
	//新增好友
	connect(FriendManager::instance(), &FriendManager::NewFriend, this, [=](const QString& user_id)
		{
			auto myfriend = FriendManager::instance()->findFriend(user_id);
			auto grouping = myfriend->getGrouping();
			addFriendItem(getFriendTopItem(grouping), myfriend->getFriend());
		});
	//信息更新
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			qDebug() << "用户信息更新99999999999999999999999999";
			auto user = FriendManager::instance()->findFriend(user_id);
			auto groupingItem = getFriendTopItem(user->getGrouping());
			auto item= findItemByIdInGroup(groupingItem, user_id);
			if (item)
			{
				auto itemWidget = qobject_cast<ItemWidget*>(m_friendList->itemWidget(item, 0));
				itemWidget->setUser(user->getFriend());
			}
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
	emit updateFriendgrouping();
	//自定义topItem
	TopItemWidget* topItemWidget = new TopItemWidget(m_friendList);
	topItemWidget->setName(friendName);
	//topItemWidget->setItem(friendListItem);
	m_friendList->setItemWidget(friendListItem, 0, topItemWidget);
	qDebug() << "创建了分组item" << friendName;
	return topItemWidget;
}

//添加子item
void ContactList::addFriendItem(QTreeWidgetItem* firendList, const QJsonObject& obj)
{
	auto user_id = obj["user_id"].toString();
	auto friendItem = findItemByIdInGroup(firendList, user_id);
	if(friendItem!=nullptr)
	{
		qDebug() << "该好友已存在";
		return;
	}
    friendItem = new QTreeWidgetItem(firendList);
	friendItem->setData(0, Qt::UserRole,user_id );
	friendItem->setSizeHint(0, QSize(m_friendList->width(), 60));
	qDebug() << "username:" << obj["username"].toString();
	//自定义Item
	ItemWidget* itemWidget = new ItemWidget(this);
	itemWidget->setUser(obj);
	m_friendList->setItemWidget(friendItem, 0, itemWidget);

	//通过itemwidget找到自定义的小部件
	auto topItemWidget = qobject_cast<TopItemWidget*>(m_friendList->itemWidget(firendList, 0));
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
//获取子item
QTreeWidgetItem* ContactList::findItemByIdInGroup(QTreeWidgetItem* group, const QString& userId) {
	if (!group) return nullptr;  // 避免空指针

	for (int i = 0; i < group->childCount(); ++i) {
		QTreeWidgetItem* friendItem = group->child(i);
		if (friendItem->data(0, Qt::UserRole).toString() == userId) {
			return friendItem;  // 找到匹配 ID，返回 item
		}
	}
	return nullptr;  // 没找到
}

TopItemWidget* ContactList::addGroupListItem(QString groupName)
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
void ContactList::addGroupItem(QTreeWidgetItem* groupListItem, QString groupName)
{
	//auto groupItem = new QTreeWidgetItem(groupListItem, groupName);
}
QTreeWidgetItem* ContactList::getGroupTopItem(QString groupName)
{
	return nullptr;
}
//通知数量更新
void ContactList::updateFriendNoticeCount()
{
	m_fNoticeUnreadCount++;
	ui->friendNoticeCountLab->setVisible(true);
	ui->friendNoticeCountLab->setText(QString::number(m_fNoticeUnreadCount));
}
//退出清除前一个账户信息
void ContactList::clearContactList()
{
	m_friendList->clear();
	m_groupList->clear();
	m_fNamelist.clear();
	m_gNamelist.clear();
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


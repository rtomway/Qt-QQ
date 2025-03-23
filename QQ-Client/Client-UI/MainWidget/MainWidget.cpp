#pragma once
#include "MainWidget.h"
#include "ui_MainWidget.h"
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <QBoxlayout>
#include <QLabel>
#include <QMouseEvent>
#include <memory>
#include <QPointer>
#include <QJsonDocument>
#include <QJsonObject>

#include "ImageUtil.h"
#include "SMaskWidget.h"
#include "Friend.h"
#include "FriendManager.h"
#include "ChatManager.h"
#include "EventBus.h"

MainWidget::MainWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::MainWidget)
	, m_addpersonMenu(new QMenu(this))
	, m_moreMenu(new QMenu(this))
	, m_messagePage(new MessagePage(this))
	, m_contactPage(new ContactPage(this))
	, m_noticePage(new NoticeWidget(this))
	, m_emptyPage(new QWidget(this))
	, m_chatMessageListWidget(new QListWidget(this))
	, m_friendSearchListWidget(new SearchFriend(this))
	, m_contactListWidget(new ContactListWidget(this))
	,m_groupInviteWidget(new GroupInviteWidget(this))
{
	ui->setupUi(this);
	init();
	initMoreMenu();
	initStackedListWidget();
	initStackedPage();

	this->setWindowFlag(Qt::FramelessWindowHint);
	ui->headLab->installEventFilter(this);
	this->installEventFilter(this);
	// m_searchList 中有子控件逐个安装过滤器
	ui->searchEdit->installEventFilter(this);
	m_friendSearchListWidget->setFocusPolicy(Qt::StrongFocus);
	m_friendSearchListWidget->installEventFilter(this);
	QList<QWidget*> childWidgets = m_friendSearchListWidget->findChildren<QWidget*>();
	for (QWidget* child : childWidgets) {
		child->installEventFilter(this);
	}
	this->setFocusPolicy(Qt::StrongFocus);
	this->setObjectName("MainWidget");
	this->setStyleSheet(R"(QWidget#MainWidget{border-radius: 10px;})");
	QFile file(":/stylesheet/Resource/StyleSheet/MainWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else {
		qDebug() << "样式表打开失败";
	}
	//编辑信息蒙层主窗口
	SMaskWidget::instance()->setMainWidget(this);
	//m_contaciListWidget信号通知
	//点击通知 进入通知界面
	connect(m_contactListWidget, &ContactListWidget::friendNotice, this, [=]
		{
			ui->messageStackedWidget->setCurrentWidget(m_noticePage);
			m_noticePage->setCurrentWidget(NoticeWidget::FriendNoticeWidget);
		});
	connect(m_contactListWidget, &ContactListWidget::groupNotice, this, [=]
		{
			ui->messageStackedWidget->setCurrentWidget(m_noticePage);
			m_noticePage->setCurrentWidget(NoticeWidget::GroupNoticeWidget);
		});
	//点击好友查看好友信息
	connect(m_contactListWidget, &ContactListWidget::clickedFriend, this, [=](const QString& user_id)
		{
			m_contactPage->setUser(user_id);
			ui->messageStackedWidget->setCurrentWidget(m_contactPage);
			ui->rightWidget->setStyleSheet("background-color:white");
			qDebug() << m_contactPage->parent();

		});
	//分组更新
	connect(m_contactListWidget, &ContactListWidget::updateFriendgrouping, m_contactPage, &ContactPage::updateFriendgrouping);

	//好友搜索
	connect(ui->searchEdit, &QLineEdit::textChanged, this, [=](const QString& text)
		{
			m_friendSearchListWidget->clearFriendList();
			if (text.isEmpty())
				return;
			auto searchResultHash = FriendManager::instance()->findFriends(text);
			if (searchResultHash.isEmpty()) {
				qWarning() << "No friends found for text: " << text;
				return;
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
				m_friendSearchListWidget->addSearchItem(obj, pixmap);
			}
		});
	//点击用户消息项 进入会话界面（加载用户信息）
	connect(m_chatMessageListWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem* item)
		{
			qDebug() << "进入会话界面（加载用户信息）";
			auto itemWidget = qobject_cast<MessageListItem*>(m_chatMessageListWidget->itemWidget(item));
			itemWidget->clearUnreadMessage();
			//已经处于当前用户会话界面并且是显示状态
			if (m_messagePage == ui->messageStackedWidget->currentWidget() && m_messagePage->getCurrentID() == itemWidget->getId())
				return;
			ui->messageStackedWidget->setCurrentWidget(m_messagePage);
			//清空界面
			m_messagePage->clearMessageWidget();
			//将当前用户信息以及聊天记录加载到会话界面
			m_messagePage->setCurrentUser(itemWidget->getUser());
		});

	//好友中心通知
	//个人信息头像加载
	connect(FriendManager::instance(), &FriendManager::FriendManagerLoadSuccess, this, [=](const QPixmap& avatar)
		{
			auto pixmap = ImageUtils::roundedPixmap(avatar, QSize(50, 50));
			ui->headLab->setPixmap(pixmap);
		});
	//好友信息更新
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			if (user_id != FriendManager::instance()->getOneselfID())
				return;
			auto user = FriendManager::instance()->findFriend(user_id);
			auto pixmap = ImageUtils::roundedPixmap(user->getAvatar(), QSize(40, 40));
			ui->headLab->setPixmap(pixmap);
		});
	//好友信息更新,消息项相关信息更新
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, [=](const QString& user_id)
		{
			qDebug() << "信息更新" << user_id;
			auto messageItem = findListItem(user_id);
			if (messageItem)
			{
				qDebug() << "信息更新";
				auto user = FriendManager::instance()->findFriend(user_id);
				auto Item = qobject_cast<MessageListItem*>(m_chatMessageListWidget->itemWidget(messageItem));
				Item->setUser(user->getFriend());
			}
		});
	//新增好友
	connect(FriendManager::instance(), &FriendManager::NewFriend, this, [=](const QString& user_id)
		{
			auto myfriend = FriendManager::instance()->findFriend(user_id);
			auto friendObj = myfriend->getFriend();
			//m_messagePage->setUser(friendObj);
			ChatManager::instance()->addChat(user_id, std::make_shared<ChatMessage>(FriendManager::instance()->getOneselfID(), user_id));
			addmessageListItem(friendObj);
		});
	//好友信息界面跳转到会话界面
	connect(FriendManager::instance(), &FriendManager::chatWithFriend, this, [=](const QString& user_id)
		{
			m_btn_Itemgroup->button(-2)->setChecked(true);
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
			ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			auto messageItem = findListItem(user_id);
			if (!messageItem) //判断消息项是否存在
			{
				auto user = FriendManager::instance()->findFriend(user_id);
				messageItem = addmessageListItem(user->getFriend());
			}
			m_chatMessageListWidget->setCurrentItem(messageItem);
			emit m_chatMessageListWidget->itemClicked(messageItem);

		});

	//接受信息通知
	//接受到消息 用户消息项更新
	connect(EventBus::instance(), &EventBus::textCommunication, this, [=](const QJsonObject& obj)
		{
			qDebug() << "接受消息json:" << obj;
			auto m_userId = FriendManager::instance()->getOneselfID();
			auto friend_id = obj["user_id"].toString();
			auto message = obj["message"].toString();
			auto item = findListItem(friend_id);
			auto itemWidget = qobject_cast<MessageListItem*>(m_chatMessageListWidget->itemWidget(item));
			//自己给自己发消息
			if (friend_id == m_userId)
			{
				itemWidget->setUser(obj);
				itemWidget->clearUnreadMessage();
				return;
			}
			//接收信息加入聊天记录(自己无需添加)
			m_messagePage->updateChatMessage(friend_id, m_userId, QVariant::fromValue(message));
			if (item)
			{
				//有消息项 追加消息内容
				itemWidget->setUser(obj);
				//判断当下是否是该user_id会话界面
				if (m_messagePage->getCurrentID() == friend_id && ui->messageStackedWidget->currentWidget() == m_messagePage)
				{
					m_messagePage->updateReciveMessage(message);
					itemWidget->clearUnreadMessage();
				}
			}
			else
			{
				//没有消息项创建一个
				item = addmessageListItem(obj);
			}
		});
	connect(EventBus::instance(), &EventBus::pictureCommunication, this, [=](const QJsonObject& obj,const QPixmap&pixmap)
		{
			auto m_userId = FriendManager::instance()->getOneselfID();
			auto friend_id = obj["user_id"].toString();
			auto item = findListItem(friend_id);
			auto itemWidget = qobject_cast<MessageListItem*>(m_chatMessageListWidget->itemWidget(item));
			//自己给自己发消息
			if (friend_id == m_userId)
			{
				itemWidget->setUser(obj);
				itemWidget->clearUnreadMessage();
				return;
			}
			//接收信息加入聊天记录(自己无需添加)
			m_messagePage->updateChatMessage(friend_id, m_userId, QVariant::fromValue(pixmap));
			if (item)
			{
				//有消息项 追加消息内容
				itemWidget->setUser(obj);
				//判断当下是否是该user_id会话界面
				if (m_messagePage->getCurrentID() == friend_id && ui->messageStackedWidget->currentWidget() == m_messagePage)
				{
					m_messagePage->updateReciveMessage(pixmap);
					itemWidget->clearUnreadMessage();
				}
			}
			else
			{
				//没有消息项创建一个
				item = addmessageListItem(obj);
			}
		});
	//好友申请通知
	connect(EventBus::instance(), &EventBus::addFriend, this, [=]
		{
			if (ui->messageStackedWidget->currentWidget() != m_noticePage)
			{
				m_contactListWidget->updateFriendNoticeCount();
			}
		});
	//被拒通知
	connect(EventBus::instance(), &EventBus::rejectAddFriend, this, [=]
		{
			if (ui->messageStackedWidget->currentWidget() != m_noticePage)
			{
				m_contactListWidget->updateFriendNoticeCount();
			}
		});
	//好友删除
	connect(EventBus::instance(), &EventBus::deleteFriend, this, [=](const QString& user_id)
		{
			auto item = findListItem(user_id);
			m_chatMessageListWidget->takeItem(m_chatMessageListWidget->row(item));
			ui->messageStackedWidget->setCurrentWidget(m_emptyPage);
			m_contactPage->clearWidget();
		});
}
MainWidget::~MainWidget()
{
	delete ui;
}
//界面初始化
void MainWidget::init()
{
	resize(1080, 680);
	//界面按钮列表
	ui->listWidget->setFixedWidth(60);
	ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//列表按钮组
	m_btn_Itemgroup = new QButtonGroup(this);
	//界面按钮列表图标
	ui->qqLab->setAlignment(Qt::AlignCenter);
	additemCenter(QString(":/icon/Resource/Icon/message.png"));
	additemCenter(QString(":/icon/Resource/Icon/contact.png"));
	additemCenter(QString(":/icon/Resource/Icon/QQspace.png"));
	additemCenter(QString(":/icon/Resource/Icon/channel.png"));
	additemCenter(QString(":/icon/Resource/Icon/game.png"));
	additemCenter(QString(":/icon/Resource/Icon/shortview.png"));
	additemCenter(QString(":/icon/Resource/Icon/more_2.png"));
	ui->listWidget->setIconSize(QSize(50, 50));
	m_btn_Itemgroup->button(-2)->setChecked(true);
	//页面切换
	connect(m_btn_Itemgroup, &QButtonGroup::idClicked, this, [=](int id)
		{
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
			m_chatMessageListWidget->clearSelection();
			switch (id)
			{
			case -2:
				ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
				ui->messageStackedWidget->setCurrentWidget(m_emptyPage);
				break;
			case -3:
				ui->listStackedWidget->setCurrentWidget(m_contactListWidget);
				ui->messageStackedWidget->setCurrentWidget(m_emptyPage);
				break;
			default:
				break;
			}
		});
	//窗口操作
	ui->hideBtn->setIcon(QIcon(":/icon/Resource/Icon/hide.png"));
	ui->expandBtn->setIcon(QIcon(":/icon/Resource/Icon/expand.png"));
	ui->exitBtn->setIcon(QIcon(":/icon/Resource/Icon/x.png"));
	connect(ui->hideBtn, &QPushButton::clicked, this, [=]
		{
			emit hideWidget();
		});
	connect(ui->expandBtn, &QPushButton::clicked, this, [=]
		{
			emit expandWidget();
		});
	connect(ui->exitBtn, &QPushButton::clicked, this, [=]
		{
			emit exitWidget();
		});
	//好友搜索框
	ui->searchEdit->setPlaceholderText("搜索");
	//+号(添加好友或群组)
	auto creatGroup = m_addpersonMenu->addAction(QIcon(":/icon/Resource/Icon/createGroup.png"), "创建群聊");
	auto addFriend = m_addpersonMenu->addAction(QIcon(":/icon/Resource/Icon/addperson.png"), "加好友/群");
	//+  弹出添加菜单
	ui->addBtn->setIcon(QIcon(":/icon/Resource/Icon/add.png"));
	connect(ui->addBtn, &QPushButton::clicked, [=]
		{
			m_addpersonMenu->popup(mapToGlobal(QPoint(ui->addBtn->geometry().x() + 70, ui->addBtn->geometry().y() + 30)));
		});
	//添加好友界面
	connect(addFriend, &QAction::triggered, [=]
		{
			m_addFriendWidget = std::make_unique<AddFriendWidget>();
			m_addFriendWidget->show();
		});
	//群聊创建
	connect(creatGroup, &QAction::triggered, [=]
		{
			m_groupInviteWidget->cloneFriendTree(*m_contactListWidget);
			//蒙层
			SMaskWidget::instance()->popUp(m_groupInviteWidget);
			auto mainWidgetSize = SMaskWidget::instance()->getMainWidgetSize();
			int x = (mainWidgetSize.width() - m_groupInviteWidget->width()) / 2;
			int y = (mainWidgetSize.height() - m_groupInviteWidget->height()) / 2;
			SMaskWidget::instance()->setPopGeometry(QRect(x, y, this->width(), this->height()));
		});
}
//子列表
void MainWidget::initStackedListWidget()
{
	//列表窗口
	ui->listStackedWidget->addWidget(m_chatMessageListWidget);
	ui->listStackedWidget->addWidget(m_friendSearchListWidget);
	ui->listStackedWidget->addWidget(m_contactListWidget);
	ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
	m_chatMessageListWidget->setObjectName("MessageList");
}
//子页面
void MainWidget::initStackedPage()
{
	//会话,通知，信息界面
	ui->messageStackedWidget->addWidget(m_messagePage);
	ui->messageStackedWidget->addWidget(m_contactPage);
	ui->messageStackedWidget->addWidget(m_noticePage);
	ui->messageStackedWidget->addWidget(m_emptyPage);
}
//更多（菜单）
void MainWidget::initMoreMenu()
{
	//账号退出清空数据
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/quit.png"), "退出账号", [=]
		{
			FriendManager::instance()->clearFriendManager();
			ChatManager::instance()->clearAllChats();
			m_contactListWidget->clearContactList();
			m_contactPage->clearWidget();
			m_chatMessageListWidget->clear();
			m_btn_Itemgroup->button(-2)->setChecked(true);
			ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			ui->messageStackedWidget->setCurrentWidget(m_emptyPage);
			emit quitsuccess();
		});
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/about.png"), "关于", [=]
		{

		});
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/set.png"), "设置", [=]
		{

		});
	//菜单弹出
	connect(ui->otherBtn, &QPushButton::clicked, [=]
		{
			m_moreMenu->popup(mapToGlobal(QPoint(ui->otherBtn->geometry().x() + 30, ui->otherBtn->geometry().y() + 400)));
		});
}
//界面按钮居中
void MainWidget::additemCenter(const QString& src)
{
	auto item = new QListWidgetItem(ui->listWidget);
	item->setSizeHint(QSize(ui->listWidget->width(), 50));

	auto itemWidget = new QWidget(ui->listWidget);

	auto hlayout = new QHBoxLayout(itemWidget);
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addStretch();
	QPixmap pixmap(src);
	auto itemBtn = new QPushButton(itemWidget);
	itemBtn->setFixedSize(32, 32);
	itemBtn->setCheckable(true);
	itemBtn->setIcon(pixmap);
	m_btn_Itemgroup->addButton(itemBtn);
	hlayout->addWidget(itemBtn);
	hlayout->addStretch();

	itemBtn->setStyleSheet(R"(
					QPushButton{
						border-radius:4px;
						}
					QPushButton:hover{
						background-color:#c6c4c4;
						}
					QPushButton:checked{
						background-color:#c6c4c4;
						}
		)");

	ui->listWidget->setItemWidget(item, itemWidget);
}
//创建消息通知项并设置标识id
QListWidgetItem* MainWidget::addmessageListItem(const QJsonObject& obj)
{
	auto user_id = obj["user_id"].toString();
	auto item = findListItem(user_id);
	if (item != nullptr)
	{
		qDebug() << "消息项已存在" << user_id;
		return nullptr;
	}
	//为item设置用户id
	item = new QListWidgetItem(m_chatMessageListWidget);
	item->setData(Qt::UserRole, user_id);
	item->setSizeHint(QSize(m_chatMessageListWidget->width(), 70));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new MessageListItem(m_chatMessageListWidget);
	itemWidget->setUser(obj);
	//关联item和widget
	m_chatMessageListWidget->setItemWidget(item, itemWidget);
	return item;
}
//通过用户id找到用户消息项item
QListWidgetItem* MainWidget::findListItem(const QString& user_id)
{
	for (auto i = 0; i < m_chatMessageListWidget->count(); i++)
	{
		auto item = m_chatMessageListWidget->item(i);
		if (user_id == item->data(Qt::UserRole).toString())
		{
			return item;
			break;
		}
	}
	return nullptr;
}
//事件重写
bool MainWidget::eventFilter(QObject* watched, QEvent* event)
{
	//弹出个人信息小窗口
	if (watched == ui->headLab && event->type() == QEvent::MouseButtonPress)
	{
		m_contactWidget = std::make_unique<ContactPage>();
		auto oneselfID = FriendManager::instance()->getOneselfID();
		m_contactWidget->setUser(oneselfID);
		auto position = ui->headLab->mapToGlobal(QPoint(0, 0));
		m_contactWidget->setGeometry(position.x(), position.y(), 0, 0);
		m_contactWidget->show();
		qDebug() << m_contactWidget->parent();
		return true;
	}
	else if (event->type() == QEvent::MouseButtonPress)
	{
		//隐藏个人信息小窗口
		if (m_contactWidget)
			m_contactWidget->hide();
	}
	//搜索栏
	if (watched == ui->searchEdit)
	{
		if (event->type() == QEvent::FocusIn)
		{
			ui->listStackedWidget->setCurrentWidget(m_friendSearchListWidget);
		}
		else if (event->type() == QEvent::FocusOut)
		{
			QWidget* newFocusWidget = QApplication::focusWidget(); // 获取新焦点的控件
			if (!m_friendSearchListWidget->isAncestorOf(newFocusWidget)) { // 只有新焦点不在 m_searchList 里才切换
				ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			}
		}
		else if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Escape) {
				ui->searchEdit->clearFocus();
				ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			}
		}

	}
	//好友搜索界面
	// 监听 m_searchList 和它的子控件
	QWidget* watchedWidget = qobject_cast<QWidget*>(watched);
	if (watchedWidget && (watchedWidget == m_friendSearchListWidget || m_friendSearchListWidget->isAncestorOf(watchedWidget))) {
		if (event->type() == QEvent::FocusOut) {
			QWidget* newFocusWidget = QApplication::focusWidget();
			if (newFocusWidget && !m_friendSearchListWidget->isAncestorOf(newFocusWidget)) {
				ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			}
		}
	}
	return false;
}





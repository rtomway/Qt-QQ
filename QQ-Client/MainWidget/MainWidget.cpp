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
#include "Client.h"
#include "AddFriendWidget.h"
#include "NoticeWidget.h"
#include "ContactList.h"
#include "Friend.h"
#include "FriendManager.h"

MainWidget::MainWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::MainWidget)
	, m_addpersonMenu(new QMenu(this))
	, m_moreMenu(new QMenu(this))
	, m_messageList(new QListWidget(this))
	, m_messagePage(new MessagePage(this))
	, m_contactPage(new ContactPage(this))
	, m_emptyWidget(new QWidget(this))
{
	ui->setupUi(this);
	init();
	initMoreMenu();
	this->setWindowFlag(Qt::FramelessWindowHint);
	ui->headLab->installEventFilter(this);
	this->installEventFilter(this);

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
	//个人信息头像加载
	connect(FriendManager::instance(), &FriendManager::UserAvatarLoaded, this, [=](const QPixmap& avatar)
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
	//点击用户消息项 进入会话界面（加载用户信息）
	connect(m_messageList, &QListWidget::itemClicked, this, [=](QListWidgetItem* item)
		{
			auto itemWidget = qobject_cast<MessageListItem*>(m_messageList->itemWidget(item));
			itemWidget->updateUnreadMessage();
			//已经处于当前用户会话界面并且是显示状态
			if (m_messagePage == ui->messageStackedWidget->currentWidget() && m_messagePage->getCurrentID() == itemWidget->getId())
				return;
			ui->messageStackedWidget->setCurrentWidget(m_messagePage);
			//清空界面
			m_messagePage->clearMessageWidget();
			//将当前用户信息以及聊天记录加载到会话界面
			m_messagePage->setCurrentUser(itemWidget->getUser());
		});
	//点击好友查看好友信息
	connect(ContactList::instance(), &ContactList::clickedFriend, this, [=](const QJsonObject& obj)
		{
			m_contactPage->setUser(obj);
			ui->messageStackedWidget->setCurrentWidget(m_contactPage);
			ui->rightWidget->setStyleSheet("background-color:white");
			qDebug() << m_contactPage->parent();

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
				auto Item = qobject_cast<MessageListItem*>(m_messageList->itemWidget(messageItem));
				Item->setUser(user->getFriend());
			}
		});
	//好友信息界面跳转到会话界面
	connect(m_contactPage, &ContactPage::sendMessage, this, [=](const QString& user_id)
		{
			m_btn_Itemgroup->button(-2)->setChecked(true);
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
			ui->listStackedWidget->setCurrentWidget(m_messageList);
			auto messageItem = findListItem(user_id);
			if (!messageItem) //判断消息项是否存在
			{
				auto user = FriendManager::instance()->findFriend(user_id);
				messageItem = addmessageListItem(user->getFriend());
			}
			m_messageList->setCurrentItem(messageItem);
			emit m_messageList->itemClicked(messageItem);

		});
	//点击通知 进入通知界面
	connect(ContactList::instance(), &ContactList::friendNotice, this, [=]
		{
			ui->messageStackedWidget->setCurrentWidget(NoticeWidget::instance());
		});
	connect(ContactList::instance(), &ContactList::groupNotice, this, [=]
		{
			ui->messageStackedWidget->setCurrentWidget(NoticeWidget::instance());
		});
	//接受到消息 用户消息项更新
	connect(Client::instance(), &Client::communication, this, [=](const QJsonObject& obj)
		{
			qDebug() << "接受消息json:"<<obj;
			auto m_user_id = FriendManager::instance()->getOneselfID();
			auto friend_id = obj["user_id"].toString();
			auto message = obj["message"].toString();
			auto item = findListItem(friend_id);
			//接收信息加入聊天记录
			m_messagePage->updateChatMessage(friend_id, m_user_id, message);
			if (item)
			{
				//有消息项 追加消息内容
				auto itemWidget = qobject_cast<MessageListItem*>(m_messageList->itemWidget(item));
				itemWidget->setUser(obj);
				//判断当下是否是该user_id会话界面
				if (m_messagePage->getCurrentID() == friend_id&&ui->messageStackedWidget->currentWidget() == m_messagePage)
				{
					m_messagePage->updateReciveMessage(message);
					itemWidget->updateUnreadMessage();
				}

			}
			else
			{
				//没有消息项创建一个
				auto item = addmessageListItem(obj);
				auto itemWidget = qobject_cast<MessageListItem*>(m_messageList->itemWidget(item));
				itemWidget->setUser(obj);
			}
		});
	//新增好友
	connect(FriendManager::instance(), &FriendManager::NewFriend, this, [=](const QString& user_id)
		{
			auto myfriend = FriendManager::instance()->findFriend(user_id);
			auto friendObj = myfriend->getFriend();
			m_messagePage->setUser(friendObj);
			addmessageListItem(friendObj);
		});
	//好友申请通知
	connect(Client::instance(), &Client::addFriend, this, [=]
		{
			if (ui->messageStackedWidget->currentWidget() != NoticeWidget::instance())
			{
				ContactList::instance()->updateFriendNoticeCount();
			}
		});
	//被拒通知
	connect(Client::instance(), &Client::rejectAddFriend, this, [=]
		{
			if (ui->messageStackedWidget->currentWidget() != NoticeWidget::instance())
			{
				ContactList::instance()->updateFriendNoticeCount();
			}
		});
}

MainWidget::~MainWidget()
{
	delete ui;
}

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
	//页面切换
	connect(m_btn_Itemgroup, &QButtonGroup::idClicked, this, [=](int id)
		{
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
			m_messageList->clearSelection();
			switch (id)
			{
			case -2:
				ui->listStackedWidget->setCurrentWidget(m_messageList);
				ui->messageStackedWidget->setCurrentWidget(m_emptyWidget);
				break;
			case -3:
				ui->listStackedWidget->setCurrentWidget(ContactList::instance());
				ui->messageStackedWidget->setCurrentWidget(m_emptyWidget);
				break;
			default:
				break;
			}
		});
	//好友列表与消息项
	ui->listStackedWidget->addWidget(m_messageList);
	ui->listStackedWidget->addWidget(ContactList::instance());
	ui->listStackedWidget->setCurrentWidget(m_messageList);
	m_messageList->setObjectName("MessageList");
	//会话,通知，信息界面
	ui->messageStackedWidget->addWidget(m_messagePage);
	ui->messageStackedWidget->addWidget(m_contactPage);
	ui->messageStackedWidget->addWidget(m_emptyWidget);
	ui->messageStackedWidget->addWidget(NoticeWidget::instance());
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
	//界面按钮
	m_btn_Itemgroup->button(-2)->setChecked(true);
	connect(m_btn_Itemgroup, &QButtonGroup::idPressed, [=](int id)
		{

		});
	//更多（菜单）
	connect(ui->otherBtn, &QPushButton::clicked, [=]
		{
			m_moreMenu->popup(mapToGlobal(QPoint(ui->otherBtn->geometry().x() + 30, ui->otherBtn->geometry().y() + 400)));
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
	//添加好友
	connect(addFriend, &QAction::triggered, [=]
		{
			qDebug() << "哈哈";
			QPointer<AddFriendWidget>addfriendWidget = new AddFriendWidget;
			addfriendWidget->show();
		});
}

void MainWidget::initMoreMenu()
{
	//账号退出清空数据
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/quit.png"), "退出账号", [=]
		{
			FriendManager::instance()->clearFriendManager();
			ContactList::instance()->clearContactList();
			m_btn_Itemgroup->button(-2)->setChecked(true);
			ui->listStackedWidget->setCurrentWidget(m_messageList);
			ui->messageStackedWidget->setCurrentWidget(m_emptyWidget);
			emit quitsuccess();
		});
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/about.png"), "关于", [=]
		{

		});
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/set.png"), "设置", [=]
		{

		});

}

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
	auto item= findListItem(user_id);
	if (item!=nullptr)
	{
		qDebug() << "消息项已存在" << user_id;
		return nullptr;
	}
	//为item设置用户id
	item = new QListWidgetItem(m_messageList);
	item->setData(Qt::UserRole, user_id);
	item->setSizeHint(QSize(m_messageList->width(), 70));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new MessageListItem(m_messageList);
	itemWidget->setUser(obj);
	//关联item和widget
	m_messageList->setItemWidget(item, itemWidget);
	return item;
}

//通过用户id找到用户消息项item
QListWidgetItem* MainWidget::findListItem(const QString& user_id)
{
	for (auto i = 0; i < m_messageList->count(); i++)
	{
		auto item = m_messageList->item(i);
		if (user_id == item->data(Qt::UserRole).toString())
		{
			qDebug() << "消息项？";
			return item;
			break;
		}
	}
	qDebug() << "消息项没有";
	return nullptr;
}

bool MainWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui->headLab && event->type() == QEvent::MouseButtonPress)
	{
		//弹出个人信息小窗口
		m_contactWidget = std::make_unique<ContactPage>();
		auto oneselfID= FriendManager::instance()->getOneselfID();
		auto oneself = FriendManager::instance()->findFriend(oneselfID);
		m_contactWidget->setUser(oneself->getFriend());
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
	return false;
}





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

#include "User.h"
#include "ImageUtil.h"
#include "SMaskWidget.h"
#include "Client.h"
#include "AddFriendWidget.h"
#include "NoticeWidget.h"
#include "ContactList.h"


MainWidget::MainWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	,ui(new Ui::MainWidget)
	, m_addpersonMenu(new QMenu(this))
	, m_moreMenu(new QMenu(this))
	,m_messageList(new QListWidget(this))
	,m_messagePage(new MessagePage(this))
	,m_contactPage(new ContactPage(this))
	,m_emptyWidget(new QWidget(this))
{
	ui->setupUi(this);
	init();
	initMoreMenu();
	this->setWindowFlag(Qt::FramelessWindowHint);
	
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
	
	SMaskWidget::instance()->setMainWidget(this);

	//接受到消息 用户消息项更新
	connect(Client::instance(), &Client::communication, this, [=](const QJsonObject& obj)
		{
			qDebug() << "接受消息";
			auto m_user_id = User::instance()->getUserId();
			auto friend_id = obj["user_id"].toString();
			auto message = obj["message"].toString();
			auto item= findListItem(friend_id);
			//接收信息加入聊天记录
			m_messagePage->updateChatMessage(friend_id, m_user_id, message);
			if (item)
			{
				//有消息项 追加消息内容
				auto itemWidget = qobject_cast<MessageListItem*>(m_messageList->itemWidget(item));
				itemWidget->setUser(obj);
				//判断当下是否是该user_id会话界面
				if (m_messagePage->getCurrentID() == friend_id)
				{
					m_messagePage->updateReciveMessage(message);
					itemWidget->updateUnreadMessage();
				}
				
			}
			else
			{
				//没有消息项创建一个
				auto item= addmessageListItem(obj);
				auto itemWidget = qobject_cast<MessageListItem*>(m_messageList->itemWidget(item));
				itemWidget->setUser(obj);
			}
		});
	//好友添加成功
	connect(Client::instance(), &Client::agreeAddFriend, this, [=](const QJsonObject& obj)
		{
			//新增好友聊天记录并添加消息项
			m_messagePage->setUser(obj);
			addmessageListItem(obj);
		});
	//同意好友添加
	connect(ContactList::instance(), &ContactList::agreeAddFriend, this, [=](const QJsonObject& obj)
		{
			//新增好友聊天记录并添加消息项
			m_messagePage->setUser(obj);
			addmessageListItem(obj);
		});
}

MainWidget::~MainWidget()
{
	delete ui;
}

void MainWidget::init()
{
	resize(1080,680);

	ui->listWidget->setFixedWidth(60);
	ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	ui->qqLab->setAlignment(Qt::AlignCenter);
	//个人信息头像
	ui->headLab->setPixmap(ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(40, 40)));
	//列表按钮组
	m_btn_Itemgroup = new QButtonGroup(this);
	
	//左上列表添加项
	additemCenter(QString(":/icon/Resource/Icon/message.png"));
	additemCenter(QString(":/icon/Resource/Icon/contact.png"));
	additemCenter(QString(":/icon/Resource/Icon/QQspace.png"));
	additemCenter(QString(":/icon/Resource/Icon/channel.png"));
	additemCenter(QString(":/icon/Resource/Icon/game.png"));
	additemCenter(QString(":/icon/Resource/Icon/shortview.png"));
	additemCenter(QString(":/icon/Resource/Icon/more_2.png"));
	ui->listWidget->setIconSize(QSize(50, 50));
	ui->addBtn->setIcon(QIcon(":/icon/Resource/Icon/add.png"));

	//页面切换
	connect(m_btn_Itemgroup, &QButtonGroup::idClicked,this, [=](int id)
		{
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
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

	//中下 listwidget
	ui->listStackedWidget->addWidget(m_messageList);
	ui->listStackedWidget->addWidget(ContactList::instance());
	ui->listStackedWidget->setCurrentWidget(m_messageList);
	m_messageList->setObjectName("MessageList");

	//右边界面
	ui->messageStackedWidget->addWidget(m_messagePage);
	ui->messageStackedWidget->addWidget(m_contactPage);
	ui->messageStackedWidget->addWidget(m_emptyWidget);
	ui->messageStackedWidget->addWidget(NoticeWidget::instance());
	//点击用户消息项 进入会话界面（加载用户信息）
	connect(m_messageList, &QListWidget::itemClicked,this,[=](QListWidgetItem*item)
		{
			auto itemWidget = qobject_cast<MessageListItem*>(m_messageList->itemWidget(item));
			itemWidget->updateUnreadMessage();
			//已经处于当前用户会话界面
			if (m_messagePage->getCurrentID() == itemWidget->getId())
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
			//查询用户各项信息
			QVariantMap queryUser;
			queryUser["user_id"] = User::instance()->getUserId();
			queryUser["query_id"] = obj["user_id"].toString();

			Client::instance()->sendMessage("queryUserDetail", queryUser)
				->ReciveMessage([=](const QString& message)
					{
						QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
						if (doc.isObject())
						{
							auto obj = doc.object();
							auto data = obj["data"].toObject();
							if (obj["code"].toInt() == 0)//查询成功 将用户信息加载至界面
							{
								m_contactPage->setUser(data);
							}
							else
							{
								qDebug() << obj["message"].toString();
							}
						}
					});
			ui->messageStackedWidget->setCurrentWidget(m_contactPage);
			ui->rightWidget->setStyleSheet("background-color:white");
			
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

	ui->hideBtn->setIcon(QIcon(":/icon/Resource/Icon/hide.png"));
	ui->expandBtn->setIcon(QIcon(":/icon/Resource/Icon/expand.png"));
	ui->exitBtn->setIcon(QIcon(":/icon/Resource/Icon/x.png"));
	

	//左上列表选项组
	m_btn_Itemgroup->button(-2)->setChecked(true);
	connect(m_btn_Itemgroup, &QButtonGroup::idPressed,[=](int id)
	{
		
	});

	//左下按钮组
	connect(ui->otherBtn, &QPushButton::clicked, [=]
		{
			m_moreMenu->popup(mapToGlobal(QPoint(ui->otherBtn->geometry().x()+30, ui->otherBtn->geometry().y()+400)));
		});
	
	//中上搜索添加
	auto creatGroup=m_addpersonMenu->addAction(QIcon(":/icon/Resource/Icon/createGroup.png"), "创建群聊");
	auto addFriend=m_addpersonMenu->addAction(QIcon(":/icon/Resource/Icon/addperson.png"), "加好友/群");
	//+号
	connect(ui->addBtn, &QPushButton::clicked, [=]
		{
			m_addpersonMenu->popup(mapToGlobal(QPoint(ui->addBtn->geometry().x()+70, ui->addBtn->geometry().y() + 30)));
		});
	
	//添加好友
	connect(addFriend, &QAction::triggered, [=]
		{
			qDebug() << "哈哈";
			QPointer<AddFriendWidget>addfriendWidget = new AddFriendWidget;
			addfriendWidget->show();
		});
	ui->searchEdit->setPlaceholderText("搜索");

	//右上按钮
	connect(ui->hideBtn, &QPushButton::clicked, this, &MainWidget::hideWidget);
	connect(ui->expandBtn, &QPushButton::clicked, this, &MainWidget::expandWidget);
	connect(ui->exitBtn, &QPushButton::clicked, this, &MainWidget::exitWidget);

	
}

void MainWidget::initMoreMenu()
{
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/quit.png"), "退出账号", [=]
		{
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
	//为item设置用户id
	auto item = new QListWidgetItem(m_messageList);
	auto user_id = obj["user_id"].toString();
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
			return item;
			break;
		}
	}
	return nullptr;
}

void MainWidget::resizeEvent(QResizeEvent* event)
{
	
}





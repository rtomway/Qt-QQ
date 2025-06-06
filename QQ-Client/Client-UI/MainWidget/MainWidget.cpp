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
#include "ChatRecordManager.h"
#include "EventBus.h"
#include "AvatarManager.h"
#include "GroupManager.h"
#include "FMessageItemWidget.h"
#include "GMessageItemWidget.h"
#include "MessageSender.h"
#include "MessageRecord.h"
#include "LoginUserManager.h"
#include "GroupCreateWidget.h"
#include "UserProfileDispatcher.h"


MainWidget::MainWidget(QWidget* parent)
	:AngleRoundedWidget(parent)
	, ui(new Ui::MainWidget)
	, m_addpersonMenu(new QMenu(this))
	, m_moreMenu(new QMenu(this))
	, m_chatWidget(new ChatWidget(this))
	, m_friendProfilePage(new FriendProfilePage(this))
	, m_groupProfilePage(new GroupProfilePage(this))
	, m_noticePage(new NoticeWidget(this))
	, m_emptyPage(new QWidget(this))
	, m_chatMessageListWidget(new QListWidget(this))
	, m_friendSearchListWidget(new SearchFriend(this))
	, m_contactListWidget(new ContactListWidget(this))
	, m_groupInviteWidget(new GroupCreateWidget(this))
{
	ui->setupUi(this);
	init();
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
	setMouseTracking(true);  // 主窗口自身
}

MainWidget::~MainWidget()
{
	delete ui;
}

//界面初始化
void MainWidget::init()
{
	//界面
	initMoreMenu();
	initStackedListWidget();
	initStackedPage();
	initLayout();
	//信号连接
	connectFriendManagerSignals();
	connectGroupManagerSignals();
	connectWindowControlSignals();
	//个人信息头像加载
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, this, [=]
		{
			m_loginUserId = LoginUserManager::instance()->getLoginUserID();
			AvatarManager::instance()->getAvatar(m_loginUserId, ChatType::User, [=](const QPixmap& pixmap)
				{
					auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(50, 50));
					ui->headLab->setPixmap(headPix);
				});
		});
	//接受到消息 用户消息项更新
	connect(EventBus::instance(), &EventBus::textCommunication, this, [=](const QJsonObject& obj)
		{
			auto friend_id = obj["user_id"].toString();
			auto message = obj["message"].toString();
			auto time = obj["time"].toString();
			//接收信息加入聊天记录
			ChatMessage chatMessage;
			chatMessage.sendId = friend_id;
			chatMessage.receiveId = m_loginUserId;
			chatMessage.data = QVariant::fromValue(message);
			chatMessage.time = QDateTime::fromString(time, "MM-dd hh:mm");
			chatMessage.messageType = MessageType::Text;
			chatMessage.chatType = ChatType::User;
			ChatRecordManager::instance()->addMessageToChat(chatMessage);
			auto key = itemKey(friend_id, ChatType::User);
			auto item = findListItem(key);
			if (!item)
			{
				//没有消息项创建一个
				item = addmessageListItem(friend_id, ChatType::User);
				auto newItemWidget = qobject_cast<FMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				newItemWidget->updateUnReadMessage(message, time);
				newItemWidget->setItemWidget(friend_id);
				return;
			}
			//有消息项 追加消息内容
			auto itemWidget = qobject_cast<FMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
			itemWidget->updateUnReadMessage(message, time);
			itemWidget->setItemWidget(friend_id);
			//更新会话界面消息内容
			if (m_chatWidget->isCurrentChat(ChatType::User, friend_id))
			{
				m_chatWidget->updateReceiveMessage(chatMessage);
				//判断当下是否是会话界面
				if (ui->messageStackedWidget->currentWidget() == m_chatWidget)
				{
					itemWidget->clearUnRead();
				}
			}
		});
	connect(EventBus::instance(), &EventBus::pictureCommunication, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto friend_id = obj["user_id"].toString();
			auto message = obj["message"].toString();
			auto time = obj["time"].toString();
			//接收信息加入聊天记录(自己无需添加)
			ChatMessage chatMessage;
			chatMessage.sendId = friend_id;
			chatMessage.receiveId = m_loginUserId;
			chatMessage.data = QVariant::fromValue(pixmap);
			chatMessage.time = QDateTime::fromString(time, "MM-dd hh:mm");
			chatMessage.messageType = MessageType::Image;
			chatMessage.chatType = ChatType::User;
			ChatRecordManager::instance()->addMessageToChat(chatMessage);
			//消息项处理
			auto key = itemKey(friend_id, ChatType::User);
			auto item = findListItem(key);
			if (!item)
			{
				//没有消息项创建一个
				item = addmessageListItem(friend_id, ChatType::User);
				auto newItemWidget = qobject_cast<FMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				newItemWidget->updateUnReadMessage(message, time);
				newItemWidget->setItemWidget(friend_id);
				return;
			}
			//有消息项 追加消息内容
			auto itemWidget = qobject_cast<FMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
			itemWidget->updateUnReadMessage(message, time);
			itemWidget->setItemWidget(friend_id);
			//更新会话界面消息内容
			if (m_chatWidget->isCurrentChat(ChatType::User, friend_id))
			{
				m_chatWidget->updateReceiveMessage(chatMessage);
				//判断当下是否是会话界面
				if (ui->messageStackedWidget->currentWidget() == m_chatWidget)
				{
					itemWidget->clearUnRead();
				}
			}
		});
	connect(GroupManager::instance(), &GroupManager::groupTextCommunication, this, [=](const QJsonObject& obj)
		{
			auto group_id = obj["group_id"].toString();
			auto user_id = obj["user_id"].toString();
			auto message = obj["message"].toString();
			auto time = obj["time"].toString();
			//更新聊天记录
			ChatMessage chatMessage;
			chatMessage.sendId = user_id;
			chatMessage.receiveId = group_id;
			chatMessage.data = QVariant::fromValue(message);
			chatMessage.time = QDateTime::fromString(time, "MM-dd hh:mm");
			chatMessage.messageType = MessageType::Text;
			chatMessage.chatType = ChatType::Group;
			ChatRecordManager::instance()->addMessageToChat(chatMessage);
			//消息项处理
			auto key = itemKey(group_id, ChatType::Group);
			auto item = findListItem(key);
			if (!item)
			{
				//没有消息项创建一个
				item = addmessageListItem(group_id, ChatType::Group);
				auto newItemWidget = qobject_cast<GMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				newItemWidget->updateUnReadMessage(user_id, message, time);
				newItemWidget->setItemWidget(group_id);
				return;
			}
			//有消息项 追加消息内容
			auto itemWidget = qobject_cast<GMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
			itemWidget->updateUnReadMessage(user_id, message, time);
			itemWidget->setItemWidget(group_id);
			//更新会话界面消息内容
			if (m_chatWidget->isCurrentChat(ChatType::Group, group_id))
			{
				m_chatWidget->updateReceiveMessage(chatMessage);
				//判断当下是否是会话界面
				if (ui->messageStackedWidget->currentWidget() == m_chatWidget)
				{
					itemWidget->clearUnRead();
				}
			}
		});
	connect(GroupManager::instance(), &GroupManager::groupPictureCommunication, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto send_id = obj["user_id"].toString();
			auto group_id = obj["group_id"].toString();
			auto message = obj["message"].toString();
			auto time = obj["time"].toString();
			//接收信息加入聊天记录(自己无需添加)
			ChatMessage chatMessage;
			chatMessage.sendId = send_id;
			chatMessage.receiveId = group_id;
			chatMessage.data = QVariant::fromValue(pixmap);
			chatMessage.time = QDateTime::fromString(time, "MM-dd hh:mm");
			chatMessage.messageType = MessageType::Image;
			chatMessage.chatType = ChatType::Group;
			ChatRecordManager::instance()->addMessageToChat(chatMessage);
			//消息项处理
			auto key = itemKey(group_id, ChatType::Group);
			auto item = findListItem(key);
			if (!item)
			{
				//没有消息项创建一个
				item = addmessageListItem(group_id, ChatType::Group);
				auto newItemWidget = qobject_cast<GMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				newItemWidget->updateUnReadMessage(send_id, message, time);
				newItemWidget->setItemWidget(group_id);
				return;
			}
			//有消息项 追加消息内容
			auto itemWidget = qobject_cast<GMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
			itemWidget->updateUnReadMessage(send_id, message, time);
			itemWidget->setItemWidget(group_id);
			//更新会话界面消息内容
			if (m_chatWidget->isCurrentChat(ChatType::Group, group_id))
			{
				m_chatWidget->updateReceiveMessage(chatMessage);
				//判断当下是否是会话界面
				if (ui->messageStackedWidget->currentWidget() == m_chatWidget)
				{
					itemWidget->clearUnRead();
				}
			}
		});
}

//界面布局
void MainWidget::initLayout()
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
			m_groupInviteWidget->loadData();
			//蒙层
			SMaskWidget::instance()->popUp(m_groupInviteWidget);
			auto mainWidgetSize = SMaskWidget::instance()->getMainWidgetSize();
			int x = (mainWidgetSize.width() - m_groupInviteWidget->width()) / 2;
			int y = (mainWidgetSize.height() - m_groupInviteWidget->height()) / 2;
			SMaskWidget::instance()->setPopGeometry(QRect(x, y, m_groupInviteWidget->width(), m_groupInviteWidget->height()));
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
	ui->messageStackedWidget->addWidget(m_chatWidget);
	ui->messageStackedWidget->addWidget(m_friendProfilePage);
	ui->messageStackedWidget->addWidget(m_groupProfilePage);
	ui->messageStackedWidget->addWidget(m_noticePage);
	ui->messageStackedWidget->addWidget(m_emptyPage);
}

//更多（菜单）
void MainWidget::initMoreMenu()
{
	//账号退出清空数据
	m_moreMenu->addAction(QIcon(":/icon/Resource/Icon/quit.png"), "退出账号", [=]
		{
			//数据管理
			FriendManager::instance()->clearFriendManager();
			GroupManager::instance()->clearGroupManager();
			ChatRecordManager::instance()->clearAllChats();
			//界面重置
			m_btn_Itemgroup->button(-2)->setChecked(true);
			ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			ui->messageStackedWidget->setCurrentWidget(m_emptyPage);
			//界面清空
			clearChatMessageListWidget();
			m_friendSearchListWidget->clearFriendList();
			m_contactListWidget->clearContactList();
			m_chatWidget->clearChatWidget();
			m_friendProfilePage->clearWidget();
			m_groupProfilePage->clearWidget();
			m_noticePage->clearNoticeWidget();

			// 关闭并销毁好友资料页
			if (m_friendProfileWidget) {
				m_friendProfileWidget->close(); // 可选：先关闭窗口
				m_friendProfileWidget.reset();  // 释放内存
			}
			// 关闭并销毁添加好友页
			if (m_addFriendWidget) {
				m_addFriendWidget->close();
				m_addFriendWidget.reset();
			}


			emit quitSuccess();
			MessageSender::instance()->disConnect();
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

//连接好友中心信号
void MainWidget::connectFriendManagerSignals()
{
	//好友信息更新,消息项相关信息更新
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, [=](const QString& user_id)
		{
			auto key = itemKey(user_id, ChatType::User);
			auto messageItem = findListItem(key);
			if (messageItem)
			{
				auto itemWidget = qobject_cast<ItemWidget*>(m_chatMessageListWidget->itemWidget(messageItem));
				itemWidget->setItemWidget(user_id);
			}
		});
	//好友头像更新,消息项相关信息更新
	connect(AvatarManager::instance(), &AvatarManager::UpdateUserAvatar, [=](const QString& user_id)
		{
			auto key = itemKey(user_id, ChatType::User);
			auto messageItem = findListItem(key);
			if (messageItem)
			{
				qDebug() << "头像信息更新";
				auto user = FriendManager::instance()->findFriend(user_id);
				auto itemWidget = qobject_cast<FMessageItemWidget*>(m_chatMessageListWidget->itemWidget(messageItem));
				itemWidget->setItemWidget(user_id);
			}
		});
	//新增好友
	connect(FriendManager::instance(), &FriendManager::NewFriend, this, [=](const QString& user_id)
		{
			ChatRecordManager::instance()->addUserChat(user_id, std::make_shared<ChatRecordMessage>(m_loginUserId, user_id, ChatType::User));
			addmessageListItem(user_id, ChatType::User);
		});
	//删除好友
	connect(FriendManager::instance(), &FriendManager::deleteFriend, this, [=](const QString& user_id)
		{
			auto key = itemKey(user_id, ChatType::User);
			auto item = findListItem(key);
			m_chatMessageListWidget->takeItem(m_chatMessageListWidget->row(item));
			if (m_chatWidget->isStackedCurrentChat(ChatType::User, user_id))
			{
				ui->messageStackedWidget->setCurrentWidget(m_emptyPage);
			}
			m_friendProfilePage->clearWidget();
		});
}

//连接群组中心信号
void MainWidget::connectGroupManagerSignals()
{
	//头像更新
	connect(AvatarManager::instance(), &AvatarManager::UpdateUserAvatar, this, [=](const QString& user_id)
		{
			if (user_id != m_loginUserId)
				return;
			AvatarManager::instance()->getAvatar(user_id, ChatType::User, [=](const QPixmap& pixmap)
				{
					auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
					ui->headLab->setPixmap(headPix);
				});
		});
	//创建群组
	connect(GroupManager::instance(), &GroupManager::createGroupSuccess, this, [=](const QString& group_id)
		{
			ChatRecordManager::instance()->addGroupChat(group_id, std::make_shared<ChatRecordMessage>(m_loginUserId, group_id, ChatType::Group));
			addmessageListItem(group_id, ChatType::Group);
		});
	//新加群组
	connect(GroupManager::instance(), &GroupManager::newGroup, this, [=](const QString& group_id)
		{
			ChatRecordManager::instance()->addGroupChat(group_id, std::make_shared<ChatRecordMessage>(m_loginUserId, group_id, ChatType::Group));
			addmessageListItem(group_id, ChatType::Group);
		});
	//新增群成员
	connect(GroupManager::instance(), &GroupManager::newGroupMember, this, [=](const QString& group_id, const QString& user_id)
		{
			auto group = GroupManager::instance()->findGroup(group_id);
			auto member = group->getMember(user_id);
			auto& user_name = member.member_name;
			//更新聊天记录
			ChatMessage chatMessage;
			chatMessage.sendId = user_id;
			chatMessage.receiveId = group_id;
			auto message = user_name + "加入群聊";
			auto time = QDateTime::currentDateTime();
			chatMessage.data = QVariant::fromValue(message);
			chatMessage.time = time;
			chatMessage.messageType = MessageType::System;
			chatMessage.chatType = ChatType::Group;
			ChatRecordManager::instance()->addMessageToChat(chatMessage);
			auto key = itemKey(group_id, ChatType::Group);
			auto item = findListItem(key);
			if (item)
			{
				auto itemWidget = qobject_cast<GMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				//有消息项 追加消息内容
				itemWidget->updateUnReadMessage(user_id, message, time.toString("MM-dd hh:mm"));
				itemWidget->setItemWidget(group_id);
				//判断当下是否是该user_id会话界面
				if (ui->messageStackedWidget->currentWidget() == m_chatWidget && m_chatWidget->isCurrentChat(ChatType::Group, group_id))
				{
					m_chatWidget->loadChatPage(ChatType::Group, group_id);
					m_chatWidget->updateReceiveMessage(chatMessage);
					itemWidget->clearUnRead();
				}
			}
			else
			{
				//没有消息项创建一个
				item = addmessageListItem(group_id, ChatType::Group);
				auto newItemWidget = qobject_cast<GMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				newItemWidget->updateUnReadMessage(user_id, message, time.toString("MM-dd hh:mm"));
				newItemWidget->setItemWidget(group_id);
			}
		});
	//退出群组
	connect(GroupManager::instance(), &GroupManager::exitGroup, this, [=](const QString& group_id, const QString& user_id)
		{
			auto key = itemKey(group_id, ChatType::Group);
			auto item = findListItem(key);
			m_chatMessageListWidget->takeItem(m_chatMessageListWidget->row(item));
			ui->messageStackedWidget->setCurrentWidget(m_emptyPage);
			m_chatWidget->clearChatWidget();
			//m_groupProfilePage->clearWidget();
		});
}

//连接成员对象控件等信号
void MainWidget::connectWindowControlSignals()
{
	//点击通知 进入通知界面
	connect(m_contactListWidget, &ContactListWidget::friendNotice, this, [=]
		{
			ui->messageStackedWidget->setCurrentWidget(m_noticePage);
			m_noticePage->setCurrentWidget(NoticeWidget::FriendNoticeWidget);
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
		});
	connect(m_contactListWidget, &ContactListWidget::groupNotice, this, [=]
		{
			ui->messageStackedWidget->setCurrentWidget(m_noticePage);
			m_noticePage->setCurrentWidget(NoticeWidget::GroupNoticeWidget);
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
		});
	//新增通知
	connect(m_noticePage, &NoticeWidget::friendNotice, this, [=]
		{
			if (ui->messageStackedWidget->currentWidget() != m_noticePage)
			{
				m_contactListWidget->updateFriendNoticeCount();
			}
		});
	connect(m_noticePage, &NoticeWidget::groupNotice, this, [=]
		{
			if (ui->messageStackedWidget->currentWidget() != m_noticePage)
			{
				m_contactListWidget->updateGroupNoticeCount();
			}
		});
	//点击好友查看好友信息
	connect(m_contactListWidget, &ContactListWidget::clickedFriend, this, [=](const QString& user_id)
		{
			m_friendProfilePage->setFriendProfile(user_id);
			ui->messageStackedWidget->setCurrentWidget(m_friendProfilePage);
			ui->rightWidget->setStyleSheet("background-color:white");
		});
	connect(m_contactListWidget, &ContactListWidget::clickedGroup, this, [=](const QString& group_id)
		{
			m_groupProfilePage->setGroupProfile(group_id);
			ui->messageStackedWidget->setCurrentWidget(m_groupProfilePage);
			ui->rightWidget->setStyleSheet("background-color:white");
		});
	//分组更新
	connect(m_contactListWidget, &ContactListWidget::updateFriendgrouping, m_friendProfilePage, &FriendProfilePage::updateFriendgrouping);
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
				auto& obj = it.value()->getFriend();
				AvatarManager::instance()->getAvatar(it.value()->getFriendId(), ChatType::User, [=](const QPixmap& pixmap)
					{
						auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
						m_friendSearchListWidget->addSearchItem(obj, headPix);
					});
			}
		});
	//点击消息项 进入会话界面（加载用户信息）
	connect(m_chatMessageListWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem* item)
		{
			auto id = item->data(Qt::UserRole).toString().mid(1);
			ChatType type = static_cast<ChatType>(item->data(Qt::UserRole + 1).toInt());
			//已经处于当前用户会话界面并且是显示状态
			if (m_chatWidget == ui->messageStackedWidget->currentWidget() && m_chatWidget->isStackedCurrentChat(type, id))
				return;
			qDebug() << "进入会话界面（加载用户信息）";
			//清除消息项未读
			if (type == ChatType::User)
			{
				auto fItemWidget = qobject_cast<FMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				fItemWidget->clearUnRead();
			}
			else if (type == ChatType::Group)
			{
				auto gItemWidget = qobject_cast<GMessageItemWidget*>(m_chatMessageListWidget->itemWidget(item));
				gItemWidget->clearUnRead();
			}
			//会话界面
			ui->messageStackedWidget->setCurrentWidget(m_chatWidget);
			//将当前用户信息以及聊天记录加载到会话界面
			m_chatWidget->loadChatPage(type, id);
		});
	//跳转会话界面
	connect(FriendManager::instance(), &FriendManager::chatWithFriend, this, [=](const QString& user_id)
		{
			m_btn_Itemgroup->button(-2)->setChecked(true);
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
			ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			auto key = itemKey(user_id, ChatType::User);
			auto messageItem = findListItem(key);
			if (!messageItem) //判断消息项是否存在
			{
				auto user = FriendManager::instance()->findFriend(user_id);
				messageItem = addmessageListItem(user_id, ChatType::User);
			}
			m_chatMessageListWidget->setCurrentItem(messageItem);
			emit m_chatMessageListWidget->itemClicked(messageItem);
		});
	connect(GroupManager::instance(), &GroupManager::chatWithGroup, this, [=](const QString& group_id)
		{
			m_btn_Itemgroup->button(-2)->setChecked(true);
			ui->rightWidget->setStyleSheet("background-color:rgb(240,240,240)");
			ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
			auto key = itemKey(group_id, ChatType::Group);
			auto messageItem = findListItem(key);
			if (!messageItem) //判断消息项是否存在
			{
				auto group = GroupManager::instance()->findGroup(group_id);
				messageItem = addmessageListItem(group_id, ChatType::Group);
			}
			m_chatMessageListWidget->setCurrentItem(messageItem);
			emit m_chatMessageListWidget->itemClicked(messageItem);
		});
	//刷新会话界面
	connect(EventBus::instance(), &EventBus::updateChatWidget, this, [=](ChatType type, const QString& group_id)
		{
			m_chatWidget->loadChatPage(type, group_id);
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
QListWidgetItem* MainWidget::addmessageListItem(const QString& id, ChatType type)
{
	auto key = itemKey(id, type);
	auto item = findListItem(key);
	if (item != nullptr)
	{
		qDebug() << "消息项已存在" << id;
		return nullptr;
	}
	//为item设置用户id
	item = new QListWidgetItem(m_chatMessageListWidget);
	item->setSizeHint(QSize(m_chatMessageListWidget->width(), 70));
	ItemWidget* itemWidget = nullptr;
	if (type == ChatType::User)
	{
		itemWidget = new FMessageItemWidget();
	}
	else if (type == ChatType::Group)
	{
		itemWidget = new GMessageItemWidget(m_chatMessageListWidget);
	}
	item->setData(Qt::UserRole, key);
	item->setData(Qt::UserRole + 1, static_cast<int>(type));
	itemWidget->setItemWidget(id);
	//关联item和widget
	m_chatMessageListWidget->setItemWidget(item, itemWidget);
	return item;
}

//itemKey
QString MainWidget::itemKey(const QString& id, ChatType type)
{
	auto key_pre = static_cast<int>(type);
	auto key = QString::number(key_pre) + id;
	return key;
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
		}
	}
	return nullptr;
}

// 清空列表
void MainWidget::clearChatMessageListWidget()
{
	m_chatMessageListWidget->clear();
}

//列表界面恢复
void MainWidget::updateStackedListWidget()
{
	switch (m_btn_Itemgroup->checkedId())
	{
	case -2:
		ui->listStackedWidget->setCurrentWidget(m_chatMessageListWidget);
		break;
	case -3:
		ui->listStackedWidget->setCurrentWidget(m_contactListWidget);
		break;
	default:
		break;
	}
}

//事件重写
bool MainWidget::eventFilter(QObject* watched, QEvent* event)
{
	//弹出个人信息小窗口
	if (watched == ui->headLab && event->type() == QEvent::MouseButtonPress)
	{
		auto position = ui->headLab->mapToGlobal(QPoint(ui->headLab->width(), 0));
		UserProfileDispatcher::instance()->showUserProfile(m_loginUserId, position, PopUpPosition::Right);
		return true;
	}
	//搜索栏
	if (watched == ui->searchEdit)
	{
		if (event->type() == QEvent::FocusIn)
		{
			ui->listStackedWidget->setCurrentWidget(m_friendSearchListWidget);
		}

		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Escape) {
				ui->searchEdit->clearFocus();
				updateStackedListWidget();
			}
		}
		if (event->type() == QEvent::FocusOut)
		{
			QWidget* newFocusWidget = QApplication::focusWidget(); // 获取新焦点的控件
			if (!m_friendSearchListWidget->isAncestorOf(newFocusWidget))
			{		// 只有新焦点不在 m_searchList 里才切换
				updateStackedListWidget();
			}
		}
	}
	// 监听 m_searchList 和它的子控件
	QWidget* watchedWidget = qobject_cast<QWidget*>(watched);
	if (watchedWidget && (watchedWidget == m_friendSearchListWidget || m_friendSearchListWidget->isAncestorOf(watchedWidget)))
	{
		if (event->type() == QEvent::FocusOut)
		{
			QWidget* newFocusWidget = QApplication::focusWidget();
			if (newFocusWidget && !m_friendSearchListWidget->isAncestorOf(newFocusWidget))
			{
				updateStackedListWidget();
			}
		}
	}
	return false;
}





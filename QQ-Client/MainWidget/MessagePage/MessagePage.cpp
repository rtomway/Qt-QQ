#include "MessagePage.h"
#pragma once
#include "MessagePage.h"
#include "ui_MessagePage.h"
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <QJsonObject>
#include <QJsonArray>
#include <QBoxLayout>
#include <QTimer>
#include <QMouseEvent>

#include "ImageUtil.h"
#include "Client.h"
#include "MessageListItem.h"
#include "MessageBubble.h"
#include "SConfigFile.h"
#include "FriendManager.h"
#include "Friend.h"
#include "FriendSetWidget.h"

MessagePage::MessagePage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::MessagePage)
	, m_setWidget(new FriendSetWidget(this))
	, m_animation(new QPropertyAnimation(m_setWidget, "geometry"))
	, m_opacityAnimation(new QPropertyAnimation(m_setWidget, "windowOpacity"))
{
	ui->setupUi(this);
	init();
	this->setObjectName("MessagePage");
	QFile file(":/stylesheet/Resource/StyleSheet/MessagePage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
	this->installEventFilter(this);
	installEventFilterForChildren(this);
	m_setWidget->setFixedWidth(250);
	m_setWidget->setFocusPolicy(Qt::StrongFocus);
	// 移除默认标志并添加无边框标志
	m_setWidget->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);  // 只设置无边框和工具窗口标志
	// 设置初始的裁剪区域，确保初始时不可见
	QRegion region(0, 0, 0, height(), QRegion::Rectangle);
	m_setWidget->setMask(region);  // 裁剪区域
}

MessagePage::~MessagePage()
{
	delete ui;
}

void MessagePage::init()
{
	m_friend_headPix = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(100, 100), 66);
	//设置输入字体大小
	QFont font = ui->messageTextEdit->font();
	font.setPointSize(13);
	ui->messageTextEdit->setFont(font);
	//在自己信息中心加载完成后读取
	connect(FriendManager::instance(), &FriendManager::FriendManagerLoadSuccess, this, [=]
		{
			auto user_id = FriendManager::instance()->getOneselfID();
			m_oneself = FriendManager::instance()->findFriend(user_id);
		});
	//用户信息更新后
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			auto user = FriendManager::instance()->findFriend(user_id);
			auto pixmap = ImageUtils::roundedPixmap(user->getAvatar(), QSize(100, 100), 66);
			if (user_id == m_oneself->getFriendId())
			{
				qDebug() << "自己的头像更新";
				m_oneself = user;
			}
			//刷新界面
			setCurrentUser(user->getFriend());
		});
	//发送文字消息
	connect(ui->sendBtn, &QPushButton::clicked, [=]
		{
			auto pixmap = ImageUtils::roundedPixmap(m_oneself->getAvatar(), QSize(100, 100), 66);
			QString msg = ui->messageTextEdit->toPlainText();
			if (msg.isEmpty())
				return;
			//消息显示至聊天框
			MessageBubble* messageBubble = new MessageBubble(pixmap, msg, MessageBubble::BubbleRight);
			ui->messageListWidget->addItem(messageBubble);
			ui->messageListWidget->setItemWidget(messageBubble, messageBubble);
			//将消息加入至聊天记录中
			auto user_id = FriendManager::instance()->getOneselfID();
			updateChatMessage(user_id, m_currentID, msg);
			//下拉至底部并清空输入
			ui->messageListWidget->scrollToBottom();
			ui->messageTextEdit->clear();
			//发送给服务器通过服务器转发
			QJsonObject json;
			json["user_id"] = user_id;
			json["message"] = msg;
			json["to"] = m_friend_id;
			auto config = new SConfigFile("config.ini");
			if (user_id == m_friend_id)
			{
				Client::instance()->emit communication(json);
				return;
			}
			QVariantMap messageMap = json.toVariantMap();
			/*messageMap["message"] = msg;
			messageMap["user_id"] = FriendManager::instance()->getOneselfID();
			messageMap["to"] = m_friend_id;*/
			qDebug() << "当前用户id" << config->value("user_id");
			Client::instance()->sendMessage("communication", messageMap);
		});
	ui->messageListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//设置面板
	connect(ui->moreBtn, &QPushButton::clicked, this, [=]
		{
			m_setWidget->setId(m_friend_id);
			qDebug() << "设置面板";
			if (m_setWidget->isVisible()) {
				qDebug() << "aaakkk";
				m_setWidget->hide();  // 隐藏停靠面板
				return;
			}
		auto setHeight = this->height() - ui->setWidget->height();
		qDebug() << "setHeight" << setHeight << this->height();

		// 获取窗口的全局坐标和右边缘
		QPoint windowTopLeft = this->mapToGlobal(QPoint(0, 0)); // 获取窗口的左上角的全局坐标
		int windowRight = windowTopLeft.x() + this->width(); // 获取窗口的右边缘的全局坐标

		qDebug() << "Window Right (Global):" << windowRight;

		// 动画设置
		m_animation->setEasingCurve(QEasingCurve::Linear);
		m_animation->setDuration(300);  // 动画持续时间

		// 设置动画的起始和结束位置（全局坐标系）
		m_animation->setStartValue(QRect(windowRight, windowTopLeft.y() + ui->setWidget->height(), 0, setHeight));  // 从外部开始
		m_animation->setEndValue(QRect(windowRight - m_setWidget->width(), windowTopLeft.y() + ui->setWidget->height(), m_setWidget->width(), setHeight));  // 结束时的位置

		// 动画过程中更新裁剪区域
		connect(m_animation, &QPropertyAnimation::valueChanged, [=](const QVariant& value) {
			QRect rect = value.toRect();  // 获取动画值
			int x = rect.x();  // 获取当前x坐标
			int visibleWidth = this->width() - x;  // 计算当前可见宽度

			// 使用局部坐标更新裁剪区域
			QPoint localPos = this->mapFromGlobal(rect.topLeft());  // 将全局坐标转换为局部坐标
			int visibleWidthAdjusted = this->width() - localPos.x();  // 调整宽度

			// 设置新的裁剪区域，确保滑出时的部分不显示
			QRegion region(0, 0, visibleWidthAdjusted, height(), QRegion::Rectangle);
			m_setWidget->setMask(region);  // 更新裁剪区域
			});
		m_setWidget->show();
		m_setWidget->activateWindow();
		m_setWidget->setFocus();
		
		// 延迟执行动画，确保 isVisible() 状态稳定
		QTimer::singleShot(10, this, [this]() {
			m_animation->start();
			});
		});
}

//进入会话 加载当前会话信息
void MessagePage::setCurrentUser(const QJsonObject& obj)
{
	m_friend_username = obj["username"].toString();
	m_friend_id = obj["user_id"].toString();
	m_currentID = m_friend_id;
	auto pixmap = FriendManager::instance()->findFriend(m_currentID)->getAvatar();
	m_friend_headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100), 66);

	auto chatMessage = m_chats[m_friend_id];
	//加载聊天记录
	loadChatMessage(chatMessage);

	//信息更新到界面
	updateMessageWidget();

}
//新增好友聊天
void MessagePage::setUser(const QJsonObject& obj)
{
	m_friend_id = obj["user_id"].toString();
	m_chats.insert(m_friend_id, ChatMessage(FriendManager::instance()->getOneselfID(), m_friend_id));
}
//获取当前会话id
QString MessagePage::getCurrentID()
{
	return m_currentID;
}
//更新会话界面信息
void MessagePage::updateMessageWidget()
{
	ui->nameLab->setText(m_friend_username);

}
//加载聊天记录至聊天框
void MessagePage::loadChatMessage(const ChatMessage& chatMessage)
{
	clearMessageWidget();
	QList<Message> messages = chatMessage.getMessage();
	for (auto message : messages)
	{
		message.readMessage();
		//判断消息是谁发送
		if (message.getSenderId() == FriendManager::instance()->getOneselfID())
		{
			//自己发的消息
			auto content = message.getMessage();
			QPixmap pixmap = ImageUtils::roundedPixmap(m_oneself->getAvatar(), QSize(100, 100), 66);
			MessageBubble* messageBubble = new MessageBubble(pixmap, content, MessageBubble::BubbleRight);
			ui->messageListWidget->addItem(messageBubble);
			ui->messageListWidget->setItemWidget(messageBubble, messageBubble);
		}
		else
		{
			//好友发的消息
			auto content = message.getMessage();
			auto user = FriendManager::instance()->findFriend(m_friend_id);
			QPixmap pixmap = ImageUtils::roundedPixmap(user->getAvatar(), QSize(100, 100), 66);
			MessageBubble* messageBubble = new MessageBubble(pixmap, content, MessageBubble::BubbleLeft);
			ui->messageListWidget->addItem(messageBubble);
			ui->messageListWidget->setItemWidget(messageBubble, messageBubble);
		}
	}

}
//处于会话界面 接收消息直接更新
void MessagePage::updateReciveMessage(const QString& Recivemessage)
{
	MessageBubble* message = new MessageBubble(m_friend_headPix, Recivemessage, MessageBubble::BubbleLeft);
	ui->messageListWidget->addItem(message);
	ui->messageListWidget->setItemWidget(message, message);
	ui->messageListWidget->scrollToBottom();
}
//更新聊天记录
void MessagePage::updateChatMessage(const QString& sender_id, const QString& receiver_id, const QString& msg)
{
	Message message(sender_id, receiver_id, msg, QDateTime::currentDateTime());
	message.readMessage();
	if (sender_id == FriendManager::instance()->getOneselfID())
	{
		m_chats[receiver_id].addMessage(message);
		message.readMessage();
	}
	else
	{
		m_chats[sender_id].addMessage(message);
	}

}
//清空消息
void MessagePage::clearMessageWidget()
{
	ui->messageListWidget->clear();
}
//清空聊天记录
void MessagePage::clearChatMessages()
{
	m_chats.clear();
}
//事件重写
bool MessagePage::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint globalPos = mouseEvent->globalPos();
		qDebug() << globalPos;
		// 检查点击位置是否在 m_setWidget 内
		if (!m_setWidget->geometry().contains(m_setWidget->mapFromGlobal(globalPos))) {
			qDebug() << "点击位置";
			// 检查点击位置是否在 moreBtn 内
			if (!this->ui->moreBtn->geometry().contains(this->ui->moreBtn->mapFromGlobal(globalPos))) {
				// 如果点击位置既不在 m_setWidget 也不在 moreBtn 内，则隐藏 m_setWidget
				m_setWidget->hide();
			}
		}
	}
	return QWidget::eventFilter(watched, event);
}
void MessagePage::resizeEvent(QResizeEvent* ev)
{
	// 获取窗口的全局坐标和右边缘
	QPoint windowTopLeft = this->mapToGlobal(QPoint(0, 0)); // 获取窗口的左上角的全局坐标
	int windowRight = windowTopLeft.x() + this->width(); // 获取窗口的右边缘的全局坐标
	qDebug() << "resize";
	m_setWidget->setGeometry(windowRight-m_setWidget->width(), ui->setWidget->height(), 250, this->height() - ui->setWidget->height());
	qDebug() << m_setWidget->height();
}
void MessagePage::installEventFilterForChildren(QWidget* parent)
{
	// 遍历 parent 的所有子控件
	for (QObject* child : parent->children()) {
		QWidget* childWidget = qobject_cast<QWidget*>(child);
		if (childWidget) {
			// 为子控件安装事件过滤器
			if (childWidget == ui->moreBtn || childWidget == m_setWidget)
			{
				qDebug() << "childWidget";
				continue;
			}
			childWidget->installEventFilter(this);
			// 如果子控件还有子控件，递归安装事件过滤器
			installEventFilterForChildren(childWidget);
		}
	}
}






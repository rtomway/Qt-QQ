#pragma once
#include "MessagePage.h"
#include "ui_MessagePage.h"
#include <QFile>
#include "MessageListItem.h"
#include "MessageBubble.h"
#include <QPainter>
#include <QPainterPath>
#include "ImageUtil.h"
#include "Client.h"
#include <QJsonObject>
#include <QJsonArray>
#include"SConfigFile.h"
#include "User.h"
#include "FriendManager.h"
#include "Friend.h"

MessagePage::MessagePage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::MessagePage)
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
	connect(FriendManager::instance(), &FriendManager::UserAvatarLoaded, this, [=]
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
			updateChatMessage(User::instance()->getUserId(), m_currentID, msg);
			//下拉至底部并清空输入
			ui->messageListWidget->scrollToBottom();
			ui->messageTextEdit->clear();
			//发送给服务器通过服务器转发
			auto config = new SConfigFile("config.ini");

			QVariantMap messageMap;
			messageMap["message"] = msg;
			messageMap["username"] = User::instance()->getUserName();
			messageMap["user_id"] = User::instance()->getUserId();
			messageMap["to"] = m_friend_id;
			qDebug() << "当前用户id" << config->value("user_id");
			Client::instance()->sendMessage("communication", messageMap);
		});
	ui->messageListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

//进入会话 加载当前会话信息
void MessagePage::setCurrentUser(const QJsonObject& obj)
{
	m_friend_username = obj["username"].toString();
	m_friend_id = obj["user_id"].toString();
	m_currentID = m_friend_id;
	auto pixmap= FriendManager::instance()->findFriend(m_currentID)->getAvatar();
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
	m_chats.insert(m_friend_id, ChatMessage(User::instance()->getUserId(), m_friend_id));
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
		if (message.getSenderId() == User::instance()->getUserId())
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
	if (sender_id == User::instance()->getUserId())
	{
		m_chats[receiver_id].addMessage(message);
		message.readMessage();
	}
	else
	{
		m_chats[sender_id].addMessage(message);
	}

}

void MessagePage::clearMessageWidget()
{
	ui->messageListWidget->clear();
}





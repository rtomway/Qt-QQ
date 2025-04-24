#include "ChatPage.h"
#include "ui_ChatPage.h"
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <QJsonObject>
#include <QJsonArray>
#include <QBoxLayout>
#include <QFileDialog>
#include <QScrollBar>

#include "ImageUtil.h"
#include "FriendManager.h"
#include "ChatRecordManager.h"
#include "EventBus.h"
#include "AvatarManager.h" 
#include "TipMessageItemWidget.h"
#include "LoginUserManager.h"


ChatPage::ChatPage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::ChatPage)
{
	ui->setupUi(this);
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, this, [=]
		{
			m_loginUser = LoginUserManager::instance()->getLoginUser();
		});
	init();
	this->setObjectName("MessagePage");
	QFile file(":/stylesheet/Resource/StyleSheet/ChatPage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
}
ChatPage::~ChatPage()
{
	delete ui;
}
void ChatPage::init()
{
	//在自己信息中心加载完成后读取
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, this, [=]
		{
			m_loginUser = LoginUserManager::instance()->getLoginUser();
		});
	//设置输入字体大小
	QFont font = ui->messageTextEdit->font();
	font.setPointSize(13);
	ui->messageTextEdit->setFont(font);
	// 设置垂直滚动条按像素滚动
	ui->messageListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	//设置滚动条始终出现防止抖动
	ui->messageListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//调整滚动速度
	ui->messageListWidget->verticalScrollBar()->setSingleStep(10);
	ui->messageTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//聊天记录加载
void ChatPage::loadChatMessage(const ChatRecordMessage& chatMessage)
{
	this->clearMessageWidget();
	for (const auto& messagePtr : chatMessage.getMessages())
	{
		messagePtr->readMessage();
		auto type = messagePtr->getType();
		auto time = messagePtr->getTime();
		this->insertTipMessage(messagePtr->getTime());
		//判断消息是谁发送
		bool isSelf = ((messagePtr)->getSenderId() == LoginUserManager::instance()->getLoginUserID());
		QString senderId = isSelf ? m_loginUser->getFriendId() : messagePtr->getSenderId();
		AvatarManager::instance()->getAvatar(senderId, ChatType::User, [=](const QPixmap& avatar)
			{
				auto headPix = ImageUtils::roundedPixmap(avatar, QSize(100, 100), 66);
				// 判断消息类型并处理
				switch (type)
				{
				case MessageType::Text:
				{
					auto textMessage = dynamic_cast<TextMessage*>(messagePtr.get());
					qDebug() << "---------------------------TextMessage----------------";
					auto& message = textMessage->getTextMessage();
					MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleTextRight : MessageBubble::BubbleTextLeft;
					createTextMessageBubble(headPix, message, bubbleType, messagePtr->getSenderId());
					break;
				}
				case MessageType::Image:
				{
					auto imageMessage = dynamic_cast<ImageMessage*>(messagePtr.get());
					qDebug() << "---------------------ImageMessage-------------------";
					auto& message = imageMessage->getImageMessage();
					MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleImageRight : MessageBubble::BubbleImageLeft;
					createImageMessageBubble(headPix, message, bubbleType, messagePtr->getSenderId());
					break;
				}
				case MessageType::System:
				{
					auto systemMessage = dynamic_cast<SystemMessage*>(messagePtr.get());
					qDebug() << "--------------------------SystemMessage------------------";
					auto& message = systemMessage->getSystemMessage();
					insertTipMessage(message);
					break;
				}
				default:
					break;
				}
			});
	}
}
//当前界面接受消息更新
void ChatPage::updateReceiveMessage(const QString& send_id, const QVariant& messageData, MessageType type)
{
	switch (type)
	{
	case MessageType::Text:
	{
		auto message = messageData.toString();
		AvatarManager::instance()->getAvatar(send_id, ChatType::User, [=](const QPixmap& pixmap)
			{
				insertTipMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
				auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
				createTextMessageBubble(headPix, message, MessageBubble::BubbleTextLeft, send_id);
			});
		break;
	}
	case MessageType::Image:
	{
		auto pixmap = messageData.value<QPixmap>();
		AvatarManager::instance()->getAvatar(send_id, ChatType::User, [=](const QPixmap& pixmap_2)
			{
				insertTipMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
				auto headPix = ImageUtils::roundedPixmap(pixmap_2, QSize(40, 40));
				createImageMessageBubble(headPix, pixmap, MessageBubble::BubbleImageLeft, send_id);
			});
		break;
	}
	case MessageType::System:
	{
		auto message = messageData.toString();
		insertTipMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
		insertTipMessage(message);
		break;
	}

	default:
		break;
	}
}
//清空
void ChatPage::clearMessageWidget()
{
	ui->messageTextEdit->clear();
	ui->messageListWidget->clear();
}

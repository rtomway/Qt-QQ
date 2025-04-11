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

ChatPage::ChatPage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::ChatPage)
{
	ui->setupUi(this);
	connect(FriendManager::instance(), &FriendManager::FriendManagerLoadSuccess, this, [=]
		{
			auto id = FriendManager::instance()->getOneselfID();
			m_oneself = FriendManager::instance()->findFriend(id);
		});
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

ChatPage::~ChatPage()
{
	delete ui;
}
void ChatPage::init()
{
	//在自己信息中心加载完成后读取
	connect(FriendManager::instance(), &FriendManager::FriendManagerLoadSuccess, this, [=]
		{
			auto user_id = FriendManager::instance()->getOneselfID();
			m_oneself = FriendManager::instance()->findFriend(user_id);
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

void ChatPage::installEventFilterForChildren(QWidget* parent)
{

}
//聊天记录加载
void ChatPage::loadChatMessage(const ChatRecordMessage& chatMessage)
{
	this->clearMessageWidget();
	for (const auto& messagePtr : chatMessage.getMessages())
	{
		messagePtr->readMessage();
		//判断消息是谁发送
		bool isSelf = ((messagePtr)->getSenderId() == FriendManager::instance()->getOneselfID());
		QString senderId = isSelf ? m_oneself->getFriendId() : messagePtr->getSenderId();
		AvatarManager::instance()->getAvatar(senderId, ChatType::User,[=](const QPixmap& avatar) 
			{
				auto headPix = ImageUtils::roundedPixmap(avatar, QSize(100, 100), 66);
				// 判断消息类型并处理
				if (auto textMessage = dynamic_cast<TextMessage*>(messagePtr.get())) {
					qDebug() << "TextMessage";
					auto content = textMessage->getTextMessage();
					MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleTextRight : MessageBubble::BubbleTextLeft;
					createTextMessageBubble(headPix, content, bubbleType, messagePtr->getSenderId());
				}
				else if (auto imageMessage = dynamic_cast<ImageMessage*>(messagePtr.get())) {
					qDebug() << "ImageMessage";
					QPixmap image = imageMessage->getImageMessage();
					MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleImageRight : MessageBubble::BubbleImageLeft;
					createImageMessageBubble(headPix, image, bubbleType, messagePtr->getSenderId());
				}
			});
	}
}
//聊天记录更新
void ChatPage::updateChatMessage(const QString& sender_id, const QString& receiver_id, const QVariant& msg)
{
	// 创建消息指针
	std::shared_ptr<MessageRecord> message;

	if (msg.type() == QVariant::String)
	{
		// 创建文本消息
		qDebug() << "存储创建文本消息";
		message = std::make_shared<TextMessage>(sender_id, receiver_id, QDateTime::currentDateTime(), msg.toString());
	}
	else if (msg.canConvert<QPixmap>())
	{
		// 创建图片消息
		qDebug() << "存储创建图片消息";
		QPixmap pixmap = msg.value<QPixmap>();
		message = std::make_shared<ImageMessage>(sender_id, receiver_id, QDateTime::currentDateTime(), pixmap);
	}
	storeMessage(sender_id, receiver_id, message);
}
//当前界面接受消息更新
void ChatPage::updateReciveMessage(const QString& user_id, const QString& message)
{
	AvatarManager::instance()->getAvatar(user_id, ChatType::User, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
			createTextMessageBubble(headPix, message, MessageBubble::BubbleTextLeft, user_id);
		});
}
void ChatPage::updateReciveMessage(const QString& user_id, const QPixmap& pixmap)
{
	AvatarManager::instance()->getAvatar(user_id, ChatType::User, [=](const QPixmap& pixmap_2)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap_2, QSize(40, 40));
			createImageMessageBubble(headPix, pixmap, MessageBubble::BubbleTextLeft, user_id);
		});
}

void ChatPage::clearMessageWidget()
{
	ui->messageTextEdit->clear();
	ui->messageListWidget->clear();
}

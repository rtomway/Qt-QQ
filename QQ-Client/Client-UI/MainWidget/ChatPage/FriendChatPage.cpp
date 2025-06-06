#include "FriendChatPage.h"
#include "ui_ChatPage.h"
#include <QBuffer>
#include <QMouseEvent>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>

#include "EventBus.h"
#include "MessageSender.h"
#include "PacketCreate.h"
#include "FriendManager.h"
#include "ChatRecordManager.h"
#include "AvatarManager.h"
#include "ImageUtil.h"
#include "TipMessageItemWidget.h"
#include "LoginUserManager.h"


FriendChatPage::FriendChatPage(QWidget* parent)
	:ChatPage(parent)
	, m_friendPannel(new FriendSetPannelWidget(this))
{
	init();
}

FriendChatPage::~FriendChatPage()
{
	delete ui;
}

void FriendChatPage::init()
{
	//好友面板
	auto pannelLayout = new QVBoxLayout(m_setWidget);
	pannelLayout->setContentsMargins(2, 2, 2, 2);
	pannelLayout->addWidget(m_friendPannel);

	//用户信息更新后(处于会话界面)
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			if (m_friend && user_id != m_friend->getFriendId())
				return;
			setChatWidget(user_id);
		});
	//用户头像更新后(处于会话界面)
	connect(AvatarManager::instance(), &AvatarManager::UpdateUserAvatar, this, [=](const QString& user_id)
		{
			if (m_friend && m_loginUser)
			{
				if (user_id != m_friend->getFriendId() && user_id != m_loginUser->getFriendId())
					return;
				loadChatMessage(*m_chat);
			}
		});
	//发送消息
	connect(ui->sendBtn, &QPushButton::clicked, this, [=]
		{
			AvatarManager::instance()->getAvatar(m_loginUser->getFriendId(), ChatType::User, [=](const QPixmap& pixmap)
				{
					auto& user_id = LoginUserManager::instance()->getLoginUserID();
					auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(100, 100), 66);
					if (!m_imageMessagePath.isEmpty())
					{
						//图片消息
						sendImageMessageToServer(user_id, headPix);
					}
					else
					{
						//文字消息
						sendTextMessageToServer(user_id, headPix);
					}
				});
		});
	//选择待发送图片
	connect(ui->pictureBtn, &QPushButton::clicked, this, [=]
		{
			auto imageMessagePath = QFileDialog::getOpenFileNames(this, "选择图片", "",
				"Images(*.jpg *.png *.jpeg *.bnp)");
			m_imageMessagePath.append(imageMessagePath);
			// 如果选择了有效的图片文件路径
			if (!imageMessagePath.isEmpty()) {
				// 获取 QTextEdit 的文档对象
				QTextDocument* doc = ui->messageTextEdit->document();
				// 创建文本光标
				QTextCursor cursor = ui->messageTextEdit->textCursor();
				// 获取最大显示宽度
				int maxWidth = 100;  // 可以根据需要调整
				for (const QString& imagePath : imageMessagePath)
				{
					// 使用 QImage 加载图片
					QImage image(imagePath);

					// 获取原始尺寸
					QSize originalSize = image.size();

					// 按比例缩放图片
					QSize scaledSize = originalSize.scaled(maxWidth, 1000, Qt::KeepAspectRatio); // 限制最大宽度，并保持比例

					// 创建图片格式
					QTextImageFormat imageFormat;
					imageFormat.setWidth(scaledSize.width());  // 设置缩放后的宽度
					imageFormat.setHeight(scaledSize.height());  // 设置缩放后的高度
					imageFormat.setName(imagePath);  // 设置图片路径

					// 插入图片到光标所在的位置
					cursor.insertImage(imageFormat);

					// 保证文本框自动滚动到底部
					ui->messageTextEdit->setTextCursor(cursor);
				}
			}
		});
	//设置面板
	connect(ui->moreBtn, &QPushButton::clicked, this, [=]
		{
			if (!m_setWidget->isHidden())
			{
				hideSetWidget();
				return;
			}
			showSetWidget();
		});
}

//设置会话界面信息
void FriendChatPage::setChatWidget(const QString& id)
{
	//数据加载
	if (!m_friend|| m_friend->getFriendId() != id)
	{
		m_currentChat = false;
		m_friend = FriendManager::instance()->findFriend(id);
	}
	else
	{
		m_currentChat = true;
	}

	m_title = m_friend->getFriendName();
	//聊天记录加载
	m_chat = ChatRecordManager::instance()->getChatRecord(id, ChatType::User);
	if (!m_currentChat)
	{
		loadChatMessage(*m_chat);
	}
	refreshChatWidget();
}

//刷新会话界面
void FriendChatPage::refreshChatWidget()
{
	ui->nameLab->setText(m_title);
	m_friendPannel->loadFriendPannel(m_friend->getFriendId());
}

//判断当前会话
bool FriendChatPage::isCurrentChat(const QString& id) const
{
	if (m_friend && id == m_friend->getFriendId())
		return true;
	else
		return false;
}

//消息的发送
void FriendChatPage::sendImageMessageToServer(const QString& user_id, const QPixmap& headPix)
{
	for (const QString& imagePath : m_imageMessagePath)
	{
		QPixmap pixmap(imagePath);
		//消息显示至聊天框
		this->insertTipMessage(QDateTime::currentDateTime().toString("MM-dd hh:mm"));
		createImageMessageBubble(headPix, pixmap, MessageBubble::BubbleImageRight);
		//将消息加入至聊天记录中
		ChatMessage chatMessage;
		chatMessage.sendId = user_id;
		chatMessage.receiveId = m_friend->getFriendId();
		chatMessage.data = QVariant::fromValue(pixmap);
		chatMessage.messageType = MessageType::Image;
		chatMessage.chatType = ChatType::User;
		chatMessage.time = QDateTime::currentDateTime();
		ChatRecordManager::instance()->addMessageToChat(chatMessage);
		//移除发送的图片
		m_imageMessagePath.removeOne(imagePath);
		//向服务端发送
		QByteArray byteArray;
		QBuffer buffer(&byteArray);
		buffer.open(QIODevice::WriteOnly);
		// 将 QPixmap 转换为 PNG 并存入 QByteArray
		if (!pixmap.save(&buffer, "PNG")) 
		{
			qDebug() << "Failed to convert avatar to PNG format.";
			return;
		}
		QVariantMap params;
		params["user_id"] = user_id;
		params["to"] = m_friend->getFriendId();
		params["message"] = "picture";
		params["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
		params["size"] = byteArray.size();

		//二进制数据的发送
		auto packet = PacketCreate::binaryPacket("pictureCommunication", params, byteArray);
		QByteArray userData;
		PacketCreate::addPacket(userData, packet);
		auto allData = PacketCreate::allBinaryPacket(userData);

		MessageSender::instance()->sendBinaryData(allData);
	}
	ui->messageTextEdit->clear();
}
void FriendChatPage::sendTextMessageToServer(const QString& user_id, const QPixmap& headPix)
{
	QString msg = ui->messageTextEdit->toPlainText();
	if (msg.isEmpty())
		return;
	//消息显示至聊天框
	this->insertTipMessage(QDateTime::currentDateTime().toString("MM-dd hh:mm"));
	createTextMessageBubble(headPix, msg, MessageBubble::BubbleTextRight);
	//将消息加入至聊天记录中
	ChatMessage chatMessage;
	chatMessage.sendId = user_id;
	chatMessage.receiveId = m_friend->getFriendId();
	chatMessage.data = QVariant::fromValue(msg);
	chatMessage.messageType = MessageType::Text;
	chatMessage.time = QDateTime::currentDateTime();
	chatMessage.chatType = ChatType::User;
	ChatRecordManager::instance()->addMessageToChat(chatMessage);
	//清空输入
	ui->messageTextEdit->clear();
	if (user_id == m_friend->getFriendId())
		return;
	//发送给服务器通过服务器转发
	QJsonObject textMessageObj;
	textMessageObj["user_id"] = user_id;
	textMessageObj["message"] = msg;
	textMessageObj["to"] = m_friend->getFriendId();
	textMessageObj["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
	auto message = PacketCreate::textPacket("textCommunication", textMessageObj);
	MessageSender::instance()->sendMessage(message);
}

//消息气泡
void FriendChatPage::createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	MessageBubble* bubble = new MessageBubble(avatar, pixmap, bubbleType);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}
void FriendChatPage::createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	MessageBubble* bubble = new MessageBubble(avatar, message, bubbleType);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}

//系统消息或者时间分割条
void FriendChatPage::insertTipMessage(const QString& text)
{
	auto tipMessageItemWidget = new TipMessageItemWidget(text, ui->messageListWidget);
	auto tipmessageItem = new QListWidgetItem(ui->messageListWidget);
	tipmessageItem->setSizeHint(tipMessageItemWidget->sizeHint());
	ui->messageListWidget->setItemWidget(tipmessageItem, tipMessageItemWidget);
}

//清空界面
void FriendChatPage::clearChatPage()
{
	m_friend = nullptr;
	m_chat = nullptr;
	clearMessageWidget();
}




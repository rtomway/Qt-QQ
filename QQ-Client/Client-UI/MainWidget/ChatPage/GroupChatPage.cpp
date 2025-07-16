#include "GroupChatPage.h"
#include "ui_ChatPage.h"
#include <QVariant>
#include <QFileDialog>
#include <QBuffer>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>
#include <QJsonDocument>
#include <QMessageBox>

#include "GroupListItemWidget.h"
#include "GroupMemberGridWidget.h"
#include "EventBus.h"
#include "GroupManager.h"
#include "ChatRecordManager.h"
#include "Client-ServiceLocator/NetWorkServiceLocator.h"
#include "AvatarManager.h"
#include "GlobalTypes.h"
#include "ImageUtil.h"
#include "TipMessageItemWidget.h"
#include "PacketCreate.h"

#include "ImageMessageBubble.h"
#include "TextMessageBubble.h"


GroupChatPage::GroupChatPage(QWidget* parent)
	:ChatPage(parent)
	,m_groupPannel(new GroupSetPannelWidget(m_setWidget))
{
	init();
}

GroupChatPage::~GroupChatPage()
{
	delete ui;
}

void GroupChatPage::init()
{
	auto pannelLayout = new QVBoxLayout(m_setWidget);
	pannelLayout->setContentsMargins(2, 2, 2, 2);
	pannelLayout->addWidget(m_groupPannel);

	//发送
	connect(ui->sendBtn, &QPushButton::clicked, this, [=]
		{
			AvatarManager::instance()->getAvatar(m_loginUser->getFriendId(), ChatType::User, [=](const QPixmap& pixmap)
				{
					auto& user_id = m_loginUser->getFriendId();
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
	//隐藏群面板
	connect(EventBus::instance(), &EventBus::hideGroupSetPannel, this, &GroupChatPage::hideSetWidget);
	//群成员移除
	connect(GroupManager::instance(), &GroupManager::updateGroupProfile, this, [=](const QString& group_id)
		{
			if (m_group && group_id == m_group->getGroupId())
			{
				setChatWidget(group_id);
			}
		});
}

//设置会话界面信息
void GroupChatPage::setChatWidget(const QString& id)
{
	if (!m_group || m_group->getGroupId() != id)
	{
		m_currentChat = false;
		m_group = GroupManager::instance()->findGroup(id);
	}
	else
	{
		m_currentChat = true;
	}

	m_chat = ChatRecordManager::instance()->getChatRecord(id, ChatType::Group);
	if (!m_currentChat)
	{
		loadChatMessage(*m_chat);
	}

	//界面信息
	m_title = QString("%1(%2)").arg(m_group->getGroupName()).arg(m_group->count());
	refreshChatWidget();
}

//刷新会话界面
void GroupChatPage::refreshChatWidget()
{
	ui->nameLab->setText(m_title);
	m_groupPannel->loadGroupPannel(m_group->getGroupId());
}

//判断当前会话
bool GroupChatPage::isCurrentChat(const QString& id) const
{
	if (m_group && id == m_group->getGroupId())
		return true;
	else
		return false;
}

//消息发送
void GroupChatPage::sendImageMessageToServer(const QString& id, const QPixmap& headPix)
{
	for (const QString& imagePath : m_imageMessagePath)
	{
		QPixmap pixmap(imagePath);
		//消息显示至聊天框
		this->insertTipMessage(QDateTime::currentDateTime().toString("MM-dd hh:mm"));
		createImageMessageBubble(headPix, pixmap, MessageBubble::BubbleRight, id);
		//将消息加入至聊天记录中 
		ChatMessage chatMessage;
		chatMessage.sendId = id;
		chatMessage.receiveId = m_group->getGroupId();
		chatMessage.data = QVariant::fromValue(pixmap);
		chatMessage.messageType = MessageType::Image;
		chatMessage.chatType = ChatType::Group;
		chatMessage.time = QDateTime::currentDateTime();
		ChatRecordManager::instance()->addMessageToChat(chatMessage);
		//移除发送的图片
		m_imageMessagePath.removeOne(imagePath);
		//向服务端发送
		QByteArray byteArray;
		QBuffer buffer(&byteArray);
		buffer.open(QIODevice::WriteOnly);
		// 将 QPixmap 转换为 PNG 并存入 QByteArray
		if (!pixmap.save(&buffer, "PNG")) {
			qDebug() << "Failed to convert avatar to PNG format.";
			return;
		}
		QVariantMap params;
		params["user_id"] = id;
		params["group_id"] = m_group->getGroupId();
		params["message"] = "picture";
		params["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
		params["size"] = byteArray.size();

		//二进制数据的发送
		auto packet = PacketCreate::binaryPacket("groupPictureCommunication", params, byteArray);
		QByteArray userData;
		PacketCreate::addPacket(userData, packet);
		auto allData = PacketCreate::allBinaryPacket(userData);

		NetWorkServiceLocator::instance()->sendWebBinaryData(allData);
	}
	ui->messageTextEdit->clear();
}
void GroupChatPage::sendTextMessageToServer(const QString& user_id, const QPixmap& headPix)
{
	//文字消息
	QString msg = ui->messageTextEdit->toPlainText();
	if (msg.isEmpty())
		return;
	//消息显示至聊天框
	this->insertTipMessage(QDateTime::currentDateTime().toString("MM-dd hh:mm"));
	createTextMessageBubble(headPix, msg, MessageBubble::BubbleRight, user_id);
	//将消息加入至聊天记录中
	ChatMessage chatMessage;
	chatMessage.sendId = user_id;
	chatMessage.receiveId = m_group->getGroupId();
	chatMessage.data = QVariant::fromValue(msg);
	chatMessage.messageType = MessageType::Text;
	chatMessage.chatType = ChatType::Group;
	chatMessage.time = QDateTime::currentDateTime();
	ChatRecordManager::instance()->addMessageToChat(chatMessage);
	//清空输入
	ui->messageTextEdit->clear();
	//发送给服务器通过服务器转发
	QJsonObject groupMessageObj;
	groupMessageObj["message"] = msg;
	groupMessageObj["user_id"] = m_loginUser->getFriendId();
	groupMessageObj["group_id"] = m_group->getGroupId();
	groupMessageObj["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
	auto message = PacketCreate::textPacket("groupTextCommunication", groupMessageObj);
	NetWorkServiceLocator::instance()->sendWebTextMessage(message);
}

//消息气泡
void GroupChatPage::createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	auto member = m_group->getMember(user_id);
	auto& memberName = member.member_name;
	auto& memberRole = member.member_role;

	MessageBubble::MessageBubbleInitParams initParams;
	initParams.user_id = user_id;
	initParams.head_img = avatar;
	initParams.data = QVariant(pixmap);
	initParams.bubbleType = bubbleType;
	initParams.group_memberName = memberName;
	initParams.group_memberRole = memberRole;

	MessageBubble* bubble = new ImageMessageBubble(initParams);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}
void GroupChatPage::createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	auto member = m_group->getMember(user_id);
	auto& memberName = member.member_name;
	auto& memberRole = member.member_role;

	MessageBubble::MessageBubbleInitParams initParams;
	initParams.user_id = user_id;
	initParams.head_img = avatar;
	initParams.data = QVariant(message);
	initParams.bubbleType = bubbleType;
	initParams.group_memberName = memberName;
	initParams.group_memberRole = memberRole;

	MessageBubble* bubble = new TextMessageBubble(initParams);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}

//插入系统消息
void GroupChatPage::insertTipMessage(const QString& text)
{
	auto tipMessageItemWidget = new TipMessageItemWidget(text, ui->messageListWidget);
	auto tipmessageItem = new QListWidgetItem(ui->messageListWidget);
	tipmessageItem->setSizeHint(tipMessageItemWidget->sizeHint());
	ui->messageListWidget->setItemWidget(tipmessageItem, tipMessageItemWidget);
}

//清空
void GroupChatPage::clearChatPage()
{
	m_group = nullptr;
	m_chat = nullptr;
	clearMessageWidget();
}


#include "GroupChatPage.h"
#include "ui_ChatPage.h"
#include <QVariant>
#include <QFileDialog>
#include <QBuffer>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QJsonDocument>

#include "GroupManager.h"
#include "ChatRecordManager.h"
#include "MessageSender.h"
#include "AvatarManager.h"
#include "GlobalTypes.h"
#include "ImageUtil.h"
#include "TipMessageItemWidget.h"
#include "PacketCreate.h"
#include "GroupListItemWidget.h"
#include "EventBus.h"
#include "GroupMemberGridWidget.h"

GroupChatPage::GroupChatPage(QWidget* parent)
	:ChatPage(parent)
{
	init();
}
GroupChatPage::~GroupChatPage()
{
	delete ui;
}
void GroupChatPage::init()
{
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
			m_setWidget->setId(m_group->getGroupId());
			qDebug() << "设置面板";
			qDebug() << m_setWidget->isHidden();
			if (!m_setWidget->isHidden())
			{
				hideSetWidget();
				return;
			}
			showSetWidget();
		});
}
//设置会话界面信息
void GroupChatPage::setChatWidget(const QString& id)
{
	qDebug() << "------------------------------群组聊天界面--------------------------";
	if (!m_group)
	{
		m_currentChat = false;
		m_group = GroupManager::instance()->findGroup(id);
	}
	else
	{
		if (m_group->getGroupId() != id)
		{
			m_currentChat = false;
			m_group = GroupManager::instance()->findGroup(id);
		}
		else
		{
			m_currentChat = true;
		}
	}
	qDebug() << "mygoup:" << m_group->getGroupId() << m_group->getGroupName();
	m_chat = ChatRecordManager::instance()->getChatRecord(id, ChatType::Group);
	if (!m_currentChat)
	{
		qDebug() << "-------------群组聊天记录的加载-群组id:" << m_group->getGroupId();
		loadChatMessage(*m_chat);
	}
	//界面信息
	m_title = QString("%1(%2)").arg(m_group->getGroupName()).arg(m_group->count());
	initSetWidget();
	refreshChatWidget();
}
//刷新会话界面
void GroupChatPage::refreshChatWidget()
{
	ui->nameLab->setText(m_title);
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
		createImageMessageBubble(headPix, pixmap, MessageBubble::BubbleImageRight, id);
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

		MessageSender::instance()->sendBinaryData(allData);
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
	createTextMessageBubble(headPix, msg, MessageBubble::BubbleTextRight, user_id);
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
	MessageSender::instance()->sendMessage(message);
}
//初始化设置面板
void GroupChatPage::initSetWidget()
{
	m_setWidget->clearListWidget();
	auto groupItemWidget = new GroupListItemWidget(m_setWidget);
	groupItemWidget->setItemWidget(m_group->getGroupId());
	groupItemWidget->showGroupId();
	groupItemWidget->setStyleSheet(R"(
		QWidget{ border: none;border-radius:10px;background-color:white;}
		)");
	m_setWidget->addItemWidget(groupItemWidget,80);

	auto groupMemberGrid = new GroupMemberGridWidget(this);
	groupMemberGrid->setGroupMembersGrid(m_group->getGroupId());
	groupMemberGrid->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_setWidget->addItemWidget(groupMemberGrid, groupMemberGrid->sizeHint().height());
	

	auto exitGroupBtn = new QPushButton(m_setWidget);
	exitGroupBtn->setText("退出群聊");
	exitGroupBtn->setStyleSheet(R"(
		QPushButton{background-color:white;border:1px solid white;height:25px;border-radius:5px;color:red}
		QPushButton:hover{background-color:rgb(240,240,240);}
		)");
	m_setWidget->addItemWidget(exitGroupBtn, 30);
	connect(exitGroupBtn, &QPushButton::clicked, this, [=]
		{
			auto deleteResult = QMessageBox::question(nullptr, "退出群聊", "请确认是否退出");
			if (deleteResult == QMessageBox::No)
				return;
			m_setWidget->hide();
			EventBus::instance()->emit exitGroup(m_group->getGroupId(),m_loginUser->getFriendId());
			QJsonObject deleteObj;
			deleteObj["user_id"] = m_loginUser->getFriendId();
			deleteObj["username"] = m_loginUser->getFriendName();
			deleteObj["group_id"] = m_group->getGroupId();
			QJsonDocument doc(deleteObj);
			QByteArray data = doc.toJson(QJsonDocument::Compact);
			MessageSender::instance()->sendHttpRequest("exitGroup", data, "application/json");
		});
}
//消息气泡
void GroupChatPage::createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	auto member = m_group->getMember(user_id);
	auto& memberName = member.member_name;
	auto& memberRole = member.member_role;
	qDebug() << "*************************************接收群组图片消息:" << memberName;
	if (pixmap.isNull())
		qDebug() << "哈哈哈哈哈哈";
	MessageBubble* bubble = new MessageBubble(avatar, pixmap, bubbleType, memberName, memberRole);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}
void GroupChatPage::createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& user_id)
{
	auto member = m_group->getMember(user_id);
	auto& memberName = member.member_name;
	auto& memberRole = member.member_role;
	qDebug() << "member:" << memberName << memberRole;
	MessageBubble* bubble = new MessageBubble(avatar, message, bubbleType, memberName, memberRole);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}

void GroupChatPage::insertTipMessage(const QString& text)
{
	auto tipMessageItemWidget = new TipMessageItemWidget(text, ui->messageListWidget);
	auto tipmessageItem = new QListWidgetItem(ui->messageListWidget);
	tipmessageItem->setSizeHint(tipMessageItemWidget->sizeHint());  // 设置Item大小
	ui->messageListWidget->setItemWidget(tipmessageItem, tipMessageItemWidget);
}

void GroupChatPage::clearChatPage()
{
	m_group = nullptr;
	m_chat = nullptr;
	clearMessageWidget();
}


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
#include <QFileDialog>
#include <QScrollBar>
#include <QBuffer>

#include "PacketCreate.h"
#include "ImageUtil.h"
#include "MessageSender.h"
#include "FriendManager.h"
#include "GroupManager.h"
#include "FriendSetWidget.h"
#include "ChatRecordManager.h"
#include "EventBus.h"
#include "AvatarManager.h"

MessagePage::MessagePage(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::MessagePage)
	, m_setWidget(new FriendSetWidget(this))
	, m_animation(new QPropertyAnimation(m_setWidget, "geometry"))
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
	m_setWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);  // 只设置无边框和工具窗口标志
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
	m_friend = nullptr;
	m_group = nullptr;
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
//加载会话界面信息
void MessagePage::loadMessagePage(const QString& id, ChatType type)
{
	switch (type)
	{
	case ChatType::User:
		qDebug() << "当前会话type:" << id << "User";
		if (!m_friend)
		{
			m_friend = FriendManager::instance()->findFriend(id);
		}
		if (m_friend->getFriendId() != id)
		{
			m_friend = FriendManager::instance()->findFriend(id);
		}
		m_chat = ChatRecordManager::instance()->getChat(id);
		loadChatMessage(*m_chat);
		initFriedMessaePage();
		break;
	case ChatType::Group:
		qDebug() << "当前会话type:" << id << "Group";
		if (!m_group)
		{
			m_group = GroupManager::instance()->findGroup(id);
		}
		if (m_group->getGroupId() != id)
		{
			m_group = GroupManager::instance()->findGroup(id);
		}
		qDebug() << "群聊会话id：" << m_group->getGroupId() << m_group->getGroupName();
		m_chat = ChatRecordManager::instance()->getGroupChat(id);
		loadChatMessage(*m_chat);
		initGroupMessaePage();
		break;
	default:
		break;
	}
}
//好友私聊界面
void MessagePage::initFriedMessaePage()
{
	//界面信息
	ui->nameLab->setText(m_friend->getFriendName());
	//用户信息更新后(处于会话界面)
	connect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, [=](const QString& user_id)
		{
			if (user_id != m_friend->getFriendId())
				return;
			auto user = FriendManager::instance()->findFriend(user_id);
		});
	//用户头像更新后(处于会话界面)
	connect(AvatarManager::instance(), &AvatarManager::UpdateUserAvatar, this, [=](const QString& user_id)
		{
			if (user_id != m_friend->getFriendId() && user_id != m_oneself->getFriendId())
				return;
			auto chatMessage = ChatRecordManager::instance()->getChat(m_friend->getFriendId());
			loadChatMessage(*chatMessage);
		});
	//发送消息
	connect(ui->sendBtn, &QPushButton::clicked, this, [=]
		{
			auto user_id = FriendManager::instance()->getOneselfID();
			auto avatar = AvatarManager::instance()->getAvatar(m_oneself->getFriendId(), ChatType::User);
			auto headPix = ImageUtils::roundedPixmap(avatar, QSize(100, 100), 66);
			//图片消息
			if (!m_imageMessagePath.isEmpty())
			{
				for (const QString& imagePath : m_imageMessagePath)
				{
					QPixmap pixmap(imagePath);
					createImageMessageBubble(headPix, pixmap, MessageBubble::BubbleImageRight);
					updateChatMessage(ChatType::User, user_id, m_friend->getFriendId(), QVariant::fromValue(pixmap));
					m_imageMessagePath.removeOne(imagePath);
					//像服务端发送
					QByteArray byteArray;
					QBuffer buffer(&byteArray);
					buffer.open(QIODevice::WriteOnly);
					// 将 QPixmap 转换为 PNG 并存入 QByteArray
					if (!pixmap.save(&buffer, "PNG")) {
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
				return;
			}
			//文字消息
			QString msg = ui->messageTextEdit->toPlainText();
			if (msg.isEmpty())
				return;
			//消息显示至聊天框
			createTextMessageBubble(headPix, msg, MessageBubble::BubbleTextRight);
			//将消息加入至聊天记录中
			updateChatMessage(ChatType::User, user_id, m_friend->getFriendId(), QVariant::fromValue(msg));
			//清空输入
			ui->messageTextEdit->clear();
			//发送给服务器通过服务器转发
			QJsonObject json;
			json["user_id"] = user_id;
			json["message"] = msg;
			json["to"] = m_friend->getFriendId();
			json["time"] = QDateTime::currentDateTime().toString("MM-dd hh:mm");
			if (user_id == m_friend->getFriendId())
			{
				EventBus::instance()->emit textCommunication(json);
				return;
			}
			QVariantMap messageMap = json.toVariantMap();
			MessageSender::instance()->sendMessage("textCommunication", messageMap);
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
			m_setWidget->setId(m_friend->getFriendId());
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
//群聊界面
void MessagePage::initGroupMessaePage()
{
	//界面信息
	auto groupname = QString("%1(%2)").arg(m_group->getGroupName()).arg(m_group->count());
	ui->nameLab->setText(groupname);
	auto user_id = m_oneself->getFriendId();
	auto avatar = AvatarManager::instance()->getAvatar(m_oneself->getFriendId(), ChatType::User);
	auto headPix = ImageUtils::roundedPixmap(avatar, QSize(100, 100), 66);
	qDebug() << "user_id" << user_id;
	const Member& member = m_group->getMember(user_id);
	auto memberName = member.member_name;
	auto memberRole = member.member_role;
	qDebug() << "member:" << member.member_id << member.member_name << member.member_role;
	connect(ui->sendBtn, &QPushButton::clicked, this, [=]
		{
			//文字消息
			QString msg = ui->messageTextEdit->toPlainText();
			if (msg.isEmpty())
				return;
			//消息显示至聊天框
			createTextMessageBubble(headPix, msg, MessageBubble::BubbleTextRight, memberName, memberRole);
			qDebug() << m_group->getGroupId();
			//将消息加入至聊天记录中
			updateChatMessage(ChatType::Group, user_id, m_group->getGroupId(), QVariant::fromValue(msg));
			//清空输入
			ui->messageTextEdit->clear();
			//发送给服务器通过服务器转发

		});
}
//判断当前聊天界面
bool MessagePage::isCurrentChat(const QString& id, ChatType type) const
{
	if (type == ChatType::User) {
		return m_friend && m_friend->getFriendId() == id;
	}
	else if (type == ChatType::Group) {
		return m_group && m_group->getGroupId() == id;
	}
	return false;
}
//加载聊天记录至聊天框
void MessagePage::loadChatMessage(const ChatRecordMessage& chatMessage)
{
	//clearMessageWidget();
	auto type = chatMessage.type();
	bool isGroup = (type == ChatType::Group);
	QString memberName = QString();
	QString memberRole = QString();
	for (const auto& messagePtr : chatMessage.getMessages())
	{
		messagePtr->readMessage();
		//判断消息是谁发送
		bool isSelf = ((messagePtr)->getSenderId() == FriendManager::instance()->getOneselfID());
		QPixmap avatar;
		if (isSelf) {
			avatar = AvatarManager::instance()->getAvatar(m_oneself->getFriendId(), ChatType::User);
		}
		else {
			avatar = AvatarManager::instance()->getAvatar(messagePtr->getSenderId(), ChatType::User);
		}
		auto headPix = ImageUtils::roundedPixmap(avatar, QSize(100, 100), 66);
		if (isGroup)
		{
			auto member = m_group->getMember(messagePtr->getSenderId());
			memberName = member.member_name;
			memberRole = member.member_role;
		}
		// 判断消息类型并处理
		if (auto textMessage = dynamic_cast<TextMessage*>(messagePtr.get())) {
			qDebug() << "TextMessage";
			auto content = textMessage->getTextMessage();
			MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleTextRight : MessageBubble::BubbleTextLeft;
			if (!isGroup)
				createTextMessageBubble(headPix, content, bubbleType);
			else
				createTextMessageBubble(headPix, content, bubbleType, memberName, memberRole);
		}
		else if (auto imageMessage = dynamic_cast<ImageMessage*>(messagePtr.get())) {
			qDebug() << "ImageMessage";
			QPixmap image = imageMessage->getImageMessage();
			MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleImageRight : MessageBubble::BubbleImageLeft;
			if (!isGroup)
				createImageMessageBubble(headPix, image, bubbleType);
			else
				createImageMessageBubble(headPix, image, bubbleType, memberName, memberRole);
		}
	}
}
//创建消息并显示
void MessagePage::createImageMessageBubble(const QPixmap& avatar, const QPixmap& pixmap, MessageBubble::BubbleType bubbleType, const QString& memberName, const QString& memberRole)
{
	MessageBubble* bubble = new MessageBubble(avatar, pixmap, bubbleType, memberName, memberRole); // 右侧图片
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
}
void MessagePage::createTextMessageBubble(const QPixmap& avatar, const QString& message, MessageBubble::BubbleType bubbleType, const QString& memberName, const QString& memberRole)
{
	qDebug() << "text:" << memberName << memberRole;
	MessageBubble* bubble = new MessageBubble(avatar, message, bubbleType, memberName, memberRole);
	ui->messageListWidget->addItem(bubble);
	ui->messageListWidget->setItemWidget(bubble, bubble);
	ui->messageListWidget->scrollToBottom();
	update();
}
//处于会话界面 接收消息直接更新
void MessagePage::updateReciveMessage(const QString& Recivemessage)
{
	auto avatar = AvatarManager::instance()->getAvatar(m_friend->getFriendId(), ChatType::User);
	MessageBubble* message = new MessageBubble(avatar, Recivemessage, MessageBubble::BubbleTextLeft);
	ui->messageListWidget->addItem(message);
	ui->messageListWidget->setItemWidget(message, message);
	ui->messageListWidget->scrollToBottom();
}
void MessagePage::updateReciveMessage(const QPixmap& pixmap)
{
	qDebug() << "xxxxxxx接受图片消息" << pixmap;
	auto avatar = AvatarManager::instance()->getAvatar(m_friend->getFriendId(), ChatType::User);
	MessageBubble* message = new MessageBubble(avatar, pixmap, MessageBubble::BubbleImageLeft);
	ui->messageListWidget->addItem(message);
	ui->messageListWidget->setItemWidget(message, message);
	ui->messageListWidget->scrollToBottom();
}
//更新聊天记录
void MessagePage::updateChatMessage(ChatType type, const QString& sender_id, const QString& receiver_id, const QVariant& msg)
{
	// 创建消息指针（多态）
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
	if (message) {
		message->readMessage();
		if (type == ChatType::User)
		{
			if (sender_id == FriendManager::instance()->getOneselfID()) {
				ChatRecordManager::instance()->updateChat(receiver_id)->addMessage(message);
			}
			else {
				ChatRecordManager::instance()->updateChat(sender_id)->addMessage(message);
			}
		}
		else if (type == ChatType::Group)
		{
			qDebug() << receiver_id << "----";
			auto chat = ChatRecordManager::instance()->updateGroupChat(receiver_id);
			qDebug() << "ddddddddddd" << chat;
			chat->addMessage(message);
		}


	}
}
//清空消息
void MessagePage::clearMessageWidget()
{
	ui->messageListWidget->clear();
	ui->messageTextEdit->clear();
	m_friend = nullptr;
	m_group = nullptr;
	disconnect(FriendManager::instance(), &FriendManager::UpdateFriendMessage, this, nullptr);
	disconnect(AvatarManager::instance(), &AvatarManager::UpdateUserAvatar, this, nullptr);
	disconnect(ui->sendBtn, &QPushButton::clicked, this, nullptr);
	disconnect(ui->pictureBtn, &QPushButton::clicked, this, nullptr);
	disconnect(ui->moreBtn, &QPushButton::clicked, this, nullptr);
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
	m_setWidget->setGeometry(windowRight - m_setWidget->width(), ui->setWidget->height(), 250, this->height() - ui->setWidget->height());
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
				continue;
			}
			childWidget->installEventFilter(this);
			// 如果子控件还有子控件，递归安装事件过滤器
			installEventFilterForChildren(childWidget);
		}
	}
}






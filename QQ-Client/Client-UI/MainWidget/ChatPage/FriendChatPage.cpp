#include "FriendChatPage.h"
#include "ui_ChatPage.h"
#include <QBuffer>
#include <QMouseEvent>
#include <QTimer>
#include <QFileDialog>

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
	, m_setWidget(new FriendSetWidget(this))
	, m_animation(new QPropertyAnimation(m_setWidget, "geometry"))
{
	init();
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
FriendChatPage::~FriendChatPage()
{
	delete ui;
}
void FriendChatPage::init()
{
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
//设置会话界面信息
void FriendChatPage::setChatWidget(const QString& id)
{
	qDebug() << "FriendChatPage::setChatWidget(const QString& id)" << id;
	//数据加载
	if (!m_friend)
	{
		m_currentChat = false;
		m_friend = FriendManager::instance()->findFriend(id);
	}
	else
	{
		qDebug() << "m_friend->getFriendId()" << m_friend->getFriendId();
		if (m_friend->getFriendId() != id)
		{
			m_currentChat = false;
			m_friend = FriendManager::instance()->findFriend(id);
		}
		else
		{
			m_currentChat = true;
		}
	}
	m_title = m_friend->getFriendName();
	//聊天记录加载
	m_chat = ChatRecordManager::instance()->getChatRecord(id, ChatType::User);
	qDebug() << "m_currentChat:" << m_currentChat;
	if (!m_currentChat)
	{
		qDebug() << "-------------好友聊天记录的加载";
		loadChatMessage(*m_chat);
	}
	refreshChatWidget();
}
//刷新会话界面
void FriendChatPage::refreshChatWidget()
{
	ui->nameLab->setText(m_title);
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
		chatMessage.messageType = MessageType::Text;
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
	tipmessageItem->setSizeHint(tipMessageItemWidget->sizeHint());  // 设置Item大小
	ui->messageListWidget->setItemWidget(tipmessageItem, tipMessageItemWidget);
}
void FriendChatPage::clearChatPage()
{
	m_friend = nullptr;
	m_chat = nullptr;
	clearMessageWidget();
}
//侧边栏
void FriendChatPage::installEventFilterForChildren(QWidget* parent)
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
bool FriendChatPage::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint globalPos = mouseEvent->globalPosition().toPoint();
		// 检查点击位置是否在 m_setWidget 内
		if (!m_setWidget->geometry().contains(m_setWidget->mapFromGlobal(globalPos))) {
			// 检查点击位置是否在 moreBtn 内
			if (!this->ui->moreBtn->geometry().contains(this->ui->moreBtn->mapFromGlobal(globalPos))) {
				// 如果点击位置既不在 m_setWidget 也不在 moreBtn 内，则隐藏 m_setWidget
				m_setWidget->hide();
			}
		}
	}
	return QWidget::eventFilter(watched, event);
}
void FriendChatPage::resizeEvent(QResizeEvent* ev)
{
	// 获取窗口的全局坐标和右边缘
	QPoint windowTopLeft = this->mapToGlobal(QPoint(0, 0)); // 获取窗口的左上角的全局坐标
	int windowRight = windowTopLeft.x() + this->width(); // 获取窗口的右边缘的全局坐标
	qDebug() << "resize";
	m_setWidget->setGeometry(windowRight - m_setWidget->width(), ui->setWidget->height(), 250, this->height() - ui->setWidget->height());
	qDebug() << m_setWidget->height();
}




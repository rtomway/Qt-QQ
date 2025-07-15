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
#include <QMouseEvent>
#include <QTimer>


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
	, m_setWidget(new QWidget(this))
	, m_showAnimation(new QPropertyAnimation(m_setWidget, "geometry"))
	, m_hideAnimation(new QPropertyAnimation(m_setWidget, "geometry"))
{
	ui->setupUi(this);
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
	qApp->installEventFilter(this);
	m_setWidget->setObjectName("setPannel");
	m_setWidget->setFixedWidth(250);
	m_setWidget->setFocusPolicy(Qt::StrongFocus);
	//移除默认标志并添加无边框标志
	m_setWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	// 设置初始的裁剪区域，确保初始时不可见
	QRegion region(0, 0, 0, height(), QRegion::Rectangle);
	m_setWidget->setMask(region);  // 裁剪区域
	m_setWidget->hide();

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

	//在自己信息中心加载完成后读取
	connect(LoginUserManager::instance(), &LoginUserManager::loginUserLoadSuccess, this, [=]
		{
			m_loginUser = LoginUserManager::instance()->getLoginUser();
		});
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
					auto& message = textMessage->getTextMessage();
					MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleRight : MessageBubble::BubbleLeft;
					createTextMessageBubble(headPix, message, bubbleType, messagePtr->getSenderId());
					break;
				}
				case MessageType::Image:
				{
					auto imageMessage = dynamic_cast<ImageMessage*>(messagePtr.get());
					auto& message = imageMessage->getImageMessage();
					MessageBubble::BubbleType bubbleType = isSelf ? MessageBubble::BubbleRight : MessageBubble::BubbleLeft;
					createImageMessageBubble(headPix, message, bubbleType, messagePtr->getSenderId());
					break;
				}
				case MessageType::System:
				{
					auto systemMessage = dynamic_cast<SystemMessage*>(messagePtr.get());
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
				createTextMessageBubble(headPix, message, MessageBubble::BubbleLeft, send_id);
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
				createImageMessageBubble(headPix, pixmap, MessageBubble::BubbleLeft, send_id);
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

//event--设置面板
bool ChatPage::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint globalPos = mouseEvent->globalPosition().toPoint();
		// 检查点击位置是否在 m_setWidget 内
		if (!m_setWidget->geometry().contains(globalPos) && !m_setWidget->isHidden())
		{
			hideSetWidget();
		}
	}
	return QWidget::eventFilter(watched, event);
}

//resizeEvent-面板geometry
void ChatPage::resizeEvent(QResizeEvent* ev)
{
	// 获取窗口的全局坐标和右边缘
	QPoint windowTopLeft = this->mapToGlobal(QPoint(0, 0)); // 获取窗口的左上角的全局坐标
	int windowRight = windowTopLeft.x() + this->width(); // 获取窗口的右边缘的全局坐标
	m_setWidget->setGeometry(windowRight - m_setWidget->width(), ui->setWidget->height(), 250, this->height() - ui->setWidget->height());
}

//显示设置面板
void ChatPage::showSetWidget()
{
	auto setHeight = this->height() - ui->setWidget->height();
	// 获取窗口的全局坐标和右边缘
	QPoint windowTopLeft = this->mapToGlobal(QPoint(0, 0)); // 获取窗口的左上角的全局坐标
	int windowRight = windowTopLeft.x() + this->width(); // 获取窗口的右边缘的全局坐标
	// 动画设置
	m_showAnimation->setEasingCurve(QEasingCurve::Linear);
	m_showAnimation->setDuration(300);  // 动画持续时间
	// 设置动画的起始和结束位置（全局坐标系）
	m_showAnimation->setStartValue(QRect(windowRight, windowTopLeft.y() + ui->setWidget->height(), 0, setHeight));  // 从外部开始
	m_showAnimation->setEndValue(QRect(windowRight - m_setWidget->width(), windowTopLeft.y() + ui->setWidget->height(), m_setWidget->width(), setHeight));  // 结束时的位置

	// 动画过程中更新裁剪区域
	connect(m_showAnimation, &QPropertyAnimation::valueChanged, [=](const QVariant& value)
		{
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
	//延迟执行动画，确保 isVisible() 状态稳定
	QTimer::singleShot(10, this, [this]() {
		m_showAnimation->start();
		// 延迟显示控件，避免闪烁
		QTimer::singleShot(30, this, [this]() {
			m_setWidget->show();
			m_setWidget->activateWindow();
			m_setWidget->setFocus();
			});
		});
}

//隐藏设置面板
void ChatPage::hideSetWidget()
{
	m_isAnimationFinished = false;
	// 计算窗口右边界（全局坐标）
	QPoint windowTopLeft = this->mapToGlobal(QPoint(0, 0));
	int windowRight = windowTopLeft.x() + this->width();
	int setHeight = this->height() - ui->setWidget->height();

	// 配置动画参数
	m_hideAnimation->setEasingCurve(QEasingCurve::Linear);
	m_hideAnimation->setDuration(300);  // 动画时长与显示一致

	// 设置动画起止位置（从当前位置滑动到右侧屏幕外）
	m_hideAnimation->setStartValue(m_setWidget->geometry());  // 从当前状态开始
	m_hideAnimation->setEndValue(QRect(
		windowRight,                                      // 目标X：屏幕右侧外
		windowTopLeft.y() + ui->setWidget->height(),      // Y与显示动画一致
		m_setWidget->width(),                             // 宽度不变
		setHeight                                         // 高度不变
	));

	// 动画过程中更新裁剪区域（与显示动画对称）
	connect(m_hideAnimation, &QPropertyAnimation::valueChanged, [=](const QVariant& value) 
		{
			QRect rect = value.toRect();
			int visibleWidth = windowRight - rect.x();  // 计算可见宽度
			QRegion region(0, 0, visibleWidth, height(), QRegion::Rectangle);
			m_setWidget->setMask(region);
		});

	//动画结束时完全隐藏
	connect(m_hideAnimation, &QPropertyAnimation::finished, [=]() 
	{
		if (!m_isAnimationFinished) //利用标志位避免valueChanged,进入多次
		{
			m_isAnimationFinished = true;  
			m_setWidget->clearMask();
			m_setWidget->hide();
			//m_setWidget->clearMask();   
		}
	});

	// 启动动画
	m_hideAnimation->start();
}

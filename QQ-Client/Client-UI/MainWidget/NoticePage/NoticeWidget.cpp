#include "NoticeWidget.h"
#include <QBoxLayout>
#include <QFile>

#include "ContactListWidget.h"
#include "EventBus.h"


NoticeWidget::NoticeWidget(QWidget* parent)
	:QWidget(parent)
	, m_stackedWidget(new QStackedWidget(this))
	, m_friendNoticeList(new QListWidget(this))
	, m_groupNoticeList(new QListWidget(this))
	, m_noticeTitle(new QLabel("好友通知", this))
{
	init();
	initStackedWidget();
	QFile file(":/stylesheet/Resource/StyleSheet/NoticeWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
}
//设置当前通知界面
void NoticeWidget::setCurrentWidget(NoticeWidget::NoticeCurrentWidget noticeWidget)
{
	if (noticeWidget == NoticeCurrentWidget::FriendNoticeWidget)
	{
		m_noticeTitle->setText("好友通知");
		m_stackedWidget->setCurrentWidget(m_friendNoticeList);
	}
	else
	{
		m_noticeTitle->setText("群聊通知");
		m_stackedWidget->setCurrentWidget(m_groupNoticeList);
	}
}
//界面初始化
void NoticeWidget::init()
{
	m_noticeTitle->setObjectName("titleLabel");
	auto mlayout = new QVBoxLayout(this);
	mlayout->addWidget(m_noticeTitle);
	mlayout->addWidget(m_stackedWidget);

	//好友申请通知
	connect(EventBus::instance(), &EventBus::addFriend, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			addFreindNoticeItem(obj, pixmap,NoticeItemWidget::NoticeType::RequestNotice);
		});
	//被拒绝通知
	connect(EventBus::instance(), &EventBus::rejectAddFriend, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			addFreindNoticeItem(obj, pixmap,NoticeItemWidget::NoticeType::ReplyNotice);
		});
}
//init堆栈窗口
void NoticeWidget::initStackedWidget()
{
	m_stackedWidget->addWidget(m_friendNoticeList);
	m_stackedWidget->addWidget(m_groupNoticeList);
}
//添加通知项
void NoticeWidget::addFreindNoticeItem(const QJsonObject& obj, const QPixmap& pixmap, NoticeItemWidget::NoticeType type)
{
	auto listWidget = dynamic_cast<QListWidget*>(m_stackedWidget->currentWidget());
	auto item = new QListWidgetItem(listWidget);
	item->setSizeHint(QSize(listWidget->width(), 100));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new NoticeItemWidget(listWidget);
	itemWidget->setUser(obj, pixmap,type);
	//关联item和widget
	listWidget->setItemWidget(item, itemWidget);
}





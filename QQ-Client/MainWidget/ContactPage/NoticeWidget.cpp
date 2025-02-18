#include "NoticeWidget.h"
#include "NoticeWidget.h"
#include <QBoxLayout>
#include "FriendNoticeItemWidget.h"
#include <QFile>
#include "Client.h"
#include "ContactList.h"



NoticeWidget::NoticeWidget(QWidget* parent)
	:QWidget(parent)
	,m_stackedWidget(new QStackedWidget(this))
	,m_friendNoticeList(new QListWidget(this))
	,m_groupNoticeList(new QListWidget(this))
	,m_noticeTitle(new QLabel("好友通知", this))
{
	init();
	initStackedWidget();
	QFile file(":/stylesheet/Resource/StyleSheet/NoticeWidget.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}

}
NoticeWidget::~NoticeWidget()
{

}
NoticeWidget* NoticeWidget::instance()
{
	static NoticeWidget* ins;
	if (!ins)
	{
		ins = new NoticeWidget;
	}
	return ins;
}

void NoticeWidget::init()
{
	m_noticeTitle->setObjectName("titleLabel");
	auto mlayout = new QVBoxLayout(this);
	mlayout->addWidget(m_noticeTitle);
	mlayout->addWidget(m_stackedWidget);

	//好友申请通知界面
	connect(ContactList::instance(), &ContactList::friendNotice,[=]
		{
			m_noticeTitle->setText("好友通知");
			m_stackedWidget->setCurrentWidget(m_friendNoticeList);
		});
	//群聊申请通知界面
	connect(ContactList::instance(), &ContactList::groupNotice, [=]
		{
			m_noticeTitle->setText("群聊通知");
			m_stackedWidget->setCurrentWidget(m_groupNoticeList);
		});
	//好友申请通知
	connect(Client::instance(),&Client::addFriend, this, [=](const QJsonObject& obj)
		{
			addFreindNoticeItem(obj);
		});
	connect(Client::instance(), &Client::addGroup, this, [=](const QJsonObject& obj)
		{

		});
	//被拒绝通知
	connect(Client::instance(), &Client::rejectAddFriend, this, [=](const QJsonObject& obj)
		{
			addFreindNoticeItem(obj);
		});
}

void NoticeWidget::initStackedWidget()
{
	m_stackedWidget->addWidget(m_friendNoticeList);
	m_stackedWidget->addWidget(m_groupNoticeList);
}


void NoticeWidget::addFreindNoticeItem(const QJsonObject& obj)
{
	auto item = new QListWidgetItem(m_friendNoticeList);
	item->setSizeHint(QSize(m_friendNoticeList->width(), 100));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new FriendNoticeItemWidget(m_friendNoticeList);
	itemWidget->setUser(obj);
	//itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//关联item和widget
	m_friendNoticeList->setItemWidget(item, itemWidget);
}





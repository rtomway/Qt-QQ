#include "NoticeWidget.h"
#include "NoticeWidget.h"
#include <QBoxLayout>
#include "FriendNoticeItemWidget.h"



NoticeWidget::NoticeWidget(QWidget* parent)
	:QWidget(parent)
	,m_stackedWidget(new QStackedWidget(this))
	,m_friendNoticeList(new QListWidget(this))
	,m_groupNoticeList(new QListWidget(this))
	,m_noticeTitle(new QLabel("好友通知", this))
{
	init();
	initStackedWidget();
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
	auto mlayout = new QVBoxLayout(this);
	mlayout->addWidget(m_noticeTitle);
	mlayout->addWidget(m_stackedWidget);
	//通知界面切换
	connect(ContactList::instance(), &ContactList::friendNotice,[=]
		{
			m_noticeTitle->setText("好友通知");
			m_stackedWidget->setCurrentWidget(m_friendNoticeList);
		});
	connect(ContactList::instance(), &ContactList::groupNotice, [=]
		{
			m_noticeTitle->setText("群聊通知");
			m_stackedWidget->setCurrentWidget(m_groupNoticeList);
		});
	addFreindNoticeItem();
	addFreindNoticeItem();
	auto a = new FriendNoticeItemWidget();
	a->show();
}

void NoticeWidget::initStackedWidget()
{
	m_stackedWidget->addWidget(m_friendNoticeList);
	m_stackedWidget->addWidget(m_groupNoticeList);
}

void NoticeWidget::addFreindNoticeItem()
{
	auto item = new QListWidgetItem(m_friendNoticeList);
	item->setSizeHint(QSize(m_friendNoticeList->width(), 100));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new FriendNoticeItemWidget(m_friendNoticeList);
	//itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//关联item和widget
	m_friendNoticeList->setItemWidget(item, itemWidget);
}

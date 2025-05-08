#include "NoticeWidget.h"
#include <QBoxLayout>
#include <QFile>

#include "ContactListWidget.h"
#include "EventBus.h"
#include "TempManager.h"
#include "ItemWidget.h"
#include "FNoticeItemWidget.h"
#include "GNoticeItemWidget.h"


NoticeWidget::NoticeWidget(QWidget* parent)
	:QWidget(parent)
	, m_stackedWidget(new QStackedWidget(this))
	, m_friendNoticeList(new QListWidget(this))
	, m_groupNoticeList(new QListWidget(this))
	, m_noticeTitle(new QLabel(this))
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
	m_friendNoticeList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_friendNoticeList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_groupNoticeList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_groupNoticeList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	auto mlayout = new QVBoxLayout(this);
	mlayout->addWidget(m_noticeTitle);
	mlayout->addWidget(m_stackedWidget);

	//好友申请
	connect(TempManager::instance(), &TempManager::friendAddRequest, this, [=](const QString& user_id)
		{
			addNoticeItem(user_id, ChatType::User, false);
			emit friendNotice();
		});
	//拒绝好友申请
	connect(TempManager::instance(), &TempManager::friendAddReply, this, [=](const QString& user_id)
		{
			addNoticeItem(user_id, ChatType::User, true);
			emit friendNotice();
		});
	//群聊邀请失败
	connect(TempManager::instance(), &TempManager::rejectAddGroup, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group, true);
			emit groupNotice();
		});
	//群聊申请失败
	connect(TempManager::instance(), &TempManager::groupAddFailed, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group, true);
			emit groupNotice();
		});
	//群聊邀请
	connect(TempManager::instance(), &TempManager::GroupInvite, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group, false);
			this->emit groupNotice();
		});
	//群聊邀请成功
	connect(TempManager::instance(), &TempManager::GroupInviteSuccess, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group, true);
			this->emit groupNotice();
		});
	//申请加入群聊
	connect(TempManager::instance(), &TempManager::groupAddRequest, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group, false);
			emit groupNotice();
		});
	//群成员退出群聊
	connect(TempManager::instance(), &TempManager::groupMemberExitGroup, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group,true);
			emit groupNotice();
		});
	//被好友移除
	connect(TempManager::instance(), &TempManager::friendDeleted, this, [=](const QString& user_id)
		{
			addNoticeItem(user_id, ChatType::User, true);
			emit friendNotice();
		});
}
//init堆栈窗口
void NoticeWidget::initStackedWidget()
{
	m_stackedWidget->addWidget(m_friendNoticeList);
	m_stackedWidget->addWidget(m_groupNoticeList);
}
//添加通知项
void NoticeWidget::addNoticeItem(const QString& id, ChatType type, bool isReply)
{
	ItemWidget* itemWidget = nullptr;
	if (type == ChatType::User)
	{
		m_stackedWidget->setCurrentWidget(m_friendNoticeList);
		itemWidget = new FNoticeItemWidget(this);
		auto fItemWidget = dynamic_cast<FNoticeItemWidget*>(itemWidget);
		fItemWidget->setMode(isReply);
	}
	else if (type == ChatType::Group)
	{
		m_stackedWidget->setCurrentWidget(m_groupNoticeList);
		itemWidget = new GNoticeItemWidget(this);
		auto gItemWidget = dynamic_cast<GNoticeItemWidget*>(itemWidget);
		gItemWidget->setMode(isReply);
	}
	itemWidget->setItemWidget(id);
	auto listWidget = dynamic_cast<QListWidget*>(m_stackedWidget->currentWidget());
	auto item = new QListWidgetItem(listWidget);
	item->setSizeHint(QSize(listWidget->width(), 100));
	listWidget->setItemWidget(item, itemWidget);
}
//清空
void NoticeWidget::clearNoticeWidget()
{
	m_friendNoticeList->clear();
	m_groupNoticeList->clear();
}





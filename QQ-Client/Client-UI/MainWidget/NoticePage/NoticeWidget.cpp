﻿#include "NoticeWidget.h"
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
void NoticeWidget::setCurrentWidget(NoticeCurrentWidget noticeWidget)
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
	this->setObjectName("NoticeWidget");
	m_noticeTitle->setObjectName("titleLabel");
	m_friendNoticeList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_friendNoticeList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_groupNoticeList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_groupNoticeList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	auto mlayout = new QVBoxLayout(this);
	mlayout->addWidget(m_noticeTitle);
	mlayout->addWidget(m_stackedWidget);

	//群组请求通知
	connect(TempManager::instance(), &TempManager::notice_groupRequest, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group, false);
			emit groupNotice();
		});
	//群组回复通知
	connect(TempManager::instance(), &TempManager::notice_groupReply, this, [=](const QString& group_id)
		{
			addNoticeItem(group_id, ChatType::Group, true);
			emit groupNotice();
		});
	//好友请求通知
	connect(TempManager::instance(), &TempManager::notice_friendRequest, this, [=](const QString& user_id)
		{
			addNoticeItem(user_id, ChatType::User, false);
			emit friendNotice();
		});
	//好友回复通知
	connect(TempManager::instance(), &TempManager::notice_friendReply, this, [=](const QString& user_id)
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





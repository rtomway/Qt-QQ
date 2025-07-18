﻿#include "SearchItemWidget.h"
#include "ui_SearchItemWidget.h"
#include <QJsonObject>
#include <QPointer>
#include <QtConcurrent/QtConcurrent>
#include <memory>

#include "AddWidget.h"
#include "ImageUtil.h"
#include "FriendManager.h"
#include "GroupManager.h"


SearchItemWidget::SearchItemWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::SearchItemWidget)
{
	ui->setupUi(this);
	init();
}

SearchItemWidget::~SearchItemWidget()
{
	delete ui;
}

void SearchItemWidget::init()
{
	//添加
	connect(ui->addBtn, &QPushButton::clicked, [=]
		{
			switch (m_type)
			{
			case ChatType::User:
			{
				if (m_isAdded)
				{
					FriendManager::instance()->emit chatWithFriend(m_search_id);
					return;
				}
				QPointer<AddWidget>addWidget = new AddWidget();
				addWidget->setUser(this->getUser(), m_searchPix);
				addWidget->show();
			}
			break;
			case ChatType::Group:
			{
				if (m_isAdded)
				{
					GroupManager::instance()->emit chatWithGroup(m_search_id);
					return;
				}
				QPointer<AddWidget>addWidget = new AddWidget();
				addWidget->setGroup(this->getGroup(), m_searchPix);
				addWidget->show();
			}
			break;
			default:
				break;
			}
		});
}

//设置用户信息
void SearchItemWidget::setUser(const QJsonObject& obj)
{
	m_type = ChatType::User;
	m_searchName = obj["username"].toString();
	m_search_id = obj["user_id"].toString();
	ui->nameLab->setText(m_searchName);
	ui->idLab->setText(m_search_id);
	if (obj.contains("hasAdd"))
	{
		m_isAdded = obj["hasAdd"].toBool();
	}
	if (m_isAdded)
	{
		ui->addBtn->setText("发消息");
	}
	else
	{
		ui->addBtn->setText("添加");
	}
}

//设置群组
void SearchItemWidget::setGroup(const QJsonObject& obj)
{
	m_type = ChatType::Group;
	m_search_id = obj["group_id"].toString();
	m_searchName = obj["group_name"].toString();
	ui->idLab->setText(m_search_id);
	ui->nameLab->setText(m_searchName);
	if (obj.contains("hasAdd"))
	{
		m_isAdded = obj["hasAdd"].toBool();
	}
	if (m_isAdded)
	{
		ui->addBtn->setText("发消息");
	}
	else
	{
		ui->addBtn->setText("加入");
	}
}

//获取群组信息
QJsonObject SearchItemWidget::getGroup()
{
	QJsonObject obj;
	obj["group_id"] = m_search_id;
	obj["group_name"] = m_searchName;
	obj["isSend"] = true;
	return obj;
}

//设置头像
void SearchItemWidget::setPixmap(const QPixmap& pixmap)
{
	//占位图
	QPixmap placeholder = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/default.png"), QSize(40, 40));
	ui->headLab->setPixmap(placeholder);

	//启动异步任务
	QFuture<QPixmap> future = QtConcurrent::run([=]() {
		return ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
		});

	QFutureWatcher<QPixmap>* watcher = new QFutureWatcher<QPixmap>(this);
	connect(watcher, &QFutureWatcher<QPixmap>::finished, this, [this, watcher]()
		{
			m_searchPix = watcher->result();
			ui->headLab->setPixmap(m_searchPix);
			watcher->deleteLater();
		});
	watcher->setFuture(future);
}

//用户信息获取
QJsonObject SearchItemWidget::getUser()
{
	QJsonObject obj;
	obj["user_id"] = m_search_id;
	obj["username"] = m_searchName;
	obj["isSend"] = true;
	return obj;
}


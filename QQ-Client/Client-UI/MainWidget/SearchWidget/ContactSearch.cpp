#include "ContactSearch.h"
#include <QBoxLayout>
#include <QFile>

#include "SearchItemWidget.h"
#include "FriendManager.h"
#include "GroupManager.h"
#include "AvatarManager.h"
#include "ImageUtil.h"

ContactSearch::ContactSearch(QWidget* parent)
	:QWidget(parent)
	, m_friendBtn(new QPushButton("好友", this))
	, m_groupBtn(new QPushButton("群聊", this))
	, m_searchList(new QListWidget(this))
{
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/SearchFriend.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
}

void ContactSearch::init()
{
	this->setFocusPolicy(Qt::StrongFocus);
	m_searchList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	auto mlayout = new QVBoxLayout(this);
	QWidget* switchWidget = new QWidget(this);
	auto hlayout = new QHBoxLayout(switchWidget);
	hlayout->addWidget(m_friendBtn);
	hlayout->addWidget(m_groupBtn);

	mlayout->addWidget(switchWidget);
	mlayout->addWidget(m_searchList);

	m_friendBtn->setChecked(true);
	m_friendBtn->setStyleSheet(
		"QPushButton{background-color:rgb(240,240,240)}"
	);
	connect(m_friendBtn, &QPushButton::clicked, this, [=]
		{
			m_isFriendBtn = true;
			m_searchList->clear();
			m_friendBtn->setStyleSheet(
				"QPushButton{background-color:rgb(240,240,240)}"
			);
			m_groupBtn->setStyleSheet(
				"QPushButton{background-color:white}"
				"QPushButton:hover{background-color:rgb(240,240,240)}"
			);
		});
	connect(m_groupBtn, &QPushButton::clicked, this, [=]
		{
			m_isFriendBtn = false;
			m_searchList->clear();
			m_groupBtn->setStyleSheet(
				"QPushButton{background-color:rgb(240,240,240)}"
			);
			m_friendBtn->setStyleSheet(
				"QPushButton{background-color:white}"
				"QPushButton:hover{background-color:rgb(240,240,240)}"
			);
		});

}

//搜索
void ContactSearch::searchText(const QString& text)
{
	this->clearFriendList();
	if (m_isFriendBtn)
	{
		searchFriend(text);
	}
	else
	{
		searchGroup(text);
	}
}

//好友搜索
void ContactSearch::searchFriend(const QString& text)
{
	auto searchResultHash = FriendManager::instance()->findFriends(text);
	if (searchResultHash.isEmpty())
	{
		qWarning() << "No friends found for text: " << text;
		return;
	}
	for (auto it = searchResultHash.begin(); it != searchResultHash.end(); ++it)
	{
		if (!it.value())
		{
			qWarning() << "Invalid QSharedPointer!";
			continue;
		}
		auto& obj = it.value()->getFriend();
		AvatarManager::instance()->getAvatar(it.value()->getFriendId(), ChatType::User, [=](const QPixmap& pixmap)
			{
				auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
				this->addSearchItem(obj, headPix);
			});
	}
}

//群组搜索
void ContactSearch::searchGroup(const QString& text)
{
	auto searchResultHash = GroupManager::instance()->searchGroups(text);
	if (searchResultHash.isEmpty())
	{
		qWarning() << "No friends found for text: " << text;
		return;
	}
	for (auto it = searchResultHash.begin(); it != searchResultHash.end(); ++it)
	{
		if (!it.value())
		{
			qWarning() << "Invalid QSharedPointer!";
			continue;
		}
		auto& obj = it.value()->getGroupProfile();
		AvatarManager::instance()->getAvatar(it.value()->getGroupId(), ChatType::Group, [=](const QPixmap& pixmap)
			{
				auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
				this->addSearchItem(obj, headPix);
			});
	}
}

//添加搜索项
void ContactSearch::addSearchItem(const QJsonObject& obj, const QPixmap& pixmap)
{
	//为item设置用户id
	auto item = new QListWidgetItem(m_searchList);
	item->setSizeHint(QSize(m_searchList->width(), 70));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new SearchItemWidget(m_searchList);
	if (m_isFriendBtn)
	{
		itemWidget->setUser(obj);
	}
	else
	{
		itemWidget->setGroup(obj);
	}
	itemWidget->setPixmap(pixmap);
	//关联item和widget
	m_searchList->setItemWidget(item, itemWidget);
}

//清空搜索列表
void ContactSearch::clearFriendList()
{
	m_searchList->clear();
}

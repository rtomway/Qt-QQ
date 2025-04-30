#include "SearchFriend.h"
#include <QBoxLayout>
#include <QFile>

#include "SearchItemWidget.h"

SearchFriend::SearchFriend(QWidget* parent)
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
void SearchFriend::init()
{
	this->setFocusPolicy(Qt::StrongFocus);
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
//添加搜索项
void SearchFriend::addSearchItem(const QJsonObject& obj, const QPixmap& pixmap)
{
	//为item设置用户id
	auto item = new QListWidgetItem(m_searchList);
	item->setSizeHint(QSize(m_searchList->width(), 70));
	//将用户相关信息添加到item关联窗口
	auto itemWidget = new SearchItemWidget(ChatType::User,m_searchList);
	itemWidget->setUser(obj);
	itemWidget->setPixmap(pixmap);
	//关联item和widget
	m_searchList->setItemWidget(item, itemWidget);
}
//清空搜索列表
void SearchFriend::clearFriendList()
{
	m_searchList->clear();
}

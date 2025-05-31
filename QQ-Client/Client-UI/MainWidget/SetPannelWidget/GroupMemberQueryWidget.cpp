#include "GroupMemberQueryWidget.h"
#include "GroupManager.h"
#include "Group.h"
#include <QBoxLayout>

GroupMemberQueryWidget::GroupMemberQueryWidget(QWidget* parent)
	:QWidget(parent)
	,m_backBtn(new QPushButton(this))
	,m_searchLine(new QLineEdit(this))
	,m_memberList(new QListWidget(this))
{
	init();
	this->setStyleSheet(R"(
		QPushButton{
			font-size:18px;
		}
		QLineEdit{
			border: none;
			background-color: rgb(250,250,250);
			border-radius: 5px;
			height:30px
		}
		QListWidget{
			
		}
		)");
}

GroupMemberQueryWidget::~GroupMemberQueryWidget()
{

}

//加载群成员列表
void GroupMemberQueryWidget::loadGroupMemberList(const QString& group_id)
{
	m_group_id = group_id;
}

void GroupMemberQueryWidget::init()
{
	auto mlayout = new QVBoxLayout(this);
	auto hlayout = new QHBoxLayout();
	mlayout->addLayout(hlayout);
	m_backBtn->setText("<返回群组");
	hlayout->addWidget(m_backBtn);
	hlayout->addStretch();

	m_searchLine->setPlaceholderText("搜索");
	mlayout->addWidget(m_searchLine);
	mlayout->addWidget(m_memberList);

	connect(m_backBtn, &QPushButton::clicked, this, [=]
		{
			emit backGroupPannel();
		});
}

//添加群成员项
void GroupMemberQueryWidget::addListItemWidget(const QString& groupMember_id)
{

}

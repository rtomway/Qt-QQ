#include "GroupMemberQueryWidget.h"
#include <QBoxLayout>

#include "GroupManager.h"
#include "Group.h"
#include "GroupMemberItemWidget.h"


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
		QListWidget::item:hover{
			background-color: rgb(220,220,220);
		}
		QListWidget QWidget{
			background-color: transparent;
		}
		)");
}

GroupMemberQueryWidget::~GroupMemberQueryWidget()
{

}

//加载群成员列表
void GroupMemberQueryWidget::loadGroupMemberList(const QString& group_id)
{
	clearMemberListItem();
	m_group_id = group_id;
	auto group= GroupManager::instance()->findGroup(m_group_id);
	auto groupmember_idList = group->getGroupMembersIdList();
	for (auto& groupMember_id : groupmember_idList)
	{
		addListItemWidget(groupMember_id);
	}

	auto& groupMemberHash = group->getMembers();
	for (auto it = groupMemberHash.cbegin(); it != groupMemberHash.cend(); ++it)
	{
		auto& member = it.value();
		m_idList.append(member.member_id);
		m_nameList.append(member.member_name);
		m_groupHash[member.member_id] = member.member_name;
	}
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

	//返回群面板
	connect(m_backBtn, &QPushButton::clicked, this, [=]
		{
			emit backGroupPannel();
		});
	//群成员搜索
	connect(m_searchLine, &QLineEdit::textChanged, this, [=](const QString&text)
		{
			if (text.isEmpty())
			{
				loadGroupMemberList(m_group_id);
				return;
			}
				
			auto match_list = matchText(text);
			clearMemberListItem();
			for (auto& id : match_list)
			{
				addListItemWidget(id);
			}
			update();
		});
}

//添加群成员项
void GroupMemberQueryWidget::addListItemWidget(const QString& groupMember_id)
{
	auto item = new QListWidgetItem(m_memberList);
	item->setSizeHint(QSize(m_memberList->width(), 60));
	auto itemWidget = new GroupMemberItemWidget(m_group_id, m_memberList);
	itemWidget->setItemWidget(groupMember_id);
	m_memberList->setItemWidget(item, itemWidget);
}

//清空群成员项
void GroupMemberQueryWidget::clearMemberListItem()
{
	m_memberList->clear();
}

//搜索匹配结果
QStringList GroupMemberQueryWidget::matchText(const QString& text)
{
	QStringList match_list;

	for (auto it=m_groupHash.cbegin();it!=m_groupHash.cend();++it)
	{
		if (it.key().contains(text) || it.value().contains(text))
		{
			match_list.append(it.key());
		}
	}
	return match_list;
}



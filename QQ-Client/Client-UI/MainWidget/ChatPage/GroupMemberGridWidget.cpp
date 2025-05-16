#include "GroupMemberGridWidget.h"
#include <QPushButton>
#include <QGridLayout>
#include "GroupManager.h"


GroupMemberGridWidget::GroupMemberGridWidget(QWidget* parent)
	: QWidget(parent)
{
	this->setObjectName("GroupMemberGrid");
	this->setAttribute(Qt::WA_StyledBackground, true);
	this->setStyleSheet(R"(
			QWidget#GroupMemberGrid
			{background-color:white;border:none;border-radius:10px;}
			QWidget QWidget
			{background-color:transparent;border:none;border-radius:10px;}
			QWidget QLabel
			{background-color:transparent;}
			QWidget QPushButton
			{background-color:transparent;color:gray;}
		)");
}

GroupMemberGridWidget::~GroupMemberGridWidget()
{

}

void GroupMemberGridWidget::setGroupMembersGrid(const QString& group_id)
{
	m_group = GroupManager::instance()->findGroup(group_id);
	init();
}

void GroupMemberGridWidget::init()
{
	if (!m_group)
		return;
	//总布局
	auto mLayout = new QVBoxLayout(this);
	this->setLayout(mLayout);
	mLayout->setContentsMargins(6, 6, 6, 6);
	mLayout->setSpacing(10);
	//title
	auto hLayout = new QHBoxLayout();
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(0);
	auto titleLab = new QLabel("群聊成员", this);
	auto queryMoreBtn = new QPushButton(this);
	queryMoreBtn->setText("查看更多成员>");
	hLayout->addWidget(titleLab);
	hLayout->addStretch();
	hLayout->addWidget(queryMoreBtn);
	mLayout->addLayout(hLayout);
	//群成员
	const auto& members = m_group->getMembers();
	// 每行5个成员
	const int rowMemberCount = 5;
	int row = 0, column = 0;
	auto gridLayout = new QGridLayout();
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(6);
	//紧凑
	gridLayout->setAlignment(Qt::AlignTop);
	for (const auto& member : members)
	{
		auto memberAvatarItem = new GroupMemberAvatarWidget(member.member_id, member.member_name,MemberWidgetType::Avatar, this);
		m_avatarList.append(memberAvatarItem);
		//将群成员头像添加到布局中
		gridLayout->addWidget(memberAvatarItem, row, column);
		column++;
		if (column > rowMemberCount)
		{
			column %= rowMemberCount;
			row++;
		}
		if (row == 3 && column == 4)
		{
			column++;
			break;
		}	
	}
	//网格末尾添加邀请按钮
	auto inviteItem = new GroupMemberAvatarWidget(QString(),"邀请", MemberWidgetType::Invite, this);
	m_avatarList.append(inviteItem);
	gridLayout->addWidget(inviteItem, row, column);
	mLayout->addLayout(gridLayout);
}


void GroupMemberGridWidget::clearGroupMemberGrid()
{
	
}

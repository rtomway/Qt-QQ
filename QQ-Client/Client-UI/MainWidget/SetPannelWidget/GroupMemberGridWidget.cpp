#include "GroupMemberGridWidget.h"
#include <QPushButton>
#include "GroupManager.h"
#include "GroupInviteWidget.h"


GroupMemberGridWidget::GroupMemberGridWidget(QWidget* parent)
	: QWidget(parent)
	, m_layout(new QVBoxLayout(this))
	, m_gridLayout(new QGridLayout())
{
	init();
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

//设置群成员面板数据
void GroupMemberGridWidget::setGroupMembersGrid(const QString& group_id)
{
	m_group = GroupManager::instance()->findGroup(group_id);
	loadGridMember();
	if (!m_groupInviteWidget)
	{
		m_groupInviteWidget = new GroupInviteWidget(group_id,this);
	}
	m_groupInviteWidget->loadData();
}

void GroupMemberGridWidget::init()
{
	//总布局
	m_layout->setContentsMargins(6, 6, 6, 6);
	m_layout->setSpacing(10);
	//title
	auto hLayout = new QHBoxLayout();
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(0);
	auto titleLab = new QLabel("群聊成员", this);
	auto queryMoreBtn = new QPushButton(this);
	queryMoreBtn->setText("查看更多成员>");
	connect(queryMoreBtn, &QPushButton::clicked, this, [=]
		{
			emit queryMoreGroupMember();
		});
	hLayout->addWidget(titleLab);
	hLayout->addStretch();
	hLayout->addWidget(queryMoreBtn);
	m_layout->addLayout(hLayout);
	m_layout->addLayout(m_gridLayout);

	
}

//成员加载
void GroupMemberGridWidget::loadGridMember()
{
	if (!m_group)
		return;
	clearGroupMemberGrid();
	//群成员
	const auto& members = m_group->getMembers();
	// 每行4个成员
	const int rowMemberCount = 4;
	int row = 0, column = 0;
	m_gridLayout->setContentsMargins(0, 0, 0, 0);
	m_gridLayout->setSpacing(6);
	//紧凑
	m_gridLayout->setAlignment(Qt::AlignTop);
	for (const auto& member : members)
	{
		auto memberAvatarItem = new GroupMemberAvatarWidget(m_group->getGroupId(), member.member_id, member.member_name, MemberWidgetType::Avatar, this);
		m_avatarList.append(memberAvatarItem);
		//将群成员头像添加到布局中
		m_gridLayout->addWidget(memberAvatarItem, row, column);
		column++;
		if (column >= rowMemberCount)
		{
			column %= rowMemberCount;
			row++;
		}
		if (row == 3 && column == 3)
		{
			column++;
			break;
		}
	}
	//网格末尾添加邀请按钮
	auto inviteItem = new GroupMemberAvatarWidget(m_group->getGroupId(), QString(), "邀请", MemberWidgetType::Invite, this);
	m_avatarList.append(inviteItem);
	m_gridLayout->addWidget(inviteItem, row, column);

	connect(inviteItem, &GroupMemberAvatarWidget::groupInvite, this, [=](const QString&group_id)
		{
			m_groupInviteWidget->loadData();
			//蒙层
			SMaskWidget::instance()->popUp(m_groupInviteWidget);
			auto mainWidgetSize = SMaskWidget::instance()->getMainWidgetSize();
			int x = (mainWidgetSize.width() - m_groupInviteWidget->width()) / 2;
			int y = (mainWidgetSize.height() - m_groupInviteWidget->height()) / 2;
			SMaskWidget::instance()->setPopGeometry(QRect(x, y, m_groupInviteWidget->width(), m_groupInviteWidget->height()));
		});

	emit heightChanged( this->sizeHint().height());
}

//清空成员item
void GroupMemberGridWidget::clearGroupMemberGrid()
{
	// 删除所有头像控件
	for (auto avatar : m_avatarList) {
		m_gridLayout->removeWidget(avatar); // 从布局中移除
		avatar->deleteLater();             // 安全删除
	}
	m_avatarList.clear();
}

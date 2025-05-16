#include "GroupMemberAvatarWidget.h"
#include <QBoxLayout>
#include <QMouseEvent>
#include <QJsonObject>
#include <QJsonDocument>

#include "ImageUtil.h"
#include "AvatarManager.h"
#include "GroupManager.h"
#include "Group.h"
#include "FriendManager.h"
#include "Friend.h"
#include "MessageSender.h"
#include "LoginUserManager.h"

GroupMemberAvatarWidget::GroupMemberAvatarWidget(const QString& groupMember_id, const QString& groupMember_name,MemberWidgetType type,QWidget* parent)
	:QWidget(parent)
	,m_avatarLab(new QLabel(this))
	,m_groupMember_id(groupMember_id)
	,m_nameLab(new QLabel(groupMember_name,this))
	,m_type(type)
{
	this->setAttribute(Qt::WA_Hover, true);  // 必须启用！
	this->setMouseTracking(true);  // 关键！启用鼠标跟踪
	this->setAttribute(Qt::WA_StyledBackground, true); // 必须启用
	this->installEventFilter(this);
	
	//鼠标点击穿透子控件
	m_avatarLab->setAttribute(Qt::WA_TransparentForMouseEvents, true);  
	m_nameLab->setAttribute(Qt::WA_TransparentForMouseEvents, true);    
	m_nameLab->setAlignment(Qt::AlignHCenter);
	switch (m_type)
	{
	case Avatar:
	{
		AvatarManager::instance()->getAvatar(groupMember_id, ChatType::User, [&](const QPixmap& pixmap)
			{
				m_avatarLab->setPixmap(ImageUtils::roundedPixmap(pixmap, QSize(30, 30)));
			});
	}	
		break;
	case Invite:
	{
		auto pixmap = QPixmap(":/picture/Resource/Picture/invite.png");
		m_avatarLab->setPixmap(ImageUtils::roundedPixmap(pixmap, QSize(30, 30)));
	}
		break;
	default:
		break;
	}
	
	
	init();
}

GroupMemberAvatarWidget::~GroupMemberAvatarWidget()
{
	qDebug() << "GroupMemberAvatarWidget destroyed: " << this;
}


void GroupMemberAvatarWidget::init()
{
	this->setFixedSize(40,65);
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_avatarLab->setFixedHeight(40);
	m_nameLab->setFixedHeight(15);
	auto vLayout = new QVBoxLayout(this);
	vLayout->setAlignment(Qt::AlignHCenter);  // 关键！水平居中
	vLayout->setContentsMargins(0,0,0,0);
	vLayout->setSpacing(0);
	vLayout->addWidget(m_avatarLab);
	vLayout->addWidget(m_nameLab);
	vLayout->addStretch();
}

void GroupMemberAvatarWidget::leftButtonPress()
{
	switch (m_type)
	{
	case Avatar:
	{
		m_userProfileWidget = std::make_unique<UserProfilePage>();
		bool isFriend = false;
		auto user = FriendManager::instance()->findFriend(m_groupMember_id);
		if (user)
		{
			isFriend = true;
			m_userProfileWidget->setUserProfilePage(user->getFriend(), isFriend);
			auto position = this->mapToGlobal(QPoint(0, 0));
			m_userProfileWidget->setGeometry(position.x() - m_userProfileWidget->width()+this->width(), position.y(),0,0);
			m_userProfileWidget->show();
			return;
		}
		//向服务器发送queryUser
		QJsonObject queryObj;
		queryObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
		queryObj["query_id"] = m_groupMember_id;
		QJsonDocument doc(queryObj);
		QByteArray data = doc.toJson(QJsonDocument::Compact);
		MessageSender::instance()->sendHttpRequest("queryUser", data, "application/json",
			[this,isFriend](const QJsonObject& params, const QByteArray& avatarData)
			{
				qDebug() << "接收数据:" << params;
				m_userProfileWidget->setUserProfilePage(params, isFriend);
				auto position = this->mapToGlobal(QPoint(0, 0));
				m_userProfileWidget->setGeometry(position.x() - m_userProfileWidget->width() + this->width(), position.y(), 0, 0);
				m_userProfileWidget->show();
			});
		
	}
		break;
	case Invite:
		break;
	default:
		break;
	}
}

void GroupMemberAvatarWidget::rightButtonPress()
{
	switch (m_type)
	{
	case Avatar:
		break;
	case Invite:
		break;
	default:
		break;
	}
}

bool GroupMemberAvatarWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::HoverEnter)
	{
		qDebug() << "hover";
		this->setStyleSheet("QWidget{background-color:rgb(240,240,240);}QLabel{background-color:transparent;}");
		return true;  // 事件已处理
	}
	else if (event->type() == QEvent::HoverLeave)
	{
		this->setStyleSheet("QWidget{background-color:transparent;}QLabel{background-color:transparent;}");
		return true;  // 事件已处理
	}
	
	if (event->type() == QEvent::MouseButtonPress)
	{
		qDebug() << "点击";
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() == Qt::LeftButton)
		{
			qDebug() << "左键";
			leftButtonPress();
		}
		else if (mouseEvent->button() == Qt::RightButton)
		{
			qDebug() << "右键";
			rightButtonPress();
		}
		return true;  // 事件已处理
	}
	return QWidget::eventFilter(watched, event);  // 其他事件交给父类
}

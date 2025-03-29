#include "FriendSelectedWidget.h"

#include "FriendManager.h"
#include "ImageUtil.h"
#include "AvatarManager.h"

FriendSelectedWidget::FriendSelectedWidget(QWidget* parent)
	:QWidget(parent)
	, m_layout(new QHBoxLayout(this))
	, m_selected(new QCheckBox(this))
	, m_headLab(new QLabel(this))
	, m_nameLab(new QLabel(this))
{
	init();
}
void FriendSelectedWidget::init()
{
	m_layout->addWidget(m_selected);
	m_layout->addWidget(m_headLab);
	m_layout->addWidget(m_nameLab);
	m_layout->addStretch();
	connect(m_selected, &QCheckBox::stateChanged, this, [=](bool isChecked)
		{
			emit checkFriend(isChecked);
		});
}

void FriendSelectedWidget::setUser(const QString& user_id)
{
	if (!m_oneself || m_oneself->getFriendId() != user_id)
	{
		m_oneself = FriendManager::instance()->findFriend(user_id);
	}
	m_json = m_oneself->getFriend();
	m_nameLab->setText(m_json["username"].toString());
	m_userId = user_id;
	auto avatar = AvatarManager::instance()->getAvatar(m_userId, ChatType::User);
	auto pixmap = ImageUtils::roundedPixmap(avatar, QSize(40, 40));
	m_headLab->setPixmap(pixmap);
	if (user_id == FriendManager::instance()->getOneselfID())
	{
		m_selected->setChecked(true);
		m_selected->setEnabled(false);
		m_selected->update();
	}
}

void FriendSelectedWidget::setGroupMember(const QString& member_id, const QString& memberName)
{
	m_nameLab->setText(memberName);
	m_userId = member_id;
	auto avatar = AvatarManager::instance()->getAvatar(member_id, ChatType::User);
	auto pixmap = ImageUtils::roundedPixmap(avatar, QSize(40, 40));
	m_headLab->setPixmap(pixmap);
}

const QString& FriendSelectedWidget::getUserId()
{
	return m_userId;
}

void FriendSelectedWidget::setChecked(bool isCheck)
{
	m_selected->setChecked(isCheck);
}

bool FriendSelectedWidget::isChecked()
{
	return m_selected->isChecked();
}



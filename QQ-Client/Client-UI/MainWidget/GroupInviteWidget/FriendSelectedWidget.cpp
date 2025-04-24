#include "FriendSelectedWidget.h"

#include "FriendManager.h"
#include "ImageUtil.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"

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
//用户信息设置
void FriendSelectedWidget::setUser(const QString& user_id)
{
	if (!m_oneself || m_oneself->getFriendId() != user_id)
	{
		m_oneself = FriendManager::instance()->findFriend(user_id);
	}
	m_json = m_oneself->getFriend();
	m_nameLab->setText(m_json["username"].toString());
	m_userId = user_id;
	AvatarManager::instance()->getAvatar(m_userId, ChatType::User, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
			m_headLab->setPixmap(headPix);
		});

	if (user_id == LoginUserManager::instance()->getLoginUserID())
	{
		m_selected->setChecked(true);
		m_selected->setEnabled(false);
		m_selected->update();
	}
}
//群成员设置
void FriendSelectedWidget::setGroupMember(const QString& member_id, const QString& memberName)
{
	m_nameLab->setText(memberName);
	m_userId = member_id;
	AvatarManager::instance()->getAvatar(m_userId, ChatType::User, [=](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
			m_headLab->setPixmap(headPix);
		});
}
//获取用户id
const QString& FriendSelectedWidget::getUserId()
{
	return m_userId;
}
//设置点击
void FriendSelectedWidget::setChecked(bool isCheck)
{
	m_selected->setChecked(isCheck);
}
//获取点击状态
bool FriendSelectedWidget::isChecked()
{
	return m_selected->isChecked();
}
//设置点击
void FriendSelectedWidget::setEnabled(bool status)
{
	m_selected->setEnabled(status);
	if (!status)
	{
		m_selected->setStyleSheet(R"(
		QCheckBox::indicator:checked {
			background: #c7e3f8; /* 勾选时的背景色 */
			border: 2px solid #c7e3f8;
			image: url(:/icon/Resource/Icon/check.png);
			})");
	}
}



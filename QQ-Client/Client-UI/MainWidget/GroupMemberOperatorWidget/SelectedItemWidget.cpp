#include "SelectedItemWidget.h"

#include "ImageUtil.h"
#include "AvatarManager.h"
#include "LoginUserManager.h"

SelectedItemWidget::SelectedItemWidget(const QString& user_id, const QString& username, QWidget* parent)
	: QWidget(parent)
	, m_layout(new QHBoxLayout(this))
	, m_selected(new QCheckBox(this))
	, m_headLab(new QLabel(this))
	, m_nameLab(new QLabel(this))
	, m_userId(user_id)
	, m_userName(username)
{
	init();
}

void SelectedItemWidget::init()
{
	//布局
	m_layout->addWidget(m_selected);
	m_layout->addWidget(m_headLab);
	m_layout->addWidget(m_nameLab);
	m_layout->addStretch();
	connect(m_selected, &QCheckBox::stateChanged, this, [=](bool isChecked)
		{
			emit checked(isChecked);
		});
	//数据插入
	m_nameLab->setText(m_userName);
	AvatarManager::instance()->getAvatar(m_userId, ChatType::User, [this](const QPixmap& pixmap)
		{
			auto headPix = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));
			m_headLab->setPixmap(headPix);
		});
}

//获取用户id
const QString& SelectedItemWidget::getUserId()const
{
	return m_userId;
}

//设置点击
void SelectedItemWidget::setChecked(bool isCheck)
{
	m_selected->setChecked(isCheck);
}

//获取点击状态
bool SelectedItemWidget::isChecked()
{
	return m_selected->isChecked();
}

//禁用点击
void SelectedItemWidget::setEnabled(bool status)
{
	m_selected->setChecked(true);
	m_selected->setEnabled(status);
	if (!status)
	{
		m_selected->setStyleSheet(R"(
		QCheckBox::indicator:checked {
			background: #c7e3f8; 
			border: 2px solid #c7e3f8;
			image: url(:/icon/Resource/Icon/check.png);
			})");
	}
}



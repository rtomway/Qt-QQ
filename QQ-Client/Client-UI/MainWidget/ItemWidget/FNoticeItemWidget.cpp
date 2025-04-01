#include "FNoticeItemWidget.h"
#include "ui_ItemWidget.h"
#include "TempManager.h"
#include "AvatarManager.h"
#include "ImageUtil.h"
#include <QBoxLayout>

FNoticeItemWidget::FNoticeItemWidget(QWidget* parent)
	:ItemWidget(parent)
	, m_noticeMessageLab(new QLabel(this))
	, m_timeLab(new QLabel(this))
	, m_okBtn(new QPushButton("同意", this))
	, m_cancelBtn(new QPushButton("拒绝", this))
{
	init();
	//同意好友添加
	connect(m_okBtn, &QPushButton::clicked, this, [=]
		{
			m_okBtn->setText("已同意");
			m_cancelBtn->setVisible(false);
			m_okBtn->setEnabled(false);
			QJsonObject replyObj = m_json;
			replyObj["isSend"] = false;
			m_addWidget = std::make_unique<AddWidget>(); // 使用智能指针
			m_addWidget->setUser(replyObj, ui->headLab->pixmap());
			m_addWidget->show();
		});
}

void FNoticeItemWidget::init()
{
	auto nameLayout = ui->nameWidget->layout();
	nameLayout->addWidget(m_noticeMessageLab);
	nameLayout->addWidget(m_timeLab);
	ui->rightWidget->setLayout(new QHBoxLayout(ui->rightWidget));
	auto rightLayout = ui->rightWidget->layout();
	rightLayout->addWidget(m_okBtn);
	rightLayout->addWidget(m_cancelBtn);
	m_okBtn->setFixedWidth(90);
	m_cancelBtn->setFixedWidth(90);
	ui->preMessageLab->setText("留言：");
}

void FNoticeItemWidget::setItemWidget(const QString& user_id)
{
	auto noticeData = TempManager::instance()->getFriendRequestInfo(user_id);
	m_json = noticeData.requestData;
	m_headPix = noticeData.avatar;
	auto headPix = ImageUtils::roundedPixmap(m_headPix, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
	ui->nameLab->setText(m_json["username"].toString());
	m_noticeMessageLab->setText(m_json["noticeMessage"].toString());
	m_timeLab->setText(m_json["time"].toString());
}



#include "SearchItemWidget.h"
#include "ui_SearchItemWidget.h"
#include "QJsonObject"
#include <QPointer>
#include "AddWidget.h"
#include "ImageUtil.h"


SearchItemWidget::SearchItemWidget(QWidget* parent)
	:QWidget(parent)
	, ui(new Ui::SearchItemWidget)
{
	ui->setupUi(this);
	init();


}

SearchItemWidget::~SearchItemWidget()
{
	delete ui;
}

void SearchItemWidget::init()
{
	connect(ui->addBtn, &QPushButton::clicked, [=]
		{
			QPointer<AddWidget>addWidget = new AddWidget();
			addWidget->setUser(this->getUser(), m_userHead);
			addWidget->show();
		});
}

void SearchItemWidget::setUser(const QJsonObject& obj)
{
	m_userName = obj["username"].toString();
	m_user_id = obj["user_id"].toString();
	ui->nameLab->setText(m_userName);
	ui->idLab->setText(m_user_id);
	ui->addBtn->setText("添加");
}

QJsonObject SearchItemWidget::getUser()
{
	QJsonObject obj;
	obj["user_id"] = m_user_id;
	obj["username"] = m_userName;
	obj["isSend"] = true;
	return obj;
}

void SearchItemWidget::setPixmap(const QPixmap& pixmap)
{
	m_userHead = pixmap;
	auto headPix = ImageUtils::roundedPixmap(m_userHead, QSize(40, 40));
	ui->headLab->setPixmap(headPix);
}

void SearchItemWidget::setGroup(const QJsonObject& obj)
{
	m_group_id = obj["group_id"].toString();
	m_groupName = obj["group_name"].toString();
	ui->idLab->setText(m_group_id);
	ui->nameLab->setText(m_groupName);
	ui->addBtn->setText("加入");
}

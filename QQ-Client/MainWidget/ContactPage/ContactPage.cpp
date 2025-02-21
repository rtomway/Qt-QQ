#include "ContactPage.h"
#include "ui_ContactPage.h"
#include "ImageUtil.h"
#include <QFile>
#include <QToolButton>
#include "SMaskWidget.h"
#include "User.h"


ContactPage::ContactPage(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::ContactPage)
	,m_detailEditWidget(new ContactDetailWidget)
{
	ui->setupUi(this);
	init();
	QFile file(":/stylesheet/Resource/StyleSheet/ContactPage.css");
	if (file.open(QIODevice::ReadOnly))
	{
		this->setStyleSheet(file.readAll());
	}
	else
	{
		qDebug() << file.fileName() << "打开失败";
	}
	
}

ContactPage::~ContactPage()
{
	delete ui;
}

void ContactPage::init()
{
	this->setObjectName("ContactPage");
	ui->headLab->setPixmap(ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/h2.jpg"), QSize(100,100)));
	ui->nameLab->setText("哈哈");
	ui->onlineBtn->setIcon(QIcon(":/icon/Resource/Icon/online.png"));
	ui->onlineBtn->setText("在线");
	ui->likeBtn->setIcon(QIcon(":/icon/Resource/Icon/like.png"));
	ui->likeBtn->setText(QString::number(m_likeCount));
	connect(ui->likeBtn, &QToolButton::clicked, [=]()
		{
			ui->likeBtn->setText(QString::number(++m_likeCount));
		});
	ui->genderBtn->setIcon(QIcon(":/icon/Resource/Icon/man.png"));
	ui->genderBtn->setText("男");
	ui->ageLab->setText(QString("25")+"岁");
	ui->friendGroupBtn->setIcon(QIcon(":/icon/Resource/Icon/friendgroup.png"));
	ui->signaltureBtn->setIcon(QIcon(":/icon/Resource/Icon/signalture.png"));
	ui->groupcomBox->addItem("我的好友");
		ui->groupcomBox->addItem("我的好友1");
	ui->groupcomBox->addItem("我的好友2");
	ui->groupcomBox->addItem("我的好友3");


	connect(ui->editdetailBtn, &QPushButton::clicked, [=]()
		{
			SMaskWidget::instance()->popUp(m_detailEditWidget);
			auto mainWidgetSize= SMaskWidget::instance()->getMainWidgetSize();
			int x = (mainWidgetSize.width() - m_detailEditWidget->width()) / 2;
			int y = (mainWidgetSize.height() - m_detailEditWidget->height()) / 2;
			SMaskWidget::instance()->setPopGeometry(QRect(x,y,this->width(),this->height()));
		});
}

void ContactPage::setUser(const QJsonObject& obj)
{
	m_json = obj;
	if (User::instance()->getUserId() != m_json["user_id"].toString())
	{
		ui->editdetailBtn->setVisible(false);
	}
	if (m_json["birthday"].toString().isEmpty())
	{
		ui->line_3->setVisible(false);
		ui->birthdayLab->setVisible(false);
	}
	if (m_json["resident"].toString().isEmpty())
	{
		ui->line_4->setVisible(false);
		ui->label_2->setVisible(false);
		ui->residentLab->setVisible(false);
	}
	ui->nameLab->setText(m_json["username"].toString());
	ui->idLab->setText(m_json["user_id"].toString());
	ui->genderBtn->setText(m_json["gender"].toBool() ? "男" : "女");
	ui->ageLab->setText(QString::number(m_json["age"].toInt()) + "岁");

}

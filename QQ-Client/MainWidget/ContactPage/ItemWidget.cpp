#include "ItemWidget.h"
#include "ui_ItemWidget.h"
#include <QRegion>
#include <QPainter>
#include <QPainterPath>
#include <ImageUtil.h>

ItemWidget::ItemWidget(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::ItemWidget)
{
	ui->setupUi(this);
	init();
}

ItemWidget::~ItemWidget()
{
	delete ui;
}

void ItemWidget::init()
{
	ui->nameLab->setStyleSheet(QString("QLabel{font-size:14px}"));
	QPixmap pixmap = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(40,40));
	ui->headLab->setPixmap(pixmap);
}

void ItemWidget::setName(QString name)
{
	ui->nameLab->setText(name);
}



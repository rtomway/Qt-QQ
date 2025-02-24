#include "TopItemWidget.h"
#include "TopItemWidget.h"
#include <QCoreApplication>
#include <QMouseEvent>

TopItemWidget::TopItemWidget(QWidget* parent)
	:QWidget(parent)
	, m_hLayout(new QHBoxLayout(this))
	, m_headLab(new RotationLabel(this))
	, m_nameLab(new QLabel(this))
	, m_countLab(new QLabel(QString::number(0), this))
	, m_item(nullptr)
{
	init();
}

TopItemWidget::~TopItemWidget()
{
	delete m_item;
}

void TopItemWidget::init()
{
	//m_headLab->setPixmap(QPixmap(":/icon/Resource/Icon/rightarrow.png"));
	//m_headLab->setFixedSize(15, 15);
	//m_headLab->setScaledContents(true);
	m_hLayout->addWidget(m_headLab);
	m_hLayout->addWidget(m_nameLab);
	m_hLayout->addStretch();
	m_hLayout->addWidget(m_countLab);

	//m_headLab->setStyleSheet(QString("QLabel:hover{background-color:white;}"));
}

void TopItemWidget::setCount(const int& x)
{
	m_countLab->setText(QString::number(x));
}

void TopItemWidget::setName(const QString& name)
{
	m_nameLab->setText(name);
}

QString TopItemWidget::getName()const
{
	return m_nameLab->text();
}

void TopItemWidget::setItem(QTreeWidgetItem* item)
{
	m_item = item;
}

QTreeWidgetItem* TopItemWidget::item()
{
	return m_item;
}

void TopItemWidget::setAgale()
{
	m_headLab->setAgale();
	// 创建一个鼠标按下事件
	QMouseEvent* pressEvent = new QMouseEvent(QEvent::MouseButtonPress,
		QPointF(m_headLab->width() / 2, m_headLab->height() / 2),
		Qt::LeftButton,
		Qt::LeftButton,
		Qt::NoModifier);

	// 发送鼠标按下事件给 QLabel
	QCoreApplication::sendEvent(m_headLab, pressEvent);

	// 手动删除事件对象
	delete pressEvent;
}

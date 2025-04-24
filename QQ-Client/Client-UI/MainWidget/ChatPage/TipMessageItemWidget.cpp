#include "TipMessageItemWidget.h"
#include <QBoxLayout>

TipMessageItemWidget::TipMessageItemWidget(const QString& text, QWidget* parent)
	:QWidget(parent)
	, m_label(new QLabel(text, this))
	, m_layout(new QHBoxLayout(this))
{
	this->setStyleSheet(R"(QWidget{background-color:red;})");
	init();
}

void TipMessageItemWidget::init()
{
	qDebug() << "-----------------TipMessageItemWidget";
	qDebug() << "TipMessageItemWidget" << this;
	m_label->setStyleSheet(R"(
		background-color:white;
		font:gray;
		border-radius:4px;
		width:40px;
	)");
	m_layout->addStretch();
	m_layout->addWidget(m_label);
	m_layout->addStretch();
}


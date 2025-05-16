#include "SetPannelWidget.h"
#include <QBoxLayout>

SetPannelWidget::SetPannelWidget(QWidget* parent)
	:QWidget(parent)
	,m_listWidget(new QListWidget(this))
{
	init();
}

void SetPannelWidget::init()
{
	this->setStyleSheet(R"(
		QWidget{ border: none;background-color:rgb(235,235,235);}
		QListWidget{ border: none;background-color:transparent; outline: none;}
		QListWidget::item {
        background: transparent;  /* 确保 item 透明 */
        border: none;
    }
		)");
	auto m_Layout = new QHBoxLayout(this);
	m_Layout->addWidget(m_listWidget);
	m_Layout->setContentsMargins(10, 10, 10, 10);
}

void SetPannelWidget::setId(const QString& id)
{
	m_pannelId = id;
}

void SetPannelWidget::addItemWidget(QWidget* itemWidget, int height)
{
	auto listItem = new QListWidgetItem(m_listWidget);
	listItem->setSizeHint(QSize(m_listWidget->width(), height));
	m_listWidget->setItemWidget(listItem, itemWidget);
	addSpaceItem();
}

void SetPannelWidget::addSpaceItem(int height)
{
	auto listItem = new QListWidgetItem(m_listWidget);
	listItem->setSizeHint(QSize(m_listWidget->width(), height));
}

void SetPannelWidget::clearListWidget()
{
	m_listWidget->clear();
}



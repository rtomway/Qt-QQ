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
		QWidget{ border: none;background-color:red;}
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

QListWidgetItem* SetPannelWidget::getItemByWidget(QWidget* widget) const
{
	if (!m_listWidget || !widget)
		return nullptr;

	const int count = m_listWidget->count();
	for (int i = 0; i < count; ++i)
	{
		QListWidgetItem* item = m_listWidget->item(i);
		QWidget* itemWidget = m_listWidget->itemWidget(item);
		if (itemWidget == widget)
			return item;
	}
	return nullptr;
}



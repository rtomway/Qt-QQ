#include "ImageMessageBubble.h"
#include <QPainter>

#define Text_yOffset 20

ImageMessageBubble::ImageMessageBubble(const MessageBubbleInitParams& bubbleInitParams, QWidget* parent)
	:MessageBubble(bubbleInitParams,parent)
{
	m_pixmapMessage = bubbleInitParams.data.value<QPixmap>();
}

void ImageMessageBubble::drawContent(QPainter& painter)
{
	// 绘制图片
	QRect imageRect = m_bubbleRect.adjusted(0, 10, 0, 0);
	painter.drawPixmap(imageRect, m_pixmapMessage);
}

void ImageMessageBubble::updateBubbleSize()
{
	//头像位置
	if (m_type == BubbleLeft)
	{
		m_profileRect.moveTopLeft(QPoint(_xOffset + 10, _yOffset));
	}
	else
	{
		m_profileRect.moveTopRight(QPoint(width() - _xOffset - 10, _yOffset));
	}

	//图片的发送
	if (m_pixmapMessage.isNull())
		return;

	// 按比例缩放图片 确定气泡区域大小
	const int maxWidth = this->width() / 3; // 限制最大宽度
	const int maxHeight = 300;

	QSize scaledSize = m_pixmapMessage.size().scaled(
		maxWidth,
		maxHeight,
		Qt::KeepAspectRatio
	);
	m_bubbleRect.setSize(scaledSize);

	// 调整位置
	if (m_type == BubbleLeft) 
	{
		m_bubbleRect.moveTopLeft(QPoint(m_profileRect.width() + 15, _yOffset));
	}
	else 
	{
		m_bubbleRect.moveTopRight(QPoint(width() - m_profileRect.width() - 15, _yOffset));
	}

	//设置高度
	if (m_bubbleRect.height() < 0) {
		m_bubbleRect.setHeight(0);
	}
	if (!m_groupMemberName.isEmpty())
	{
		m_bubbleRect.adjust(0, Text_yOffset, 0, 0);
		setFixedHeight(m_bubbleRect.height()  + Text_yOffset);
	}
	else
	{
		setFixedHeight(m_bubbleRect.height());
	}
}

void ImageMessageBubble::resizeEvent(QResizeEvent* ev)
{
	updateBubbleSize();
	auto listw = QListWidgetItem::listWidget();
	if (listw)
	{
		QListWidgetItem::setSizeHint(QSize(1, size().height()));
	}
	update();
}

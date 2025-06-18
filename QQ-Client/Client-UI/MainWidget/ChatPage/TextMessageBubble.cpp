#include "TextMessageBubble.h"
#include <QPainter>

#define SpaceWidth() (_xOffset + m_profileRect.width() + 5 * m_textMargin) // 增加额外边距
#define Text_yOffset 20

TextMessageBubble::TextMessageBubble(const MessageBubbleInitParams& bubbleInitParams, QWidget* parent)
	:MessageBubble(bubbleInitParams, parent)
{
	m_textMessage = bubbleInitParams.data.toString();

}

void TextMessageBubble::drawContent(QPainter& painter)
{
	// 绘制文字气泡
	painter.drawRoundedRect(m_bubbleRect, 10, 10);

	//绘制文本
	painter.setPen(Qt::black);
	QRect textRect = m_bubbleRect.adjusted(5, 5, -5, -5);
	painter.drawText(m_textRect, Qt::TextWordWrap, m_textMessage);

}

//气泡区域大小以及位置
void TextMessageBubble::updateBubbleSize()
{
	if (m_type == BubbleLeft)
	{
		m_profileRect.moveTopLeft(QPoint(_xOffset + 10, _yOffset));
	}
	else
	{
		m_profileRect.moveTopRight(QPoint(width() - _xOffset - 10, _yOffset));
	}
	//文本
	if (m_type == MessageBubble::BubbleLeft)
	{
		m_bubbleRect = m_textRect.adjusted(-m_textMargin, -m_textMargin, m_textMargin, m_textMargin);
	}
	else if (m_type == MessageBubble::BubbleRight)
	{
		m_bubbleRect = m_textRect.adjusted(-m_textMargin, -m_textMargin, m_textMargin, m_textMargin);
	}

	//保证高度不小于0
	if (m_bubbleRect.height() < 0) {
		m_bubbleRect.setHeight(0);
	}
	if (!m_groupMemberName.isEmpty())
	{
		setFixedHeight(m_bubbleRect.height() + m_textMargin + Text_yOffset);
	}
	else
	{
		setFixedHeight(m_bubbleRect.height() + m_textMargin);
	}

	setContentsMargins(m_textRect.x(), m_textRect.y(),
		width() - (m_textRect.right() + 1),
		height() - (m_textRect.bottom() + 1)
	);
}

//文本大小
void TextMessageBubble::updateTextRect()
{
	auto fm = this->fontMetrics();
	auto bRect = fm.boundingRect(QRect(), Qt::AlignLeft, m_textMessage);
	if (m_type == MessageBubble::BubbleLeft)
	{
		if (!m_groupMemberName.isEmpty())
		{
			m_textRect.moveTopLeft(QPoint(
				_xOffset + m_profileRect.width() + 3 * m_textMargin,
				_yOffset + m_textMargin + Text_yOffset
			));
		}
		else
		{
			m_textRect.moveTopLeft(QPoint(
				_xOffset + m_profileRect.width() + 3 * m_textMargin,
				_yOffset + m_textMargin
			));
		}

		if (bRect.width() <= width() - SpaceWidth())
		{
			m_textRect.setSize(bRect.size());
		}
		else
		{
			m_textRect.setWidth(width() - SpaceWidth());
			m_textRect.setHeight(realLineNumber() * fm.lineSpacing() + fm.lineSpacing() / 2);
		}
	}
	else if (m_type == MessageBubble::BubbleRight)
	{
		if (bRect.width() <= width() - SpaceWidth())
		{
			m_textRect.setX(width() - bRect.width() - m_profileRect.width() - 3 * m_textMargin);
			if (!m_groupMemberName.isEmpty())
			{
				m_textRect.setY(_yOffset + m_textMargin + Text_yOffset);
			}
			else
			{
				m_textRect.setY(_yOffset + m_textMargin);
			}
			m_textRect.setSize(bRect.size());
		}
		else
		{
			m_textRect.setX(_xOffset + 2 * m_textMargin);
			if (!m_groupMemberName.isEmpty())
			{
				m_textRect.setY(m_textMargin + Text_yOffset);
			}
			else
			{
				m_textRect.setY(m_textMargin);
			}

			m_textRect.setWidth(width() - SpaceWidth());
			m_textRect.setHeight(realLineNumber() * fm.lineSpacing() + fm.lineSpacing() / 2);
		}
	}
}

int  TextMessageBubble::textHeight()const
{
	return this->fontMetrics().boundingRect(QRect(), Qt::AlignLeft, m_textMessage).height();
}

void TextMessageBubble::resizeEvent(QResizeEvent* ev)
{
	updateTextRect();
	updateBubbleSize();

	auto listw = QListWidgetItem::listWidget();
	if (listw)
	{
		QListWidgetItem::setSizeHint(QSize(1, size().height()));
	}
	update();
}

QList<int> TextMessageBubble::textWidthList()const
{
	QList<int> list;
	auto fm = this->fontMetrics();

	qsizetype lineFeedpos = 0;		////\n的位置
	QString tstr = m_textMessage;
	for (int i = 0; i < tstr.length(); i = lineFeedpos + 1)
	{
		lineFeedpos = tstr.indexOf("\n", i);
		//最后不是\n，需要补加一行
		if (lineFeedpos == -1)
		{
			int curLineW = fm.boundingRect(QRect(), Qt::AlignLeft, tstr.mid(i)).width();
			list.append(curLineW);
			break;
		}
		else
		{
			int curLineW = fm.boundingRect(QRect(), Qt::AlignLeft, tstr.mid(i, lineFeedpos - i)).width();
			list.append(curLineW);
		}


	}
	return list;
}

int TextMessageBubble::lineNumber() const
{
	return 	textWidthList().size();
}

int TextMessageBubble::realLineNumber() const
{
	auto list = textWidthList();
	int nLine = list.size();
	for (auto w : list)
	{
		auto num = w / m_textRect.width();
		//只有一行
		if (num == 0)
		{

		}
		//多出一行(一行太长，容纳不下了，就换行了，就多了一行)
		else
		{
			nLine += num;
		}
	}
	return nLine;
}





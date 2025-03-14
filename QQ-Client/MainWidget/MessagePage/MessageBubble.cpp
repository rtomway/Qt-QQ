﻿#include"MessageBubble.h"
#include<QPainter>
#include<QResizeEvent>
#include<QPainterPath>

#define SpaceWidth() (_xOffset + m_profileRect.width() + 5 * m_textMargin)

MessageBubble::MessageBubble(QWidget* parent)
	:MessageBubble(QPixmap(), "xxxxxxxxxxxx")
{
}

MessageBubble::MessageBubble(const QPixmap& head_img, const QString& message, BubbleType type, QWidget* parent)
	:QLabel(parent)
	, m_type(type)
	, m_message(message)
	, m_head_img(head_img)
{
	//为英文字符时 70个一行
	int x = 0;
	for (int i = 0; i < m_message.length(); i++)
	{
		if (i % (x * 70 + 70 + x) == 0 && i != 0)
		{
			m_message.insert(i, '\n');
			x++;
		}
	}
	this->init();
	if (head_img.isNull())
	{
		m_head_img = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	QLabel::setText(m_message);
	setWordWrap(true);

}
void MessageBubble::init()
{
	setAlignment(Qt::AlignTop);
	setWordWrap(false);
	setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);
	QLabel::setFont(QFont("楷体", 14));
	if (m_type == MessageBubble::BubbleRight)
		this->setStyleSheet(R"(
				QLabel{
				color:white;
				}
				)");
	else
	{
		this->setStyleSheet(R"(
				QLabel{
				color:black;
				}
				)");
	}

	//头像矩形
	m_profileRect.setSize(QSize(38, 38));
}

//设置消息
void MessageBubble::setMessage(const QPixmap& head_img, const QString& message, MessageBubble::BubbleType type)
{
	m_type = type;
	m_message = message;
	m_head_img = head_img;
	QLabel::setText(m_message);
}

void MessageBubble::setHeadImage(const QPixmap& newHeadImg)
{
	m_head_img = newHeadImg;
	qDebug() << m_message;
	update();  // 触发重绘
	repaint();
}

int  MessageBubble::textHeight()const
{
	return this->fontMetrics().boundingRect(QRect(), Qt::AlignLeft, m_message).height();
}

MessageBubble::BubbleType MessageBubble::getType()
{
	return m_type;
}

void MessageBubble::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	painter.setPen(Qt::PenStyle::NoPen);
	if (m_type == MessageBubble::BubbleRight)
		painter.setBrush(QColor("#1079ef"));
	else
		painter.setBrush(QColor(Qt::white));

	//绘制头像	
	painter.drawPixmap(m_profileRect, m_head_img);

	//绘制气泡
	painter.drawRoundedRect(m_bubbleRect, 10, 10);


	//测试文本框
	if (0)
	{
		painter.setPen(Qt::GlobalColor::red);
		painter.setBrush(Qt::BrushStyle::NoBrush);
		painter.drawRect(m_textRect);
	}

	QLabel::paintEvent(ev);
}

void MessageBubble::resizeEvent(QResizeEvent* ev)
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

void MessageBubble::updateBubbleSize()
{
	if (m_type == MessageBubble::BubbleLeft)
	{
		m_profileRect.moveTopLeft(QPoint(_xOffset + 10, _yOffset));
		m_bubbleRect = m_textRect.adjusted(-m_textMargin, -m_textMargin, m_textMargin, m_textMargin);
	}
	else if (m_type == MessageBubble::BubbleRight)
	{
		m_profileRect.moveTopRight(QPoint(width() - _xOffset - 10, _yOffset));
		m_bubbleRect = m_textRect.adjusted(-m_textMargin, -m_textMargin, m_textMargin, m_textMargin);
	}
	//保证高度不小于0
	if (m_bubbleRect.height() < 0) {
		m_bubbleRect.setHeight(0);
	}
	setFixedHeight(m_bubbleRect.height() + m_textMargin);

	setContentsMargins(
		m_textRect.x(),
		m_textRect.y(),
		width() - (m_textRect.right() + 1),
		height() - (m_textRect.bottom() + 1)
	);
}

void MessageBubble::updateTextRect()
{
	auto fm = this->fontMetrics();
	auto bRect = fm.boundingRect(QRect(), Qt::AlignLeft, m_message);
	//m_textRect = rect();
	if (m_type == MessageBubble::BubbleLeft)
	{
		m_textRect.moveTopLeft(QPoint(
			_xOffset + m_profileRect.width() + 3 * m_textMargin,
			_yOffset + m_textMargin
		));

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
			m_textRect.setY(_yOffset + m_textMargin);;
			m_textRect.setSize(bRect.size());
		}
		else
		{
			m_textRect.setX(_xOffset + 2 * m_textMargin);
			m_textRect.setY(m_textMargin);

			m_textRect.setWidth(width() - SpaceWidth());
			m_textRect.setHeight(realLineNumber() * fm.lineSpacing() + fm.lineSpacing() / 2);
		}
	}
}

QList<int> MessageBubble::textWidthList()const
{
	QList<int> list;
	auto fm = this->fontMetrics();

	qsizetype lineFeedpos = 0;		////\n的位置
	QString tstr = m_message;
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

int MessageBubble::lineNumber() const
{
	return 	textWidthList().size();
}

int MessageBubble::realLineNumber() const
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

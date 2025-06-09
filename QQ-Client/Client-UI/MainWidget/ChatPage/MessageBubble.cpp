#include "MessageBubble.h"
#include <QPainter>
#include <QResizeEvent>
#include <QPainterPath>
#include <QMouseEvent>

#include "UserProfileDispatcher.h"

#define SpaceWidth() (_xOffset + m_profileRect.width() + 5 * m_textMargin)
#define Text_yOffset 20

MessageBubble::MessageBubble(QWidget* parent)
	:MessageBubble("", QPixmap(), "", MessageBubble::BubbleImageRight, "", "")
{

}

//文字构造
MessageBubble::MessageBubble(const QString& user_id,const QPixmap& head_img, const QString& message, BubbleType type, const QString& groupMemberName, const QString& groupRole, QWidget* parent)
	:QLabel(parent)
	, m_user_id(user_id)
	, m_type(type)
	, m_message(message)
	, m_head_img(head_img)
	, m_groupMemberName(groupMemberName)
	, m_groupRole(groupRole)
{
	if (type == BubbleTextLeft || type == BubbleTextRight)
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
	}
	else if (type == BubbleImageLeft || type == BubbleImageRight) {
		m_image.load(m_message); // 假设 message 是图片路径
	}

	this->init();
	if (head_img.isNull())
	{
		m_head_img = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	if (m_type == MessageBubble::BubbleTextLeft || m_type == MessageBubble::BubbleTextRight)
		QLabel::setText(m_message);
	setWordWrap(true);

}

//图片构造
MessageBubble::MessageBubble(const QString& user_id,const QPixmap& head_img, const QPixmap& pixmap, MessageBubble::BubbleType type, const QString& groupMemberName, const QString& groupRole, QWidget* parent)
	:QLabel(parent)
	, m_user_id(user_id)
	, m_type(type)
	, m_image(pixmap)
	, m_head_img(head_img)
	, m_groupMemberName(groupMemberName)
	, m_groupRole(groupRole)
{
	this->init();
	if (head_img.isNull())
	{
		m_head_img = QPixmap(":/picture/Resource/Picture/qq.png");
	}
}

void MessageBubble::init()
{
	this->installEventFilter(this);
	setAlignment(Qt::AlignTop);
	setWordWrap(false);
	setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);
	QLabel::setFont(QFont("楷体", 14));
	if (m_type == MessageBubble::BubbleTextRight)
		this->setStyleSheet(R"(QLabel{color:white;})");
	else if (m_type == MessageBubble::BubbleTextLeft)
	{
		this->setStyleSheet(R"(QLabel{color:black;})");
	}

	//头像矩形
	m_profileRect.setSize(QSize(38, 38));
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
	painter.setRenderHint(QPainter::TextAntialiasing, true); // 启用文本抗锯齿
	painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
	painter.setPen(Qt::PenStyle::NoPen);

	//文本颜色
	if (m_type == MessageBubble::BubbleTextRight || m_type == MessageBubble::BubbleImageRight)
		painter.setBrush(QColor("#1079ef"));
	else
		painter.setBrush(QColor(Qt::white));

	//绘制头像	
	painter.drawPixmap(m_profileRect, m_head_img);

	// 群聊消息显示名称和角色（左边消息）
	if (!m_groupMemberName.isEmpty()) {
		painter.save(); // 保存当前绘图状态

		// 设置名称字体
		QFont nameFont = painter.font();
		nameFont.setHintingPreference(QFont::PreferNoHinting);
		nameFont.setPointSize(12);
		painter.setFont(nameFont);
		painter.setPen(QColor(100, 100, 100));
		int nameX;
		int nameY;
		QFontMetrics fontMetrics(nameFont);  // 获取字体度量，用于计算文字宽度
		if (m_type == BubbleImageRight || m_type == BubbleTextRight)
		{
			// 计算名字的宽度
			int textWidth = fontMetrics.horizontalAdvance(m_groupMemberName);
			nameX = m_profileRect.left() - 10 - textWidth;
			nameY = m_profileRect.top();
			// 绘制成员名称
			painter.drawText(nameX, nameY + fontMetrics.ascent(), m_groupMemberName);
		}
		else
		{
			nameX = m_profileRect.right() + 10;
			nameY = m_profileRect.top();
			painter.drawText(nameX, nameY + painter.fontMetrics().ascent(), m_groupMemberName);
		}


		// 绘制角色（如果存在）
		if (!m_groupRole.isEmpty()) {
			QFont roleFont = nameFont;
			roleFont.setHintingPreference(QFont::PreferNoHinting);
			roleFont.setPointSize(12);
			painter.setFont(roleFont);
			int roleX;
			// 获取角色文本的宽度并加上间距
			int roleTextWidth = painter.fontMetrics().boundingRect(m_groupRole).width();
			int extraSpacing = 10;  // 额外的间距，确保两边有留白
			if (m_type == BubbleImageRight || m_type == BubbleTextRight)
			{
				roleX = nameX - roleTextWidth - extraSpacing - 2;  // 5 是额外的间距
			}
			else
			{
				roleX = nameX + painter.fontMetrics().boundingRect(m_groupMemberName).width() + 2;  // 5 是额外的间距
			}

			// 增加矩形宽度，并且保持一定的垂直对齐
			QRect roleRect(roleX, nameY, roleTextWidth + extraSpacing, painter.fontMetrics().height() + 4); // 4 是上下留白的空间

			// 绘制背景色（角色文本的背景）
			painter.setBrush(QColor(125, 190, 250));  // 设置背景色为红色
			painter.setPen(Qt::NoPen);  // 去掉边框
			painter.drawRoundedRect(roleRect, 6, 6); // 6, 6 是圆角半径

			// 绘制角色文本，确保文本显示在背景色上方
			painter.setPen(Qt::darkBlue);
			painter.drawText(roleRect.adjusted(5, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, m_groupRole);  // 左侧加 5px 让文字居中
		}
		painter.restore();
	}


	if ((m_type == BubbleImageLeft || m_type == BubbleImageRight) && !m_image.isNull()) {
		// 绘制图片
		QRect imageRect = m_bubbleRect.adjusted(0, 25, 0, 0);
		painter.drawPixmap(imageRect, m_image);
	}
	else {
		// 绘制文字气泡
		painter.drawRoundedRect(m_bubbleRect, 10, 10);
	}

	QLabel::paintEvent(ev);
}

void MessageBubble::resizeEvent(QResizeEvent* ev)
{
	if (m_type == BubbleTextLeft || m_type == BubbleTextRight)
		updateTextRect();
	updateBubbleSize();

	auto listw = QListWidgetItem::listWidget();
	if (listw)
	{
		QListWidgetItem::setSizeHint(QSize(1, size().height()));
	}
	update();
}

//气泡区域大小以及位置
void MessageBubble::updateBubbleSize()
{
	//文本
	if (m_type == MessageBubble::BubbleTextLeft)
	{
		m_profileRect.moveTopLeft(QPoint(_xOffset + 10, _yOffset));
		m_bubbleRect = m_textRect.adjusted(-m_textMargin, -m_textMargin, m_textMargin, m_textMargin);
	}
	else if (m_type == MessageBubble::BubbleTextRight)
	{
		m_profileRect.moveTopRight(QPoint(width() - _xOffset - 10, _yOffset));
		m_bubbleRect = m_textRect.adjusted(-m_textMargin, -m_textMargin, m_textMargin, m_textMargin);
	}
	//图片的发送
	if (m_type == BubbleImageLeft || m_type == BubbleImageRight) {
		int maxWidth = this->width() / 3; // 限制最大宽度
		if (m_image.isNull())
			return;

		// 按比例缩放图片 确定气泡区域大小
		QSize scaledSize = m_image.size().scaled(
			maxWidth,
			this->height(),
			Qt::KeepAspectRatio
		);
		m_bubbleRect.setSize(scaledSize);
		// 调整位置
		if (m_type == BubbleImageLeft) {
			m_bubbleRect.moveTopLeft(QPoint(m_profileRect.width() + 15, _yOffset));
			m_profileRect.moveTopLeft(QPoint(_xOffset + 10, _yOffset));
		}
		else {
			m_bubbleRect.moveTopRight(QPoint(width() - m_profileRect.width() - 15, _yOffset));
			m_profileRect.moveTopRight(QPoint(width() - _xOffset - 10, _yOffset));
		}
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
void MessageBubble::updateTextRect()
{
	auto fm = this->fontMetrics();
	auto bRect = fm.boundingRect(QRect(), Qt::AlignLeft, m_message);
	if (m_type == MessageBubble::BubbleTextLeft)
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
	else if (m_type == MessageBubble::BubbleTextRight)
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

bool MessageBubble::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint clikedPos = this->mapFromGlobal(mouseEvent->globalPos());
		if (m_profileRect.contains(clikedPos))
		{
			QPoint point=this->mapToGlobal(m_profileRect.topLeft());
			PopUpPosition position=PopUpPosition::Left;
			if (m_type == BubbleTextLeft || m_type == BubbleImageLeft)
			{
				position = PopUpPosition::Right;
				point =this->mapToGlobal(m_profileRect.topRight());
			}
			UserProfileDispatcher::instance()->showUserProfile(m_user_id, point, position);
			return true;
		}
	}
	return false;
}

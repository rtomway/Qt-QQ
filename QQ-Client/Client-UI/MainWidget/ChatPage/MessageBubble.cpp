#include "MessageBubble.h"
#include "MessageBubble.h"
#include "MessageBubble.h"
#include <QPainter>
#include <QResizeEvent>
#include <QPainterPath>
#include <QMouseEvent>

#include "UserProfileDispatcher.h"

MessageBubble::MessageBubble(const MessageBubbleInitParams& messageBubbleInitParams, QWidget* parent)
	:QLabel(parent)
	, m_user_id(messageBubbleInitParams.user_id)
	, m_head_img(messageBubbleInitParams.head_img)
	, m_groupMemberName(messageBubbleInitParams.group_memberName)
	, m_groupMemberRole(messageBubbleInitParams.group_memberRole)
	, m_type(messageBubbleInitParams.bubbleType)
	, m_messageData(messageBubbleInitParams.data)
{
	init();
}

void MessageBubble::init()
{
	this->installEventFilter(this);
	setAlignment(Qt::AlignTop);
	setWordWrap(false);
	setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);
	QLabel::setFont(QFont("楷体", 14));

	//头像矩形
	m_profileRect.setSize(QSize(38, 38));

}

void MessageBubble::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing, true); // 启用文本抗锯齿
	painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
	painter.setPen(Qt::PenStyle::NoPen);

	//文本颜色
	if (m_type == BubbleRight)
		painter.setBrush(QColor("#1079ef"));
	else
		painter.setBrush(QColor(Qt::white));

	//绘制头像	
	painter.drawPixmap(m_profileRect, m_head_img);

	// 群聊消息显示名称和角色（左边消息）
	if (!m_groupMemberName.isEmpty() && !m_groupMemberRole.isEmpty())
	{
		painter.save();			// 保存当前绘图状态
		drawGroupRole(painter);
		drawGroupName(painter);
		painter.restore();
	}
	//具体消息内容
	drawContent(painter);
	QLabel::paintEvent(ev);
}

void MessageBubble::drawGroupRole(QPainter& painter)
{
	// 设置名称字体
	QFont nameFont = painter.font();
	nameFont.setHintingPreference(QFont::PreferNoHinting);
	nameFont.setPointSize(12);
	painter.setFont(nameFont);
	painter.setPen(QColor(100, 100, 100));
	int nameX;
	int nameY;
	QFontMetrics fontMetrics(nameFont);  // 获取字体度量，用于计算文字宽度
	if (m_type == BubbleRight)
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
	m_memberNamePos = QPoint(nameX, nameY);
}

void MessageBubble::drawGroupName(QPainter& painter)
{
	auto nameX = m_memberNamePos.x();
	auto nameY = m_memberNamePos.y();
	// 设置名称字体
	QFont nameFont = painter.font();
	nameFont.setHintingPreference(QFont::PreferNoHinting);
	nameFont.setPointSize(12);
	QFont roleFont = nameFont;
	roleFont.setHintingPreference(QFont::PreferNoHinting);
	roleFont.setPointSize(12);
	painter.setFont(roleFont);
	int roleX;
	// 获取角色文本的宽度并加上间距
	int roleTextWidth = painter.fontMetrics().boundingRect(m_groupMemberRole).width();
	int extraSpacing = 10;
	if (m_type == BubbleRight)
	{
		roleX = nameX - roleTextWidth - extraSpacing - 2;
	}
	else
	{
		roleX = nameX + painter.fontMetrics().boundingRect(m_groupMemberName).width() + 2;  // 5 是额外的间距
	}

	// 增加矩形宽度，并且保持一定的垂直对齐
	QRect roleRect(roleX, nameY, roleTextWidth + extraSpacing, painter.fontMetrics().height() + 4); // 4 是上下留白的空间

	// 角色文本的背景
	painter.setBrush(QColor(125, 190, 250));
	painter.setPen(Qt::NoPen);
	painter.drawRoundedRect(roleRect, 6, 6);

	// 绘制角色文本
	painter.setPen(Qt::darkBlue);
	painter.drawText(roleRect.adjusted(5, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, m_groupMemberRole);  // 左侧加 5px 让文字居中

}


bool MessageBubble::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint clikedPos = this->mapFromGlobal(mouseEvent->globalPos());
		if (m_profileRect.contains(clikedPos))
		{
			QPoint point = this->mapToGlobal(m_profileRect.topLeft());
			PopUpPosition position = PopUpPosition::Left;
			if (m_type == BubbleLeft)
			{
				position = PopUpPosition::Right;
				point = this->mapToGlobal(m_profileRect.topRight());
			}
			UserProfileDispatcher::instance()->showUserProfile(m_user_id, point, position);
			return true;
		}
	}
	return false;
}

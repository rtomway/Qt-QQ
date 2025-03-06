#ifndef MESSAGEBUBBLE_H_
#define MESSAGEBUBBLE_H_

#include <QLabel>
#include <QListWidgetItem>
#include <QPixmap>
class MessageBubble : public QLabel, public QListWidgetItem
{
	Q_OBJECT
public:
	enum BubbleType
	{
		BubbleLeft,
		BubbleRight
	};
	explicit MessageBubble(QWidget* parent = nullptr);
	MessageBubble(const QPixmap& head_img, const QString& message, MessageBubble::BubbleType type = MessageBubble::BubbleRight, QWidget* parent = nullptr);
	void init();
	void setMessage(const QPixmap& head_img, const QString& message, MessageBubble::BubbleType type = MessageBubble::BubbleLeft);
	void setHeadImage(const QPixmap& newHeadImg);
	int textHeight()const;
	MessageBubble::BubbleType getType();
protected:
	void paintEvent(QPaintEvent* ev)override;
	void resizeEvent(QResizeEvent* ev)override;
	void updateBubbleSize();
	void updateTextRect();			//文本输出实际矩形区域
	QList<int> textWidthList()const;
	int lineNumber()const;			//消息文本总行数
	int realLineNumber()const;		//消息文本实际行数(不能完全显示，换行之后的)
private:
	BubbleType	m_type = BubbleRight;
	QRect		m_profileRect;		//头像矩形
	QRect		m_bubbleRect;		//气泡矩形
	int			m_textMargin = 10;	//文本距离气泡左边的距离
	QRect		m_textRect;			//文本矩形

	int _xOffset = 0;
	int _yOffset = 0;

	QPixmap m_head_img;
	QString m_message;
};



#endif // !MESSAGEBUBBLE_H_

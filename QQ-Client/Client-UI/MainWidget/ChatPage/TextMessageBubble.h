#ifndef TEXTMESSAGEBUBBLE_H_
#define TEXTMESSAGEBUBBLE_H_

#include "MessageBubble.h"

class TextMessageBubble :public MessageBubble
{
public:
	TextMessageBubble(const MessageBubbleInitParams&bubbleInitParams,QWidget* parent = nullptr);
private:
	void drawContent(QPainter&painter)override;
	void updateBubbleSize()override;
	void updateTextRect();			//文本输出实际矩形区域
	QList<int> textWidthList()const;
	int lineNumber()const;			//消息文本总行数
	int realLineNumber()const;		//消息文本实际行数(不能完全显示，换行之后的)
	int textHeight()const;
protected:
	void resizeEvent(QResizeEvent* ev)override;
private:
	int		m_textMargin = 10;	//文本距离气泡左边的距离
	QRect	m_textRect{};			//文本矩形
	QString m_textMessage;

};


#endif // !TEXTMESSAGEBUBBLE_H_

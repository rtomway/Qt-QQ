#ifndef IMAGEMESSAGEBUBBLE_H_
#define IMAGEMESSAGEBUBBLE_H_

#include "MessageBubble.h"
#include <QPixmap>

class ImageMessageBubble :public MessageBubble
{
public:
	ImageMessageBubble(const MessageBubbleInitParams& bubbleInitParams, QWidget* parent = nullptr);
private:
	void drawContent(QPainter& painter)override;
	void updateBubbleSize()override;
protected:
	void resizeEvent(QResizeEvent* ev)override;
private:
	QPixmap m_pixmapMessage;
};


#endif // !TEXTMESSAGEBUBBLE_H_

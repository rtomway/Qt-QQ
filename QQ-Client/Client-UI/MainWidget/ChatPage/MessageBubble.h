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

	struct MessageBubbleInitParams
	{
		QString user_id;
		QPixmap head_img;
		QString group_memberName;
		QString group_memberRole;
		QVariant data;
		MessageBubble::BubbleType bubbleType;
	};
	
	explicit MessageBubble(const MessageBubbleInitParams& messageBubbleInitParams,QWidget* parent = nullptr);
	MessageBubble(const QString& user_id,const QPixmap& head_img, const QString& message, BubbleType type, const QString& groupMemberName = QString(), const QString& groupRole = QString(), QWidget* parent = nullptr);
	MessageBubble(const QString& user_id,const QPixmap& head_img, const QPixmap& pixmap, BubbleType type, const QString& groupMemberName = QString(), const QString& groupRole = QString(), QWidget* parent = nullptr);
	MessageBubble::BubbleType getType();
private:
	void init();
protected:
	virtual void drawContent(QPainter& painter) = 0;
	virtual void updateBubbleSize()=0;
	void paintEvent(QPaintEvent* ev)override;
	void drawGroupRole(QPainter& painter);
	void drawGroupName(QPainter& painter);
	bool eventFilter(QObject* watched, QEvent* event)override;
protected:

	QRect m_profileRect{};		//头像矩形
	QRect	m_bubbleRect{};		//气泡矩形
	int _xOffset = -3;
	int _yOffset = 0;
	QPoint m_memberNamePos;

	QString m_user_id{};
	QPixmap m_head_img{};
	QString m_groupMemberName{};
	QString m_groupMemberRole{};
	QVariant m_messageData{};
	BubbleType	m_type{};
};



#endif // !MESSAGEBUBBLE_H_

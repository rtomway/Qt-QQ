#ifndef NOTICEWIDGET_H_
#define NOTICEWIDGET_H_


#include <QWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>

#include "GlobalTypes.h"

class NoticeWidget :public QWidget
{
	Q_OBJECT
public:
	enum NoticeCurrentWidget {
		FriendNoticeWidget,
		GroupNoticeWidget
	};
public:
	NoticeWidget(QWidget* parent = nullptr);
private:
	void init();
	void initStackedWidget();
	void addNoticeItem(const QString& id, ChatType type);
public:
	void setCurrentWidget(NoticeWidget::NoticeCurrentWidget noticeWidget);
private:
	QStackedWidget* m_stackedWidget{};
	QListWidget* m_friendNoticeList{};
	QListWidget* m_groupNoticeList{};
	QLabel* m_noticeTitle{};
signals:
	void friendNotice();
	void groupNotice();
};
#endif // !NOTICEWIDGET_H_

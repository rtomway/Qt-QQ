#ifndef NOTICEWIDGET_H_
#define NOTICEWIDGET_H_


#include <QWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include "ContactList.h"

class NoticeWidget :public QWidget
{
public:
	static NoticeWidget* instance();
	void init();
	void initStackedWidget();
private:
	NoticeWidget(QWidget* parent = nullptr);
	~NoticeWidget();
private:
	void addFreindNoticeItem();
private:
	QStackedWidget* m_stackedWidget{};
	QListWidget* m_friendNoticeList{};
	QListWidget* m_groupNoticeList{};
	QLabel* m_noticeTitle{};
	//ContactList* m_contactList{};
};
#endif // !NOTICEWIDGET_H_

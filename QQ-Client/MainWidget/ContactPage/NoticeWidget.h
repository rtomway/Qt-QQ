#ifndef NOTICEWIDGET_H_
#define NOTICEWIDGET_H_


#include <QWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>

class NoticeWidget :public QWidget
{
	Q_OBJECT
public:
	static NoticeWidget* instance();
	void init();
	void initStackedWidget();
private:
	NoticeWidget(QWidget* parent = nullptr);
	~NoticeWidget();
private:
	void addFreindNoticeItem(const QJsonObject& obj);
private:
	QStackedWidget* m_stackedWidget{};
	QListWidget* m_friendNoticeList{};
	QListWidget* m_groupNoticeList{};
	QLabel* m_noticeTitle{};
};
#endif // !NOTICEWIDGET_H_

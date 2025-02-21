#ifndef MAINWIDGET_H_
#define MAINWIDGET_H_

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QPixmap>
#include <QPushButton>
#include <Qstring>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QMenu>


#include "MessagePage.h"
#include "ContactPage.h"
#include "AngleRoundedWidget.h"
#include "MessageListItem.h"

namespace Ui { class MainWidget; }

class MainWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	MainWidget(QWidget* parent = nullptr);
	~MainWidget();
	void init();
	void initMoreMenu();
private:
	Ui::MainWidget* ui{};
private:
	QListWidget* m_messageList{};
	QButtonGroup* m_btn_Itemgroup{};
	QMenu* m_moreMenu{};
	QMenu* m_addpersonMenu{};
private:
	MessagePage* m_messagePage{};
	ContactPage* m_contactPage{};
	QWidget* m_emptyWidget{};
public:
	void additemCenter(const QString& src);
	QListWidgetItem* addmessageListItem(const QJsonObject& obj);
	QListWidgetItem* findListItem(const QString&user_id);
signals:
	void quitsuccess();
	void hideWidget();
	void expandWidget();
	void exitWidget();
protected:
	void resizeEvent(QResizeEvent* event)override;
};


#endif // !MAINWIDGET_H_

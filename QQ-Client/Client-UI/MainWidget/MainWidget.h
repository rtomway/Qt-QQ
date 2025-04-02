#ifndef MAINWIDGET_H_
#define MAINWIDGET_H_

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QMenu>

#include "ContactPage.h"
#include "NoticeWidget.h"
#include "ContactListWidget.h"
#include "AddFriendWidget.h"
#include "SearchFriend.h"
#include "GroupInviteWidget.h"
#include "GlobalTypes.h"

#include "ChatWidget.h"

namespace Ui { class MainWidget; }

class MainWidget :public QWidget
{
	Q_OBJECT
public:
	MainWidget(QWidget* parent = nullptr);
	~MainWidget();
private:
	void init();
	void initStackedListWidget();
	void initStackedPage();
	void initMoreMenu();
private:
	void additemCenter(const QString& src);
	QString generateMessageItemKey(const QString id, ChatType type);
	QListWidgetItem* addmessageListItem(const QString& Id, ChatType type);
	QListWidgetItem* findListItem(const QString& user_id);
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
private:
	Ui::MainWidget* ui{};
	QButtonGroup* m_btn_Itemgroup{};
	QMenu* m_moreMenu{};
	QMenu* m_addpersonMenu{};
private:
	//子列表界面
	QListWidget* m_chatMessageListWidget{};
	SearchFriend* m_friendSearchListWidget{};
	ContactListWidget* m_contactListWidget{};
	//子页面
	ChatWidget* m_chatWidget{};
	//MessagePage* m_messagePage{};
	ContactPage* m_contactPage{};
	NoticeWidget* m_noticePage{};
	QWidget* m_emptyPage{};
	//独立窗口
	std::unique_ptr<ContactPage> m_contactWidget{};
	std::unique_ptr<AddFriendWidget> m_addFriendWidget{};
	GroupInviteWidget* m_groupInviteWidget{};
signals:
	void quitsuccess();
	void hideWidget();
	void expandWidget();
	void exitWidget();
};

#endif // !MAINWIDGET_H_

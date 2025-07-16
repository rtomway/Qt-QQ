#ifndef MAINWIDGET_H_
#define MAINWIDGET_H_

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QMenu>

#include "FriendProfilePage.h"
#include "GroupProfilePage.h"
#include "NoticeWidget.h"
#include "ContactListWidget.h"
#include "SearchWidget.h"
#include "ContactSearch.h"
#include "GroupMemberOperatorWidget.h"
#include "GlobalTypes.h"

#include "ChatWidget.h"
#include "AngleRoundedWidget.h"

namespace Ui { class MainWidget; }

class MainWidget :public AngleRoundedWidget
{
	Q_OBJECT
public:
	MainWidget(QWidget* parent = nullptr);
	~MainWidget();
private:
	void init();
	void initLayout();
	void initStackedListWidget();
	void initStackedPage();
	void initMoreMenu();
private:
	//接收信号
	void connectFriendManagerSignals();
	void connectGroupManagerSignals();
	void connectWindowControlSignals();
private:
	void additemCenter(const QString& src);
	QListWidgetItem* addmessageListItem(const QString& Id, ChatType type);
	QString itemKey(const QString& id, ChatType type);
	QListWidgetItem* findListItem(const QString& user_id);
	void clearChatMessageListWidget();
	void updateStackedListWidget();
	void installEventFilterToWidget(QWidget* parentWidget);
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
private:
	Ui::MainWidget* ui{};
	QButtonGroup* m_btn_Itemgroup{};
	QMenu* m_moreMenu{};
	QMenu* m_addpersonMenu{};
	//当前登录用户id
	QString m_loginUserId{};
private:
	//子列表界面
	QListWidget* m_chatMessageListWidget{};
	ContactSearch* m_contactSearchListWidget{};
	ContactListWidget* m_contactListWidget{};
	//子页面
	ChatWidget* m_chatWidget{};
	FriendProfilePage* m_friendProfilePage{};
	GroupProfilePage* m_groupProfilePage{};
	NoticeWidget* m_noticePage{};
	QWidget* m_emptyPage{};
	//独立窗口
	std::unique_ptr<FriendProfilePage> m_friendProfileWidget{};
	std::unique_ptr<SearchWidget> m_addFriendWidget{};
	GroupMemberOperatorWidget* m_groupInviteWidget{};
signals:
	void quitSuccess();
	void hideWidget();
	void expandWidget();
	void exitWidget();
};

#endif // !MAINWIDGET_H_

#ifndef FRIENDPROFILEPAGE_H_
#define FRIENDPROFILEPAGE_H_

#include <QWidget>
#include <QJsonObject>

#include "FriendProfileEditWidget.h"
#include "AngleRoundedWidget.h"
#include "Friend.h"


namespace Ui { class FriendProfilePage; }

class FriendProfilePage :public AngleRoundedWidget
{
	Q_OBJECT
public:
	FriendProfilePage(QWidget* parent = nullptr);
	~FriendProfilePage();
private:
	void init();
public:
	void setFriendProfile(const QString& user_id);
	void clearWidget();
private:
	Ui::FriendProfilePage* ui{};
private:
	int m_likeCount{ 0 };
	QString m_friendId{};
	QSharedPointer<Friend>m_friend{};
	QJsonObject m_json{};
	FriendProfileEditWidget* m_detailEditWidget{};
private:
	bool m_isBlockedComboBox{ true };
signals:
	void chatWithFriend(const QString& friend_id);
public slots:
	void updateFriendgrouping();
};


#endif // !FRIENDPROFILEPAGE_H_

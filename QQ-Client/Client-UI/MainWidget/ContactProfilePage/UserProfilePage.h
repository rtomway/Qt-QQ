#ifndef USERPROFILEPAGE_H_
#define USERPROFILEPAGE_H_

#include <QWidget>
#include <QJsonObject>
#include <memory>
#include "SMaskWidget.h"
#include "FriendProfileEditWidget.h"
#include "AngleRoundedWidget.h"

namespace Ui { class UserProfilePage; }

enum class UserRelation
{
	LoginUser,
	Friend,
	Stranger
};

class UserProfilePage :public AngleRoundedWidget
{
	Q_OBJECT
public:
	UserProfilePage(QWidget* parent = nullptr);
	~UserProfilePage();
	void setUserProfilePage(const QJsonObject& obj,UserRelation userRelation);
private:
	void init();
	void refreshProfilePage();
	void clearDynamicWidgets();
	void editUserProfile();
	void addFriendRequest();
private:
	Ui::UserProfilePage* ui{};
	QJsonObject m_json{};
	QString m_user_id{};
	UserRelation m_userRelation;
	std::unique_ptr<FriendProfileEditWidget>m_profileEditWidget{};
	QMap<QString, QLabel*> m_dynamicLabels;
};

#endif // !USERPROFILEPAGE_H_

#ifndef USERPROFILEDISPATCHER_H_
#define USERPROFILEDISPATCHER_H_

#include <QObject>
#include <QJsonObject>
#include <QPoint>
#include "UserProfilePage.h"

enum class PopUpPosition
{
	Left,
	Right
};

class UserProfileDispatcher:public QObject
{
	Q_OBJECT
public:
	static UserProfileDispatcher* instance();
	void showUserProfile(const QString& user_id, const QPoint& anchor,PopUpPosition position);
	void getUserData(const QString& user_id,std::function<void(const QJsonObject&obj)>callback);
	QPoint calculatePopupPosition(const QPoint&anchor,const QSize&popSize, PopUpPosition position);
	UserRelation getUserRelation(const QString& user_id);
private:
	UserProfileDispatcher(QObject* parent = nullptr) {};
	UserProfileDispatcher(const UserProfileDispatcher&) = delete;
	UserProfileDispatcher& operator=(const  UserProfileDispatcher&) = delete;
private:
	UserProfilePage m_userProfilePage{};
};

#endif // !USERPROFILEDISPATCHER_H_

#include "UserProfileDispatcher.h"

#include <QJsonDocument>
#include <QApplication>
#include <QScreen>
#include "FriendManager.h"
#include "LoginUserManager.h"
#include "../Client-ServiceLocator/NetWorkServiceLocator.h"
#include "Friend.h"

UserProfileDispatcher* UserProfileDispatcher::instance()
{
    static UserProfileDispatcher instance;
    return &instance;
}

//显示用户信息弹窗
void UserProfileDispatcher::showUserProfile(const QString& user_id, const QPoint& anchor, PopUpPosition position)
{
    //获取用户信息后弹出
	this->getUserData(user_id, [=](const QJsonObject&userObj)
		{
			//获取用户关系
			auto userRelation = getUserRelation(user_id);
			//加载userProfilePage数据
			m_userProfilePage.setUserProfilePage(userObj, userRelation);
			//将userProfilePage弹出在合适的位置
			auto popupPosition = calculatePopupPosition(anchor, m_userProfilePage.size(),position);
			m_userProfilePage.setGeometry(QRect(popupPosition.x(),popupPosition.y(), 0, 0));
			//显示弹窗
			m_userProfilePage.show();
		});
}

//获取用户信息
void UserProfileDispatcher::getUserData(const QString& user_id, std::function<void(const QJsonObject& obj)>callback)
{
	QJsonObject userObj;
	QSharedPointer<Friend>user = FriendManager::instance()->findFriend(user_id);
	//先判断是否是好友，本地信息缓存
	if (user)
	{
		userObj = user->getFriend();
		callback(userObj);
		return;
	}
	//无用户信息，向服务器请求用户信息
	QJsonObject queryObj;
	queryObj["user_id"] = LoginUserManager::instance()->getLoginUserID();
	queryObj["query_id"] = user_id;
	QJsonDocument doc(queryObj);
	QByteArray data = doc.toJson(QJsonDocument::Compact);
	NetWorkServiceLocator::instance()->sendHttpRequest("queryUser", data, "application/json",
		[this,callback](const QJsonObject& params, const QByteArray& avatarData)
		{
			callback(params);
		});
}

//计算弹窗精确位置
QPoint UserProfileDispatcher::calculatePopupPosition(const QPoint& anchor, const QSize& popupSize, PopUpPosition position)
{
	QPoint position_;
	switch (position)
	{
	case PopUpPosition::Left:
		position_=QPoint(anchor.x() - popupSize.width() - 10,  
			anchor.y());  
		break;
	case PopUpPosition::Right:
		position_ = QPoint(anchor.x(),  anchor.y()); 
		break;
	default:
		break;
	}
	return position_;
}

//获取用户关系
UserRelation UserProfileDispatcher::getUserRelation(const QString& user_id)
{
	UserRelation userRelation = UserRelation::Stranger;
	if (user_id == LoginUserManager::instance()->getLoginUserID())
	{
		userRelation = UserRelation::LoginUser;
	}
	else if (FriendManager::instance()->isFriend(user_id))
	{
		userRelation = UserRelation::Friend;
	}
	return userRelation;
}


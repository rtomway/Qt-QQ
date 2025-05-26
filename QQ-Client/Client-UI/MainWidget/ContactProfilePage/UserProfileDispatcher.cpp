#include "UserProfileDispatcher.h"

#include <QJsonDocument>
#include <QApplication>
#include <QScreen>
#include "FriendManager.h"
#include "LoginUserManager.h"
#include "MessageSender.h"
#include "Friend.h"

UserProfileDispatcher* UserProfileDispatcher::instance()
{
    static UserProfileDispatcher instance;
    return &instance;
}
//显示用户信息弹窗
void UserProfileDispatcher::showUserProfile(const QString& user_id, const QPoint& anchor)
{
    //获取用户信息后弹出
	this->getUserData(user_id, [=](const QJsonObject&userObj)
		{
			//获取用户关系
			auto userRelation = getUserRelation(user_id);
			//加载userProfilePage数据
			m_userProfilePage.setUserProfilePage(userObj, userRelation);
			//将userProfilePage弹出在合适的位置
			auto popupPosition = calculatePopupPosition(anchor, m_userProfilePage.sizeHint());
			m_userProfilePage.setGeometry(anchor.x() - m_userProfilePage.width(), anchor.y(),0, 0);
			// 5. 显示弹窗
			m_userProfilePage.show();
			/*m_userProfilePage.raise();
			m_userProfilePage.activateWindow();*/
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
	MessageSender::instance()->sendHttpRequest("queryUser", data, "application/json",
		[this,callback](const QJsonObject& params, const QByteArray& avatarData)
		{
			callback(params);
		});
}
//计算弹窗精确位置
QPoint UserProfileDispatcher::calculatePopupPosition(const QPoint& anchor, const QSize& popupSize)
{
	// 默认位置：锚点左侧，垂直居中
	QPoint position(anchor.x() - popupSize.width() - 10,  // 左侧留10px间距
		anchor.y() - popupSize.height() / 2);  // 垂直居中

	// 获取当前屏幕的可用几何区域（新方法）
	QScreen* screen = QGuiApplication::screenAt(anchor);
	if (!screen) {
		screen = QGuiApplication::primaryScreen();
	}
	QRect screenRect = screen->availableGeometry();

	// 确保弹窗不会超出屏幕左边界
	position.setX(qMax(screenRect.left(), position.x()));

	// 确保弹窗不会超出屏幕上边界
	position.setY(qMax(screenRect.top(), position.y()));

	// 确保弹窗不会超出屏幕右边界
	if (position.x() + popupSize.width() > screenRect.right()) {
		position.setX(screenRect.right() - popupSize.width());
	}

	// 确保弹窗不会超出屏幕下边界
	if (position.y() + popupSize.height() > screenRect.bottom()) {
		position.setY(screenRect.bottom() - popupSize.height());
	}

	return position;
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


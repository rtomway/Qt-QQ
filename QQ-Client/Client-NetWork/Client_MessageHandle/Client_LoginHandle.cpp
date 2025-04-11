#include "Client_LoginHandle.h"
#include <QPixmap>
#include <QJsonArray>
#include <FriendManager.h>
#include <Friend.h>

#include "EventBus.h"

void Client_LoginHandle::handle_loginSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	QJsonObject loginUser = paramsObject["loginUser"].toObject();
	QJsonArray friendArray = paramsObject["friendArray"].toArray();
	auto user_name = loginUser["username"].toString();
	auto user_id = loginUser["user_id"].toString();
	//当前登录用户信息
	auto user = QSharedPointer<Friend>::create();
	user->setFriend(loginUser);
	qDebug() << "loginUser" << user->getFriend();
	FriendManager::instance()->addFriend(user);
	FriendManager::instance()->setOneselfID(user_id);
	//加载好友信息
	for (const QJsonValue& value : friendArray)
	{
		QJsonObject friendObject = value.toObject();
		auto friendUser = QSharedPointer<Friend>::create();
		friendUser->setFriend(friendObject);
		auto friend_id = friendUser->getFriendId();
		FriendManager::instance()->addFriend(friendUser);
	}
	FriendManager::instance()->emit FriendManagerLoadSuccess();
	EventBus::instance()->emit loginSuccess();
}

void Client_LoginHandle::handle_registerSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit registerSuccess(paramsObject);
}

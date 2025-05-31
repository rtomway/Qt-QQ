#include "LoginUserManager.h"
#include "FriendManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

#include "MessageSender.h"
#include "ImageUtil.h"
#include "EventBus.h"

LoginUserManager* LoginUserManager::instance()
{
	static LoginUserManager instance;
	return &instance;
}

LoginUserManager::LoginUserManager()
{
	connect(EventBus::instance(), &EventBus::initLoginUser, this, &LoginUserManager::initLoginUser);
}

//设置当前用户信息
void LoginUserManager::initLoginUser(const QJsonObject& loginUserObj)
{
	auto user_id = loginUserObj["user_id"].toString();
	auto user_name = loginUserObj["username"].toString();
	//加载当前登录用户信息
	auto loginUser = QSharedPointer<Friend>::create();
	loginUser->setFriend(loginUserObj);
	FriendManager::instance()->addFriend(loginUser);
	m_loginUser = loginUser;
	m_loginUserId = user_id;
	m_loginUserName = user_name;
	//头像保存文件指定
	ImageUtils::setLoginUser(m_loginUserId);
	//向内部发送信号
	emit loginUserLoadSuccess();
}

//获取登录用户Id
const QString& LoginUserManager::getLoginUserID() const
{
	return m_loginUserId;
}

//获取登录用户姓名
const QString& LoginUserManager::getLoginUserName() const
{
	return m_loginUserName;
}

//获取登录用户信息
const QSharedPointer<Friend>& LoginUserManager::getLoginUser() const
{
	return m_loginUser;
}

//清除信息
void LoginUserManager::clearLoginUserManager()
{
	m_loginUser = nullptr;
	m_loginUserId = QString();
	m_loginUserName = QString();
}

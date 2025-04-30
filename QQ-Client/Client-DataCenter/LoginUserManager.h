#ifndef	LOGINUSERMANAGER_H_
#define LOGINUSERMANAGER_H_

#include <QJsonObject>
#include <QObject>
#include <QSharedPointer>

#include "Friend.h"

class LoginUserManager : public QObject
{
	Q_OBJECT
public:
	// 获取单例实例的静态方法
	static LoginUserManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	LoginUserManager(const LoginUserManager&) = delete;
	LoginUserManager& operator=(const LoginUserManager&) = delete;
private:
	LoginUserManager();
public:
	//当前登录用户
	void initLoginUser(const QJsonObject& loginUserObj);
	void loadListDataFromServer(const QString& type);
	const QString& getLoginUserID()const;
	const QString& getLoginUserName()const;
	const QSharedPointer<Friend>& getLoginUser()const;
	//清除
	void clearFriendManager();
private:
	//当前登录用户
	QString m_loginUserId{};
	QString m_loginUserName{};
	QSharedPointer<Friend>m_loginUser{};
signals:
	void loginUserLoadSuccess();
};


#endif // !LOGINUSERMANAGER_H_
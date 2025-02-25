#include "User.h"

User*User::instance(){
    static User* instance = new User;
    return instance;
}

void User::setUser(const QJsonObject& obj)
{
    m_json = obj;
}
QJsonObject User::getUser()const
{
    qDebug() << "m_json[]:" << m_json["username"].toString();
    return m_json;
}
// 设置用户ID
void User::setUserId(const QString& id) {
    userId = id;
    m_json["user_id"] = userId;
    qDebug() << "User ID set to:" << userId;
 }

// 获取用户ID
QString User::getUserId() const {
    return userId;
}
// 设置用户名
void User::setUserName(const QString& name) {
    username = name;
    m_json["username"] = username;
     emit setUserSuccess();
     qDebug() << "User name set to:" << username;
}

// 获取用户头像
QString User::getUserName() const {
    return username;
}


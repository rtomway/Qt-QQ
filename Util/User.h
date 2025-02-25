#ifndef USER_H_
#define USER_H_

#include <QJsonObject>
#include <QObject>
class User: public QObject
{
    Q_OBJECT
public:
    // 获取单例实例的静态方法
    static User* instance();

    // 禁止拷贝构造函数和赋值操作符
    User(const User&) = delete;
    User& operator=(const User&) = delete;

    void setUser(const QJsonObject& obj);
   
    QJsonObject getUser()const;
    // 设置用户ID
    void setUserId(const QString& id);

    // 获取用户ID
    QString getUserId() const;
    // 设置用户名
    void setUserName(const QString& name);

    // 获取用户头像
    QString getUserName() const;

private:
    // 私有构造函数，防止外部实例化
    User() {};

    QString userId; // 存储用户ID
    QString username; // 存储用户名
    QString userHead; // 存储用户头像
    QJsonObject m_json;//用户信息
signals:
    void setUserSuccess();

};


#endif // !USER_H_
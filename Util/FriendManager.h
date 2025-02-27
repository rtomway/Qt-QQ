#ifndef FriendMANAGER_H_
#define FriendMANAGER_H_

#include <QJsonObject>
#include <QObject>
#include <QSharedPointer>
#include "Friend.h"
class FriendManager : public QObject
{
    Q_OBJECT
public:
    // 获取单例实例的静态方法
    static FriendManager* instance();
    // 禁止拷贝构造函数和赋值操作符
    FriendManager(const FriendManager&) = delete;
    FriendManager& operator=(const FriendManager&) = delete;
    //当前客户端登录用户
    static QString m_oneselfID;
    static void setOneselfID(const QString& id);

    void addFriend(const QSharedPointer<Friend>& user);
    QSharedPointer<Friend>findFriend(const QString& id)const;

private:
    // 私有构造函数，防止外部实例化
    FriendManager() {};
    //管理所有用户
    QHash<QString, QSharedPointer<Friend>>m_user;
signals:
    void setUserSuccess();

};


#endif // !FriendMANAGER_H_
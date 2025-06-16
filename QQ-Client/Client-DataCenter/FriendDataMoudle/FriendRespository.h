#ifndef FRIENDRESPOSITORY_H_
#define FRIENDRESPOSITORY_H_

#include <QJsonObject>
#include <QObject>
#include <QSharedPointer>

#include "Friend.h" 

class FriendRespository :public QObject
{
	Q_OBJECT
public:
	FriendRespository(QObject* parent = nullptr);
	~FriendRespository();
public:
public:
	//好友处理
	void addFriend(const QSharedPointer<Friend>& user);
	QSharedPointer<Friend>findFriend(const QString& id)const;
	//好友搜索
	QHash<QString, QSharedPointer<Friend>> findFriends(const QString& text)const;
	//获取所有好友信息
	QList<QSharedPointer<Friend>>getAllFriends()const;
	//获取所有好友分组
	QStringList getAllFriendGroupings()const;
	//获取所有好友Id
	QStringList getAllFriendIdList()const;
	bool isFriend(const QString& user_id);
	//删除好友
	void removeFriend(const QString& friend_id);
	//清除
	void clearFriendManager();
private:
	//管理所有用户
	QHash<QString, QSharedPointer<Friend>>m_friends{};
	QHash<QString, QString>m_grouping{};
};


#endif // !FRIENDRESPOSITORY_H_

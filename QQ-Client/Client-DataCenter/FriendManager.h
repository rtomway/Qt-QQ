#ifndef FRIENDMANAGER_H_
#define FRIENDMANAGER_H_

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
private:
	FriendManager();
	void loadFriendAvatarFromServer(const QStringList& friend_idList);
public:
	//好友处理
	void addFriend(const QSharedPointer<Friend>& user);
	QSharedPointer<Friend>findFriend(const QString& id)const;
	//好友搜索
	QHash<QString, QSharedPointer<Friend>> findFriends(const QString& text)const;
	//删除好友
	void removeFriend(const QString& friend_id);
	//清除
	void clearFriendManager();
private:
	//管理所有用户
	QHash<QString, QSharedPointer<Friend>>m_friends{};
	QHash<QString, QString>m_grouping{};
signals:
	void loadLocalAvatarFriend(const QStringList& friend_idList);
	void UpdateFriendMessage(const QString& user_id);
	void updateFriendGrouping(const QString& user_id, const QString& oldGrouping);
	void NewFriend(const QString& user_id, const QString& grouping);
	void deleteFriend(const QString& user_id);
	void chatWithFriend(const QString& user_id);
};


#endif // !FRIENDMANAGER_H_
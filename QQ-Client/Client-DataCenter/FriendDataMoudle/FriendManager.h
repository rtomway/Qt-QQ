#ifndef FRIENDMANAGER_H_
#define FRIENDMANAGER_H_

#include <QObject>

#include "FriendRespository.h"
#include "FriendService.h"

class FriendManager : public QObject
{
	Q_OBJECT
public:
	static FriendManager* instance();
private:
	FriendManager();
	FriendManager(const FriendManager&) = delete;
	FriendManager& operator=(const FriendManager&) = delete;
public:
	void init();
public:
	void addFriend(const QSharedPointer<Friend>& user);
	QSharedPointer<Friend>findFriend(const QString& id)const;
	QHash<QString, QSharedPointer<Friend>> findFriends(const QString& text)const;
	QList<QSharedPointer<Friend>>getAllFriends()const;
	QStringList getAllFriendGroupings()const;
	QStringList getAllFriendIdList()const;
	bool isFriend(const QString& user_id);
	void removeFriend(const QString& friend_id);
	void clearFriendManager();
private:
	FriendRespository* m_friendRespository;
	FriendService* m_friendService;
signals:
	void loadLocalAvatarFriend(const QStringList& friend_idList);
	void UpdateFriendMessage(const QString& user_id);
	void updateFriendGrouping(const QString& user_id, const QString& oldGrouping);
	void NewFriend(const QString& user_id, const QString& grouping);
	void deleteFriend(const QString& user_id);
	void chatWithFriend(const QString& user_id);

};


#endif // !FRIENDMANAGER_H_
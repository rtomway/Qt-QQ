#ifndef FRIENDSERVICE_H_
#define FRIENDSERVICE_H_

#include <QObject>

#include "FriendRespository.h"

class FriendService:public QObject
{
	Q_OBJECT
public:
	FriendService(FriendRespository*friendRespository,QObject* parent = nullptr);
	~FriendService();
public:
	void loadFriendList();
	void initFriendRespository(const QJsonObject& obj);
	void updateUserMessage(const QJsonObject& obj);
	void addNewFriend(const QJsonObject& obj);
	void removeFriend(const QString& user_id);
private:
	void loadFriendAvatarFromServer(const QStringList& friend_idList);
private:
	FriendRespository* m_friendRespository;
signals:
	void loadLocalAvatarFriend(const QStringList& friend_idList);
	void UpdateFriendMessage(const QString& user_id);
	void updateFriendGrouping(const QString& user_id, const QString& oldGrouping);
	void NewFriend(const QString& user_id, const QString& grouping);
	void deleteFriend(const QString& user_id);
	void chatWithFriend(const QString& user_id);
};


#endif // !FRIENDSERVICE_H_

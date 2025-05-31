#ifndef FRIEND_H_
#define FRIEND_H_

#include <QJsonObject>
#include <QPixmap>
class Friend
{
public:
	const QString& getFriendId()const;
	const QString& getFriendName()const;
	void setFriend(const QJsonObject& obj);
	const QJsonObject& getFriend()const;
	void setGrouping(const QString& grouping);
	QString getGrouping()const;
private:
	QString m_userID{};
	QString m_userName{};
	QString m_grouping{};
	QJsonObject m_json{};
};

#endif // !FRIEND_H_

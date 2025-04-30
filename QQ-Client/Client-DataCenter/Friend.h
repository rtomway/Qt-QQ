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
	int m_gender{};
	int m_age{};
	QString m_phoneNumber{};
	QString m_email{};
	QDate m_birthday{};
	QString m_signature{};
	QString m_grouping{};
	bool m_status{};
	QJsonObject m_json{};

};

#endif // !FRIEND_H_

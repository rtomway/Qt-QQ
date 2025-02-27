#include "Friend.h"

Friend::Friend(const QString& id)
	:m_userID(id)
{

}
const QString Friend::getFriendId()const
{
	return m_userID;
}

void Friend::setFriend(const QJsonObject& obj)
{
	m_json = obj;
	m_userID = obj["user_id"].toString();
	m_userName = obj["userName"].toString();
	m_gender = obj["gender"].toInt();
	m_age = obj["age"].toInt();
	m_phoneNumber = obj["phone_number"].toString();
	m_email = obj["email"].toString();
	m_avatarPath = obj["avatar_path"].toString();
	m_status = obj["status"].toBool();
}

const QJsonObject Friend::getFriend()const
{
	return m_json;
}


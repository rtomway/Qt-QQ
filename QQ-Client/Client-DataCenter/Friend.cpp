#include "Friend.h"
#include <QStandardPaths>

#include "ImageUtil.h"

//获取ID
const QString& Friend::getFriendId()const
{
	return m_userID;
}

//获取用户名
const QString& Friend::getFriendName() const
{
	return m_userName;
}

//好友信息设置
void Friend::setFriend(const QJsonObject& obj)
{
	m_json = obj;
	m_userID = obj["user_id"].toString();
	m_userName = obj["username"].toString();
	if (obj.contains("Fgrouping"))
	{
		m_grouping = obj["Fgrouping"].toString();
	}
	//信息更新不包含客户端独立设置信息
	m_json["Fgrouping"] = m_grouping;
}

//获取好友信息
const QJsonObject& Friend::getFriend()const
{
	return m_json;
}

//设置分组
void Friend::setGrouping(const QString& grouping)
{
	m_grouping = grouping;
	m_json["Fgrouping"] = m_grouping;
}

//获取分组
QString Friend::getGrouping() const
{
	return m_grouping;
}



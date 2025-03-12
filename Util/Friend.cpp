#include "Friend.h"
#include <QStandardPaths>

#include "ImageUtil.h"

//获取ID
const QString Friend::getFriendId()const
{
	return m_userID;
}
//获取用户名
const QString Friend::getFriendName() const
{
	return m_userName;
}
//好友信息设置
void Friend::setFriend(const QJsonObject& obj)
{
	m_json = obj;
	qDebug() << "添加了一个好友:" << m_json;
	m_userID = obj["user_id"].toString();
	m_userName = obj["username"].toString();
	m_gender = obj["gender"].toInt();
	m_age = obj["age"].toInt();
	m_phoneNumber = obj["phone_number"].toString();
	m_email = obj["email"].toString();
	m_avatarPath = obj["avatar_path"].toString();
	m_birthday = QDate::fromString(obj["birthday"].toString(), "MM-dd");
	m_signature = obj["signature"].toString();
	m_status = obj["status"].toBool();
	m_grouping = obj["grouping"].toString();
	loadAvatar();

}
//获取好友信息
const QJsonObject Friend::getFriend()const
{
	return m_json;
}
//设置分组
void Friend::setGrouping(const QString& grouping)
{
	m_grouping = grouping;
	m_json["grouping"] = m_grouping;
}
//获取分组
const QString Friend::getGrouping() const
{
	return m_grouping;
}
//图片加载
void Friend::loadAvatar()
{
	if (!m_avatarPath.isEmpty())
	{
		QString avatarFolder = QStandardPaths::writableLocation
		(QStandardPaths::AppDataLocation)+"/avatars";
		auto avatarPath = avatarFolder + "/" + m_avatarPath;

		// 加载图片
		QPixmap pixmap(avatarPath);

		// 检查图片是否加载成功
		if (!pixmap.isNull()) {
			m_avatar = pixmap;  // 将加载的图片保存到 m_avatar
		}
		else {
			// 处理加载失败的情况，例如设置一个默认图片
			qDebug() << "图片加载失败";
			m_avatar = QPixmap(":/picture/Resource/Picture/qq.png");
			return;
		}
	}
	else
	{
		// 处理无效路径的情况
		qDebug() << "图片路径无效";
		m_avatar = QPixmap(":/picture/Resource/Picture/qq.png");
		return;
	}
}
//获取图片
const QPixmap Friend::getAvatar() const
{
	qDebug() << "m_avatar:" << m_avatar;
	return m_avatar;
}



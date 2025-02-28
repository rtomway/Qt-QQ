#include "Friend.h"
#include "ImageUtil.h"


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
	qDebug() << "添加了一个好友:" << m_json;
	
}

const QJsonObject Friend::getFriend()const
{
	return m_json;
}

void Friend::loadAvatar()
{
	if (!m_avatarPath.isEmpty())
	{
		// 加载图片
		QPixmap pixmap(m_avatarPath);

		// 检查图片是否加载成功
		if (!pixmap.isNull()) {
			m_avatar = ImageUtils::roundedPixmap(pixmap, QSize(40, 40));;  // 将加载的图片保存到 m_avatar
		}
		else {
			// 处理加载失败的情况，例如设置一个默认图片
			qDebug() << "图片加载失败";
			return;
		}
	}
	else
	{
		// 处理无效路径的情况
		qDebug() << "图片路径无效";
		m_avatar = ImageUtils::roundedPixmap(QPixmap(":/picture/Resource/Picture/qq.png"), QSize(40, 40));
		return;
	}
}

const QPixmap Friend::getAvatar() const
{
	qDebug() << "m_avatar:" << m_avatar;
	return m_avatar;
}



#include "AvatarManager.h"
#include <QFile>
#include <QImageReader>

#include "ImageUtil.h"
#include "EventBus.h"
#include "GlobalTypes.h"

AvatarManager* AvatarManager::instance()
{
	static AvatarManager instance;
	return &instance;
}

AvatarManager::AvatarManager()
{
	connect(EventBus::instance(), &EventBus::updateUserAvatar, this, [=](const QString& user_id, const QPixmap& pixmap)
		{
			qDebug() << "----------用户头像更新-----------";
			if (ImageUtils::saveAvatarToLocal(pixmap, user_id, ChatType::User))
			{
				m_userAvatarCache.insert(user_id, new QPixmap(pixmap));  // 插入新的头像到缓存（qcatch自动管理）
				emit UpdateUserAvatar(user_id);
			}
			else
			{
				qDebug() << "头像接收失败";
			}
		});
}
//获取头像
const QPixmap& AvatarManager::getAvatar(const QString& id, ChatType type)
{
	QString avatarPath;
	switch (type)
	{
	case ChatType::User:
		// 首先检查缓存中是否有头像
		if (m_userAvatarCache.contains(id))
		{
			return *m_userAvatarCache[id];
		}
		// 如果没有缓存，将头像加载到缓存中
		updateAvatar(id, type);
		return *m_userAvatarCache[id];
		break;
	case ChatType::Group:
		if (m_groupAvatarCache.contains(id))
		{
			return *m_groupAvatarCache[id];
		}
		updateAvatar(id, type);
		return *m_groupAvatarCache[id];
		break;
	default:
		break;
	}
}

//头像更新或新增
void AvatarManager::updateAvatar(const QString& id, ChatType type)
{
	QString avatarPath;
	QPixmap avatar;
	switch (type)
	{
	case ChatType::User:
		avatarPath = ImageUtils::getUserAvatarFolderPath() + "/" + id + ".png";
		// 加载新的头像并缓存
		avatar = ImageUtils::loadAvatarFromFile(avatarPath);
		if (!avatar.isNull())
		{
			m_userAvatarCache.insert(id, new QPixmap(avatar));
		}
		break;
	case ChatType::Group:
		avatarPath = ImageUtils::getGroupAvatarFolderPath() + "/" + id + ".png";
		avatar = ImageUtils::loadAvatarFromFile(avatarPath);
		if (!avatar.isNull())
		{
			m_groupAvatarCache.insert(id, new QPixmap(avatar));
		}
		break;
	default:
		break;
	}
}


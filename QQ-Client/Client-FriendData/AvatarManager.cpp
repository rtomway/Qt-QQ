#include "AvatarManager.h"
#include <QFile>
#include <QImageReader>

#include "ImageUtil.h"
#include "EventBus.h"

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
			if (ImageUtils::saveAvatarToLocal(pixmap, user_id))
			{

				m_avatarCache.insert(user_id, new QPixmap(pixmap));  // 插入新的头像到缓存（qcatch自动管理）

				emit UpdateUserAvatar(user_id);
			}
			else
			{
				qDebug() << "头像接收失败";
			}
		});
}


const QPixmap& AvatarManager::getAvatar(const QString& user_id)
{
	// 首先检查缓存中是否有头像
	if (m_avatarCache.contains(user_id))
	{
		return *m_avatarCache[user_id];
	}
	// 如果没有缓存，从文件加载头像
	QString avatarPath = ":/default_avatar.png"; // 默认头像路径
	return loadAvatarFromFile(avatarPath);
}

//头像更新或新增
void AvatarManager::updateAvatar(const QString& user_id)
{
	auto avatarPath = ImageUtils::getAvatarFolderPath() + "/" + user_id + ".png";
	// 加载新的头像并缓存
	QPixmap avatar = loadAvatarFromFile(avatarPath);
	m_avatarCache.insert(user_id, new QPixmap(avatar));
}

QPixmap AvatarManager::loadAvatarFromFile(const QString& avatarPath)
{
	QPixmap avatar;
	if (QFile::exists(avatarPath))
	{
		avatar.load(avatarPath);
	}
	else
	{
		avatar.load(":/default_avatar.png"); // 如果头像加载失败，使用默认头像
	}
	return avatar;
}

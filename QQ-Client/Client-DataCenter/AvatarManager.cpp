#include "AvatarManager.h"
#include <QFile>
#include <QDir>

#include "ImageUtil.h"
#include "EventBus.h"

AvatarManager* AvatarManager::instance()
{
	static AvatarManager instance;
	return &instance;
}
AvatarManager::AvatarManager()
{
	//头像保存
	connect(EventBus::instance(), &EventBus::saveAvatar, this, [=](const QString& id, ChatType type, const QByteArray& data)
		{
			ImageUtils::saveAvatarToLocal(data, id, type, [=](bool result)
				{
					if (!result)
						qDebug() << "头像保存失败";
				});
		});
	//更新用户头像
	connect(EventBus::instance(), &EventBus::updateUserAvatar, this, [=](const QString& user_id, const QPixmap& pixmap)
		{
			ImageUtils::saveAvatarToLocal(pixmap.toImage(), user_id, ChatType::User, [=](bool result)
				{
					if (result)
					{
						m_userAvatarCache.insert(user_id, new QPixmap(pixmap));  // 插入新的头像到缓存（qcatch自动管理）
						emit UpdateUserAvatar(user_id);
					}
					else
					{
						qDebug() << "头像接收失败";
					}
				});
		});
}
//检测本地是否有该id头像缓存
bool AvatarManager::hasLocalAvatar(const QString& id, ChatType type)
{
	QString avatarPath;
	switch (type) {
	case ChatType::User:
		avatarPath = ImageUtils::getUserAvatarFolderPath() + "/" + id + ".png";
		break;
	case ChatType::Group:
		avatarPath = ImageUtils::getGroupAvatarFolderPath() + "/" + id + ".png";
		break;
	default:
		return false;
	}
	return QFile::exists(avatarPath);
}
// 获取头像，如果缓存中没有,加载并缓存
void AvatarManager::getAvatar(const QString& id, ChatType type, std::function<void(const QPixmap&)>callback)
{
	QString avatarPath;
	QPixmap* cached = nullptr;
	switch (type)
	{
	case ChatType::User:
		// 首先检查缓存中是否有头像
		if (m_userAvatarCache.contains(id))
		{
			cached = m_userAvatarCache.object(id);
		}
		break;
	case ChatType::Group:
		if (m_groupAvatarCache.contains(id))
		{
			cached = m_groupAvatarCache.object(id);
		}
		break;
	default:
		break;
	}
	//回调返回头像
	if (cached) {
		callback(*cached);
		return;
	}
	//无缓存,先标记
	QString key = QString::number(static_cast<int>(type)) + id;
	//该头像正在加载,返回
	if (m_loadingIds.contains(key)) {
		m_pendingCallbacks[key].append(callback);  // 将当前回调加入队列
		return;
	}
	//标记并保存回调
	m_loadingIds.insert(key);
	m_pendingCallbacks[key].append(callback);
	//加载头像
	loadAvatar(id, type);
}
//头像加载
void AvatarManager::loadAvatar(const QString& id, ChatType type)
{
	QString avatarPath;
	QString key = QString::number(static_cast<int>(type)) + id;
	switch (type)
	{
	case ChatType::User:
		avatarPath = ImageUtils::getUserAvatarFolderPath() + "/" + id + ".png";
		// 加载新的头像并缓存
		ImageUtils::loadAvatarFromFile(avatarPath, [=](QImage image)
			{
				QPixmap avatar;
				if (!image.isNull()) {
					avatar = QPixmap::fromImage(image);
					m_userAvatarCache.insert(id, new QPixmap(avatar));
				}
				else
				{
					avatar = QPixmap(":/picture/Resource/Picture/qq.png");
				}
				// 处理所有等待的回调函数
				processPendingCallbacks(key, avatar);
				// 标记加载完成
				m_loadingIds.remove(key);
			});
		break;
	case ChatType::Group:
		avatarPath = ImageUtils::getGroupAvatarFolderPath() + "/" + id + ".png";
		ImageUtils::loadAvatarFromFile(avatarPath, [=](QImage image)
			{
				QPixmap avatar;
				if (!image.isNull()) {
					avatar = QPixmap::fromImage(image);
					m_groupAvatarCache.insert(id, new QPixmap(avatar));
				}
				else
				{
					avatar = QPixmap(":/picture/Resource/Picture/qq.png");
				}
				// 处理所有等待的回调函数
				processPendingCallbacks(key, avatar);
				// 标记加载完成
				m_loadingIds.remove(key);
			});
		break;
	default:
		break;
	}
}
// 处理所有等待的回调函数
void AvatarManager::processPendingCallbacks(const QString& key, const QPixmap& avatar)
{
	if (m_pendingCallbacks.contains(key))
	{
		const QList<std::function<void(const QPixmap&)>>& callbacks = m_pendingCallbacks[key];
		for (const std::function<void(const QPixmap&)>& cb : callbacks) {
			cb(avatar);
		}
		m_pendingCallbacks.remove(key);
	}
}
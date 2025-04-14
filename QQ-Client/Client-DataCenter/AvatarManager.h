#ifndef AVATARMANAGER_H_
#define AVATARMANAGER_H_

#include <QPixmap>
#include <QCache>
#include <QObject>
#include <QSet>
#include <QMap>

#include "GlobalTypes.h"

class AvatarManager :public QObject
{
	Q_OBJECT
public:
	static AvatarManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	AvatarManager(const AvatarManager&) = delete;
	AvatarManager& operator=(const AvatarManager&) = delete;
	void getAvatar(const QString& user_id, ChatType type,std::function<void(const QPixmap&)>callback);
	void loadAvatar(const QString& user_id, ChatType type);
	void processPendingCallbacks(const QString& id, const QPixmap& avatar);
private:
	AvatarManager(); 
	// 使用 QCache 来缓存头像，避免重复加载
	QCache<QString, QPixmap> m_userAvatarCache;
	QCache<QString, QPixmap> m_groupAvatarCache;
	// 正在加载中的头像，防止重复加载
	QSet<QString> m_loadingIds;
	// 存储等待的回调，头像加载完成后统一调用
	QMap<QString, QList<std::function<void(const QPixmap&)>>> m_pendingCallbacks;
signals:
	void UpdateUserAvatar(const QString& user_id);
	void loadGroupAvatarSuccess(const QString&id);
	void loadFriendAvatarSuccess(const QString& id);
};

#endif // AVATARMANAGER_H_

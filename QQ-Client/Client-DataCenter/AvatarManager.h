#ifndef AVATARMANAGER_H_
#define AVATARMANAGER_H_

#include <QString>
#include <QPixmap>
#include <QCache>
#include <QObject>

#include "GlobalTypes.h"

class AvatarManager :public QObject
{
	Q_OBJECT
public:
	static AvatarManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	AvatarManager(const AvatarManager&) = delete;
	AvatarManager& operator=(const AvatarManager&) = delete;
	// 获取头像，如果缓存中没有,加载并缓存
	const QPixmap& getAvatar(const QString& user_id, ChatType type);
	// 更新头像
	void updateAvatar(const QString& user_id, ChatType type);
private:
	AvatarManager(); // 构造函数私有化，确保单例模式
	// 使用 QCache 来缓存头像，避免重复加载
	QCache<QString, QPixmap> m_userAvatarCache;
	QCache<QString, QPixmap> m_groupAvatarCache;
signals:
	void UpdateUserAvatar(const QString& user_id);
};

#endif // AVATARMANAGER_H_

#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QImage>
#include <QSize>
#include <QGuiApplication> 
#include <QThread>

#include "GlobalTypes.h"

class ImageUtils {
public:
	static QPixmap roundedPixmap(const QPixmap& image, QSize size, int radius);
	static QPixmap roundedPixmap(const QPixmap& image, QSize size);

	static void setLoginUser(const QString&user_id);
	static QString getUserAvatarFolderPath();
	static QString getGroupAvatarFolderPath();
	static void saveAvatarToLocal(const QString& avatarPath, const QString& id, ChatType type, std::function<void()>callBack=nullptr);
	static void saveAvatarToLocal(const QImage& image, const QString& id, ChatType type, std::function<void()>callBack=nullptr);
	static void saveAvatarToLocal(const QByteArray& data, const QString& id, ChatType type, std::function<void()>callBack=nullptr);
	static void loadAvatarFromFile(const QString& avatarPath, std::function<void(QImage)>callBack);
private:
	static bool saveAvatarToLocalTask(const QString& avatarPath, const QString& id, ChatType type);
	static bool saveAvatarToLocalTask(const QImage& image, const QString& id, ChatType type);
	static bool saveAvatarToLocalTask(const QByteArray& data, const QString& id, ChatType type);
	static QImage loadAvatarFromFileTask(const QString& avatarPath);
	static QString m_currentUser;
};

#endif // IMAGEUTILS_H
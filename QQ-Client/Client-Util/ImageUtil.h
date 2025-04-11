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
	static QString getUserAvatarFolderPath();
	static QString getGroupAvatarFolderPath();
	static void saveAvatarToLocal(const QString& avatarPath, const QString& user_id, ChatType type, std::function<void(bool)>callBack);
	static void saveAvatarToLocal(const QImage& image, const QString& user_id, ChatType type, std::function<void(bool)>callBack);
	static void loadAvatarFromFile(const QString& avatarPath, std::function<void(QImage)>callBack);
private:
	static bool saveAvatarToLocalTask(const QString& avatarPath, const QString& user_id, ChatType type);
	static bool saveAvatarToLocalTask(const QImage& image, const QString& user_id, ChatType type);
	static QImage loadAvatarFromFileTask(const QString& avatarPath);
};

#endif // IMAGEUTILS_H
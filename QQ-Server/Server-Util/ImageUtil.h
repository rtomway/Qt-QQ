#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QImage>
#include <QSize>
#include "GlobalTypes.h"

class ImageUtils {
public:
	static QString getUserAvatarFolderPath();
	static QString getGroupAvatarFolderPath();
	static void saveAvatarToLocal(const QString& avatarPath, const QString& id, ChatType type, std::function<void()>callBack = nullptr);
	static void saveAvatarToLocal(const QImage& image, const QString& id, ChatType type, std::function<void()>callBack = nullptr);
	static void saveAvatarToLocal(const QByteArray& data, const QString& id, ChatType type, std::function<void()>callBack = nullptr);
	static QByteArray loadImage(const QString& user_id, ChatType type);
private:
	static bool saveAvatarToLocalTask(const QString& avatarPath, const QString& id, ChatType type);
	static bool saveAvatarToLocalTask(const QImage& image, const QString& id, ChatType type);
	static bool saveAvatarToLocalTask(const QByteArray& data, const QString& id, ChatType type);
};

#endif // IMAGEUTILS_H
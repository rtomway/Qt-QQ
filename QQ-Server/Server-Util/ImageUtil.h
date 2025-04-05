#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QImage>
#include <QSize>
#include "GlobalTypes.h"

class ImageUtils {
public:
	static QString getAvatarFolderPath();
	static QString getUserAvatarFolderPath();
	static QString getGroupAvatarFolderPath();
	static bool saveAvatarToLocal(const QString& avatarPath, const QString& user_id, ChatType type);
	static bool saveAvatarToLocal(const QImage& image, const QString& user_id, ChatType type);
	static QByteArray loadImage(const QString& user_id, ChatType type);
};

#endif // IMAGEUTILS_H
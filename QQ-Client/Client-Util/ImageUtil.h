#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QPixmap>
#include <QSize>
#include <QGuiApplication> 

#include "GlobalTypes.h"

class ImageUtils {
public:
	static QPixmap roundedPixmap(const QPixmap& pixmap, QSize size, int radius);
	static QPixmap roundedPixmap(const QPixmap& pixmap, QSize size);
	static QString getAvatarFolderPath();
	static QString getUserAvatarFolderPath();
	static QString getGroupAvatarFolderPath();
	static bool saveAvatarToLocal(const QString& avatarPath, const QString& user_id, ChatType type);
	static bool saveAvatarToLocal(const QPixmap& pixmap, const QString& user_id, ChatType type);
	static QPixmap loadAvatarFromFile(const QString& avatarPath);
};

#endif // IMAGEUTILS_H
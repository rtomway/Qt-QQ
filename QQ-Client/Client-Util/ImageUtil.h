#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QPixmap>
#include <QSize>
#include <QGuiApplication> 

class ImageUtils {
public:
    static QPixmap roundedPixmap(const QPixmap& pixmap, QSize size, int radius);
    static QPixmap roundedPixmap(const QPixmap& pixmap, QSize size);
    static QString getAvatarFolderPath();
    static bool saveAvatarToLocal(const QString& avatarPath, const QString& user_id);
    static bool saveAvatarToLocal(const QPixmap& pixmap,const QString& user_id);
};

#endif // IMAGEUTILS_H
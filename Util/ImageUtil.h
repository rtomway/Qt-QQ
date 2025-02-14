#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QPixmap>
#include <QSize>
#include <QGuiApplication> 

class ImageUtils {
public:
    static QPixmap roundedPixmap(const QPixmap& pixmap, QSize size, int radius);
    static QPixmap roundedPixmap(const QPixmap& pixmap, QSize size);
};

#endif // IMAGEUTILS_H
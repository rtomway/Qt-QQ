#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QImage>
#include <QSize>

class ImageUtils {
public:
    static QString getAvatarFolderPath();
	//图片的保存加载
	static bool saveImage(const QString& user_id, const QImage& image);
	static QByteArray loadImage(const QString& user_id);
};

#endif // IMAGEUTILS_H
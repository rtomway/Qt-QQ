#include "ImageUtil.h"
#include <QStandardPaths>
#include <QDir>
#include <QBuffer>

//保存目录
QString ImageUtils::getAvatarFolderPath()
{
	QString avatarFolder = QStandardPaths::writableLocation
	(QStandardPaths::AppDataLocation)+"/avatars";

	// 如果目录不存在，则创建
	QDir dir;
	if (!dir.exists(avatarFolder)) {
		dir.mkpath(avatarFolder);
	}
	return avatarFolder;
}
//保存图片
bool ImageUtils::saveImage(const QString& user_id, const QImage& image)
{
	//保存目录
	QString avatarFolder = getAvatarFolderPath();
	qDebug() << "avatarFolder:" << avatarFolder;
	auto avatar_Path = avatarFolder + "/" + user_id + ".png";
	if (!image.save(avatar_Path))
	{
		qWarning() << "Failed to save avatar for user:" << user_id;
		return false;
	}
	return true;
}
//加载图片
QByteArray ImageUtils::loadImage(const QString& user_id)
{
	// 确定保存目录
	QString avatarFolder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/avatars";
	QDir().mkpath(avatarFolder);  // 确保目录存在
	auto avatarPath = avatarFolder + "/" + user_id + ".png";
	qDebug() << "Attempting to load image from:" << avatarPath;
	// 加载图片
	QFile file(avatarPath);
	if (!file.exists()) {
		qDebug() << "File does not exist:" << avatarPath;

		QImage image(":/picture/Resource/Picture/qq.png");
		// 创建一个 QByteArray 来保存图像数据
		QByteArray byteArray;

		// 将 QImage 保存到 QByteArray，指定格式为 PNG（你也可以选择其他格式）
		QBuffer buffer(&byteArray);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG"); // 这里可以更换为 "JPG", "BMP" 等格式
		return byteArray;
	}

	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Could not open the file:" << avatarPath << "Error:" << file.errorString();
		return QByteArray();
	}

	if (file.size() == 0) {
		qDebug() << "File is empty:" << avatarPath;
		return QByteArray();
	}

	QByteArray imageData = file.readAll();  // 读取图片数据

	if (imageData.isEmpty()) {
		qDebug() << "Failed to read image data from:" << avatarPath;
	}
	else {
		qDebug() << "Successfully loaded image from:" << avatarPath << "Size:" << imageData.size();
	}

	return imageData;
}
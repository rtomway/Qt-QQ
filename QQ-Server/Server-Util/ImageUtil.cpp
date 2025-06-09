#include "ImageUtil.h"
#include <QStandardPaths>
#include <QDir>
#include <QBuffer>

//用户头像保存目录
QString ImageUtils::getUserAvatarFolderPath()
{
	//保存目录
	QString avatarFolder = QStandardPaths::writableLocation
	(QStandardPaths::AppDataLocation)+"/avatars/user_avatar";

	// 如果目录不存在，则创建
	QDir dir;
	if (!dir.exists(avatarFolder)) {
		dir.mkpath(avatarFolder);
	}
	return avatarFolder;
}

//群聊头像保存目录
QString ImageUtils::getGroupAvatarFolderPath()
{
	//保存目录
	QString avatarFolder = QStandardPaths::writableLocation
	(QStandardPaths::AppDataLocation)+"/avatars/group_avatar";

	// 如果目录不存在，则创建
	QDir dir;
	if (!dir.exists(avatarFolder)) {
		dir.mkpath(avatarFolder);
	}
	return avatarFolder;
}

//保存图片
bool ImageUtils::saveAvatarToLocal(const QString& avatarPath, const QString& user_id, ChatType type)
{
	QString avatarFolderPath;
	switch (type)
	{
	case ChatType::User:
		avatarFolderPath = getUserAvatarFolderPath();
		break;
	case ChatType::Group:
		avatarFolderPath = getGroupAvatarFolderPath();
		break;
	default:
		break;
	}
	// 使用用户 ID 来命名头像文件
	QString avatarFileName = avatarFolderPath + "/" + user_id + ".png";

	QImage avatar(avatarPath);
	if (avatar.isNull()) {
		qWarning() << "头像加载失败!";
		return false;
	}
	// 保存头像
	return avatar.save(avatarFileName);
}

//保存图片
bool ImageUtils::saveAvatarToLocal(const QImage& image, const QString& user_id, ChatType type)
{
	QString avatarFolderPath;
	switch (type)
	{
	case ChatType::User:
		avatarFolderPath = getUserAvatarFolderPath();
		break;
	case ChatType::Group:
		avatarFolderPath = getGroupAvatarFolderPath();
		break;
	default:
		break;
	}
	// 使用用户 ID 来命名头像文件
	QString avatarFileName = QDir(avatarFolderPath).filePath(user_id + ".png");

	if (image.isNull()) {
		qWarning() << "image is null, cannot save.";
		return false;
	}
	// 保存头像
	return image.save(avatarFileName);
}

//加载图片
QByteArray ImageUtils::loadImage(const QString& id, ChatType type)
{
	QString avatarFolder;
	switch (type)
	{
	case ChatType::User:
		avatarFolder = getUserAvatarFolderPath();
		break;
	case ChatType::Group:
		avatarFolder = getGroupAvatarFolderPath();
		break;
	default:
		break;
	}
	QDir().mkpath(avatarFolder);  // 确保目录存在
	auto avatarPath = avatarFolder + "/" + id + ".png";
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
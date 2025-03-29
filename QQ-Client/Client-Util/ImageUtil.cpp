#include "ImageUtil.h"
#include <QPainter>
#include <QPainterPath>
#include <QStandardPaths>
#include <QDir>

//图片变形
QPixmap ImageUtils::roundedPixmap(const QPixmap& pixmap, QSize size, int radius)
{
	QPixmap scaled = pixmap.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
	QPixmap dest(size);
	dest.fill(Qt::transparent);
	QPainter painter(&dest);
	painter.setRenderHint(QPainter::Antialiasing);
	QPainterPath path;
	path.addRoundedRect(0, 0, size.width(), size.height(), radius, radius);
	painter.setClipPath(path);
	painter.drawPixmap(0, 0, scaled);
	return dest;
}
//圆形图片
QPixmap ImageUtils::roundedPixmap(const QPixmap& pixmap, QSize size)
{
	int radius = qMin(size.width(), size.height()) / 2;
	return roundedPixmap(pixmap, size, radius);
}

QString ImageUtils::getAvatarFolderPath()
{
	//保存目录
	QString avatarFolder = QStandardPaths::writableLocation
	(QStandardPaths::AppDataLocation)+"/avatars";

	// 如果目录不存在，则创建
	QDir dir;
	if (!dir.exists(avatarFolder)) {
		dir.mkpath(avatarFolder);
	}
	return avatarFolder;
}
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
bool ImageUtils::saveAvatarToLocal(const QString& avatarPath, const QString& id, ChatType type)
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
	QString avatarFileName = avatarFolderPath + "/" + id + ".png";

	QPixmap avatar(avatarPath);
	if (avatar.isNull()) {
		qWarning() << "头像加载失败!";
		return false;
	}
	// 保存头像
	return avatar.save(avatarFileName);
}
//保存图片
bool ImageUtils::saveAvatarToLocal(const QPixmap& pixmap, const QString& id, ChatType type)
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
	QString avatarFileName = QDir(avatarFolderPath).filePath(id + ".png");

	if (pixmap.isNull()) {
		qWarning() << "pixmap is null, cannot save.";
		return false;
	}
	// 保存头像
	return pixmap.save(avatarFileName);
}

QPixmap ImageUtils::loadAvatarFromFile(const QString& avatarPath)
{
	QPixmap avatar;
	if (QFile::exists(avatarPath))
	{
		avatar.load(avatarPath);
	}
	else
	{
		avatar.load(":/picture/Resource/Picture/qq.png"); // 如果头像加载失败，使用默认头像
	}
	return avatar;
}

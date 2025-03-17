#include "ImageUtil.h"
#include <QPainter>
#include <QPainterPath>
#include <QStandardPaths>
#include <QDir>


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

bool ImageUtils::saveAvatarToLocal(const QString& avatarPath, const QString& user_id)
{
	QString avatarFolderPath = getAvatarFolderPath();
	// 使用用户 ID 来命名头像文件
	QString avatarFileName = avatarFolderPath + "/" + user_id + ".png";

	QPixmap avatar(avatarPath);
	if (avatar.isNull()) {
		qWarning() << "头像加载失败!";
		return false;
	}
	// 保存头像
	return avatar.save(avatarFileName);
}
bool ImageUtils::saveAvatarToLocal(const QPixmap& pixmap,const QString& user_id)
{
	QString avatarFolderPath = getAvatarFolderPath();
	// 使用用户 ID 来命名头像文件
	QString avatarFileName = QDir(avatarFolderPath).filePath(user_id + ".png");

	if (pixmap.isNull()) {
		qWarning() << "pixmap is null, cannot save.";
		return false;
	}
	// 保存头像
	return pixmap.save(avatarFileName);
}

#include "ImageUtil.h"
#include <QPainter>
#include <QPainterPath>
#include <QStandardPaths>
#include <QDir>
#include <QtConcurrent/QtConcurrent>

QString ImageUtils::m_currentUser = QString("");
//图片变形
QPixmap ImageUtils::roundedPixmap(const QPixmap& pixmap, QSize size, int radius)
{
	if (pixmap.isNull())
	{
		qDebug() << "roundedPixmap is NUll";
		return QPixmap();
	}
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

void ImageUtils::setLoginUser(const QString& user_id)
{
	m_currentUser = user_id;
}

//用户头像保存目录
QString ImageUtils::getUserAvatarFolderPath()
{
	//保存目录
	QString avatarFolder = QStandardPaths::writableLocation
	(QStandardPaths::AppDataLocation)+QString("/avatars/%1/user_avatar").arg(m_currentUser);

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
	(QStandardPaths::AppDataLocation)+QString("/avatars/%1/group_avatar").arg(m_currentUser);

	// 如果目录不存在，则创建
	QDir dir;
	if (!dir.exists(avatarFolder)) {
		dir.mkpath(avatarFolder);
	}
	return avatarFolder;
}

//保存图片
void ImageUtils::saveAvatarToLocal(const QString& avatarPath, const QString& id, ChatType type, std::function<void()>callBack)
{
	QFuture<bool> future = QtConcurrent::run(static_cast<bool(*)(const QString&, const QString&, ChatType)>(&ImageUtils::saveAvatarToLocalTask), avatarPath, id, type);
	// 创建 QFutureWatcher 来监听任务
	QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>();
	// 连接 finished 信号，当任务完成时触发
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, [watcher, callBack]() {
		// 获取任务的结果
		bool result = watcher->result();
		// 调用回调处理结果
		if (result)
		{
			if (callBack)
				callBack();
		}
		else
		{

		}
		// 删除 watcher，释放内存
		watcher->deleteLater();
		});
	// 设置 QFutureWatcher 监听任务
	watcher->setFuture(future);
}
bool ImageUtils::saveAvatarToLocalTask(const QString& avatarPath, const QString& id, ChatType type)
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

	QImage avatar(avatarPath);
	if (avatar.isNull()) {
		qWarning() << "头像加载失败!";
		return false;
	}
	// 保存头像
	return avatar.save(avatarFileName);
}

//保存图片
void ImageUtils::saveAvatarToLocal(const QImage& image, const QString& id, ChatType type, std::function<void()>callBack)
{
	QFuture<bool> future = QtConcurrent::run(static_cast<bool(*)(const QImage&, const QString&, ChatType)>(&ImageUtils::saveAvatarToLocalTask), image, id, type);
	// 创建 QFutureWatcher 来监听任务
	QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>();
	// 连接 finished 信号，当任务完成时触发
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, [watcher, callBack]() {
		// 获取任务的结果
		bool result = watcher->result();
		// 调用回调处理结果
		if (result)
		{
			if (callBack)
				callBack();
		}
		else
		{

		}
		// 删除 watcher，释放内存
		watcher->deleteLater();
		});
	// 设置 QFutureWatcher 监听任务
	watcher->setFuture(future);
}
bool ImageUtils::saveAvatarToLocalTask(const QImage& image, const QString& id, ChatType type)
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

	if (image.isNull()) {
		qWarning() << "image is null, cannot save.";
		return false;
	}
	// 保存头像
	return image.save(avatarFileName);
}

//保存图片
void ImageUtils::saveAvatarToLocal(const QByteArray& data, const QString& id, ChatType type, std::function<void()> callBack)
{
	QFuture<bool> future = QtConcurrent::run(static_cast<bool(*)(const QByteArray&, const QString&, ChatType)>(&ImageUtils::saveAvatarToLocalTask), data, id, type);
	// 创建 QFutureWatcher 来监听任务
	QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>();
	// 连接 finished 信号，当任务完成时触发
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, [watcher, callBack]() {
		// 获取任务的结果
		bool result = watcher->result();
		// 调用回调处理结果
		if (result)
		{
			if (callBack)
				callBack();
		}
		else
		{

		}
		// 删除 watcher，释放内存
		watcher->deleteLater();
		});
	// 设置 QFutureWatcher 监听任务
	watcher->setFuture(future);
}
bool ImageUtils::saveAvatarToLocalTask(const QByteArray& data, const QString& id, ChatType type)
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

	// 写入二进制数据到文件
	QFile file(avatarFileName);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning() << "无法保存文件：" << avatarFileName;
		return false;
	}
	// 写入数据
	file.write(data);
	file.close();

	return true;
}

//加载图片
void ImageUtils::loadAvatarFromFile(const QString& avatarPath, std::function<void(QImage)>callBack)
{
	QFuture<QImage> future = QtConcurrent::run(static_cast<QImage(*)(const QString&)>(&ImageUtils::loadAvatarFromFileTask), avatarPath);
	// 创建 QFutureWatcher 来监听任务
	QFutureWatcher<QImage>* watcher = new QFutureWatcher<QImage>();
	// 连接 finished 信号，当任务完成时触发
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, [watcher, callBack]() {
		// 获取任务的结果
		QImage image = watcher->result();
		// 调用回调处理结果
		callBack(image);
		// 删除 watcher，释放内存
		watcher->deleteLater();
		});
	// 设置 QFutureWatcher 监听任务
	watcher->setFuture(future);
}
QImage  ImageUtils::loadAvatarFromFileTask(const QString& avatarPath)
{
	QImage avatar;
	if (QFile::exists(avatarPath))
	{
		if (!avatar.load(avatarPath))
		{
			qDebug() << "loadAvatarFromFile:加载图片失败：" << avatarPath;
		}
	}
	else
	{
		if (!avatar.load(":/picture/Resource/Picture/qq.png"))// 如果头像加载失败，使用默认头像
		{
			qDebug() << "loadAvatarFromFile:默认图片加载失败";
		}
	}
	return avatar;
}




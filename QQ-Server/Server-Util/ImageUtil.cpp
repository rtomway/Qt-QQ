#include "ImageUtil.h"
#include <QStandardPaths>
#include <QDir>
#include <QBuffer>
#include <QtConcurrent/QtConcurrent>

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
	QObject::connect(watcher, &QFutureWatcher<bool>::finished, [watcher, callBack]()
		{
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
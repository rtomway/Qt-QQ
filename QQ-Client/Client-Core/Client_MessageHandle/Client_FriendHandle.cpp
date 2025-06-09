#include "Client_FriendHandle.h"
#include <QPixmap>

#include "EventBus.h"
#include "ImageUtil.h"

void Client_FriendHandle::handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "无数据";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_friendRequest(paramsObject, pixmap);
		});
}

void Client_FriendHandle::handle_newFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	ImageUtils::saveAvatarToLocal(data, user_id, ChatType::User, [=](bool result)
		{
			if (!result)
			{
				qDebug() << "头像保存失败";
				return;
			}
			EventBus::instance()->emit newFriend(paramsObject);
		});
}

void Client_FriendHandle::handle_friendDeleted(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "无数据";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_friendReply(paramsObject, pixmap);
		});
}

void Client_FriendHandle::handle_rejectAddFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "无数据";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_friendReply(paramsObject, pixmap);
		});
}

void Client_FriendHandle::handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit textCommunication(paramsObject);
}

void Client_FriendHandle::handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), 
		[paramsObject, data]() 
	{
		QPixmap pixmap;
		if (data.isEmpty())
		{
			qDebug() << "无数据";
		}
		else if (!pixmap.loadFromData(data))
		{
			qDebug() << "client-头像加载失败";
			return;
		};
		EventBus::instance()->emit pictureCommunication(paramsObject, pixmap);
	});
}

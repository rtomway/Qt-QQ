#include "Client_FriendHandle.h"
#include <QPixmap>

#include "EventBus.h"

void Client_FriendHandle::handle_addFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap pixmap;
	if (data.isEmpty())
	{
		qDebug() << "无数据";
		pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	else if (!pixmap.loadFromData(data))
	{
		qDebug() << "client-头像加载失败";
		return;
	};
	EventBus::instance()->emit addFriend(paramsObject, pixmap);
}

void Client_FriendHandle::handle_newFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar";
		avatar = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	EventBus::instance()->emit newFriend(paramsObject, avatar);
}

void Client_FriendHandle::handle_rejectAddFriend(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap pixmap;
	if (data.isEmpty())
	{
		qDebug() << "无数据";
		pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
		EventBus::instance()->emit rejectAddFriend(paramsObject, pixmap);
	}
	if (!pixmap.loadFromData(data))
	{
		qDebug() << "client-头像加载失败";
		return;
	};
	EventBus::instance()->emit rejectAddFriend(paramsObject, pixmap);
}

void Client_FriendHandle::handle_textCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit textCommunication(paramsObject);
}

void Client_FriendHandle::handle_pictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
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
}

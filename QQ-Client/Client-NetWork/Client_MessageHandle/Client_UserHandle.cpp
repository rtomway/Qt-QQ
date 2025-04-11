#include "Client_UserHandle.h"
#include <QPixmap>
#include <EventBus.h>

void Client_UserHandle::handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data)
{
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar";
		avatar = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	EventBus::instance()->emit searchUser(paramsObject, avatar);
}

void Client_UserHandle::handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit updateUserMessage(paramsObject);
}

void Client_UserHandle::handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	qDebug() << "好友" << user_id << "的头像更新---------- - ";
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}

	EventBus::instance()->emit updateUserAvatar(user_id, avatar);
}

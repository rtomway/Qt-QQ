#include "Client_GroupHandle.h"
#include <QPixmap>

#include "ImageUtil.h"
#include "EventBus.h"

void Client_GroupHandle::handle_addGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]() {
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
		EventBus::instance()->emit addGroup(paramsObject, pixmap);
		});
}
void Client_GroupHandle::handle_createGroupSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "handle_createGroupSuccess:" << paramsObject;
	auto group_id = paramsObject["group_id"].toString();
	ImageUtils::saveAvatarToLocal(data, group_id, ChatType::Group, [=](bool result)
		{
			if (result)
				EventBus::instance()->emit createGroupSuccess(paramsObject);
		});

}
void Client_GroupHandle::handle_groupInvite(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]() {
		auto user_id = paramsObject["user_id"].toString();
		qDebug() << "群聊邀请";
		// 1️⃣ 把 QByteArray 转换成 QPixmap
		QPixmap avatar;
		if (!avatar.loadFromData(data))  // 从二进制数据加载图片
		{
			qWarning() << "Failed to load avatar for user:" << user_id;
			return;
		}
		EventBus::instance()->emit groupInvite(paramsObject, avatar);
		});
}
void Client_GroupHandle::handle_groupTextCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit groupTextCommunication(paramsObject);
}
void Client_GroupHandle::handle_groupPictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]() {
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
		EventBus::instance()->emit groupPictureCommunication(paramsObject, pixmap);
		});
}
void Client_GroupHandle::handle_newGroupMember(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	EventBus::instance()->emit saveAvatar(user_id, ChatType::User, data);
	EventBus::instance()->emit newGroupMember(paramsObject);
}
void Client_GroupHandle::handle_groupInviteSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "邀请成功";
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to groupMemberLoad avatar for user:";
		return;
	}
	EventBus::instance()->emit groupInviteSuccess(paramsObject, avatar);
}
void Client_GroupHandle::handle_groupMemberLoad(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto member_id = paramsObject["user_id"].toString();
	EventBus::instance()->emit saveAvatar(member_id, ChatType::User, data);
	EventBus::instance()->emit groupMemberload(paramsObject);
}
void Client_GroupHandle::handle_newGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto group_id = paramsObject["group_id"].toString();
	ImageUtils::saveAvatarToLocal(data, group_id, ChatType::Group, [=](bool result)
		{
			if (!result)
				return;
			EventBus::instance()->emit newGroup(paramsObject);
		});

}
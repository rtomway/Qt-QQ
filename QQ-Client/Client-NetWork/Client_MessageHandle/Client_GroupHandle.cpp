#include "Client_GroupHandle.h"
#include <QPixmap>

#include "EventBus.h"

void Client_GroupHandle::handle_createGroupSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	qDebug() << "handle_createGroupSuccess:" << paramsObject;
	EventBus::instance()->emit createGroupSuccess(paramsObject);
}
void Client_GroupHandle::handle_groupInvite(const QJsonObject& paramsObject, const QByteArray& data)
{
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
}
void Client_GroupHandle::handle_groupTextCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit groupTextCommunication(paramsObject);
}
void Client_GroupHandle::handle_newGroupMember(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	qDebug() << "新群成员" << user_id;
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}

	EventBus::instance()->emit newGroupMember(paramsObject, avatar);
}
void Client_GroupHandle::handle_groupInviteSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	qDebug() << "邀请成功";
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to load avatar for user:" << user_id;
		return;
	}

	EventBus::instance()->emit groupInviteSuccess(paramsObject, avatar);
}
void Client_GroupHandle::handle_groupMemberLoad(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto user_id = paramsObject["user_id"].toString();
	qDebug() << "邀请成功";
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to groupMemberLoad avatar for user:" << user_id;
		return;
	}

	EventBus::instance()->emit loadGroupMember(paramsObject, avatar);
}
void Client_GroupHandle::handle_groupLoad(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto group_id = paramsObject["group_id"].toString();
	qDebug() << "邀请成功";
	// 1️⃣ 把 QByteArray 转换成 QPixmap
	QPixmap avatar;
	if (!avatar.loadFromData(data))  // 从二进制数据加载图片
	{
		qWarning() << "Failed to groupLoad avatar for user:" << group_id;
		return;
	}

	EventBus::instance()->emit loadGroup(paramsObject, avatar);
}
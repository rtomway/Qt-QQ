#include "Client_GroupHandle.h"
#include <QPixmap>

#include "ImageUtil.h"
#include "EventBus.h"

void Client_GroupHandle::handle_addGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_groupRequest(paramsObject, pixmap);
		});
}

void Client_GroupHandle::handle_createGroupSuccess(const QJsonObject& paramsObject, const QByteArray& data)
{

	qDebug() << "handle_createGroupSuccess:" << paramsObject;
	auto group_id = paramsObject["group_id"].toString();
	ImageUtils::saveAvatarToLocal(data, group_id, ChatType::Group, [=]()
		{
			qDebug() << "handle_createGroupSuccess_33" << paramsObject;
			EventBus::instance()->emit createGroupSuccess(paramsObject);
		});

}

void Client_GroupHandle::handle_groupInvite(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			auto user_id = paramsObject["user_id"].toString();
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_groupRequest(paramsObject, pixmap);
		});
}

void Client_GroupHandle::handle_groupTextCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit groupTextCommunication(paramsObject);
}

void Client_GroupHandle::handle_groupPictureCommunication(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
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
	QPixmap pixmap;
	if (data.isEmpty() || !pixmap.loadFromData(data))
	{
		qDebug() << "load Data Failed";
		pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
	}
	EventBus::instance()->emit notice_groupReply(paramsObject, pixmap);
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
	ImageUtils::saveAvatarToLocal(data, group_id, ChatType::Group, [=]()
		{
			EventBus::instance()->emit newGroup(paramsObject);
		});

}

void Client_GroupHandle::handle_groupMemberDeleted(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit groupMemberDeleted(paramsObject);
}

void Client_GroupHandle::handle_groupMemberExitGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_groupReply(paramsObject, pixmap);
		});
}

void Client_GroupHandle::handle_rejectAddGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_groupReply(paramsObject, pixmap);
		});
}

void Client_GroupHandle::handle_groupAddFailed(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_groupReply(paramsObject, pixmap);
		});
}

void Client_GroupHandle::handle_disbandGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	auto group_id = paramsObject["group_id"].toString();
	EventBus::instance()->emit disbandGroup(group_id);
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_groupReply(paramsObject, pixmap);
		});
}

void Client_GroupHandle::handle_beRemovedGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit removeGroup(paramsObject);
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(),
		[paramsObject, data]()
		{
			QPixmap pixmap;
			if (data.isEmpty() || !pixmap.loadFromData(data))
			{
				qDebug() << "load Data Failed";
				pixmap = QPixmap(":/picture/Resource/Picture/qq.png");
			}
			EventBus::instance()->emit notice_groupReply(paramsObject, pixmap);
		});
}

void Client_GroupHandle::handle_batch_groupMemberDeleted(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit batch_groupMemberDeleted(paramsObject);
}

void Client_GroupHandle::handle_updateGroupAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]()
		{
			auto group_id = paramsObject["group_id"].toString();
			QPixmap avatar;
			if (!avatar.loadFromData(data))
			{
				qWarning() << "Failed to load avatar for group:" << group_id;
				return;
			}
			EventBus::instance()->emit updateGroupAvatar(group_id, avatar);
		});
}

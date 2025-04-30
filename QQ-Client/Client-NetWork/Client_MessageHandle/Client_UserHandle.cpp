#include "Client_UserHandle.h"
#include "Client_UserHandle.h"
#include <QPixmap>
#include <EventBus.h>
#include <QCoreApplication>

void Client_UserHandle::handle_searchUser(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]() {
		QPixmap avatar;
		if (!avatar.loadFromData(data)) {
			qWarning() << "Failed to load avatar";
			avatar = QPixmap(":/picture/Resource/Picture/qq.png");
		}
		EventBus::instance()->emit searchUser(paramsObject, avatar);
		});
}

void Client_UserHandle::handle_searchGroup(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]() {
		QPixmap avatar;
		if (!avatar.loadFromData(data)) {
			qWarning() << "Failed to load avatar";
			avatar = QPixmap(":/picture/Resource/Picture/qq.png");
		}
		EventBus::instance()->emit searchGroup(paramsObject, avatar);
		});
}

void Client_UserHandle::handle_updateUserMessage(const QJsonObject& paramsObject, const QByteArray& data)
{
	EventBus::instance()->emit updateUserMessage(paramsObject);
}

void Client_UserHandle::handle_updateUserAvatar(const QJsonObject& paramsObject, const QByteArray& data)
{
	// 将操作抛到主线程执行
	QMetaObject::invokeMethod(QCoreApplication::instance(), [paramsObject, data]() {
		auto user_id = paramsObject["user_id"].toString();
		QPixmap avatar;
		if (!avatar.loadFromData(data))
		{
			qWarning() << "Failed to load avatar for user:" << user_id;
			return;
		}
		EventBus::instance()->emit updateUserAvatar(user_id, avatar);
		});
}

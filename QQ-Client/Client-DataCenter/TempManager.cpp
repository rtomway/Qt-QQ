#include "TempManager.h"
#include "EventBus.h"

TempManager* TempManager::instance()
{
	static TempManager ins;
	return &ins;
}

TempManager::TempManager()
{
	connect(EventBus::instance(), &EventBus::addFriend, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto user_id = obj["user_id"].toString();
			addFriendRequest(user_id, obj, pixmap);
			emit FriendRequest(user_id);
		});
	connect(EventBus::instance(), &EventBus::rejectAddFriend, this, [=](const QJsonObject& obj)
		{
			auto user_id = obj["user_id"].toString();
			emit FriendResponse(user_id);
		});
	connect(EventBus::instance(), &EventBus::groupInvite, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto group_id = obj["group_id"].toString();
			addGroupRequest(group_id, obj, pixmap);
			emit GroupInvite(group_id);
		});
}

void TempManager::addFriendRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap)
{
	// 检查好友请求是否已存在，如果存在则不重复添加
	if (!m_tempFriendRequests.contains(id)) {
		TempRequestInfo request;
		request.requestData = requestInfo;
		request.avatar = pixmap;
		m_tempFriendRequests[id] = request;
		qDebug() << "Added friend request for user ID:" << id;
	}
}

void TempManager::addGroupRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap)
{
	// 检查群组请求是否已存在，如果存在则不重复添加
	if (!m_tempGroupRequests.contains(id)) {
		TempRequestInfo request;
		request.requestData = requestInfo;
		request.avatar = pixmap;
		m_tempGroupRequests[id] = request;
		qDebug() << "Added group invite for group ID:" << id;
	}
}

TempRequestInfo TempManager::getFriendRequestInfo(const QString& id)
{
	// 如果存在请求，返回请求信息，否则返回空
	if (m_tempFriendRequests.contains(id)) {
		return m_tempFriendRequests[id];
	}
	return TempRequestInfo();
}

TempRequestInfo TempManager::getGroupRequestInfo(const QString& id)
{
	// 如果存在群组邀请，返回邀请信息，否则返回空
	if (m_tempGroupRequests.contains(id)) {
		return m_tempGroupRequests[id];
	}
	return TempRequestInfo();
}

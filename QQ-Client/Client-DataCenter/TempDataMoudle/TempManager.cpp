#include "TempManager.h"
#include "EventBus.h"

TempManager* TempManager::instance()
{
	static TempManager ins;
	return &ins;
}

TempManager::TempManager()
{
	//群组请求
	connect(EventBus::instance(), &EventBus::notice_groupRequest, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto group_id = obj["group_id"].toString();
			addGroupRequest(group_id, obj, pixmap);
			emit notice_groupRequest(group_id);
		});
	//群组回复
	connect(EventBus::instance(), &EventBus::notice_groupReply, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto group_id = obj["group_id"].toString();
			addGroupReply(group_id, obj, pixmap);
			emit notice_groupReply(group_id);
		});
	//好友请求
	connect(EventBus::instance(), &EventBus::notice_friendRequest, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto user_id = obj["user_id"].toString();
			addFriendRequest(user_id, obj, pixmap);
			emit notice_friendRequest(user_id);
		});
	//好友回复
	connect(EventBus::instance(), &EventBus::notice_friendReply, this, [=](const QJsonObject& obj, const QPixmap& pixmap)
		{
			auto user_id = obj["user_id"].toString();
			addFriendReply(user_id, obj, pixmap);
			emit notice_friendReply(user_id);
		});
}

// 请求
void TempManager::addFriendRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap)
{
	TempRequestInfo request;
	request.requestData = requestInfo;
	request.avatar = pixmap;
	m_tempFriendRequests[id] = request;
}
void TempManager::addGroupRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap)
{
	TempRequestInfo request;
	request.requestData = requestInfo;
	request.avatar = pixmap;
	m_tempGroupRequests[id] = request;
}

// 获取请求的详细信息
TempRequestInfo TempManager::getFriendRequestInfo(const QString& id)
{
	if (m_tempFriendRequests.contains(id)) 
	{
		return m_tempFriendRequests[id];
	}
	return TempRequestInfo();
}
TempRequestInfo TempManager::getGroupRequestInfo(const QString& id)
{
	if (m_tempGroupRequests.contains(id)) 
	{
		return m_tempGroupRequests[id];
	}
	return TempRequestInfo();
}

// 回复
void TempManager::addFriendReply(const QString& id, const QJsonObject& replyInfo, const QPixmap& pixmap)
{
	TempReplyInfo reply;
	reply.replyData = replyInfo;
	reply.avatar = pixmap;
	m_tempFriendReplys[id] = reply;
}
void TempManager::addGroupReply(const QString& id, const QJsonObject& replyInfo, const QPixmap& pixmap)
{
	TempReplyInfo reply;
	reply.replyData = replyInfo;
	reply.avatar = pixmap;
	m_tempGroupReplys[id] = reply;
}

// 获取回复的详细信息
TempReplyInfo TempManager::getFriendReplyInfo(const QString& id)
{
	if (m_tempFriendReplys.contains(id)) 
	{
		return m_tempFriendReplys[id];
	}
	return TempReplyInfo();
}
TempReplyInfo TempManager::getGroupReplyInfo(const QString& id)
{
	if (m_tempGroupReplys.contains(id)) 
	{
		return m_tempGroupReplys[id];
	}
	return TempReplyInfo();
}


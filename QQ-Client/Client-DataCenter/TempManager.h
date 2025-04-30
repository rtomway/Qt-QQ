#ifndef TEMPMANAGER_H_
#define TEMPMANAGER_H_

#include <QObject>
#include <QJsonObject>
#include <QPixmap>

struct TempRequestInfo {
	QJsonObject requestData;  // 请求相关的JSON数据
	QPixmap avatar;          // 对应用户或群组的头像
};
struct TempReplyInfo {
	QJsonObject replyData;
	QPixmap avatar;
};

class TempManager :public QObject
{
	Q_OBJECT
public:
	// 获取单例实例的静态方法
	static TempManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	TempManager(const TempManager&) = delete;
	TempManager& operator=(const TempManager&) = delete;
private:
	TempManager();
public:
	//请求
	void addFriendRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap);
	void addGroupRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap);
	TempRequestInfo getFriendRequestInfo(const QString& id);
	TempRequestInfo getGroupRequestInfo(const QString& id);
	//回复
	void addFriendReply(const QString& id, const QJsonObject& replyInfo, const QPixmap& pixmap);
	void addGroupReply(const QString& id, const QJsonObject& replyInfo, const QPixmap& pixmap);
	TempReplyInfo getFriendReplyInfo(const QString& id);
	TempReplyInfo getGroupReplyInfo(const QString& id);
private:
	// 存储所有临时请求数据（如好友申请、群组邀请）
	QMap<QString, TempRequestInfo> m_tempFriendRequests{};
	QMap<QString, TempRequestInfo> m_tempGroupRequests{};
	// 存储所有请求的回复数据（同意或拒绝）
	QMap<QString, TempReplyInfo> m_tempFriendReplys{};
	QMap<QString, TempReplyInfo> m_tempGroupReplys{};
signals:
	void FriendRequest(const QString& user_id);
	void FriendResponse(const QString& user_id);
	void GroupInvite(const QString& group_id);
	void GroupInviteSuccess(const QString& group_id);
	void groupAddRequest(const QString& group_id);
};

#endif // !TEMPMANAGER_H_

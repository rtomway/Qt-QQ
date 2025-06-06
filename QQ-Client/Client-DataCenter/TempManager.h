#ifndef TEMPMANAGER_H_
#define TEMPMANAGER_H_

#include <QObject>
#include <QJsonObject>
#include <QPixmap>

struct TempRequestInfo 
{
	QJsonObject requestData;  // 请求相关的JSON数据
	QPixmap avatar;          // 对应用户或群组的头像
};
struct TempReplyInfo 
{
	QJsonObject replyData;
	QPixmap avatar;
};

class TempManager :public QObject
{
	Q_OBJECT
public:
	static TempManager* instance();
private:
	TempManager();
	TempManager(const TempManager&) = delete;
	TempManager& operator=(const TempManager&) = delete;
public:
	TempRequestInfo getFriendRequestInfo(const QString& id);
	TempRequestInfo getGroupRequestInfo(const QString& id);

	TempReplyInfo getFriendReplyInfo(const QString& id);
	TempReplyInfo getGroupReplyInfo(const QString& id);
private:
	//请求
	void addFriendRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap);
	void addGroupRequest(const QString& id, const QJsonObject& requestInfo, const QPixmap& pixmap);
	//回复
	void addFriendReply(const QString& id, const QJsonObject& replyInfo, const QPixmap& pixmap);
	void addGroupReply(const QString& id, const QJsonObject& replyInfo, const QPixmap& pixmap);
private:
	// 存储所有请求数据
	QMap<QString, TempRequestInfo> m_tempFriendRequests{};
	QMap<QString, TempRequestInfo> m_tempGroupRequests{};
	// 存储所有回复/通知数据
	QMap<QString, TempReplyInfo> m_tempFriendReplys{};
	QMap<QString, TempReplyInfo> m_tempGroupReplys{};
signals:
	void notice_groupRequest(const QString& group_id);
	void notice_groupReply(const QString& group_id);
	void notice_friendRequest(const QString& user_id);
	void notice_friendReply(const QString& user_id);
};

#endif // !TEMPMANAGER_H_

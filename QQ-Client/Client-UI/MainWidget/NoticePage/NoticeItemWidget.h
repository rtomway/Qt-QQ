#ifndef NOTICEITEMWIDGET_H_
#define NOTICEITEMWIDGET_H_

#include <QWidget>
#include <memory>

#include "AddWidget.h"

namespace Ui { class NoticeItemWidget; }

class NoticeItemWidget :public QWidget
{
	Q_OBJECT
public:
	enum class NoticeType
	{
		RequestNotice,
		ReplyNotice,
	};
	enum class RequestType
	{
		FriendAddRequest,
		GroupInviteRequest,
		UserAddGroupRequest
	};
public:
	NoticeItemWidget(QWidget* parent = nullptr);
	~NoticeItemWidget();
private:
	void init();
public:
	 void setUser(const QJsonObject& obj,const QPixmap&pixmap,NoticeItemWidget::NoticeType type);
private:
	Ui::NoticeItemWidget* ui{};
private:
	QString m_userName{};
	QString m_userId{};
	QString m_groupName{};
	QString m_groupId{};	
	QPixmap m_headPix{};
	RequestType m_requestType{};
	std::unique_ptr<AddWidget> m_addWidget;
signals:
	void newlyFriend(const QJsonObject& obj);
};

#endif // !NOTICEITEMWIDGET_H_

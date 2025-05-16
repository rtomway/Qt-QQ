#ifndef GROUPMEMBERAVATARWIDGET_H_
#define GROUPMEMBERAVATARWIDGET_H_

#include <QWidget>
#include <QLabel>
#include "UserProfilePage.h"
enum MemberWidgetType
{
	Avatar,
	Invite
};		

class GroupMemberAvatarWidget :public QWidget
{
	Q_OBJECT
public:
	GroupMemberAvatarWidget(const QString& groupMember_id, const QString& groupMember_name,MemberWidgetType type= MemberWidgetType::Avatar,QWidget* parent = nullptr);
	~GroupMemberAvatarWidget();
private:
	void init();
	void leftButtonPress();
	void rightButtonPress();
protected:
	bool eventFilter(QObject* watched, QEvent* event)override;
private:
	QLabel* m_avatarLab{};
	QLabel* m_nameLab{};
	QString m_groupMember_id{};
	MemberWidgetType m_type{};
	//左键独立窗口
	std::unique_ptr<UserProfilePage> m_userProfileWidget{};
};

#endif // !GROUPMEMBERAVATARWIDGET_H_

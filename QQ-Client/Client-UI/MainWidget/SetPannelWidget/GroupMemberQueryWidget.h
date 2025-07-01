#ifndef GROUPMEMBERQUERYWIDGETY_H_
#define GROUPMEMBERQUERYWIDGETY_H_

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>

class GroupMemberQueryWidget :public QWidget
{
	Q_OBJECT
public:
	GroupMemberQueryWidget(QWidget* parent = nullptr);
	~GroupMemberQueryWidget();
public:
	void loadGroupMemberList(const QString& group_id);
private:
	void init();
	void addListItemWidget(const QString&groupMember_id);
	void clearMemberListItem();
	QStringList matchText(const QString& text);
private:
	QPushButton* m_backBtn{};
	QLineEdit* m_searchLine{};
	QListWidget* m_memberList{};
	QString m_group_id;
	QStringList m_nameList{};
	QStringList m_idList{};
	QHash<QString, QString> m_groupHash;
signals:
	void backGroupPannel();
};


#endif // !GROUPMEMBERQUERYWIDGETY_H_

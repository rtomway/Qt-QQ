#ifndef GROUP_H_
#define GROUP_H_

#include <QString>
#include <QHash>
#include <QPixmap>
#include <QJsonObject>

struct Member
{
	QString member_id;
	QString member_name;
	QString member_role;
};
class Group
{
public:
	void setGroup(const QJsonObject& groupObj);
	void addMember(const QJsonObject& memberObj);
	void setMemberName(const QString& member_id, const QString& member_name);
	void removeMember(const QString& member_id);
	const QString& getGroupId()const;
	const QString& getGroupOwerId()const;
	const QString& getGroupName()const;
	void setGrouping(const QString& grouping);
	const QString& getGrouping()const;
	const Member& getMember(const QString& member_id)const;
	const QHash<QString, Member>& getMembers() const;
	const int& count()const;
private:
	QString m_groupId{};
	QString m_groupName{};
	QString m_groupOwnerId{};
	QString m_grouping{};
	QHash<QString, Member>m_members;
};

#endif // !GROUP_H_

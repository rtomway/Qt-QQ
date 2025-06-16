#ifndef GROUP_H_
#define GROUP_H_

#include <QHash>
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
	const QJsonObject& getGroupProfile();
	//群组、群成员的加载
	void setGroup(const QJsonObject& groupObj);
	void batchLoadGroupMember(const QJsonArray& memberArray);
	void loadGroupMember(const QJsonObject& memberObj);
	//群组信息的获取
	const QString& getGroupId()const;
	const QString& getGroupOwerId()const;
	const QString& getGroupName()const;
	const QString& getGrouping()const;
	Member getMember(const QString& member_id)const;
	QStringList getGroupMembersIdList()const;
	const QHash<QString, Member>& getMembers() const;
	const int& count()const;
	//群组操作
	void addMember(const QJsonObject& memberObj);
	void removeMember(const QString& member_id);
	void setGrouping(const QString& grouping);
private:
	QString memberRole(const QString& role);

private:
	QString m_groupId{};
	QString m_groupName{};
	QString m_groupOwnerId{};
	QString m_grouping{};
	int m_groupMemberCount{};
	QJsonObject m_groupJson{};
	QHash<QString, Member>m_members{};
};

#endif // !GROUP_H_

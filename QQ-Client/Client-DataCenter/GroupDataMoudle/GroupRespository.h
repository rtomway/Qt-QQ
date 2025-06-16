#ifndef GROUPRESPOSITORY_H_
#define GROUPRESPOSITORY_H_

#include <QJsonObject>
#include <QObject>
#include <QSharedPointer>

#include "Group.h" 

class GroupRespository :public QObject
{
	Q_OBJECT
public:
	GroupRespository(QObject* parent = nullptr);
	~GroupRespository();
public:
	//群组操作
	void addGroup(const QSharedPointer<Group>& group);
	void removeGroup(const QString& groupId);
	QSharedPointer<Group> findGroup(const QString& groupId) const;
	const QHash<QString, QSharedPointer<Group>>& getGroups()const;
	void clearGroupRespository();
private:
	QHash<QString, QSharedPointer<Group>> m_groups{};
};


#endif // !GROUPRESPOSITORY_H_

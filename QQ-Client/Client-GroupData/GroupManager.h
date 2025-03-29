#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_

#include <QString>
#include <QHash>
#include <QPixmap>
#include <QJsonObject>
#include <QObject>
#include <memory>

#include "Group.h"

class GroupManager :public QObject
{
	Q_OBJECT
public:
	// 获取单例实例的静态方法
	static GroupManager* instance();
	// 禁止拷贝构造函数和赋值操作符
	GroupManager(const GroupManager&) = delete;
	GroupManager& operator=(const GroupManager&) = delete;
	void addGroup(const QSharedPointer<Group>& group);
	void removeGroup(const QString& groupId);
	QSharedPointer<Group> findGroup(const QString& groupId) const;
private:
	explicit GroupManager();
private:
	QHash<QString, QSharedPointer<Group>> m_groups;
signals:
	void newGroup(const QString& user_id);
};

#endif // !GROUPMANAGER_H_

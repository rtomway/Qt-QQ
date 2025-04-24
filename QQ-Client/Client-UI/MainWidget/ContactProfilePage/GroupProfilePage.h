#ifndef GROUPPROFILEPAGE_H_
#define GROUPPROFILEPAGE_H_

#include <QWidget>
#include <QJsonObject>

#include "Group.h"

namespace Ui { class GroupProfilePage; }

class GroupProfilePage :public QWidget
{
	Q_OBJECT
public:
	GroupProfilePage(QWidget* parent = nullptr);
	void init();
public:
	void setGroupProfile(const QString& group_id);
	void clearWidget();
private:
	Ui::GroupProfilePage* ui{};
	QSharedPointer<Group>m_group{};
	QString m_groupId{};
	QJsonObject m_groupJson{};
signals:
	void chatWithGroup(const QString& group_id);
};

#endif // GROUPPROFILEPAGE_H_




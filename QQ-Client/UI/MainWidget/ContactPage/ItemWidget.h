#ifndef ITEMWIDGET_H_
#define ITEMWIDGET_H_

#include <QWidget>
#include <QJsonObject>

#include "Friend.h"

namespace Ui { class ItemWidget; }
class ItemWidget :public QWidget
{
	Q_OBJECT
public:
	ItemWidget(QWidget* parent = nullptr);
	~ItemWidget();
	void init();
private:
	Ui::ItemWidget* ui{};
	QJsonObject m_json{};
	QString m_userId{};
	QString m_grouping{};
	QSharedPointer<Friend>m_oneself{};
public:
	void setUser(const QString& user_id);
	void setGrouping(const QString& grouping);
	const QJsonObject& getUser();
	const QString& getGrouping();
	const QString& getUserId();
};

#endif // !ITEMWIDGET_H_

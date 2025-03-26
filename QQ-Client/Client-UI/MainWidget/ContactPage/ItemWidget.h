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
private:
	void init();
public:
	void setUser(const QString& user_id);
	void updatePixmap();
	void setGrouping(const QString& grouping);
	const QJsonObject& getUser();
	const QString& getGrouping();
	const QString& getUserId();
private:
	Ui::ItemWidget* ui{};
	QSharedPointer<Friend>m_oneself{};
	QJsonObject m_json{};
	QString m_userId{};
	QString m_grouping{};
};

#endif // !ITEMWIDGET_H_

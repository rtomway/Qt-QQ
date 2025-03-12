#ifndef ITEMWIDGET_H_
#define ITEMWIDGET_H_

#include <QWidget>
#include <QJsonObject>

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
	QString m_grouping{};
public:
	void setUser(const QJsonObject& obj);
	void setGrouping(const QString& grouping);
	const QJsonObject& getUser();
	const QString& getGrouping();
};

#endif // !ITEMWIDGET_H_

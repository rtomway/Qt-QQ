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
public:
	void setUser(const QJsonObject& obj);
	const QJsonObject& getUser();
};

#endif // !ITEMWIDGET_H_

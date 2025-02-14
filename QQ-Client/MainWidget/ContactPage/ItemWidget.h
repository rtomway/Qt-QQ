#ifndef ITEMWIDGET_H_
#define ITEMWIDGET_H_

#include <QWidget>

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
public:
	void setName(QString name);
};

#endif // !ITEMWIDGET_H_

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
	virtual void setItemWidget(const QString& user_id) = 0;
protected:
	Ui::ItemWidget* ui{};
};

#endif // !ITEMWIDGET_H_

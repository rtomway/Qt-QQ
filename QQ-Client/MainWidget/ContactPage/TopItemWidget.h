#ifndef TOPITEMWIDGET_H_
#define TOPITEMWIDGET_H_

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QTreeWidgetItem>

#include "RotationLabel.h"

class TopItemWidget :public QWidget
{
	Q_OBJECT
public:
	TopItemWidget(QWidget* parent = nullptr);
	~TopItemWidget();
	void init();
private:
	QBoxLayout* m_hLayout{};
	RotationLabel* m_headLab{};
	QLabel* m_nameLab{};
	QLabel* m_countLab{};
	QTreeWidgetItem* m_item{};
public:
	void setCount(int x);
	void setName(QString name);
	void setItem(QTreeWidgetItem* item);
	QTreeWidgetItem* item();
	void setAgale();
};

#endif // !TOPITEMWIDGET_H_
